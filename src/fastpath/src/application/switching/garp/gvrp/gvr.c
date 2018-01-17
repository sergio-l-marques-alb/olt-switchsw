/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    gvr.c
* @purpose     GVRP API definitions and functions
* @component   GARP
* @comments    none
* @create      02/23/2001
* @author      Hassan
* @author
* @end
*
**********************************************************************/

#include "string.h"
#include "l7_common.h"
#include "usmdb_sim_api.h"
#include "defaultconfig.h"
#include "osapi.h"
#include "sysapi.h"
#include "log.h"
#include "nimapi.h"
#include "dtlapi.h"
#include "dot1q_api.h"
#include "garpapi.h"
#include "garpctlblk.h"
#include "gidapi.h"
#include "gvrapi.h"
#include "garpcfg.h"
#include "gipapi.h"
#include "gvd.h"
#include "gvf.h"
#include "dot1dgarp.h"

#include "dot1s_api.h"
#include "gidtt.h"
#include "garp_debug.h"

enum
{
  Unused_index = Number_of_gid_machines
};

static L7_uchar8 tx_pdu[GARP_MAX_PDU_SIZE];

#define GVRP_MSG_SIZE    4
extern gvrpInfo_t *gvrpInfo;
extern void setMachineLeaveIn(Gid_machine *machine);
extern GARPCBptr GARPCB;
extern Gvr *gvrp_app;

static const L7_uchar8 Garp_gvrp_mac_address[6] =
{
  0x01, 0x80, 0xc2, 0x00, 0x00, 0x21
};

/******************************************************************************
 * GVR : GARP VLAN REGISTRATION APPLICATION : CREATION, DESTRUCTION
 ******************************************************************************
 */


