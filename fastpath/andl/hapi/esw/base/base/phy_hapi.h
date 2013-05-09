/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename phy.h
*
* @purpose This file contains the information to manage phys
*
* @component hapi
*
* @comments none
*
* @create 2/28/01
*
* @author nsummers
*
* @end
*
**********************************************************************/

#ifndef INCLUDE_PHY_H
#define INCLUDE_PHY_H

#include "dapi_struct.h"
#include "dapi.h"


#define HAPI_BROAD_PHY_CTL_REG_AUTO_NEG_BIT     1 << 12
#define HAPI_BROAD_PHY_CTL_REG_LOOPBACK_BIT     1 << 14
#define HAPI_BROAD_PHY_CTL_REG_SPEED_100_BIT    1 << 13
#define HAPI_BROAD_PHY_CTL_REG_DUPLEX_FULL_BIT  1 << 8

#define HAPI_BROAD_PHY_AUTO_NEGOTIATION_HCD_MASK  0x0700

#define HAPI_BROAD_PHY_AUTO_NEGOTIATION_10_HALF   0x0000
#define HAPI_BROAD_PHY_AUTO_NEGOTIATION_10_FULL   0x0001
#define HAPI_BROAD_PHY_AUTO_NEGOTIATION_100_HALF  0x0002
#define HAPI_BROAD_PHY_AUTO_NEGOTIATION_100_FULL  0x0003

#define HAPI_BROAD_PHY_PORT_QUERY_CYCLE_IN_SECONDS 2



/*********************************************************************
*
* @purpose Initialize the phy polling/interrupt
*
* @param   DAPI_t *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   Creates a task for now.  Needs to change in the future to check
*  		     whether or not to do polling or interrupt.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPhyInit(DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Initializes the Phys and Grabs the inital state of Link Status
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
L7_RC_t hapiBroadPortPhyInit(L7_ushort16 unitNum, L7_ushort16 slotNum, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Check the link status on the USP, if the status has changed,
*          a callback will be performed to the application.
*
* @param   L7_ulong32  numArgs  - number of arguments for the task
* @param   L7_ulong   *argArray - arg array to be passed
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadLinkCheck(DAPI_USP_t *usp, DAPI_t *dapi_g); 

/*********************************************************************
*
* @purpose Poll for the phy link status
*
* @param   L7_ulong32  numArgs - number of arguments for the task
* @param   DAPI_t     *dapi_g  - the driver object
*
* @returns L7_RC_t result
*
* @notes   Doesn't actually do the read
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadBC5218Task(L7_ulong32 numArgs, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose This routine set the speed and duplex for a phy
*
* @param   DAPI_USP_t         *usp    - needs to be a valid usp
* @param   DAPI_PORT_SPEED_t   speed  - the speed you wish to set the usp to
* @param   DAPI_PORT_DUPLEX_t  duplex - the duplex you wish to set the usp to
* @param   DAPI_t             *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPhyModeSet(DAPI_USP_t *usp, DAPI_PORT_SPEED_t speed, DAPI_PORT_DUPLEX_t duplex, DAPI_t *dapi_g);
 
/*********************************************************************
*
* @purpose This routine get the speed and duplex for a phy
*
* @param   DAPI_USP_t         *usp    - needs to be a valid usp
* @param   DAPI_PORT_SPEED_t   speed  - the speed to set the usp to
* @param   DAPI_PORT_DUPLEX_t  duplex - the duplex to set the usp to
* @param   DAPI_t             *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes  
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPhyModeGet(DAPI_USP_t *usp, DAPI_PORT_SPEED_t *speed, DAPI_PORT_DUPLEX_t *duplex,
                            L7_BOOL *isLinkUp, L7_BOOL *isSfpLink, 
                            L7_BOOL *isTxPauseAgreed, L7_BOOL *isRxPauseAgreed, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose This routine set the speed and duplex for a phy
*
* @param   DAPI_USP_t  *usp    - needs to be a valid usp
* @param   L7_BOOL  enable - not yet used in this function
* @param   DAPI_t      *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPhyLoopbackSet(DAPI_USP_t *usp, L7_BOOL enable, DAPI_t *dapi_g);


/*********************************************************************
*
* @purpose This routine resolves the MAC pause from PHY pause advertisement
*           of its own and its link partner if link comes UP in full duplex and 
*           system flow control is ON . If link comes UP in half duplex, this routine
*           sets the jam if system flow control is ON.
*           If system flow control is OFF, this routine turns off both the MAC pause and jam
*
* @param   DAPI_USP_t  *usp    - needs to be a valid usp
* @param   DAPI_t      *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadMacPauseResolveOnLinkUp(DAPI_USP_t *usp, DAPI_t *dapi_g);


#endif
