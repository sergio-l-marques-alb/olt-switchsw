/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    gmrapi.h
* @purpose     GMR defintions and functions
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
#ifndef INCLUDE_GMR_H_
#define INCLUDE_GMR_H_

#include "garpapi.h"
#include "garp.h"

#define GmrpMacAddressSize (L7_MAC_ADDR_LEN)

typedef struct /* gmr */
{
  Garp       g;

  L7_ushort16  vlan_id;

  void       *gmd; /*  Registration Entry Database */

  L7_uint32  number_of_gmd_entries;

  L7_uint32  last_gmd_used_plus1;
  
  void *        gmrSema;

} Gmr;

/******************************************************************************
 * GMR : GARP MULTICAST REGISTRATION APPLICATION : GARP ATTRIBUTES
 ******************************************************************************
 */

typedef enum
{
  Gmr_All_attributes, Gmr_Multicast_attribute, Gmr_Legacy_attribute 
}
Gmr_Attribute_type;

typedef enum
{
  Gmr_Forward_all, Gmr_Forward_unregistered
} Gmr_Legacy_control;

#define Number_of_legacy_controls  2

typedef struct gmrpInstance_s
{
  L7_BOOL inuse;
  Gmr *gmr;
}gmrpInstance_t;

/*----------------------------------------------------------------------------*/
/* GMR : GARP MULTICAST REGISTRATION APPLICATION : CREATION, DESTRUCTION      */
/*----------------------------------------------------------------------------*/
/*********************************************************************
* @purpose  Create GMRP
*
* @param    process_id    garp process id
* @param    vlan_id       Vlan id
* @param    gmr           pointer to created gmr
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes   	Creates a new instance of GMR, allocating and initialising a control
*           block, returning True and a pointer to this instance if creation suceeds.
*           Also creates instances of MCD (the MultiCast registration Database) and
*           of GIP (which controls information propagation).
*
*           Ports are created by the system and added to GMR separately (see
*           gmr_added_port() and gmr_removed_port() below).
*
*           The operating system supplied process_id is for use in subsequent calls
*           to operating system services. The system itself ensures the temporal
*           scope of process_id, guarding against timers yet to expire for destroyed
*           processes etc. Although process_id will be implicitly supplied by many
*           if not most systems, it is made explicit in this implementation for
*           clarity.
* 
*           The vlan_id provides the context within which this instance of GMR.
*           vlan_id 0 is taken to refer to the base LAN, i.e. the LAN as seen by
*           802.1D prior to the invention of VLANs. This assumption may be subject
*           to further 802.1 discussion.
*       
* @end
*********************************************************************/
extern L7_BOOL gmr_create_gmr(L7_uint32 process_id, L7_ushort16 vlan_id, void **gmr);

/*********************************************************************
* @purpose  Destroy Gmr 
*
* @param    my_gmr          pointer to gmrp
*
* @returns  void
*
* @notes   Destroys an instance of GMR, destroying and deallocating the associated
*          instances of MCD and GIP, and any instances of GID remaining.
*
* @end
*********************************************************************/
extern void gmr_destroy_gmr(void *gmr);

/*********************************************************************
* @purpose  a port is added
*
* @param    my_gmr        pointer to gmrp
* @param    port_no       port number
*
* @returns  void
*
* @notes   The system has created a new port for this application and added it to
*          the ring of GID ports. This function should provide any management
*          initilization required for the port for legacy control or multicast
*          filtering attributes, such as might be stored in a permanent database
*          either specifically for the port or as part of a template.
*
*          Newly created ports are 'connected' for the purpose of GARP information
*          propagation using the separate function gip_connect_port(). Prior to
*          doing this the system should initialize the newly created ports with
*          any permanent database controls for specific multicast values.
*
*          It is assumed that new ports will be 'connected' correctly before the
*          application continues. The rules for connection are not encoded within
*          GMR. They depend on the relaying connectivity of the system as a whole,
*          and can be summarized as follows:
*	       if (my_gmr->vlan_id == Lan)
*	       {
*		       if stp_forwarding(port_no) gmr_connect_port(port_no);
*	       }
*	       else if vlan_forwarding(vlan_id, port_no)
*	       {
*		       gmr_connect_port(port_no);
*	       }
*
*          As the system continues to run it should invoke gmr_disconnect_port()
*          and gmr_connect_port() as required to maintain the required connectivity.
*       
* @end
*********************************************************************/
extern void gmr_added_port(void *my_gmr, L7_uint32 port_no);

/*********************************************************************
* @purpose  rocess a GMR command 
*
* @param    my_gmr        pointer to gmrp
* @param    port_no       port number
*
* @returns  void
*
* @notes   	The system has removed and destroyed the GID port. This function should
*           provide any application specific cleanup required.
*       
* @end
*********************************************************************/
extern void gmr_removed_port(void *my_gmr, L7_uint32 port_no);