static L7_RC_t gvrpUpdateStats(L7_uint32 event, L7_uint32 port_no)
{


  switch(event)
  {
    /* Tx Events*/
    case Gid_tx_joinempty:
      gvrpInfo->garpStats[port_no].sJE++;
      break;
    case Gid_tx_joinin:
      gvrpInfo->garpStats[port_no].sJIn++;
      break;
    case Gid_tx_empty:
      gvrpInfo->garpStats[port_no].sEmp++;
      break;
    case Gid_tx_leaveempty:
      gvrpInfo->garpStats[port_no].sLE++;
      break;
    case Gid_tx_leavein:
      gvrpInfo->garpStats[port_no].sLIn++;
      break;
    case Gid_tx_leaveall:
      gvrpInfo->garpStats[port_no].sLA++;
      break;
    /* RX events*/
    case Gid_rcv_joinempty:
      gvrpInfo->garpStats[port_no].rJE++;
      break;
    case Gid_rcv_joinin:
      gvrpInfo->garpStats[port_no].rJIn++;
      break;
    case Gid_rcv_empty:
      gvrpInfo->garpStats[port_no].rEmp++;
      break;
    case Gid_rcv_leaveempty:
      gvrpInfo->garpStats[port_no].rLE++;
      break;
    case Gid_rcv_leavein:
      gvrpInfo->garpStats[port_no].rLIn++;
      break;
    case Gid_rcv_leaveall:
      gvrpInfo->garpStats[port_no].rLA++;
      break;
    default: /* do nothing for others */
      break;
  }

  return L7_SUCCESS;
}

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
L7_BOOL gvr_create_gvr(L7_uint32 process_id, void **gvr)
{
  Gvr *my_gvr = (Gvr *)osapiMalloc(L7_GARP_COMPONENT_ID, (L7_uint32)sizeof(Gvr));
  L7_BOOL returnVal = L7_FALSE;

  if (my_gvr != L7_NULL)
  {
    my_gvr->g.process_id   = (L7_int32)process_id;
    my_gvr->g.gid          = L7_NULL;

    if (gip_create_gip(Number_of_gid_machines, &my_gvr->g.gip)==L7_TRUE)
    {
      my_gvr->g.max_gid_index = Number_of_gid_machines - 1;
      my_gvr->g.last_gid_used = 0;

      my_gvr->g.join_indication_fn  = gvr_join_indication;
      my_gvr->g.leave_indication_fn = gvr_leave_indication;
      my_gvr->g.join_propagated_fn  = gvr_join_leave_propagated;
      my_gvr->g.leave_propagated_fn = gvr_join_leave_propagated;
      my_gvr->g.transmit_fn         = gvr_tx;
      my_gvr->g.added_port_fn       = gvr_added_port;
      my_gvr->g.removed_port_fn     = gvr_removed_port;

      if (gvd_create_gvd(&my_gvr->gvd) == L7_TRUE)
      {
        my_gvr->number_of_gvd_entries = L7_MAX_VLANS;
        my_gvr->last_gvd_used_plus1   = 0;
        *gvr = my_gvr;

        my_gvr->g.app = GARP_GVRP_APP;
        returnVal = L7_TRUE;
      }
      else
      {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GARP_COMPONENT_ID,
                "GVRP: Failed to Create GVR DB.\n");
        gip_destroy_gip(my_gvr->g.gip);
        osapiFree(L7_GARP_COMPONENT_ID, my_gvr);
      }
    }
    else
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GARP_COMPONENT_ID,
              "GVRP: Failed to Create GIP.\n");
      osapiFree(L7_GARP_COMPONENT_ID, my_gvr);
    }
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GARP_COMPONENT_ID,
            "GVRP: Failed to Create GVR.\n");

  }

  return returnVal;
}


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
void gvr_destroy_gvr(Gvr *my_gvr)
{
  L7_BOOL rcb;
  L7_RC_t rc;
  Gid *my_port;

  gip_destroy_gip(my_gvr->g.gip);

  while ((my_port = my_gvr->g.gid) != L7_NULL)
  {
    rcb = gid_destroy_port(&my_gvr->g, my_port->port_no);

    /* delete the Vlan */
    rc = dot1qVlanDelete(my_gvr->vlan_id, DOT1Q_GVRP);
  }

  gvd_destroy_gvd();
}


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
void gvr_added_port(void *my_gvr, L7_uint32 port_no)
{  /*
  * Query the Permanent Database for Static VLAN Entries with "Registration
  * Forbidden" or "Registration Fixed" for this port. Repeat the following
  * steps until there are no more entries to be found.
  *
  * Check that the VLAN ID is represented in VLD. If not create it, and
  * create GID machines for all the other ports with control state "Normal
  * Registration" and create the GID machine for this port. Change the
  * control state for this port's GID machine to forbidden or fixed as
  * required.
  *
  */
  L7_RC_t rc;

  /* Tell the hardware to enable this port for GVRP. */
  rc = dtlDot1qGVRPEnable(port_no);

}


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
void gvr_removed_port(void *my_gvr, L7_uint32 port_no)
{  /*
  * Provide any GVR specific cleanup or management alert functions for the
  * removed port.
  */
  L7_RC_t rc;

  /* Tell the hardware to disable this port for GVRP. */
  rc = dtlDot1qGVRPDisable(port_no);

}


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
void gvr_join_indication(void *my_gvr, void *my_port, L7_uint32 gid_index)
{  /*
  *  This is a second release functionality
  */
  L7_BOOL      rcb;
  L7_RC_t      rc;
  Vlan_id      key;
  Gid          *local_port = (Gid*)my_port;

  rcb = gvd_get_key(gid_index, &key);

  GARP_TRACE_PROTOCOL_EVENTS("gvr_join_indication: vlan %d, port %d\n",
                             key, local_port->port_no );



  rc = dot1qVlanMemberSet(key,local_port->port_no,
                            L7_DOT1Q_FIXED,DOT1Q_GVRP,DOT1Q_SWPORT_MODE_NONE);


}


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
void gvr_join_leave_propagated(void *my_gvr, void *my_port, L7_uint32 gid_index)
{  /*
  * Nothing to be done since, unlike GMR with its Forward All Unregistered
  * port mode, a join indication on one port does not cause filtering to be
  * instantiated on another.
  */
}


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
void gvr_leave_indication(void *my_gvr, void *my_port, L7_uint32 leaving_gid_index)
{  /*
  *  This is a second release functionality
  */
  L7_RC_t       rc;
  L7_BOOL       rcb;
  L7_uint32     j;
  Vlan_id       key;
  Gid           *to_port;
  L7_BOOL       morePorts = L7_FALSE;
  Gid           *local_port = (Gid*)my_port;
  Gvr           *local_gvr = (Gvr*)my_gvr;
  L7_uint32     type = 0;
  L7_uint32     mode = 0;

  rcb = gvd_get_key(leaving_gid_index, &key);

  GARP_TRACE_PROTOCOL_EVENTS("gvr_leave_indication: vlan %d, port %d\n",
                             key, local_port->port_no );

  /* tell Vlan to remove this VLAN */
  if (local_port->machines[leaving_gid_index].incoming_port == L7_TRUE)
  {
    to_port = local_port;

    /* find if this attribute is advertised over more than one port */
    do
    {
      if (to_port->machines[leaving_gid_index].incoming_port == L7_TRUE)
      {
        if (to_port != local_port)
        {
          /* yes, its registered over more than one port */
          morePorts = L7_TRUE;
          break;
        }
      }

      /* advance to next in the ring */
      to_port = to_port->next_in_connected_ring;

    } while (to_port != local_port);



    rc = dot1qVlanMemberAndTypeGet(key, local_port->port_no, &mode, &type);

    if ((L7_DOT1Q_ADMIN == type) &&
        (L7_DOT1Q_NORMAL_REGISTRATION == mode) && (L7_SUCCESS == rc))
    {
        /* This will execute in case a static vlan was created
         * (<vlan database>, <vlan vid)> commands), and then
         * JoinIn/JoinEmpty PDU for the same VID was received, and
         * now the dynamic registration has expired.
         * We have to call dot1qVlanMemberSet() to change the
         * status of this_port->port_no to L7_DOT1Q_FORBIDDEN, so that
         * this port be shown as "excluded"
         * by <show vlan vid> command */
        dot1qVlanMemberSet(key, local_port->port_no,
                           L7_DOT1Q_FORBIDDEN, DOT1Q_GVRP,DOT1Q_SWPORT_MODE_NONE);

        /* also, it is important not to remove the static VLAN from
         * the GVRP database; we want the static VLAN to be declared
         * with JoinEmpty PDUs after expiration of LeaveAll timers --
         * that is why we set morePorts to L7_TRUE */
        morePorts = L7_TRUE;
    }

    if ((rc==L7_SUCCESS) &&
           ((L7_DOT1Q_DOT1X_REGISTERED==type)&&(mode==L7_DOT1Q_FIXED)))
    {
        dot1qVlanMemberSet(key, local_port->port_no,
                           L7_DOT1Q_FORBIDDEN, DOT1Q_GVRP,DOT1Q_SWPORT_MODE_NONE);
    }
 

    /* delete the attribute if its not registered over more than one port */
    /* and there is only one port left in the ring                        */
    if (L7_FALSE == morePorts)
    {
      to_port = local_port;
      do
      {
        for (j = leaving_gid_index ; j <= (L7_uint32)local_gvr->g.last_gid_used ; j++)
        {
          to_port->machines[j].applicant = to_port->machines[j+1].applicant;
          to_port->machines[j].registrar = to_port->machines[j+1].registrar;
          to_port->machines[j].incoming_port = to_port->machines[j+1].incoming_port;
          to_port->application->gip[j] = to_port->application->gip[j+1];
        }

        if ((to_port->last_transmitted >= leaving_gid_index) &&
            (to_port->last_transmitted > 0))
        {
          to_port->last_transmitted--;
        }

        to_port = to_port->next_in_connected_ring;
      } while (to_port != local_port);

      if (to_port->application->last_gid_used > 0)
      {
        to_port->application->last_gid_used--;
      }

      if (local_gvr->last_gvd_used_plus1 > 0)
      {
        local_gvr->last_gvd_used_plus1--;
      }

      /* delete the Vlan */
      rc = dot1qVlanDelete((L7_uint32)key, DOT1Q_GVRP);

      /* delete the attribute from Database */
      rcb = gvd_delete_entry(leaving_gid_index);

    } /* if (L7_FALSE == morePorts) */
    else
    {
      to_port = local_port;

      to_port->machines[leaving_gid_index].incoming_port = L7_FALSE;

      /* the registration on to_port has expired, so set Registrar to Mt (Empty).
       * Don't change the Applicant (which is in Qa state most likely), since
       * other port(s) still has/have this attribute registered, and the Applicant
       * should send JoinEmpty messages from the current port
       * upon Leave All events. */
      to_port->machines[leaving_gid_index].registrar     = Mt;

      /* remove from vlan */
      if ((rc==L7_SUCCESS) &&
          (((L7_DOT1Q_DYNAMIC==type)&&(mode==L7_DOT1Q_FIXED))||
          ((L7_DOT1Q_ADMIN==type) && (mode==L7_DOT1Q_NORMAL_REGISTRATION))))
          /* this check is to determine if vlan is static , but port has been added
             to vlan dynamically */
      {
        dot1qVlanMemberSet((L7_uint32)key,to_port->port_no,
                               L7_DOT1Q_NORMAL_REGISTRATION, DOT1Q_GVRP,DOT1Q_SWPORT_MODE_NONE);
      }
    }
  }

}

