/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_init.h
*
* @purpose   This file contains the broad hapi interface and other common broad
*            routines
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

#ifndef INCLUDE_BROAD_INIT_H
#define INCLUDE_BROAD_INIT_H

#include "dapi.h"

/*********************************************************************
*
* @purpose Initialize any non card specific stuff
*
* @param   DAPI_t *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadInit(L7_ulong32 cardId, void *handle);
                                           
/*********************************************************************
*
* @purpose Initializes the frame handling pointers and calls the 
*          to create a cpu board
*
* @param   DAPI_t *dapi_g - system information
*
* @returns L7_RC_t result
*
* @notes   Handles setting up the board support
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadCpuBoardInit(DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Allocate any system memory needed, BROAD_SYSTEM_t and anything else
*
* @param   DAPI_t *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadMemInit(DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Initializes BROAD line card
*
* @param   L7_ushort16  unitNum - the unit being initialized
* @param   L7_ushort16  slotNum - the slot being initialized
* @param   DAPI        *dapi_g  - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPhysicalCardInsert(DAPI_USP_t *dapiUsp, DAPI_CMD_t cmd, void *data, void *handle);


/*********************************************************************
*
* @purpose Initializes fabric only line card 
*
*
* @returns L7_RC_t result
*
* @notes   This was done for 5675 based Chassis Control Module.
*                 Special Card insert for Fabric only board 
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPhysicalCfmFabricCardInsert(DAPI_USP_t *dapiUsp, DAPI_CMD_t cmd, void *data, void *handle);

/*********************************************************************
*
* @purpose Initializes Power Supply card
*
* @param  unitNum         unit number for this logical card
* @param  slotNum         slot number for this logical card
* @param *dapi_g          system information
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPowerSupplyCardInsert(DAPI_USP_t *dapiUsp, DAPI_CMD_t cmd, 
                                       void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Uninitializes Power Supply card
*
* @param  unitNum         unit number for this logical card
* @param  slotNum         slot number for this logical card
* @param *dapi_g          system information
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPowerSupplyCardRemove(DAPI_USP_t *dapiUsp, DAPI_CMD_t cmd, 
                                       void *data, void *handle);

/*********************************************************************
*
* @purpose Initializes Fan card
*
* @param  unitNum         unit number for this logical card
* @param  slotNum         slot number for this logical card
* @param *dapi_g          system information
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadFanCardInsert(DAPI_USP_t *dapiUsp, DAPI_CMD_t cmd, 
                               void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Uninitializes Fan card
*
* @param  unitNum         unit number for this logical card
* @param  slotNum         slot number for this logical card
* @param *dapi_g          system information
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadFanCardRemove(DAPI_USP_t *dapiUsp, DAPI_CMD_t cmd, 
                               void *data, void *handle);

/*********************************************************************
*
* @purpose Initializes Logical Cpu card
*
* @param   L7_ushort16  unitNum - the unit being initialized
* @param   L7_ushort16  slotNum - the slot being initialized
* @param   DAPI        *dapi_g  - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadCpuCardInsert(DAPI_USP_t *dapiUsp, DAPI_CMD_t cmd, void *data, void *handle);


/*********************************************************************
*
* @purpose Initializes generic logical cards
*
* @param  unitNum         unit number for this logical card
* @param  slotNum         slot number for this logical card
* @param *dapi_g          system information
*
* @returns L7_RC_t result
*
* @notes used for interfaces that do not have direct hardware support
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadGenericCardInsert(DAPI_USP_t *dapiUsp, DAPI_CMD_t cmd,
                                   void *data, void *handle);

/*********************************************************************
*
* @purpose Uninitializes ALL card types physical, cpu, lag, router
*
* @param   L7_ushort16  unitNum - the unit being initialized
* @param   L7_ushort16  slotNum - the slot being initialized
* @param   DAPI        *dapi_g  - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadCardRemove(DAPI_USP_t *dapiUsp, DAPI_CMD_t cmd, void *data, void *handle);

/*********************************************************************
*
* @purpose Uninitializes ALL card types physical, cpu, lag, router 
*
*
* @returns L7_RC_t result
*
* @notes   This was done for 5675 based Chassis Control Module.
*                 Special Card Remove Function for Fabric only board
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadCfmFabricCardRemove(DAPI_USP_t *dapiUsp, DAPI_CMD_t cmd, void *data, void *handle);

/*********************************************************************
*
* @purpose Initializes fabric only line card 
*
*
* @returns L7_RC_t result
*
* @notes   This was done for 5675 based Chassis Control Module.
*                 /Special Card insert for Fabric only board *
*
* @end
*
*********************************************************************/

