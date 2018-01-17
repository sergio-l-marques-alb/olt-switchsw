/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    gidtt.h
* @purpose     GID transition tables defintion and function
* @component   GARP
* @comments    none
* @create      02/23/2001
* @author      Hassan
* @author      
* @end
*             
**********************************************************************/
/*********************************************************************
 *                   
 ********************************************************************/
#ifndef INCLUDE_GIDTT_H_
#define INCLUDE_GIDTT_H_

#include "gidapi.h"


/******************************************************************************
 * GIDTT : GARP INFORMATION DISTRIBUTION PROTOCOL : TRANSITION TABLES
 ******************************************************************************
 */

/*********************************************************************
* @purpose  an event is received.
*
* @param    Gid          port
* @param    Gid_machine  machine
* @param    Gid_event    event
*
* @returns  event
*
* @notes   
*
*       
* @end
*********************************************************************/
extern Gid_event gidtt_event(Gid         *my_port,
							 Gid_machine *machine,
							 Gid_event    event);

/*********************************************************************
* @purpose  find if a machine need to transmit 
*
* @param    Gid          port
* @param    Gid_machine  machine
*
* @returns  event
*
* @notes   
*
*       
* @end
*********************************************************************/
extern Gid_event gidtt_tx(Gid         *my_port,
						  Gid_machine *machine);

/*********************************************************************
* @purpose  register a machine
*
* @param    Gid_machine  machine
*
* @returns  L7_FALSE
* @returns  L7_TRUE
*
* @notes    Returns True if the Registrar is in, or if registration is fixed.
*
*       
* @end
*********************************************************************/
extern L7_BOOL gidtt_in(Gid_machine *machine);

/*********************************************************************
* @purpose  check if a machine is active
*
* @param    Gid_machine  machine
*
* @returns  L7_FALSE
* @returns  L7_TRUE
*
* @notes    Returns False iff the Registrar is Normal registration, Empty, and the
*           Application is Normal membership, Very Anxious Observer.
*       
* @end
*********************************************************************/
extern L7_BOOL gidtt_machine_active(Gid_machine *machine);

/*********************************************************************
* @purpose  check if the machine is in forbidden regsitration
*
* @param    Gid_machine  machine
*
* @returns  L7_FALSE
* @returns  L7_TRUE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_BOOL isMachineRegForbidden(Gid_machine *machine);

/*********************************************************************
* @purpose  check if the machine is in forbidden or fixed regsitration 
*
* @param    Gid_machine  machine
*
* @returns  L7_FALSE
* @returns  L7_TRUE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_BOOL isMachineRegForbiddenOrFixed(Gid_machine *machine);

/*********************************************************************
* @purpose  init the machine to Va and Mt
*
* @param    Gid_machine  machine
*
* @returns  none
*
* @notes    none
*       
* @end
*********************************************************************/
void initMachine(Gid_machine *machine);

/*********************************************************************
* @purpose  set the machine to be leaving
*
* @param    Gid_machine  machine
*
* @returns  none
*
* @notes    none
*       
* @end
*********************************************************************/
void setMachineLeave(Gid_machine *machine);

/*********************************************************************
* @purpose  check if this is the port received this machine
*
* @param    Gid_machine  machine
*
* @returns  L7_FALSE
* @returns  L7_TRUE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_BOOL isIncomingPort(Gid_machine *machine);

/*********************************************************************
* @purpose  change the registrar state when a message is sent out
*
* @param    Gid_machine  machine
* @param    Gid_event    directive
*
* @returns  L7_FALSE
* @returns  L7_TRUE
*
* @notes    none
*       
* @end
*********************************************************************/
void gidtt_txmsg(Gid_machine *machine,Gid_event directive);


enum Registrar_states
{  
	Inn,
	Lv,
	Mt,

	Inr,             /* In, registration fixed     */
	Lvr,
	Mtr,

	Inf,             /* In, registration forbidden */
	Lvf,
	Mtf
};

#endif