/******************************************************************************
 * GVR : GARP VLAN REGISTRATION APPLICATION : RECEIVE MESSAGE PROCESSING
 ******************************************************************************
 */

/*********************************************************************
* @purpose  Place holder for management alert functions indicating
*           registrations for more VLANs have been received than can
*           be accepted.
*
* @param
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
static void gvr_db_full(Gvr *my_gvr, Gid *my_port)
{
  /*
  * Place holder for management alert functions indicating registrations
  * for more VLANs have been received than can be accepted.
  */

  L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GARP_COMPONENT_ID,
          "GVRP: database is full.\n");
}

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
static void gvr_rcv_msg(Gvr *my_gvr, Gid *my_port, Gvf_msg *msg)
{  /*
  * Process one received message.
  *
  * A LeaveAll message never causes an indication (join or leave directly),
  * even for the point to point link protocol enhancements (where an
  * ordinary Leave does). No further work is needed here.
  *
  * A LeaveAllRange message is currently treated exactly as a LeaveAll
  * (i.e. the range is ignored).
  *
  * All the remaining messages refer to a single attribute (i.e. a single
  * registered VLAN). Try to find a matching entry in the gvd database.
  * If one is found dispatch the message to a routine which will
  * handle both the local GID effects and the GIP propagation to other ports.
  *
  * If no entry is found Leave and Empty messages can be discarded, but
  * JoinIn and JoinEmpty messages demand further treatment. First, an attempt
  * is made to create a new entry using free space (in the database, which
  * corresponds to a free GID machine set). If this fails an attempt may be
  * made to recover space from a machine set which is in an unused or less
  * significant state. Finally the database is consider full and the received
  * message is discarded.
  *
  * Once (if) an entry is found, Leave, Empty, JoinIn, and JoinEmpty are
  * all submitted to GID (gid_rcv_msg()) which will generate and propagate
  * Join or Leave indications as necessary.
  *
  * JoinIn and JoinEmpty may cause Join indications, which are then propagated
  * by GIP.
  *
  * On a shared medium, Leave and Empty will not give rise to indications
  * immediately. However this routine does test for and propagate
  * Leave indications so that it can be used unchanged with a point to point
  * protocol enhancement.
  *
  */
  L7_BOOL   rcb;
  L7_uint32 gid_index  = Unused_index;

  if ((msg->event == Gid_rcv_leaveall) || (msg->event == Gid_rcv_leaveall_range))
  {
    gid_rcv_leaveall(my_port);
  }
  else
  {
    if (!gvd_find_entry(msg->key1, &gid_index))
    {
      if ((msg->event == Gid_rcv_joinin) ||
          (msg->event == Gid_rcv_joinempty) ||
          (msg->event == Gid_normal_registration) ||
          (msg->event == Gid_fix_registration) ||
          (msg->event == Gid_forbid_registration))
      {
        if (my_port->registrationForbid  == L7_FALSE &&
            my_port->vlanCreationForbid == L7_FALSE)
        {
          if (!gvd_create_entry(msg->key1, &gid_index) ||
              ( my_gvr->last_gvd_used_plus1 >=  my_gvr->number_of_gvd_entries))
          {
            gvr_db_full(my_gvr, my_port);
            gvrpInfo->failedReg[my_port->port_no]++;
          }
          else
          {
            /* create the Vlan */
            if ((Gid_fix_registration != msg->event) &&
                (L7_SUCCESS != dot1qVlanCreate(msg->key1, DOT1Q_GVRP)))
            {
              gvrpInfo->failedReg[my_port->port_no]++;
              /* delete the entry if the Vlan could not be created */
              rcb = gvd_delete_entry(gid_index);
              /* do not process the message */
              gid_index = Unused_index;
            }
            else
            {
              /* increment the counters */
              my_gvr->vlan_id  = msg->key1;
              my_gvr->last_gvd_used_plus1++;
              my_port->application->last_gid_used++;
              my_port->application->gip[gid_index] = 0;
            }
          }/* !gvd_create_entry*/
        } /*registrationForbid == false*/
      }  /* msg->event == joinin et.al.*/
    } /* !gvd_find_entry*/

    if (gid_index != Unused_index)
    {
      gid_rcv_msg(my_port, gid_index, msg->event);
    }
  } /* end else */

   /* statistics update */
  gvrpUpdateStats(msg->event,my_port->port_no);

}


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
void gvr_rcv(Gvr *my_gvr, L7_uint32 port_no, GarpPdu *pdu)
{  /*
  * Process an entire received pdu for this instance of GVR: initialise
  * the GVF pdu parsing routine, and, while messages last, read and process
  * them one at a time.
  */
  L7_BOOL   rcb;
  Gvf       gvf;
  Gvf_msg   msg;
  Gid       *my_port;
  L7_uint32 gid_index;
  L7_uint32 type;
  L7_uint32 mode;

  if (gid_find_port(my_gvr->g.gid, port_no, (void*)&my_port))
  {
    if (my_port->is_enabled && my_port->is_connected)
    {
      /* increment the number of received pkts */
      gvrpInfo->received[port_no]++;

      rcb = gvf_rdmsg_init(pdu, &gvf, port_no);

      while (gvf_rdmsg(&gvf, &msg, port_no))
      {
        if (msg.event != Gid_rcv_joinin &&
             msg.event != Gid_rcv_joinempty &&
             msg.event != Gid_rcv_empty &&
             msg.event != Gid_rcv_leavein &&
             msg.event != Gid_rcv_leaveempty &&
             msg.event != Gid_rcv_leaveall &&
             msg.event != Gid_rcv_leaveall_range
             )
         {
           gvrpInfo->garpErrorStats[port_no].invalidAttrEvent++;
           /*continue;*/  /* Not disregarding the invalid event to keep status quo with the old code */
         }
         if (msg.key1 < L7_DOT1Q_MIN_VLAN_ID || msg.key1 > L7_DOT1Q_MAX_VLAN_ID)
         {
           gvrpInfo->garpErrorStats[port_no].invalidAttrValue++;
           /*continue;*/ /* Not disregarding the invalid value to keep status quo with the old code */
         }

        if (gvd_find_entry(msg.key1, &gid_index) &&
           (dot1qVlanMemberAndTypeGet((L7_uint32)msg.key1,
                                       my_port->port_no, &mode, &type) != L7_SUCCESS))
        {
          if ((type == L7_DOT1Q_DYNAMIC) ||
              ((type == L7_DOT1Q_ADMIN) &&
               /*((msg.event == Gid_rcv_joinin) ||
               (msg.event == Gid_rcv_joinempty)) &&*/
               (mode != L7_DOT1Q_FIXED)))
          {
            if ((type == L7_DOT1Q_ADMIN) && (mode == L7_DOT1Q_NORMAL_REGISTRATION) &&
                ((msg.event == Gid_rcv_joinin) || (msg.event == Gid_rcv_joinempty)))
            {
                /* This will execute in case a static vlan was created
                 * (<vlan database>, <vlan vid)> commands), and then
                 * JoinIn/JoinEmpty PDU for the same VID was received.
                 * We have to call dot1qVlanMemberSet() to change the
                 * status of my_port->port_no to L7_DOT1Q_FIXED, so that
                 * this port be shown as "included"
                 * by <show vlan vid> command */
                dot1qVlanMemberSet(msg.key1, my_port->port_no,
                                   L7_DOT1Q_FIXED, DOT1Q_GVRP,DOT1Q_SWPORT_MODE_NONE);
            }

            gvr_rcv_msg(my_gvr, my_port, &msg);
          }

        }
        else
        {
           gvr_rcv_msg(my_gvr, my_port, &msg);
        }
      }

      gip_do_actions(my_port);
    }

  }
  /* gid_rlse_rcv_pdu: Insert any system specific action required. */
}


