/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_cpu_intf.h
*
* @purpose   This file contains the interface to the CPU
*
* @component hapi
*
* @comments
*
* @create    2/22/2001
*
* @author    nsummers
*
* @end
*
**********************************************************************/

#ifndef INCLUDE_BROAD_CPU_INTF_H
#define INCLUDE_BROAD_CPU_INTF_H

#include "dapi.h"

#include "bcmx/lplist.h"
#include "bcmx/bcmx.h"
#include "bcmx/lport.h"
#include "bcmx/rx.h"


/*********************************************************************
*
* @purpose Sends a packet from the CPU
*
* @param   DAPI_USP_t *usp    - Used to get the driver ports.  Needs to be a valid usp.
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_FRAME_SEND
* @param   void       *data   - DAPI_FRAME_CMD_t.cmdData.send
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSend(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose All data frames come through this
*
* @param   L7_netBufHandle  frameHdl - frame handle for this data 
* @param   DAPI_USP_t      *usp      - Used to notify the application of the frame.
*                                      Needs to be a valid usp.
* @param   DAPI_t          *dapi_g   - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
/* bcm_rx_t hapiBroadReceive(int unit, bcm_pkt_t *pkt, void *cookie); */

bcm_rx_t hapiBroadReceive(L7_int32 unit, bcm_pkt_t *pktData, void *cookie);

/*********************************************************************
*
* @purpose Function to process PDU frames
*
* @param   L7_netBufHandle  frameHdl - frame handle for this data 
* @param   DAPI_USP_t      *usp      - Used to notify the application of the frame.
*                                      Needs to be a valid usp.
* @param   DAPI_t          *dapi_g   - the driver object
*
* @returns L7_BOOL
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_BOOL hapiBroadReceivePdu(L7_netBufHandle frameHdl, 
                            L7_ushort16 vlanID, DAPI_USP_t *usp, 
                            L7_uchar8 rx_untagged,DAPI_t *dapi_g,
                            L7_ushort16 innerVlanID);

/*********************************************************************
*
* @purpose Determines if a port is forwarding
*
* @param   DAPI_USP_t      *usp      - port of interest
* @param   L7_ushort16      vlanId   - vlan of interest
* @param   DAPI_t          *dapi_g   - the driver object
*
* @returns L7_BOOL
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_BOOL hapiBroadPortIsForwarding(DAPI_USP_t *usp, L7_ushort16 vlanId, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose This routine get the port bit map of same VLAN which are link up
*
* @param   L7_int32    unit
* @param   L7_short16  vlanID
* @param   L7_uint32  *pbmp
* @param   DAPI_t     *dapi_g
*
* @returns NONE
*
* @notes   This routine is for use on reception... it includes all ports
*          w/ link up that are in the VLAN, but excludes the port
*          that originally received the pkt.
*
* @end
*
*********************************************************************/
void hapiBroadGetSameVlanPbmp(L7_ushort16 vlanID, DAPI_USP_t *usp, bcmx_lplist_t *memberLplist, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose This routine get the port bit map of same VLAN which are link up
*
* @param   L7_int32    unit
* @param   L7_short16  vlanID
* @param   L7_uint32  *pbmp
* @param   DAPI_t     *dapi_g
*
* @returns NONE
*
* @notes   This routine is for use on transmission... it includes all ports
*          w/ link up that are in the VLAN.
*
* @end
*
*********************************************************************/
void hapiBroadGetSameVlanPbmpTx(L7_ushort16 vlanID, bcmx_lplist_t *taggedLplist, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose This routine sets the tagging status of the ports passed in the lplist
*
* @param   vlanID          vlan to use for tagging status
* @param   *lplist         starting list of members both tagged and untagged
* @param   *tagedlplist    resulting list of members tagged members
* @param   *untagedlplist  resulting list of members untagged members
* @param   *dapi_g
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadTaggedStatusLplistSet(L7_ushort16 vlanId, bcmx_lplist_t *lplist, bcmx_lplist_t *taggedLplist,
                                       bcmx_lplist_t *untaggedLplist, DAPI_t *dapi_g);
/*********************************************************************
*
* @purpose This routine clears bits in a port bitmap of the src port
*
* @param   L7_int32    unit
* @param   DAPI_USP_t *usp
* @param   L7_uint32  *pbmp
* @param   DAPI_t     *dapi_g
*
* @returns NONE
*
* @notes   none
*
* @end
*
*********************************************************************/
void hapiBroadPruneRxPort(DAPI_USP_t *usp, bcmx_lplist_t *lplist, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose This routine clears bits in a port bitmap of the destination
*          ports that are not link up, and makes sure only one LAG member
*          will forward the packet for any given LAG.
*
* @param   L7_int32    unit
* @param   L7_uint32  *pbmp
* @param   DAPI_t     *dapi_g
*
* @returns NONE
*
* @notes   none
*
* @end
*
*********************************************************************/
void hapiBroadPruneTxPorts(bcmx_lplist_t *lplist, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose This routine clears bits in a port bitmap of the destination
*          ports that are not dot1x authorized.
*
* @param   L7_int32    unit
* @param   L7_uint32  *pbmp
* @param   DAPI_t     *dapi_g
*
* @returns NONE
*
* @notes   none
*
* @end
*
*********************************************************************/
void hapiBroadPruneTxUnauthorizedPorts(bcmx_lplist_t *lplist, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose This routine clears bits in a port bitmap of the destination
*          ports that are not forwarding.
*
* @param   L7_int32    unit
* @param   L7_ushort16 vlanId
* @param   L7_uint32  *pbmp
* @param   DAPI_t     *dapi_g
*
* @returns NONE
*
* @notes   none
*
* @end
*
*********************************************************************/
void hapiBroadPruneTxDiscardingPorts(L7_ushort16 vlanId, bcmx_lplist_t *lplist, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose This routine xxx
*
* @param   L7_int32   unit
* @param   void*      ptr
*
* @returns NONE
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadL3McastRPF(L7_netBufHandle frameHdl, L7_ushort16 vlanID, DAPI_USP_t *usp, 
                                DAPI_t *dapi_g);
#endif
