/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    gidapi.h
* @purpose     GID internal api to GARP applications
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
#ifndef INCLUDE_GID_API_H_
#define INCLUDE_GID_API_H_

#include "l7_common.h"
#include "defaultconfig.h"
#include "osapi.h"
#include "garp.h"
#include "garpcfg.h"
#include "gvrapi.h"
#include "gmrapi.h"
#include "garp_timer.h"

/******************************************************************************
 * GID : GARP INFORMATION DISTRIBUTION PROTOCOL : OVERVIEW
 ******************************************************************************
 *
 * In this reference implementation there is a single instance of GID, the
 * GARP Information Distribution protocol per application instance per
 * physical or logical port in the system (the base spanning tree operates
 * over physical ports, VLAN registration operates over physical ports to
 * offer one (or zero) logical ports per physical port per VLAN, an instance
 * of the Multicast registration operates either over physical ports or over
 * the logical ports of a VLAN depending on whether the registration is within
 * the scope of a VLAN or not).
 *
 * This single GID instance operates a number of GID machines, one for each
 * attribute of interest to the application instance (an attribute is the
 * smallest unit which can be registered by GARP, e.g. a single multicast
 * address - join and leave indications occur for individual attributes).
 * GID knows nothing of the sematics of individual attributes - it is only
 * interested in the state of the GID machine for each attribute and the
 * GID events which change that state and give rise to further protocol
 * events. Each attribute is identified by its GID index, which in this
 * implementation is a simple index into an array of GID machines. An
 * attribute's GID index is the same for every port belonging to an application
 * instance.
 *
 * The point of operating a single GID instance instead of completely separate
 * machines is to allow there to be a single set of GID timers per port, and
 * to facilitate easy packing of messages for individual attributes into a
 * single PDU.
 */

/******************************************************************************
 * GID : GARP INFORMATION DISTRIBUTION PROTOCOL : GID MACHINES
 ******************************************************************************
 */



typedef struct /* Gid_machine */
{
  /* The GID state of each attribute on each port is held in a GID 'machine'
 * which comprises the Applicant and Registrar states for the port,
 * including control modifiers for the states.
 *
 * The GID machine and its internal representation of GID states is not
 * accessed directly : this struct is defined here to allow the GID
 * Control Block (which is accessed externally), to be defined below.
   */

  L7_uchar8 applicant;      /* : Applicant_states */

  L7_uchar8 registrar;      /* : Registrar_states */

  L7_BOOL   incoming_port;  /* : is this the port received this attribute */

} Gid_machine;
 
/******************************************************************************
 * GID : GARP INFORMATION DISTRIBUTION PROTOCOL : PROTOCOL EVENTS
 ******************************************************************************
 *
 * A complete set of events is specified to allow events to be passed around
 * without rewriting. The other end of the spectrum would have been one
 * set of events for received messages, a set of receive indications, a
 * set of user requests etc. and separate transition tables for each.
 *
 * Event definitions are ordered in an attempt not to waste space in
 * transition tables and to pack switch cases together.
 */
typedef enum /* Gid_event */
{

  Gid_null, Gid_rcv_leaveempty, Gid_rcv_leavein, Gid_rcv_empty,
  Gid_rcv_joinempty, Gid_rcv_joinin,
  Gid_join, Gid_leave,
  Gid_normal_operation, Gid_no_protocol,
  Gid_normal_registration, Gid_fix_registration, Gid_forbid_registration,
  Gid_rcv_leaveall, Gid_rcv_leaveall_range,
  Gid_tx_leaveempty, Gid_tx_leavein, Gid_tx_empty, Gid_tx_joinempty,
  Gid_tx_joinin, Gid_tx_leaveall, Gid_tx_leaveall_range
} Gid_event;

enum
{
  Number_of_gid_rcv_events  = (Gid_rcv_joinin + 1),
  Number_of_gid_req_events  = 2,
  Number_of_gid_amgt_events = 2,
  Number_of_gid_rmgt_events = 3,
  Number_of_gid_leaveall_events = 2,
  Number_of_gid_tx_events   = 7
};

