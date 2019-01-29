/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_l2_lag.h
*
* @purpose   This file includes all the messages for L2 and prototypes
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

#ifndef INCLUDE_BROAD_L2_LAG_H
#define INCLUDE_BROAD_L2_LAG_H

#include "l7_common.h"
#include "dapi.h"
#include "dapi_trace.h"

/* LAG Marker message semaphore timeout in 'ticks' (100 ticks a second) */
#define HAPI_BROAD_LAG_MARKER_MSG_SEMA_TIMEOUT         50
#define HAPI_BROAD_LAG_ASYNC_DONE_WAIT_TIMEOUT         (L7_WAIT_FOREVER)


/* Command format for LAG port manipulation.
*/
typedef enum 
{
  HAPI_BROAD_LAG_ASYNC_PORT_ADD = 1,
  HAPI_BROAD_LAG_ASYNC_PORT_DELETE = 2,
  HAPI_BROAD_LAG_ASYNC_DONE = 3
} hapi_broad_lag_async_cmd_t;

typedef struct 
{
  hapi_broad_lag_async_cmd_t  async_cmd;
  DAPI_USP_t              usp;
  union 
  {
    struct                   
    {                        
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_uint32                   numOfMembers;
      L7_uint32                   maxFrameSize; 
      L7_uint32                   hashMode;
      DAPI_USP_t                  memberSet [L7_MAX_MEMBERS_PER_LAG];
    } lagPortAdd;            

    struct                   
    {                        
      DAPI_CMD_GET_SET_t          getOrSet;
      L7_uint32                   numOfMembers;
      L7_uint32                   maxFrameSize; 
      L7_uint32                   hashMode;
      DAPI_USP_t                  memberSet [L7_MAX_MEMBERS_PER_LAG];
    } lagPortDelete;         

  } asyncData;
} hapi_broad_lag_async_msg_t;



/*
 * Prototypes
 */