/*********************************************************************
*
* @purpose Initializes Swc Memory
*
* @param   L7_ulong32   cardId  - card database entry for this slot
* @param   L7_ushort16  unitNum     - the unit being initialized
* @param   L7_ushort16  slotNum     - the slot being initialized
* @param   DAPI_t      *dapi_g      - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSwcMemAlloc(L7_ushort16 unitNum, L7_ushort16 slotNum, DAPI_t *dapi_g);


/*********************************************************************
*
* @purpose Initializes Swc
*
* @param   SYSAPI_CARD_ENTRY_t *cardInfoPtr - card database entry for this slot
* @param   L7_ushort16  unitNum     - the unit being initialized
* @param   L7_ushort16  slotNum     - the slot being initialized
* @param   DAPI_t      *dapi_g      - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSwcInit(L7_ushort16 unitNum, L7_ushort16 slotNum, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose This function maps the slots to the physical ports by reading
*          the card database.  By filling in these parameters the system
*          can be mapped out.
*
* @param   SYSAPI_CARD_ENTRY_t *cardInfoPtr - card database entry for this slot
* @param   L7_ushort16  unitNum     - the unit being initialized
* @param   L7_ushort16  slotNum     - the slot being initialized
* @param   DAPI_t      *dapi_g      - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPortMemAlloc(L7_ushort16 unitNum, L7_ushort16 slotNum, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose This function maps the slots to the physical ports by reading
*          the card database.  By filling in these parameters the system
*          can be mapped out. Highly dependent on the Card Database for the
*					 hardware.
*
* @param   SYSAPI_CARD_ENTRY_t *cardInfoPtr - card database entry for this slot
* @param   L7_ushort16  unitNum	    - the unit being initialized
* @param   L7_ushort16  slotNum     - the slot being initialized
* @param   DAPI_t      *dapi_g      - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPhysicalPortMapGet(L7_ushort16 unitNum, L7_ushort16 slotNum, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose This function maps the slots to the cpu ports by reading
*          the card database.  By filling in these parameters the system
*          can be mapped out. Highly dependent on the Card Database for the
*          hardware.
*
* @param   cardInfoPtr - card database entry for this slot
* @param   unitNum     - the unit being initialized
* @param   slotNum     - the slot being initialized
* @param   dapi_g      - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadCpuPortMapGet(L7_ushort16 unitNum, L7_ushort16 slotNum, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Help
*
* @param   SYSAPI_CARD_ENTRY_t *cardInfoPtr - card database entry for this slot
* @param   L7_ushort16  unitNum     - the unit being initialized
* @param   L7_ushort16  slotNum     - the slot being initialized
* @param   DAPI_t      *dapi_g      - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPortInit(L7_ushort16 unitNum, L7_ushort16 slotNum, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose help
*
* @param   L7_ushort16  unitNum - the unit being initialized
* @param   L7_ushort16  slotNum - the slot being initialized
* @param   DAPI_t      *dapi_g  - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadDefaultConfigInit(L7_ushort16 unitNum,L7_ushort16 slotNum, DAPI_t *dapi_g);


/*********************************************************************
*
* @purpose On a new bcm unit insert, refresh the l2/linkscan/rx registrations.
*
* @param   L7_ushort16  unitNum - the unit being initialized
* @param   L7_ushort16  slotNum - the slot being initialized
* @param   DAPI_t      *dapi_g  - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadBcmxRegisterUnit(L7_ushort16 unitNum,L7_ushort16 slotNum, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Replays configuration that is system based in our application
*          but interface based in the hardware
*
* @returns void
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadApplyConfig(L7_uint32 unit,L7_uint32 slot, DAPI_t *dapi_g);


/*********************************************************************
*
* @purpose Initializes logical lag cards
*
* @param   L7_ushort16  unitNum - unit number for this logical card
* @param   L7_ushort16  slotNum - slot number for this logical card
* @param   DAPI_t      *dapi_g  - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadLagCardInsert(DAPI_USP_t *dapiUsp, DAPI_CMD_t cmd, void *data, void *handle);


/******************************************************************************
*
* @purpose Initializes logical router cards
*
* @param  dapiUsp         USP for the logical router card
* @param  cmd             DAPI cmd for inserting card (not used)
* @param  data            Data associated with the DAPI cmd (not used)
* @param *handle          DAPI driver object handle
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3RouterCardInsert(DAPI_USP_t *dapiUsp,
                                    DAPI_CMD_t cmd,
                                    void *data,
                                    void *handle);


/******************************************************************************
*
* @purpose Initializes logical tunnel card
*
* @param  dapiUsp         USP for the logical tunnel card
* @param  cmd             DAPI cmd for inserting card (not used)
* @param  data            Data associated with the DAPI cmd (not used)
* @param *handle          DAPI driver object handle
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
******************************************************************************/
L7_RC_t hapiBroadL3TunnelCardInsert(DAPI_USP_t *dapiUsp,
                                    DAPI_CMD_t cmd,
                                    void *data,
                                    void *handle);


/******************************************************************************
*
* @purpose Removes logical tunnel card
*
* @param  dapiUsp         USP for the logical tunnel card
* @param  cmd             DAPI cmd for removing card (not used)
* @param  data            Data associated with the DAPI cmd (not used)
* @param *handle          DAPI driver object handle
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
******************************************************************************/
L7_RC_t hapiBroadL3TunnelCardRemove(DAPI_USP_t *dapiUsp,
                                    DAPI_CMD_t cmd,
                                    void *data,
                                    void *handle);

/*********************************************************************
 *
 * @purpose  Initializes l2 tunnel card
 *
 * @param   *dapiUsp       @b{(input)} The unit and slot initialized
 * @param    cmd           @b{(input)} The corresponding dapi command
 * @param   *data          @b{(input)} Data for the command
 * @param   *handle        @b{(input)} The driver object, dapi_g
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *
 *********************************************************************/
L7_RC_t hapiBroadL2TunnelCardInsert(DAPI_USP_t *dapiUsp, DAPI_CMD_t cmd, 
                                    void *data, void *handle);

/*********************************************************************
 *
 * @purpose  De-initializes l2 tunnel card
 *
 * @param   *dapiUsp       @b{(input)} The unit and slot initialized
 * @param    cmd           @b{(input)} The corresponding dapi command
 * @param   *data          @b{(input)} Data for the command
 * @param   *handle        @b{(input)} The driver object, dapi_g
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *
 *********************************************************************/
L7_RC_t hapiBroadL2TunnelCardRemove(DAPI_USP_t *dapiUsp, DAPI_CMD_t cmd, 
                                    void *data, void *handle);


#endif