/******************************************************************************
 * GID : GARP INFORMATION DISTRIBUTION PROTOCOL : PROTOCOL TIMER VALUES
 ******************************************************************************
 *
 * Describe goals of timers, subsecond response times etc.
 */
typedef L7_uint32 milliseconds;

/* All the values are in milliseconds */
#define Gid_schedule_time            10

#if (L7_MAX_VLANS > L7_MAX_GROUP_REGISTRATION_ENTRIES)
#define MAX_TO_BE_DELETED L7_MAX_VLANS
#else
#define MAX_TO_BE_DELETED L7_MAX_GROUP_REGISTRATION_ENTRIES
#endif
enum
{
  Max_vlans = L7_MAX_VLANS
};
enum
{
  Number_of_gid_machines = Max_vlans+2
};
#define Max_multicasts               L7_MAX_GROUP_REGISTRATION_ENTRIES
#define Gmr_Number_of_gid_machines   (Number_of_legacy_controls + Max_multicasts)
#define Gid_max_to_be_deleted        max((Number_of_gid_machines), (Gmr_Number_of_gid_machines))


/******************************************************************************
 * GID : GARP INFORMATION DISTRIBUTION PROTOCOL : PROTOCOL INSTANCES
 ******************************************************************************
 */

typedef struct /* Gid */
{
  /* Each instance of GID is represented by one of these control blocks.
   * There is a single instance of GID per port for each GARP application.
   * The control block's are linked together through the next_in_port_ring
   * pointer to form a complete ring.
   *
   * Each control block contains a pointer to the GARP control block
   * representing the application instance which specifies the application's
   * join, leave, transmit, and receive functions, and its process identifier
   * (for identifying the application instance to the rest of the system
   * including timer functions.
   *
   * The port number associated with this instance of GID is specified.
   * The GID control block's for all of the ports of an application instance
   * are linked together through the next_in_port_ring pointer to form a
   * complete ring. Ports which are "connected", e.g. are all in a spanning
   * tree forwarding state, are linked through the next_in_connected_ring
   * pointer. The is_connected flag is also set for these ports, and
   * handles the case of a single connected port (is_connected is true (set)
   * only if is_enabled is also true. The GID control block definition is
   * shared to allow GIP to traverse these fields.
   *
   * GID processing for the port as a whole may be enabled or disabled : the
   * current state is recorded here in case received pdus or other events
   * are pending in the system.
   *
   * Standard GID operates as if the GID entity were connected to a shared
   * medium. GARP information may be transported more quickly if the link
   * is known to be point to point. Specifically received Leave messages
   * can give rise to immediate Leave indications, without the need to
   * solict futher Joins from other potential members attached to the shared
   * medium. Note: Mechanisms to determine if the link is point to point
   * have not yet been developed. I suggest we puruse this with 802.3.
   *
   * The control block provides a 'scratchpad' for recording actions
   * arising from gid machine processing during this invocation of GID
   * ('invocation' meaning when GARP runs, e.g. when a received packet is
   * being processed). After each invocation gid_do_actions() is called
   * to schedule a transmission immediately, start the join timer (if
   * not already started), or start the leave timer (again if not already
   * started) depending on the setting of cschedule_tx_now,
   * cstart_join_timer, and cstart_leave_timer, and whether these timers
   * have already been started (or immediate scheduling requested) as
   * recorded in tx_now_scheduled, join_timer_running, and
   * leave_timer_running. If hold_tx is true scheduling and starting timers
   * is held pending expiry of the hold timer.
   *
   * Timeout values for the join, leave, hold, and leaveall timers are
   * recorded here to allow them to be managed according to the media type
   * and speed, and whether the port attaches to point to point switch to
   * switch link, to shared media, or through a non-GARP aware switch to
   * switches of possibly varying link speeds.
   *
   * The leave time for an individual GID machine comprises three to four
   * expirations of the leave_timeout_4. This gives sufficient granularity
   * for the leave timer to protect against premature expiration (while
   * another GID participant may be preparing to send a join) without making
   * the worst case leave time overlong. It is also sufficiently coarse to
   * allow the leave timer state for each entry to be easily stored within
   * the Registrar state.
   *
   * The leaveall timeout is implemented as leaveall_countdown expirations
   * of leaveall_timeout_n. This supports suppression of Leaveall generation
   * by this machine when a Leaveall has been received, without requiring
   * the operating system to support cancelling or restarting of timers.
   * When leaveall_countdown reaches zero, the join timer is started (if not
   * already running). Whenever the join timer expires the application's
   * transmit function is invoked, which will lead to a call to gid_next_tx,
   * which in turn will return Gid_tx_leaveall. This ensure that Leaveall's
   * are transmitted at the beginning of PDU's which contain the results of
   * the local Leaveall processing - such as immediate rejoins. This is a
   * good idea for protocol robustness in the face of receiver packet loss
   * - the rejoins are only lost if the Leaveall itself is lost - and it
   * minimizes the number of pdu's sent.
   *
   * This GID control block points to an array of Gid machines allocated
   * when this instance of GID is created. It supports more GID attributes
   * than can be packed by an application formatter into a single pdu
   * (required for most simple encodings of 4096 VLANs). The tx_pending
   * flag indicates that some of the GID machines between last_transmitted
   * and last_to_transmit indices probably have messages to send. The
   * application state prior to message generation of the last_transmitted
   * machine is stored in the GID machine indexed by untransmit_machine -
   * space for this is reserved at the very end of the GID machine array,
   * which is one larger than would otherwise be required to store the
   * maximum number of attributes specified at GID creation. This allows
   * the implementation of a simple untransmit function, like the C
   * library function ungetc().
   */
  Garp        *application;

  L7_uint32   port_no;

  void        *next_in_port_ring;

  void        *next_in_connected_ring;

  L7_BOOL     is_enabled;
  L7_BOOL     is_connected;
  L7_BOOL     cschedule_tx_now;
  L7_BOOL     cstart_join_timer;
  L7_BOOL     cstart_leave_timer;
  L7_BOOL     join_timer_running;
  L7_BOOL     leave_timer_running;
  L7_BOOL     tx_pending;
  L7_BOOL     transmit_leaveall;

  L7_uint32   join_timeout;
  L7_uint32   leave_timeout;
  L7_uint32   leaveall_timeout;
  L7_uint32   gmrp_leaveall_left;

  L7_BOOL     vlanCreationForbid;
  L7_BOOL     registrationForbid;

  Gid_machine *machines;

  L7_uint32   last_transmitted;
  L7_uint32   untransmit_machine;

  garpTimerDescr_t join_timer_instance;
  garpTimerDescr_t leave_timer_instance;

} Gid;