/******************************************************************************
 * GVR : GARP VLAN REGISTRATION APPLICATION : TRANSMIT PROCESSING
 ******************************************************************************
 */

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
static void gvr_tx_msg(Gvr *my_gvr, L7_uint32 gid_index, Gvf_msg *msg)
{  /*
  * Fill in msg fields for transmission.
  */
  L7_BOOL rcb;

  if (msg->event == Gid_tx_leaveall)
  {
    msg->attribute = All_attributes;
  }
  else
  {
    msg->attribute = Vlan_attribute;

    rcb = gvd_get_key(gid_index, &msg->key1);
  }
}


/*********************************************************************
* @purpose  Send Gvr to DTLmessage
*
* @param    bufHandle       bufHandle to be sent
* @oaram    length          length of the message to be send
* @param    port_no         port number
* @param    origP*          pointer to acual data
*
* @returns  None
*
* @notes
*
* @end
*********************************************************************/
void SendGVRPPktToDtl(L7_ushort16 length, L7_uint32 port_no, L7_uchar8 *origP)
{
  DTL_CMD_TX_INFO_t dtlCmd;
  L7_uchar8         *dataStart;
  L7_netBufHandle   bufHandle;
  L7_uint32         activeState;
  L7_RC_t           rc;
  L7_ushort16       len;

  if (GARP_GVRP_IS_ENABLED == L7_TRUE)
  {
      len = length - GARP_802_3_MAC_HEADER_LEN;

      rc = nimGetIntfActiveState(port_no, &activeState);

      if (activeState == L7_ACTIVE)
      {
          /* increment stats */
          gvrpInfo->send[port_no]++;

          /* add the length field */
          tx_pdu[12] = (L7_uchar8)(len >> 8);
          tx_pdu[13] = (L7_uchar8)len;

          /* get a buffer handle */
          SYSAPI_NET_MBUF_GET(bufHandle);

          if (bufHandle == L7_NULL)
          {
              return;
          }

          /* point to the start of the data */
          SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, dataStart);

          /* set the length of the data */
          SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, length);

          /* copy the data */
          memcpy(dataStart, origP, length);

          /* send the data */
          bzero((char *)&dtlCmd,sizeof(DTL_CMD_TX_INFO_t));
          dtlCmd.intfNum = port_no;
          dtlCmd.priority = 1;
          dtlCmd.typeToSend = DTL_L2RAW_UNICAST;

          rc = dtlPduTransmit(bufHandle, DTL_CMD_TX_L2, &dtlCmd);

      } /* end if linkState == L7_UP && portForwarding */

  } /* end if GARP_GVRP_IS_ENABLED */
}

