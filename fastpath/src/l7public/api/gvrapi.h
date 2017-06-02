/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    gvrapi.h
* @purpose     GVRP API definitions and functions
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
#ifndef INCLUDE_GVR_API_H_
#define INCLUDE_GVR_API_H_

#include "garp.h"

/* GVR : GARP VLAN REGISTRATION APPLICATION : GARP ATTRIBUTES */

typedef enum 
{
    All_attributes =1,
    Vlan_attribute = 1
} Attribute_type;



typedef struct /* gvr */
{
	Garp      g;

	L7_uint32 vlan_id;

	void     *gvd; /* VLAN Registration Entry Database */

	L7_uint32  number_of_gvd_entries;

	L7_uint32  last_gvd_used_plus1;

} Gvr;


/* GVR : GARP VLAN REGISTRATION APPLICATION : CREATION, DESTRUCTION */

/*********************************************************************
* @purpose  Create Gvr
*
* @param    L7_uint32    process id
* @param    void**        pointer to gvr
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes   Creates a new instance of GVR, allocating and initialising a control
*          block, returning True and a pointer to this instance if creation suceeds.
*          Also creates instances of GVD (the GARP VLAN database) and of GIP
*          (which controls information propagation).
*
*          Ports are created by the system and added to GVR separately (see
*          gvr_added_port() and gvr_removed_port() below).
*
*          The operating system supplied process_id is for use in subsequent calls
*          to operating system services. The system itself ensures the temporal
*          scope of process_id, guarding against timers yet to expire for destroyed
*          processes etc. Although process_id will be implicitly supplied by many
*          if not most systems, it is made explicit in this implementation for
*          clarity.
*       
* @end
*********************************************************************/
extern L7_BOOL gvr_create_gvr(L7_uint32 process_id, void **gvr);

/*********************************************************************
* @purpose  Destroy Gvr
*
* @param    Gvr*        pointer to gvr
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes  Destroys an instance of GVR, destroying and deallocating the associated
*         instances of GVD and GIP, and any instances of GID remaining.
*       
* @end
*********************************************************************/
extern void gvr_destroy_gvr(Gvr *gvr);
  
/*********************************************************************
* @purpose  Add Gvr port
*
* @param    void*        pointer to gvr
* @param    L7_uint32    port no
*
* @returns  None
*
* @notes   The system has created a new port for this application and added it to
*          the ring of GID ports. This function ensures that Static VLAN Entries
*          from the Permanent Database are represented in the GVD database (which
*          provides VLAN ID to GID index mapping) and have GID machines in the newly
*          added port (with the correct management control state). This can result
*          in the creation of new GID machines or modification of the state of
*          existing machines.
*
*          Newly created ports are 'connected' for the purpose of GARP information
*          propagation using the separate function gip_connect_port(). This should
*          be called after this function, gvr_added_port. It may cause GVRP/GIP
*          to propagate information from the static management controls through
*          other ports.
*
*          It is assumed that new ports will be 'connected' correctly before the
*          application continues as determined by the active topology of the network,
*          i.e. if stp_forwarding(port_no) gvr_connect_port(port_no);.
*
*          As the system continues to run it should invoke gip_disconnect_port()
*          and gip_connect_port() as required to maintain the required connectivity.
*       
* @end
*********************************************************************/
extern void gvr_added_port(void *my_gvr, L7_uint32 port_no);

/*********************************************************************
* @purpose  remove Gvr port
*
* @param    void*        pointer to gvr
* @param    L7_uint32    port no
*
* @returns  None
*
* @notes  The system has removed and destroyed the GID port. This function should
*         provide any application specific cleanup required.
*       
* @end
*********************************************************************/
extern void gvr_removed_port(void *my_gvr, L7_uint32 port_no);

/******************************************************************************
 * GVR : GARP VLAN REGISTRATION APPLICATION : JOIN, LEAVE INDICATIONS
 ******************************************************************************
 */

/*********************************************************************
* @purpose  Gvr join indication
*
* @param    void*        pointer to gvr
* @param    void*        pointer to the port
* @param    L7_uint32    joining index
*
* @returns  None
*
* @notes  
*       
* @end
*********************************************************************/
extern void gvr_join_indication(void *my_gvr, void *my_port,
								L7_uint32 joining_gid_index);

/*********************************************************************
* @purpose  Gvr propagate leave 
*
* @param    void*        pointer to gvr
* @param    void*        pointer to the port
* @param    L7_uint32    joining index
*
* @returns  None
*
* @notes  
*       
* @end
*********************************************************************/
extern void gvr_join_leave_propagated(void *my_gvr, void *my_port,
								      L7_uint32 gid_index);

/*********************************************************************
* @purpose  Gvr leave indication
*
* @param    void*        pointer to gvr
* @param    void*        pointer to the port
* @param    L7_uint32    joining index
*
* @returns  None
*
* @notes  
*       
* @end
*********************************************************************/
extern void gvr_leave_indication(void *my_gvr, void *my_port,
								 L7_uint32 leaving_gid_index);


/******************************************************************************
 * GVR : GARP VLAN REGISTRATION APPLICATION : PROTOCOL & MGT EVENTS
 ******************************************************************************
 */

/*********************************************************************
* @purpose  Gvr rcv message
*
* @param    Gvr*        pointer to gvr
* @param    L7_uint32    joining index
* @param    void*        pointer to Pdu
*
* @returns  None
*
* @notes  Process an entire received pdu for this instance of GVR.
*       
* @end
*********************************************************************/
extern void gvr_rcv(Gvr *my_gvr, L7_uint32 my_port, void *pdu);

/*********************************************************************
* @purpose  Gvr tx message
*
* @param    Gvr*        pointer to gvr
* @param    void*        pointer to port
*
* @returns  None
*
* @notes  Transmit a pdu for this instance of GVR.
*       
* @end
*********************************************************************/
extern void gvr_tx(void *my_gvr, void *my_port);


/*********************************************************************
* @purpose  Gvr process message
*
* @param    Gvr*        pointer to application
* @param    void*        pointer to command
*
* @returns  None
*
* @notes  Transmit a pdu for this instance of GVR.
*       
* @end
*********************************************************************/
extern L7_BOOL gvr_proc_command(Gvr *application,void *command);

/*********************************************************************
* @purpose  Remove a static vlan and update the GIDs
*
* @param    vid    VLAN ID to remove
*
* @returns  none
*
* @notes   
*
* @end
*********************************************************************/
extern void gvr_delete_attribute(L7_uint32 vid);


#endif