/*********************************************************************
*
* @purpose Take LAG access semaphore.
*
* @param none
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
void hapiBroadLagCritSecEnter(void);

/*********************************************************************
*
* @purpose Give LAG access semaphore.
*
* @param none
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
void hapiBroadLagCritSecExit(void);

/*********************************************************************
*
* @purpose Add a LAG to the specified VLAN ID.
*
* @param hapiLagPortPtr - Pointer to the specified LAG.
* @param vlan_id        - Specified VLAN ID.
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
void hapiBroadLagAddLagToVlanHw (DAPI_USP_t *usp,
                                L7_ushort16   vlan_id,
                                DAPI_t        *dapi_g);

/*********************************************************************
*
* @purpose Remove a LAG tfrom the specified VLAN ID.
*
* @param hapiLagPortPtr - Pointer to the specified LAG.
* @param vlan_id        - Specified VLAN ID.
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
void hapiBroadLagRemoveLagFromVlanHw (DAPI_USP_t *usp,
                                     L7_ushort16   vlan_id,
                                     DAPI_t        *dapi_g);

/*********************************************************************
*
* @purpose Add/Remove a LAG from the specified Vlan list.
*
* @param usp                  - Pointer to the LAG USP
* @param vlanCmd              - L7_TRUE:   Add Lag to list of vlans
*                               L7_FALSE:  Remove Lag from list of vlans
* @param vlanMemberSet        - List of vlans
* @param vlanTagSet           - List of tagged vlans 
* @param dapi_g               - Driver object 
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
void hapiBroadLagSetLagToVlanListHw (DAPI_USP_t        *usp,
                                     L7_BOOL            vlanCmd,
                                     L7_VLAN_MASK_t    *vlanMemberSet,
                                     L7_VLAN_MASK_t    *vlanTagSet,
                                     DAPI_t            *dapi_g);

/*********************************************************************
*
* @purpose Add or remove specified LAG from hardware VLAN membership.
*      LAG ports are added or removed indivisually.
*
* @param hapiLagPortPtr - Pointer to the specified LAG.
* @param port_cmd    - 1 - Add port to the hardware VLAN membership
*            0 - Remove port from the hardware VLAN membership.
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
void hapiBroadLagAddRemoveLagFromVlans (DAPI_USP_t *usp,
                                       L7_uint32   port_cmd,
                                       DAPI_t     *dapi_g);

/*********************************************************************
*
* @purpose Hook in the lag functionality and create any data
*
* @param   DAPI_PORT_t *dapiPortPtr - generic driver port member
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadL2LagPortInit(DAPI_PORT_t *dapiPortPtr);

/*********************************************************************
*
* @purpose Takes the dapiCmd info and formats a message for each PPE
*
* @param   DAPI_USP_t *usp    - the usp of this lag
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_LAG_CREATE
* @param   void       *data   - lag config data
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadLagCreate(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Takes the dapiCmd info and formats a message for each PPE
*
* @param   DAPI_USP_t *usp    - the usp of this lag
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_LAG_PORT_ADD
* @param   void       *data   - DAPI_LAG_MGMT_CMD_t.cmdData.lagPortAdd
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadLagPortAdd(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Takes the dapiCmd info and formats a message for each PPE
*
* @param   DAPI_USP_t *usp    - the usp of this lag
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_LAG_PORT_DELETE
* @param   void       *data   - DAPI_LAG_MGMT_CMD_t.cmdData.lagPortDelete
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadLagPortDelete(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Takes the dapiCmd info and formats a message for each PPE
*
* @param   DAPI_USP_t *usp    - the usp of this lag
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_LAG_DELETE
* @param   void       *data   - DAPI_LAG_MGMT_CMD_t.cmdData.lagDelete
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadLagDelete(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Enables 802.3AD trunking mode.
*
* @param   DAPI_USP_t *usp    - the usp of this lag
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_AD_TRUNK_MODE_SET
* @param   void       *data   - DAPI_LAG_MGMT_CMD_t.cmdData.portADTrunk
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadADTrunkModeSet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Sets the Hash Mode value for a LAG.
*
* @param   DAPI_USP_t *usp    - the usp of this lag
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_AD_TRUNK_MODE_SET
* @param   void       *data   - DAPI_LAG_MGMT_CMD_t.cmdData.hashMode
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadLagHashModeSet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);


/*********************************************************************
*
* @purpose Wait for Hapi Lag Async processing to complete
*
* @param   DAPI_USP_t *usp    {(input)} Cpu Usp
* @param   DAPI_CMD_t  cmd    {(input)} 
* @param   void       *data   {(input)} DAPI_LAG_MGMT_CMD_t.cmdData.
* @param   DAPI_t     *dapi_g {(input)} The driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadLagsSync(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Use the LAG's protocol VLAN config for the member port's
*          protocol VLAN config
*
* @param   DAPI_USP_t *memberUsp
* @param   DAPI_USP_t *lagUsp
* @param   DAPI_t     *dapi_g
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadProtocolVlanLagPortAddNotify(DAPI_USP_t *memberUsp, DAPI_USP_t *lagUsp, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Use the member port's protocol VLAN config for the member port's
*          protocol VLAN config
*
* @param   DAPI_USP_t *memberUsp
* @param   DAPI_USP_t *lagUsp
* @param   DAPI_t     *dapi_g
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadProtocolVlanLagPortDeleteNotify(DAPI_USP_t *memberUsp, DAPI_USP_t *lagUsp, DAPI_t *dapi_g);


/*********************************************************************
*
* @purpose Initialize the LAG component.
*
* @param none
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
void hapiBroadLagInit (void);

/*********************************************************************
*
* @purpose Wait for LAG async work to finish.
*
* @param   timeout {(input)} Time (secs) to wait for Lag Async task to finish 
*                            processing
*
* @returns L7_RC_t: L7_SUCCESS - Operation successful
*                   L7_FAILURE - Timeout waiting for driver to finish or other
*                                failures 
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadLagAsyncDoneWait(L7_int32 timeout);

/*********************************************************************
*
* @purpose Process asynchronous LAG commands.
*
* @param   num_msgs  Number of Lag membership update messages to be processed. 
* @param   dapi_g    Driver object 
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
void hapiBroadLagAsyncProcessMessages(L7_uint32 num_msgs, DAPI_t *dapi_g);

#endif