/*********************************************************************
* @purpose  Gvr tx message
*
* @param    Gvr*      pointer to gvr
* @param    void*     pointer to port
*
* @returns  None
*
* @notes  Transmit a pdu for this instance of GVR.
*
* @end
*********************************************************************/
void gvr_tx(void *my_gvr, void *my_port)
{  /*
  * Get and prepare a pdu for the transmission, if one is not available
  * simply return, if there is more to transmit GID will reschedule a call
  * to this function.
  *
  * Get messages to transmit from GID and pack them into the pdu using GVF
  * (GARP VLAN pdu Formatter).
  */
  GarpPdu        *pdu = (GarpPdu *)tx_pdu;
  Gvf             gvf;
  Gvf_msg         msg;
  Gid_event       tx_event;
  L7_uint32       gid_index;
  Gvr             *local_Gvr = (Gvr*) my_gvr;
  L7_uchar8       *p, * origP;
  Gid             *local_port = (Gid*)my_port;
  L7_ushort16     length = 0;
  L7_uchar8       tempLength = 0;
  L7_uchar8       bridge_id[6];
  L7_uint32       instanceID;
  L7_BOOL         rcb;
  L7_uint32       unit;

  unit = usmDbThisUnitGet();

  /* initilize the buffer */
  memset(&tx_pdu[0], 0x00, GARP_MAX_PDU_SIZE);

  /* initilize the data pointers */
  p = origP = tx_pdu;

  /* GVRP DESTINATION (Always 01:80:C2:00:00:21) */
  memcpy (p,Garp_gvrp_mac_address,6);
  /* increment the pointer by the size of the address */
  p += 6;

  if (nimPhaseStatusCheck() == L7_TRUE) /* possibly going through an unconfigure */
  {
    if (nimGetIntfAddress(local_port->port_no, L7_NULL,bridge_id) == L7_SUCCESS)
    {

      memcpy(p,bridge_id, 6);
      p += 6;

      /* increment for the length field */
      p += 2 ;

      /* GARP LSAP Always 0x42 0x42*/
      *p++ = 0x42;
      *p++ = 0x42;


      /* GARP CTL */
      *p++ = 0x03;

      /* initilize length */
      length = GARP_802_3_FRAME_HEADER_LEN;

      if ((tx_event = gid_next_tx(my_port,local_Gvr, &gid_index)) != Gid_null)
      {
        if (pdu != L7_NULL)
        {
          rcb = gvf_wrmsg_init(&gvf, (GarpPdu*)p, local_Gvr->vlan_id);

          /* add the protocol id to the length */
          length += 2;

          do
          {
            msg.event = tx_event;

            gvr_tx_msg(my_gvr, gid_index, &msg);


                instanceID = dot1sVlanToMstiGet(msg.key1);

            if ((gipIsPortBelongToInstance(instanceID, local_port->port_no) == L7_SUCCESS))
            {
                if ((tempLength = gvf_wrmsg(&gvf, &msg)) == 0)
                {
                    gid_untx(my_port);
                    break;
                }
            }

            /* add the length for gvr message */
            length += tempLength;

            /* get next event */
            tx_event  = gid_next_tx(my_port, local_Gvr, &gid_index);

            /* Increment the statistic. The packet has not yet been transmitted
             * but there is a rare chance that the following code will exit
             * before calling SendGVRPPktToDtl() below, that actually transmits
             * the packet in the same task.
             */

            gvrpUpdateStats(msg.event,local_port->port_no);


            if ((length > GARP_MAX_PDU_SIZE - GVRP_MSG_SIZE - 2) &&
                (tx_event != Gid_null))
            {
              gvf_close_wrmsg(&gvf);

              /* increment length for the the endmark */
              length += 2;

              /* send the packet */
              SendGVRPPktToDtl(length,local_port->port_no,origP);

              /* initilize length */
              length = GARP_802_3_FRAME_HEADER_LEN;

              /* initilize the buffer */
              memset(&tx_pdu[length], 0x00, GARP_MAX_PDU_SIZE - length);

              /* start the message from the protocol id again */
              p = &tx_pdu[GARP_802_3_FRAME_HEADER_LEN];

              rcb = gvf_wrmsg_init(&gvf, (GarpPdu*)p, local_Gvr->vlan_id);

              /* add the protocol id to the length */
              length += 2;

            }

          } while (tx_event != Gid_null);

          gvf_close_wrmsg(&gvf);

          /* increment length for the the endmark */
          length += 2;

          /* send the packet */
          SendGVRPPktToDtl(length,local_port->port_no,origP);
        }
      }
    }
  }
}

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
void gvr_delete_attribute(L7_uint32 vid)
{
  L7_BOOL       rcb;
  Gid           *my_port;
  L7_uint32     idx,i;
  L7_BOOL       found = L7_FALSE;

  found = gvd_find_entry((Vlan_id)vid, &idx);

  my_port = gvrp_app->g.gid;

  if (my_port == L7_NULLPTR) /* if no ports exist */
  {
    if (found == L7_TRUE)
    {
      /* no ports (gids) exist but we need to clean out the GVD */
      rcb = gvd_delete_entry(idx);

      gvrp_app->g.gip[idx] = gvrp_app->g.gip[idx+1];

      if (gvrp_app->g.last_gid_used > 0)
      {
        gvrp_app->g.last_gid_used--;
      }
    }

    return;
  }

  if (found == L7_TRUE)
  {
    rcb = gvd_delete_entry(idx);

    /* delete the machine on all ports */
    do
    {
      for (i=idx; i < (L7_uint32)my_port->application->last_gid_used; i++)
      {
        my_port->machines[i].applicant = my_port->machines[i+1].applicant;
        my_port->machines[i].registrar = my_port->machines[i+1].registrar;
        my_port->machines[i].incoming_port = my_port->machines[i+1].incoming_port;
        my_port->application->gip[i] = my_port->application->gip[i+1];
      }

      if ((my_port->last_transmitted >= idx) &&
          (my_port->last_transmitted > 0))
      {
        my_port->last_transmitted--;
      }

      my_port = my_port->next_in_connected_ring;

    } while (my_port != gvrp_app->g.gid);

    if (my_port->application->last_gid_used > 0)
    {
      my_port->application->last_gid_used--;
    }
  }
}