/******************************************************************************
 * GID : GARP INFORMATION DISTRIBUTION PROTOCOL : CREATION, DESTRUCTION &c.
 *****************************************************************************/

/*********************************************************************
* @purpose  create gid port.
*
* @param    Garp       application
* @param    L7_uint32   port no
*
* @returns  L7_FALSE
* @returns  L7_TRUE
*
* @notes	Creates a new instance of GID, allocating space for GID machines as
*           required by the application, adding the port to the ring of ports
*           for the application and signalling the application that the new port
*           has been created.
*
*           On creation each GID machine is set to operate as Normal or with
*           No_protocol!!
*
*          The port is enabled when created, but not connected (see GIP).
*       
* @end
*********************************************************************/
extern L7_BOOL gid_create_port(Garp *application, L7_uint32 port_no);


/*********************************************************************
* @purpose  destroy gid port.
*
* @param    Garp       application
* @param    L7_uint32   port no
*
* @returns  L7_FALSE
* @returns  L7_TRUE
*
* @notes   Destroys the instance of GID, disconnecting the port if it is still
*          connected (causing leaves to propagate as required), then causing
*          leave indications for this port as required, finally releasing all
*          allocated space signalling the application that the port has been
*          removed.
*
*       
* @end
*********************************************************************/
extern L7_BOOL gid_destroy_port(Garp *application, L7_uint32 port_no);

/******************************************************************************
 * GID : GARP INFORMATION DISTRIBUTION PROTOCOL : USEFUL FUNCTIONS
 ******************************************************************************
 */