/*-----------------------------------------------------------------------------*/
/* GMR : GARP MULTICAST REGISTRATION APPLICATION : JOIN, LEAVE INDICATIONS     */
/*-----------------------------------------------------------------------------*/
/*****************************************************************************
* @purpose  GMRP received a join indication.
*
* @param    my_gmr              pointer to gmrp
* @param    my_port             pointer to port structure
* @param    joining_gid_index   index
*
* @returns  void
*
* @notes  This implementation of gmr_join_indication() respects the three cases
*         described in the header file for the state of the Filtering Database and
*         the registered Legacy controls (Forward All, Forward Unregistered) and
*         Multicasts. It makes some, but not a perfect attempt, to optimize
*         calls to the Filtering Database when one Legacy mode transitions to
*         another.
*       
*         Deals with joins for both Legacy Attributes and Multicast Attributes.
*         The former are represented by the first few GID indexes, and give rise
*         to three cases:
*
*         1. Neither Forward All or Forward Unregistered are currently set (i.e.
*         registered for this port), so the Filtering Database is in
*         "filter_by_default" mode, and the only multicasts that are being
*         forwarded through (out of) this port are those "registered here".
*         In addition there may be other entries in the Filtering Database
*         whose effect on this port is currently duplicated by
*         "filter_by_default" : if an entry for a multicast is present for any
*         port, the model of the Filtering Database requires that its filter or
*         forward behavior be represented explicitly for all other ports
*         - there is no per port setting which means "behave as default" mode.
*
*         2. Forward Unregistered is currently set, but Forward All is not.
*         The Filtering Database is in "forward_by_default" mode. If a Filtering
*         Database entry has been made for a multicast for any port it specifies
*         filtering for this port if the multicast is not registered here but
*         is registered for a port to which this port is connected (in the GIP
*         sense), and forwarding otherwise (i.e. multicast registered here or
*         not registered for any port to which this port is connected).
*
*         3. Forward All is currently set and takes precedence - Forward
*         Unregistered may or may not be set. The Filtering Database is in
*         "forward_by_default" mode. If a Filtering Database entry has been
*         made for any port it specifies forwarding for this port. Not all
*         multicasts registered for this port have been entered into the
*         Filtering Database.
*
*         If a call to "fdb_filter" or "fdb_forward", made for a given port and
*         multicast address, causes a Filtering Database entry to be created, the
*         other ports are set to filter or forward for that address according to
*         the setting of "forward_by_default" or "filter_by_default" current when
*         the call is made. This behavior can be used to avoid temporary filtering
*         glitches.
*
*         This function, gmr_join_indication(), changes filtering database entries
*         for the port which gives rise to the indication alone. If another port
*         is in Legacy mode B (Forward_unregistered set (registered) for that port,
*         but not Forward_all) then registration of a multicast address on this
*         port can cause it to be filtered on that other port. This is handled by
*         gmr_join_propagated() for the other ports which may be effected. It will
*         be called as a consequence of the GIP propagation of the newly registered
*         attribute (multicast address).
*       
* @end
*********************************************************************/
extern void gmr_join_indication(void *my_gmr, void *my_port,
                                unsigned joining_gid_index);
/*********************************************************************
* @purpose  propagate join
*
* @param    my_gmr              pointer to gmrp
* @param    my_port             pointer to port structure
* @param    joining_gid_index   index
*
* @returns  viod
*
* @notes   
*
*       
* @end
*********************************************************************/
extern void gmr_join_propagated(void *my_gmr, void *my_port, L7_uint32 joining_gid_index);


/*********************************************************************
* @purpose  Leave indication is received.
*
* @param    my_gmr              pointer to gmrp
* @param    my_port             pointer to port structure
* @param    joining_gid_index   index
*
* @returns  void
*
* @notes   
*       
* @end
*********************************************************************/
extern void gmr_leave_indication(void *my_gmr, void *my_port, L7_uint32 leaving_gid_index);

/*********************************************************************
* @purpose  Propagate Leave  
*
* @param    my_gmr              pointer to gmrp
* @param    my_port             pointer to port structure
* @param    joining_gid_index   index
*
* @returns  void
*
* @notes   
*       
* @end
*********************************************************************/
void gmr_leave_propagated(void *my_gmr, void *my_port, L7_uint32 leaving_gid_index);

/*-----------------------------------------------------------------------------*/
/* GMR : GARP MULTICAST REGISTRATION APPLICATION : PROTOCOL & MGT EVENTS       */
/*-----------------------------------------------------------------------------*/

/*********************************************************************
* @purpose  Process an entire received pdu for this instance of GMR.
*
* @param    my_gmr              pointer to gmrp
* @param    my_port             pointer to port structure
* @param    pdu                 pointer to received pdu
*
* @returns  void
*
* @notes   	Process an entire received pdu for this instance of GMR: initialise
*           the Gmf pdu parsing routine, and, while messages last, read and process
*           them one at a time.
*       
* @end
*********************************************************************/
extern void gmr_rcv(void *my_gmr, L7_uint32 my_port, void *pdu);

/*********************************************************************
* @purpose  Transmit a pdu for this instance of GMR.
*
* @param    my_gmr              pointer to gmrp
* @param    gid_index           index in the gmrp DB
*
* @returns  void
*
* @notes   	Get and prepare a pdu for the transmission, if one is not available
*           simply return, if there is more to transmit GID will reschedule a call
*           to this function.
*
*           Get messages to transmit from GID and pack them into the pdu using Gmf
*           (MultiCast pdu Formatter).
*       
* @end
*********************************************************************/
extern void gmr_tx(void *my_gmr, void *my_port);

/*********************************************************************
* @purpose  Process a GMR command 
*
* @param    void*         pointer to command
*
* @returns  event
*
* @notes   
*
*       
* @end
*********************************************************************/
extern L7_BOOL gmr_proc_command(void *command);

/*********************************************************************
* @purpose  Remove the GMRP Entry for the MFDB
*
* @param    void *            pointer to DB
*
* @returns  None
*
* @notes   Removes the GMRP Entry from the MFDB
*       
* @end
*********************************************************************/
extern void gmr_remove_gmrp_entry(void *gmd);

#endif