/*********************************************************************
* @purpose  delete Gvr Attribute
*
* @param    Gvr*        pointer to application
* @param    Gvf_msg*    pointer to message
*
* @returns  None
*
* @notes  delete an attribute from the database
*
* @end
*********************************************************************/
L7_BOOL delete_gvrp_attribute(Gvr *app, Vlan_id vid)
{
  L7_uint32     found_at_index;
  Gid           *my_port;
  Gid           *to_port;
  L7_BOOL       found = L7_FALSE;
  L7_BOOL       returnVal = L7_FALSE;

  found = gvd_find_entry(vid, &found_at_index);

  if (found == L7_TRUE)
  {
    /*
     * The attribute exist in the database. Ensure that all
     * ports have left-out this attribute
     */

    my_port = (Gid*)app->g.gid;
    if (my_port != L7_NULL)
    {
      to_port = my_port;
      do
      {
        setMachineLeaveIn(&to_port->machines[found_at_index]);
        to_port = to_port->next_in_connected_ring;
      } while (to_port != my_port);
    }

    /* instead of using the timer, delete directly */
    gvr_delete_attribute(vid);

    if (app->last_gvd_used_plus1 > 0)
    {
      app->last_gvd_used_plus1--;
    }

    returnVal = L7_TRUE;
  }

  return returnVal;
}



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
extern L7_BOOL gvr_proc_command(Gvr *application,void *command )
{
  L7_BOOL Rc = L7_TRUE;
  GARPCommand *cmd;
  Gvf_msg mesg;
  Gid *my_port = L7_NULL;



  cmd  = (GARPCommand *)command;

  garpTraceProcessGvrCommand(cmd);

  mesg.attribute = Vlan_attribute;
  mesg.key1 = (Vlan_id)*((Vlan_id *)&cmd->data);

  if (application->g.gid != L7_NULL)
  {
    if (cmd->port != 0)
    {
      Rc = gid_find_port(application->g.gid, cmd->port, (void*)&my_port);
    }
    else
    {
      my_port = application->g.gid;
    }
  }

  switch (cmd->command)
  {
  case GARP_JOIN:
    mesg.event = Gid_join;
    break;
  case GARP_LEAVE:
    mesg.event = Gid_leave;
    break;
  case GARP_DELETE_ATTRIBUTE:
    return(delete_gvrp_attribute(application, mesg.key1));
    break;
  case GARP_NORMAL_PARTICIPANT:
    mesg.event = Gid_normal_operation;
    break;
  case GARP_NON_PARTICIPANT:
    mesg.event = Gid_no_protocol;
    break;
  case GARP_NORMAL_REGISTRATION:
    mesg.event = Gid_normal_registration;
    break;
  case GARP_REGISTRATION_FIXED:
    mesg.event = Gid_fix_registration;
    break;
  case GARP_REGISTRATION_FORBIDDEN:
    mesg.event = Gid_forbid_registration;
    break;
  case GARP_ENABLE_PORT:
    return(gid_create_port(&application->g,cmd->port));
    break;
  case GARP_DISABLE_PORT:
    return(gid_destroy_port(&application->g,cmd->port));
    break;
  default:
    Rc = L7_FALSE;
    break;
  }

  /* If the command can be processed, and the given port  */
  /*  is enabled, go ahead and process the command.       */
  if (Rc == L7_TRUE)
  {
    if ((my_port != L7_NULL) && (my_port->is_enabled == L7_TRUE))
    {
      gvr_rcv_msg(application,my_port, &mesg);
    }
  }

  return Rc;
}