/*********************************************************************
* @purpose  find gid port.
*
* @param    Garp        application
* @param    L7_uint32   port no
* @param    void*       pointer to found port
*
* @returns  L7_FALSE
* @returns  L7_TRUE
*
* @notes   Finds the GID instance for port number port_no.
*       
* @end
*********************************************************************/
extern L7_BOOL gid_find_port(Gid *first_port, L7_uint32 port_no, void **gid);

/*********************************************************************
* @purpose  find next gid port.
*
* @param    Gid         pointer to application
*
* @returns  Gid         pointer to next Gid
*
* @notes   Finds the next port in the ring of ports for this application.
*       
* @end
*********************************************************************/
extern Gid *gid_next_port(Gid *this_port);


/*********************************************************************
* @purpose  Finds an unused GID machine.
*
* @param    Garp        application
* @param    L7_uint32   start from index
* @param    L7_uint32*  found index
*
* @returns  L7_FALSE
* @returns  L7_TRUE
*
* @notes   	Finds an unused GID machine (i.e. one with an Empty registrar and a
*           Very Anxious Observer applicant) starting the search at GID index
*           from_index, and searching to gid_last_used.
*       
* @end
*********************************************************************/
extern L7_BOOL gid_find_unused(Garp *application, L7_uint32 from_index,
                               L7_uint32 *found_index);


/******************************************************************************
 * GID : GARP INFORMATION DISTRIBUTION PROTOCOL : MGT
 ******************************************************************************
 */

/*********************************************************************
* @purpose  Changes the attribute's management state.
*
* @param    Gid          port
* @param    L7_uint32    index
* @param    Gid_event    event
* @param    Gid_event    tx_event
*
* @returns  none
*
* @notes   Changes the attribute's management state on my_port. The directive
*          can be Gid_normal_operation, Gid_no_protocol, Gid_normal_registration,
*          Gid_fix_registration, or Gid_forbid_registration. If the change in
*          management state causes a leave indication this is sent to the user
*          and propagated to other ports.
*       
* @end
*********************************************************************/
extern void gid_manage_attribute(Gid *my_port, L7_uint32 index,
                                 Gid_event directive,Gid_event tx_event);


/******************************************************************************
 * GID : GARP INFORMATION DISTRIBUTION PROTOCOL : EVENT PROCESSSING
 ******************************************************************************
 */


/*********************************************************************
* @purpose  receive message.
*
* @param    Gid          port
* @param    L7_uint32    index
* @param    Gid_event    message
*
* @returns  none
*
* @notes   Only for Gid_rcv_leave, Gid_rcv_empty, Gid_rcv_joinempty, Gid_rcv_joinin.
*          See gid_rcv_leaveall for Gid_rcv_leaveall, Gid_rcv_leaveall_range.
*
*          Joinin and JoinEmpty may cause Join indications, this function calls
*          GIP to propagate these.propagated
*
*          On a shared medium, Leave and Empty will not give rise to indications
*          immediately. However this routine does test for and propagate
*          Leave indications so that it can be used unchanged with a point to point
*          protocol enhancement.
*       
* @end
*********************************************************************/
extern void gid_rcv_msg(Gid *my_port, L7_uint32 gid_index, Gid_event msg);
extern void gid_gmr_rcv_msg(Gmr *my_gmr, Gid *my_port, L7_uint32 index, Gid_event msg);



/*********************************************************************
* @purpose  join request is received.
*
* @param    Gid          port
* @param    L7_uint32    index
*
* @returns  none
*
* @notes   can be called multiple times with no ill effect.
*
* @end
*********************************************************************/
extern void gid_join_request(Gid *my_port, L7_uint32 gid_index);

/*********************************************************************
* @purpose  Leave request is received.
*
* @param    Gid          port
* @param    L7_uint32    index
*
* @returns  none
*
* @notes   can be called multiple times with no ill effect.
*
* @end
*********************************************************************/
extern void gid_leave_request(Gid *my_port, L7_uint32 gid_index);


/*********************************************************************
* @purpose  Leave request is received.
*
* @param    Garp          application
* @param    L7_uint32     port number
* @param    void *        pointer to pdu
*
* @returns  none
*
* @notes   
*
* @end
*********************************************************************/
void gid_rcv_pdu(Garp *application, L7_uint32 port_no, void *pdu);


/*********************************************************************
* @purpose  Leave all request is received.
*
* @param    Gid          port
*
* @returns  none
*
* @notes   
*
* @end
*********************************************************************/
extern void gid_rcv_leaveall(Gid *my_port);
extern void gid_gmr_rcv_leaveall(Gmr *my_gmr, Gid *my_port);

/*********************************************************************
* @purpose  check if port is registered.
*
* @param    Gid          port
* @param    L7_uint32    index
*
* @returns  none
*
* @notes    Returns True if the Registrar is not Empty, or if 
*           Registration is fixed.
*
* @end
*********************************************************************/
extern L7_BOOL gid_registered_here(Gid *my_port, L7_uint32 gid_index);

/******************************************************************************
 * GID : GARP INFORMATION DISTRIBUTION PROTOCOL : TIMER PROCESSING
 ******************************************************************************
 */

/*********************************************************************
* @purpose  gid starting timers.
*
* @param    Gid          port
*
* @returns  none
*
* @notes  Carries out 'scratchpad' actions accumulated in this invocation of GID,
*         and outstanding 'immediate' transmissions and join timer starts which
*         have been delayed by the operation of the hold timer.
*
* @end
*********************************************************************/
void gid_do_actions(Gid *my_port);

/*********************************************************************
* @purpose  leave timer expiry function.
*
* @param    Garp          application
* @param    L7_uint32     port number
*
* @returns  none
*
* @notes   
*
* @end
*********************************************************************/
extern void gid_leave_timer_expired(L7_uint32 port_no);
extern void gid_gmr_leave_timer_expired(L7_uint32 vid, L7_uint32 port_no);

/*********************************************************************
* @purpose  Join timer expiry function.
*
* @param    Garp          application
* @param    L7_uint32     port number
*
* @returns  none
*
* @notes   
*
* @end
*********************************************************************/
extern void gid_join_timer_expired(L7_uint32 port_no);
extern void gid_gmr_join_timer_expired(L7_uint32 vid, L7_uint32 port_no);

/******************************************************************************
 * GID : GARP INFORMATION DISTRIBUTION PROTOCOL : TRANSMIT PROCESSSING
 ******************************************************************************
 */

/*********************************************************************
* @purpose  Gid next transmit
*
* @param    Gid           port
* @param    L7_uint32     port number
*
* @returns  event to be transmitted
*
* @notes    Scan the GID machines for messsages that require transmission.
*           If there is no message to transmit return Gid_null, otherwise
*           return the message as a Gid_event.
*
*           If message transmission is currently held (pending expiry of a hold
*           timer and a call to gid_release_tx()) this function will return Gid_null
*           so it may be safely called if that is convenient.
*
*           Supports sets of GID machines that can generate more messages than
*           can fit in a single application PDU. This allows this implementation
*           to support, for example, GARP registration for all 4096 VLAN identifiers.
*
*           To support the application's packing of messages into a single PDU
*           without the detailed knowledge of frame format and message encodings
*           required to tell whether a message will fit, and to avoid the application
*           having to make two calls to GID for every message - one to get the
*           message and one to say it has been taken - this routine supports the
*           gid_untx() function. This is conceptually similar to the C library
*           ungetc() function. It restores the applicant state of the last machine
*           from which a message was taken, and establishes that machine as the next
*           one from which a message should be taken - effectively pushing the
*           message back into the set of GID machines. It should only be used
*           within a single invocation of GID, otherwise intervening events and their
*           consequent state changes may be lost with unspecified results.
*
* @end
*********************************************************************/
extern Gid_event gid_next_tx(Gid *my_port, Gvr *my_gvr, L7_uint32 *gid_index);
extern Gid_event gid_gmr_next_tx(Gmr *my_gmr, Gid *my_port, L7_uint32 *index);


/*********************************************************************
* @purpose  .
*
* @param    Gid          port
*
* @returns  none
*
* @notes   See description for gid_next_tx.
*
* @end
*********************************************************************/
extern void gid_untx(Gid *my_port);

#endif

