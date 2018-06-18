/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    gidapi.c
* @purpose     GID internal api to GARP applications
* @component   GARP
* @comments    none
* @create      02/23/2001
* @author      Hassan
* @author
* @end
*
**********************************************************************/

#include "stdlib.h"
#include "string.h"
#include "l7_common.h"
#include "defaultconfig.h"
#include "osapi.h"
#include "log.h"
#include "dot1q_api.h"
#include "garpapi.h"
#include "garpctlblk.h"
#include "garpcfg.h"
#include "garp.h"
#include "gidapi.h"
#include "gipapi.h"
#include "gidtt.h"
#include "gmrapi.h"
#include "gmrp_api.h"
#include "osapi.h"
#include "dot1dgarp.h"
#include "garp_leaveall_timer.h"
#include "garp_debug.h"
#include "dll_api.h"
#include "gmd.h"
#include "rng_api.h"

extern L7_BOOL gvd_get_key(L7_uint32 index, Vlan_id *key);
extern L7_BOOL gvd_delete_entry(L7_uint32  delete_at_index);
extern L7_BOOL gmd_get_key(void *my_gmd, L7_uint32 index, L7_uchar8 *key);
extern L7_BOOL gmd_delete_entry(void *my_gmd, L7_uint32  delete_at_index);

extern L7_uint32 GenerateLeaveAllTimeoutValue(L7_uint32 leaveall_timeout);

static Gid *gid_remove_port(Gid *my_port);

extern garpCfg_t *garpCfg;
extern Gvr  *gvrp_app;

extern failed_to_enqueue_timers_t failed_timer[L7_MAX_INTERFACE_COUNT+1];
extern L7_BOOL isAttributeRegistered(Gid_machine *machine, L7_int32  last_gid_used);

/* For Debug*/
/* for tracking transitions for all Vlans */
extern L7_BOOL   bTrackVlan;

/*
 *****************************************************************************
 * GID : GARP INFORMATION DISTRIBUTION PROTOCOL : CREATION, DESTRUCTION
 *****************************************************************************
 */

/*********************************************************************
* @purpose  create gid.
*
* @param    Garp       application
* @param    L7_uint32   port no
*
* @returns  L7_FALSE
* @returns  L7_TRUE
*
* @notes    Creates a new instance of GID.
*
* @end
*********************************************************************/
static L7_BOOL gid_create_gid(Garp *application, L7_uint32 port_no, void **gid)
{
   L7_uint32   i;
   garpIntfCfgData_t *pCfg = L7_NULL;
   Gid *my_port = L7_NULL;

   *gid = L7_NULL;

   if (garpMapIntfIsConfigurable(port_no, &pCfg) == L7_FALSE)
   {
      return L7_FALSE;
   }

   if (pCfg == L7_NULL)
   {
      return L7_FALSE;
   }

    my_port = osapiMalloc(L7_GARP_COMPONENT_ID, sizeof(Gid));

   if (my_port == L7_NULLPTR)
   {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GARP_COMPONENT_ID,
              "gid_create_gid(): unable to allocate memory for application %d, port %x.\n",
              application->app, port_no);
      return L7_FALSE;
   }

   GARP_TRACE_INTERNAL_EVENTS("%s Gid %x created for port no %d application %s \n",
                               __FUNCTION__, my_port, port_no,
                              (GARP_GMRP_APP == application->app) ? "GMRP" :
                              (GARP_GVRP_APP == application->app) ? "GVRP" : "Unknown");

   memset(my_port, 0x00, sizeof(my_port));

   my_port->application            = application;
   my_port->port_no                = port_no;
   my_port->next_in_port_ring      = my_port;
   my_port->next_in_connected_ring = my_port;

   my_port->is_enabled             = L7_FALSE;
   my_port->is_connected           = L7_FALSE;

   my_port->cschedule_tx_now       = L7_FALSE;
   my_port->cstart_join_timer      = L7_FALSE;
   my_port->cstart_leave_timer     = L7_FALSE;
   my_port->join_timer_running     = L7_FALSE;
   my_port->leave_timer_running    = L7_FALSE;

   my_port->leave_timeout          = pCfg->leave_time;
   my_port->join_timeout           = pCfg->join_time;
   my_port->leaveall_timeout       = pCfg->leaveall_time;

   my_port->transmit_leaveall      = L7_FALSE;


   my_port->tx_pending         = L7_FALSE;

   my_port->last_transmitted   = /* application->last_gid_used; */ /*SK-Service Req*/
       (GARP_GMRP_APP == application->app) ? application->last_gid_used : 0;

   my_port->untransmit_machine = application->max_gid_index + 1;

   my_port->machines = (Gid_machine*)
       osapiMalloc(L7_GARP_COMPONENT_ID,
           sizeof(Gid_machine)*(application->max_gid_index + 2));

   if (my_port->machines == L7_NULLPTR)
   {
       L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GARP_COMPONENT_ID,
               "gid_create_gid(): unable to allocate memory for machines on Port %x.\n",port_no);
       osapiFree(L7_GARP_COMPONENT_ID, my_port);
       return L7_FALSE;
   }

   for (i = 0 ; i < application->max_gid_index + 2 ; i++)
   {
      initMachine(&my_port->machines[i]);
   }

   *gid = my_port;

   if (GARP_GMRP_APP == application->app)
   {
       my_port->gmrp_leaveall_left =
                       GenerateLeaveAllTimeoutValue(my_port->leaveall_timeout);
   }

   /* add the Leave All Timer for the current port/application */
   garpUpdateLeaveAllTimer(ADD_LEAVEALL_TIMER,
                           port_no, application->app,
                           application->vlan_id, 0,
                           my_port->leaveall_timeout);
   return L7_TRUE;
}


/*********************************************************************
* @purpose  destroy gid port.
*
* @param    Gid       pointer to Gid
*
* @returns  None
*
* @notes   Destroys the instance of GID, releasing previously allocated space.
*          Sends leave indications to the application for previously registered
*          attributes.
*
*
* @end
*********************************************************************/
static void gid_destroy_gid(Gid *gid)
{
  L7_uint32 gid_index;
  /* for debug */
  Vlan_id key;

  GARP_TRACE_INTERNAL_EVENTS("Entering gid_destroy_gid: app: %d, port_no %d\n",
                             gid->application, gid->port_no);


  if(gid->application->app == GARP_GMRP_APP)
  {
    for (gid_index = gid->application->last_gid_used;
         gid_index >= Number_of_legacy_controls;
         gid_index--)
    {
        gid->application->leave_indication_fn(gid->application, gid, gid_index);
    }
  }
  else  /*GVRP*/
  {
    for (gid_index = gid->application->last_gid_used+1;
         gid_index > 0;
         gid_index--)
    {
      if (gid_registered_here(gid, gid_index-1))
      {
        (void) gvd_get_key(gid_index-1, &key);
        gid->application->leave_indication_fn(gid->application, gid, gid_index-1);

      }
    }

  }

  /* now that processing is done, remove from ring */
  gid->application->gid = gid_remove_port(gid);

  osapiFree(L7_GARP_COMPONENT_ID, gid->machines);


  /* Clean up the timers.
     The osapiTimerFree call checks if the time instance is valid
     So we do not need to check if the timer is running
  */
  if (gid->join_timer_running == L7_TRUE)
  {
      if (garpTimerFree(&(gid->join_timer_instance)) != L7_SUCCESS)
          L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GARP_COMPONENT_ID,
                  "%s: join timer instance free failure for port %d\n",
                  __FUNCTION__,gid->port_no);
  }

  if (gid->leave_timer_running == L7_TRUE)
  {
  if (garpTimerFree(&(gid->leave_timer_instance)) != L7_SUCCESS)
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GARP_COMPONENT_ID,
              "%s: leave timer instance free failure for port %d\n",
              __FUNCTION__,gid->port_no);
  }

  osapiFree(L7_GARP_COMPONENT_ID, gid);
}

/*********************************************************************
* @purpose  add gid port.
*
* @param    Gid       pointer to existing ports
* @param    Gid       new port
*
* @returns  Gid      created port
*
* @notes   Adds new_port to the port ring.
*
*
* @end
*********************************************************************/
static Gid *gid_add_port(Gid *existing_ports, Gid *new_port)
{
    Gid *prior;
    Gid *next;
    L7_uint32  new_port_no;

    if (existing_ports != L7_NULL)
    {
        new_port_no = new_port->port_no;

        /* L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GARP_COMPONENT_ID,
                "GID: Add Port # %x.\n",new_port_no);*/

        next = existing_ports;
        for(;;)
        {
            prior = next;
            next = prior->next_in_port_ring;

            if (prior->port_no <= new_port_no)
            {
                if ((next->port_no <= prior->port_no) ||
                    (next->port_no >  new_port_no))
                    {
                       break;
                    }
            }
            else /* if (prior_>port_no > new_port_no) */
            {
                if ((next->port_no <= prior->port_no) &&
                    (next->port_no >  new_port_no))
                    {
                       break;
                    }
            }
        } /* end for */


        if (prior->port_no != new_port_no)
        {
            prior->next_in_port_ring    = new_port;
            new_port->next_in_port_ring = next;
            new_port->is_enabled = L7_TRUE;
        }
        else
        {
            /* syserr_panic();        */
            L7_LOG_ERROR(new_port_no);
        }

    }
    else
    {
        new_port->is_enabled = L7_TRUE;
    }

    return(new_port);
}


/*********************************************************************
* @purpose  add gid port.
*
* @param    Gid       port to be removed
*
* @returns  Gid      ports
*
* @notes   Adds new_port to the port ring.
*
*
* @end
*********************************************************************/
static Gid *gid_remove_port(Gid *my_port)
{
    Gid *prior;
    Gid *next;
    Gid *returnVal = L7_NULL;

    /* L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GARP_COMPONENT_ID,
              "GID: Removing Port # %x from the ring.\n",my_port->port_no);*/

    prior = my_port;
    while ((next = prior->next_in_port_ring) != my_port)
    {
        prior = next;
    }

    prior->next_in_port_ring = my_port->next_in_port_ring;

    if (prior != my_port)
    {
        returnVal = prior;
    }

    return returnVal;
}


/*********************************************************************
* @purpose  create gid port.
*
* @param    Garp       application
* @param    L7_uint32   port no
*
* @returns  L7_FALSE
* @returns  L7_TRUE
*
* @notes    Creates a new instance of GID, allocating space for GID machines as
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
L7_BOOL gid_create_port(Garp  *application, L7_uint32 port_no)
{
  Gid *my_port;
  L7_BOOL returnVal = L7_FALSE;

  GARP_TRACE_INTERNAL_EVENTS("Entering gid_create_port: app: %d, port_no %d\n",
                             application->app, port_no);

  if (!gid_find_port(application->gid, port_no, (void*)&my_port))
  {
    if (gid_create_gid(application, port_no, (void*)&my_port))
    {
      application->gid = gid_add_port(application->gid, my_port);
      application->added_port_fn(application, port_no);

      /* Check if port needs to be connected */
      if (GarpLinkIsActive((GARPPort)port_no))
      {
        gip_connect_port(application, port_no);
      }

      returnVal = L7_TRUE;
    }
  }

  return returnVal;
 }

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
L7_BOOL gid_destroy_port(Garp  *application, L7_uint32 port_no)
{
  Gid        *my_port;
  L7_uint32  index;
  Vlan_id    key;
  L7_uint32  type;
  L7_BOOL    returnVal = L7_FALSE;
  L7_uchar8     macAddr[L7_MAC_ADDR_LEN];
  Gmr       *my_gmr;
  L7_RC_t    rc;

  if (gid_find_port(application->gid, port_no, (void*)&my_port))
  {
    for (index = 0; index < application->last_gid_used; index++)    /*SKTBD- GMRP fix*/
    {
      if (my_port->machines[index].incoming_port == L7_TRUE)
      {
        if(application->app == GARP_GVRP_APP)
        {
          /* remove this port from vlan */
          gvd_get_key(index,&key);
          dot1qOperVlanTypeGet((L7_uint32)key,&type);
          if (type == L7_DOT1Q_DYNAMIC)
          {
            dot1qVlanMemberSet((L7_uint32)key,my_port->port_no,
                               L7_DOT1Q_NORMAL_REGISTRATION, DOT1Q_GVRP,DOT1Q_SWPORT_MODE_NONE);
          }
          else
          {
            dot1qVlanMemberSet((L7_uint32)key,my_port->port_no, L7_DOT1Q_FORBIDDEN, DOT1Q_GVRP,DOT1Q_SWPORT_MODE_NONE);
          }
        }
        else if(application->app == GARP_GMRP_APP)
        {
          if(index >= Number_of_legacy_controls)
          {
            if ((my_gmr = gmrpInstanceGet(application->vlan_id)) == L7_NULLPTR)
            {
                return L7_FALSE;
            }
            gmd_get_key(my_gmr->gmd,index-Number_of_legacy_controls,macAddr);
            /*set this port to filter for this mac addr*/
            rc = gmrpFwdPortDelete(application->vlan_id,macAddr,my_port->port_no);
            /* We are going to ignore L7_NOT_EXIST case because when we are
             * destroying a GMR we are going to delete the MFD Entry and then move
             * ahead to delete the gids.
             */
            if (rc != L7_SUCCESS && rc != L7_NOT_EXIST)
            {
                L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GARP_COMPONENT_ID,
                        "Error Removing port %d registration for vlan-mac %d - %02X:%02X:%02X:%02X:%02X:%02X."
                        " Mismatch between the gmd (gmrp database) and MFDB.",
                         my_port->port_no, application->vlan_id, macAddr[0], macAddr[1], macAddr[2],
                         macAddr[3], macAddr[4], macAddr[5]);
            }
          }
        }
      }
    }

    gip_disconnect_port(application, port_no);

    application->removed_port_fn(application, port_no);

    /* stop the timers for the current port and application */
    garpUpdateLeaveAllTimer(DELETE_TIMER, port_no, application->app, 0, 0, 0);

    GARP_TRACE_INTERNAL_EVENTS("%s Destroying gid for port %d gid %x \n",
                               __FUNCTION__, port_no, my_port);

    gid_destroy_gid(my_port);

    returnVal = L7_TRUE;
  }

  return returnVal;
}


/******************************************************************************
 * GID : GARP INFORMATION DISTRIBUTION PROTOCOL : USEFUL FUNCTIONS
 ******************************************************************************
 */

/*********************************************************************
* @purpose  find gid port
*
* @param    Garp        application
* @param    L7_uint32   port no
* @param    void*       pointer to found port
*
* @returns  L7_FALSE
* @returns  L7_TRUE
*
* @notes    Finds the GID instance for port number port_no.
*
* @end
*********************************************************************/
L7_BOOL gid_find_port(Gid *first_port, L7_uint32 port_no, void **gid)
{
  Gid *next_port    = first_port;
  L7_BOOL returnVal = L7_TRUE;

  if (first_port != L7_NULL)
  {
    while (next_port->port_no != port_no)
    {
      if ((next_port = next_port->next_in_port_ring) == first_port)
      {
        returnVal = L7_FALSE;
        break;
      }

    }

    *gid = next_port;
  }
  else
  {
    returnVal = L7_FALSE;
  }

  return returnVal;
}


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
Gid *gid_next_port(Gid *this_port)
{
    return(this_port->next_in_port_ring);
}


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
void gid_manage_attribute(Gid *my_port, L7_uint32 index,
                          Gid_event directive,Gid_event tx_event)
{  /*
    *
    */

    Gid_machine  *machine;
    Gid_event    event;
    Gid_machine  local_machine;
    /* for debug */
    Vlan_id key;

    GARP_TRACE_PROTOCOL_EVENTS("gid_manage_attribute: app: %d, index %d,directive %d, tx_event %d\n",
                               my_port->application,index, directive, tx_event);

    machine = &my_port->machines[index];

    local_machine.applicant = machine->applicant;
    local_machine.registrar = machine->registrar;

    /* for debug */
    if(bTrackVlan)
    {
        if (my_port->application->app == GARP_GVRP_APP )
        {
            (void)gvd_get_key(index, &key);
            garpDebugTrackVlanSet(key);
        }
    }

    /* now check the event */
    event   =  gidtt_event(my_port, machine, directive);

    /* for debug */
    /* reset vlan vlaue */
    if(bTrackVlan)
    {
        if (my_port->application->app == GARP_GVRP_APP )
            garpDebugTrackVlanReset();
    }

   if (event == Gid_join)
    {
        my_port->application->join_indication_fn(my_port->application,
                                                 my_port, index);
        gip_propagate_join(my_port, index);
    }
    else if (event == Gid_leave)
    {
        my_port->application->leave_indication_fn(my_port->application,
                                                  my_port, index);
        gip_propagate_leave(my_port, index);
    }

    /* update the applicant state t o the right state */
    gidtt_txmsg(&local_machine,tx_event);
    machine->applicant = local_machine.applicant;
}


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
* @notes    Finds an unused GID machine (i.e. one with an Empty registrar and a
*           Very Anxious Observer applicant) starting the search at GID index
*           from_index, and searching to gid_last_used.
*
* @end
*********************************************************************/
L7_BOOL gid_find_unused(Garp *application, L7_uint32  from_index,
                                           L7_uint32 *found_index)
{
  L7_uint32  gid_index;
  Gid       *check_port;
  L7_BOOL   returnVal = L7_FALSE;

    gid_index = from_index; check_port = application->gid;
    for (;;)
    {
        if (gidtt_machine_active(&check_port->machines[gid_index]))
        {
            if (gid_index++ > application->last_gid_used)
            {
                L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GARP_COMPONENT_ID,
                        "GID: All Ports are used.\n");
                returnVal = L7_FALSE;
                break;
            }
            check_port = application->gid;
        }
        else if ((check_port = check_port->next_in_port_ring)
                            == application->gid)
        {
            *found_index = gid_index;
            returnVal = L7_TRUE;
            L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GARP_COMPONENT_ID,
                    "GID: Ports at index %x is not used.\n",gid_index);
            break;
        }
    } /* end for */
   return returnVal;
}


/******************************************************************************
 * GID : GARP INFORMATION DISTRIBUTION PROTOCOL : EVENT PROCESSSING
 ******************************************************************************
 */

/*********************************************************************
* @purpose  process a leaveall event
*
* @param    Gid          port
*
* @returns  none
*
* @notes    only for shared media at present
*
* @end
*********************************************************************/
void gid_leaveall(Gid *my_port)
{
    L7_uint32 i;
    L7_int32  last_gid_used;
    /* for debug */
    Vlan_id key;

    last_gid_used = my_port->application->last_gid_used;

    GARP_TRACE_PROTOCOL_EVENTS("gid_leaveall: app: %d, port %d\n",
                               my_port->application->app, my_port->port_no);

    /* notify all attributes on the given port about the Leave All event */
    for (i = 0; i < last_gid_used; i++)
    {
        /* for debug */
        if(bTrackVlan)
        {
            if (my_port->application->app == GARP_GVRP_APP )
            {
                (void)gvd_get_key(i, &key);
                garpDebugTrackVlanSet(key);
            }

        }

        (void) gidtt_event(my_port, &my_port->machines[i], Gid_rcv_leaveall);

        /* for debug */
        /* reset vlan vlaue */
        if(bTrackVlan)
        {
            if (my_port->application->app == GARP_GVRP_APP )
                garpDebugTrackVlanReset();
        }
     }

    return;
}


/*********************************************************************
* @purpose  leaveall event is received
*
* @param    Gid          port
*
* @returns  none
*
* @notes    only for shared media at present
*
* @end
*********************************************************************/
void gid_rcv_leaveall(Gid *my_port)
{
    L7_uint32 currentTime;


    GARP_TRACE_PROTOCOL_EVENTS("gid_rcv_leaveall: app: %d, port %d\n",
                               my_port->application->app, my_port->port_no);

    currentTime = osapiTimeMillisecondsGet();

    /* restart the GVRP Leave All Timer running on current port */
    garpUpdateLeaveAllTimer(RESTART_LEAVEALL_TIMER,
                            my_port->port_no, GARP_GVRP_APP,
                            0, currentTime, 0);

    gid_leaveall(my_port);

    my_port->transmit_leaveall = L7_FALSE;
    my_port->cstart_join_timer = L7_TRUE;
}

/*********************************************************************
* @purpose  leaveall event is received for GMRP
*
* @param    my_gmr      pointer to GMR
* @param    my_port     pointer to GID
*
* @returns  none
*
* @notes    only for shared media at present
*
* @end
*********************************************************************/
void gid_gmr_rcv_leaveall(Gmr *my_gmr, Gid *my_port)
{

    if (my_port != L7_NULL)
    {
        GARP_TRACE_PROTOCOL_EVENTS("gid_gmr_rcv_leaveall: app: %d, port %d, vlan %d, entry address %x\n",
                               my_port->application->app, my_port->port_no,
                               my_gmr->vlan_id, my_gmr->gmd);
        /* restart GMRP Leave All Timer running on current port */
        my_port->gmrp_leaveall_left =
            GenerateLeaveAllTimeoutValue(my_port->leaveall_timeout);

        gid_leaveall(my_port);

        my_port->transmit_leaveall = L7_FALSE;
        my_port->cstart_join_timer = L7_TRUE;
    }

    return;
}

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
void gid_rcv_msg(Gid *my_port, L7_uint32 index, Gid_event msg)
{
    Gid_machine *machine;
    Gid_event    event;
    Gid         *to_port;
    /* for debug */
    Vlan_id key;

    machine = &my_port->machines[index];

     /* for debug */
    if(bTrackVlan)
    {
        if (my_port->application->app == GARP_GVRP_APP )
        {
            (void)gvd_get_key(index, &key);
            garpDebugTrackVlanSet(key);
        }

    }


    event   =  gidtt_event(my_port, machine, msg);

     /* for debug */
    /* reset vlan vlaue */
    if(bTrackVlan)
    {
        if (my_port->application->app == GARP_GVRP_APP )
            garpDebugTrackVlanReset();
    }

    if (event == Gid_join)
    {
        if (msg != Gid_fix_registration)
        {
           my_port->application->join_indication_fn(my_port->application,
                                                 my_port, index);
        }
        gip_propagate_join(my_port, index);
    }

    if ((msg == Gid_rcv_joinin) || (msg == Gid_rcv_joinempty))
    {
        /* increment the number of ports received this attribute */
        if (my_port->machines[index].incoming_port == L7_FALSE)
        {
            my_port->application->gip[index] += 1;
        }

        /* this is the receiving port */
        my_port->machines[index].incoming_port = L7_TRUE;
    }
    else if (msg == Gid_fix_registration)
    {
        to_port = my_port;
        do
        {
            /* this is a static Vlan, so it is not learned over any port */
            if (to_port->machines[index].incoming_port == L7_TRUE)
            {
                if (to_port->application->gip > 0)
                {
                    to_port->application->gip[index] -= 1;
                }
            }
            to_port->machines[index].incoming_port = L7_FALSE;
            to_port = to_port->next_in_connected_ring;
        } while (to_port != my_port);
    }
}
/*********************************************************************
* @purpose  receive message for GMRP
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
void gid_gmr_rcv_msg(Gmr *my_gmr, Gid *my_port, L7_uint32 index, Gid_event msg)
{
    Gid_machine *machine;
    Gid_event    event;
    Gid         *to_port;
    /* for debug */
    Vlan_id key;

    machine = &my_port->machines[index];

    /* for debug */
    if(bTrackVlan)
    {
        if (my_port->application->app == GARP_GVRP_APP )
        {
            (void)gvd_get_key(index, &key);
            garpDebugTrackVlanSet(key);
        }

    }

    event   =  gidtt_event(my_port, machine, msg);

     /* for debug */
    /* reset vlan vlaue */
    if(bTrackVlan)
    {
        if (my_port->application->app == GARP_GVRP_APP )
            garpDebugTrackVlanReset();
    }


    if (event == Gid_join)
    {
        my_port->application->join_indication_fn(my_port->application,
                                                 my_port, index);
        gip_propagate_join(my_port, index);
    }

    if ((msg == Gid_rcv_joinin) || (msg == Gid_rcv_joinempty))
    {
        /* increment the number of ports received this attribute */
        if (my_port->machines[index].incoming_port == L7_FALSE)
        {
            my_port->application->gip[index] += 1;
        }

        /* this is the receiving port */
        my_port->machines[index].incoming_port = L7_TRUE;
    }
    else if (msg == Gid_fix_registration)
    {
        to_port = my_port;
        do
        {
            /* this is a static group address or service requirement, so it is not learned over any port */
            if (to_port->machines[index].incoming_port == L7_TRUE)
            {
                if (to_port->application->gip > 0)
                {
                    to_port->application->gip[index] -= 1;
                }
            }
            to_port->machines[index].incoming_port = L7_FALSE;
            to_port = to_port->next_in_port_ring;
        } while (to_port != my_port);
    }
}

/*********************************************************************
* @purpose  join request is recieved.
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
void gid_join_request(Gid *my_port, L7_uint32 gid_index)
{
  /* for debug */
   Vlan_id key;


   GARP_TRACE_PROTOCOL_EVENTS("Join Request: app: %d, port_no %d, index %d\n",
                              my_port->application->app, my_port->port_no, gid_index);


   /* for debug */
   if(bTrackVlan)
   {
       if (my_port->application->app == GARP_GVRP_APP )
       {
           (void)gvd_get_key(gid_index, &key);
           garpDebugTrackVlanSet(key);
       }
   }

    (void)(gidtt_event(my_port, &my_port->machines[gid_index], Gid_join));

     /* for debug */
    /* reset vlan vlaue */
    if(bTrackVlan)
    {
        if (my_port->application->app == GARP_GVRP_APP )
            garpDebugTrackVlanReset();
    }

}


/*********************************************************************
* @purpose  Leave request is recieved.
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
void gid_leave_request(Gid *my_port, L7_uint32 gid_index)
{
  /* for debug */
    Vlan_id key;

    GARP_TRACE_PROTOCOL_EVENTS("gid_leave_request: app: %d, port %d, index %d\n",
                               my_port->application->app, my_port->port_no, gid_index);

    /* for debug */
    if(bTrackVlan)
    {
        if (my_port->application->app == GARP_GVRP_APP )
        {
            (void)gvd_get_key(gid_index, &key);
            garpDebugTrackVlanSet(key);
        }
    }

   (void)(gidtt_event(my_port, &my_port->machines[gid_index], Gid_leave));

   /* for debug */
    /* reset vlan vlaue */
   if(bTrackVlan)
   {
       if (my_port->application->app == GARP_GVRP_APP )
           garpDebugTrackVlanReset();
   }

}


/*********************************************************************
* @purpose
*
* @param
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL gid_registrar_in(Gid_machine *machine)
{
    return(gidtt_in(machine));
}

/******************************************************************************
 * GID : GARP INFORMATION DISTRIBUTION PROTOCOL : RECEIVE PROCESSSING
 ******************************************************************************
 */

/*********************************************************************
* @purpose  Leave request is recieved.
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
void gid_rcv_pdu(Garp *application, L7_uint32 port_no, void *pdu)
{  /*
    * If a GID instance for this application and port number is found and is
    * enabled pass the pdu to the application, which will parse it (using the
    * applications own pdu formatting conventions) and call gid_rcv_msg() for
    * each of the conceptual GID message componentss read from the pdu. Once
    * the application is finished with the pdu call gip_do_actions() to start
    * timers as recorded in the GID scratchpad for this port and any ports to
    * which it may have propagated joins or leaves.
    *
    * Finally release the received pdu.
    */
    Gid     *my_port;

    if (gid_find_port(application->gid, port_no, (void*)&my_port))
    {
        if (my_port->is_enabled)
        {
            application->receive_fn(application, my_port, pdu);

            gip_do_actions(my_port);
        }
    }
/* gid_rlse_rcv_pdu: Insert any system specific action required. */
}

/*********************************************************************
* @purpose   find next event to be transmitted
*
* @param    Gid        port
* @param    L7_uint32 index
*
* @returns  Gid_event   event
*
* @notes  Check to see if a leaveall should be sent, if so return Gid_tx_leaveall,
*         otherwise scan the GID machines for messsages that require transmission.
*
*         Machines will be checked for potential transmission starting with the
*         machine following last_transmitted and up to and including last_to_transmit.
*         If all machines have transmitted last_transmitted equals last_to_transmit
*         and tx_pending is False (in this case tx_pending distinguished the
*         case of all machines are yet to be checked for transmission from all have
*         been checked).
*
*        If tx_pending is True and all machines are yet to be checked, transmission
*        will start from the machine with GID index 0, rather than from immediately
*        following last_transmitted.
*
* @end
*********************************************************************/
Gid_event gid_next_tx(Gid *my_port, Gvr *my_gvr, L7_uint32 *index)
{
    L7_uint32    check_index;
    L7_uint32    stop_after;
    Gid_event    msg;
    Gid_event    returnVal = Gid_null;

    if (my_port->transmit_leaveall)
    {
        my_port->transmit_leaveall = L7_FALSE;
        returnVal =  Gid_tx_leaveall;
        my_port->tx_pending = L7_FALSE;
    }
    else if (!my_port->tx_pending)
    {
        my_port->tx_pending = L7_TRUE;
        returnVal = Gid_null;
    }
    else
    {
        check_index    = my_port->last_transmitted;
        stop_after     = my_port->application->last_gid_used;

        for(;check_index <= stop_after; check_index++)
        {
            if (stop_after == check_index)
            {
                my_port->tx_pending = L7_FALSE;
                returnVal = Gid_null;

                /* start from the begining next time */
                my_port->last_transmitted = 0;

                break;
            }

            if ((msg = gidtt_tx(my_port, &my_port->machines[check_index]))
                    != Gid_null)
            {
                *index = check_index;

                /* add one to start at the next to transmit machine */
                my_port->last_transmitted = check_index + 1;

                my_port->machines[my_port->untransmit_machine].applicant =
                my_port->machines[check_index].applicant;

                /* my_port->tx_pending = (check_index == stop_after);  */

                returnVal = msg;

                /* to force the state machine from Va to Aa */
                /* or from Aa to Qa */
                if (returnVal == Gid_tx_joinin)
                {
                    gid_manage_attribute(my_port,check_index,
                                         Gid_rcv_joinin,Gid_tx_joinin);
                }
                else  if (returnVal == Gid_tx_joinempty)
                {
                    gid_manage_attribute(my_port,check_index,
                                         Gid_join,Gid_tx_joinempty);
                }
                else if ((returnVal == Gid_tx_leaveempty) || (returnVal == Gid_tx_leavein))
                {
                    gid_manage_attribute(my_port,check_index,
                                         Gid_no_protocol,returnVal);
                }
                else if (returnVal == Gid_tx_empty)
                {
                    gid_manage_attribute(my_port,check_index,
                                         Gid_rcv_empty,Gid_tx_empty);
                }

                break;
            }
        }/* end for(;;) */

    }
    return returnVal;
}
/*********************************************************************
* @purpose   find next event to be transmitted
*
* @param    Gid        port
* @param    L7_uint32 index
*
* @returns  Gid_event   event
*
* @notes  Check to see if a leaveall should be sent, if so return Gid_tx_leaveall,
*         otherwise scan the GID machines for messsages that require transmission.
*
*         Machines will be checked for potential transmission starting with the
*         machine following last_transmitted and up to and including last_to_transmit.
*         If all machines have transmitted last_transmitted equals last_to_transmit
*         and tx_pending is False (in this case tx_pending distinguished the
*         case of all machines are yet to be checked for transmission from all have
*         been checked).
*
*        If tx_pending is True and all machines are yet to be checked, transmission
*        will start from the machine with GID index 0, rather than from immediately
*        following last_transmitted.
*
* @end
*********************************************************************/
Gid_event gid_gmr_next_tx(Gmr *my_gmr, Gid *my_port, L7_uint32 *index)
{
  L7_uint32    check_index;
  L7_uint32    stop_after;
  Gid_event    msg;
  Gid_event    returnVal = Gid_null;
  if(dot1qVlanCheckValid(my_gmr->vlan_id)!=L7_SUCCESS)
    return Gid_null;

  if (my_port->transmit_leaveall)
  {
    my_port->transmit_leaveall = L7_FALSE;
    returnVal =  Gid_tx_leaveall;
    my_port->tx_pending = L7_FALSE;
  }
  else if (!my_port->tx_pending)
  {
    my_port->tx_pending = L7_TRUE;
    returnVal = Gid_null;
  }
  else
  {
    check_index    = my_port->last_transmitted;
    stop_after     = my_port->application->last_gid_used;

    for(;check_index <= stop_after; check_index++)
    {
      if (stop_after == check_index)
        {
          my_port->tx_pending = L7_FALSE;
          returnVal = Gid_null;

          /* start from the begining next time */
          my_port->last_transmitted = Number_of_legacy_controls; /*SK-Service Req*/

          break;
        }

      if ((msg = gidtt_tx(my_port, &my_port->machines[check_index]))
          != Gid_null)
      {
        *index = check_index;

        /* add one to start at the next to transmit machine */
        my_port->last_transmitted = check_index + 1;

        my_port->machines[my_port->untransmit_machine].applicant =
        my_port->machines[check_index].applicant;

       /* my_port->tx_pending = (check_index == stop_after);*/

        returnVal = msg;

        /* to force the state machine from Va to Aa */
        /* or from Aa to Qa */
        if (returnVal == Gid_tx_joinin)
        {
          gid_manage_attribute(my_port,check_index,
                               Gid_rcv_joinin,Gid_tx_joinin);
        }
        else if (returnVal == Gid_tx_joinempty)
        {
          gid_manage_attribute(my_port,check_index,
                               Gid_join,Gid_tx_joinempty);
        }
        else if ((returnVal == Gid_tx_leaveempty) || (returnVal == Gid_tx_leavein))
        {
          gid_manage_attribute(my_port,check_index,
                               Gid_no_protocol,returnVal);
        }
        else if (returnVal == Gid_tx_empty)
        {
          gid_manage_attribute(my_port,check_index,
                               Gid_rcv_empty,Gid_tx_empty);
        }

        break;
      }
    }/* end for(;;) */

  }
  return returnVal;
}

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
void gid_untx(Gid *my_port)
{
    my_port->machines[my_port->last_transmitted].applicant =
    my_port->machines[my_port->untransmit_machine].applicant;

    if (my_port->last_transmitted == 0 && my_port->application->app == GARP_GVRP_APP)
    {
        my_port->last_transmitted  = my_port->application->last_gid_used;
    }
    else if (my_port->last_transmitted == Number_of_legacy_controls &&
             my_port->application->app == GARP_GMRP_APP)
    {
        my_port->last_transmitted  = my_port->application->last_gid_used;
    }
    else
    {
        my_port->last_transmitted--;
    }

    my_port->tx_pending = L7_TRUE;
}

/*********************************************************************
* @purpose  check if port is registrede.
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
L7_BOOL gid_registered_here(Gid *my_port, L7_uint32 gid_index)
{
    return (my_port->machines[gid_index].registrar == Mt) ? L7_FALSE : L7_TRUE;
}

/******************************************************************************
 * GID : GARP INFORMATION DISTRIBUTION PROTOCOL : TIMER PROCESSING
 ******************************************************************************
 */


/*********************************************************************
* @purpose  gid staring timers.
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
void gid_do_actions(Gid *my_port)
{
  L7_double64 timeout;
  L7_uint32   join_timeout = 0;

  /*
   * Carries out 'scratchpad' actions accumulated in this invocation of GID,
   * and outstanding 'immediate' transmissions and join timer starts which
   * have been delayed by the operation of the hold timer. Note the way in
   * which the hold timer works here. It could have been specified just to
   * impose a minimum spacing on transmissions - and run in parallel with the
   * join timer - with the effect that the longer of the hold timer and actual
   * join timer values would have determined the actual transmission time.
   * This approach was not taken because it could have led to bunching
   * transmissions at the hold time.
   *
   * Restarts the join timer if there are still transmissions pending (if
   * leaveall_countdown is zero a Leaveall is to be sent, if tx_pending is true
   * individual machines may have messages to send.
   */

   if (my_port->cstart_join_timer)
   {
     my_port->tx_pending        = L7_TRUE;
     my_port->cstart_join_timer = L7_FALSE;
   }

   if ((my_port->cschedule_tx_now) && (my_port->join_timer_running == L7_FALSE))
   {
       if (my_port->application->app == GARP_GVRP_APP)
       {
         garpTimerAdd (0, my_port->port_no,
                       Gid_schedule_time,
                       &(my_port->join_timer_instance), GARP_TIMER_GVRP_JOIN);
       }
       else
       {
         garpTimerAdd ((L7_uint32)my_port->application->vlan_id, my_port->port_no,
                       Gid_schedule_time,
                       &(my_port->join_timer_instance), GARP_TIMER_GMRP_JOIN);
       }

       my_port->join_timer_running = L7_TRUE;

       my_port->cschedule_tx_now  = L7_FALSE;
   }

   else if ( (my_port->tx_pending || (my_port->transmit_leaveall)) &&
             (!my_port->join_timer_running) )
   {
     my_port->join_timer_running = L7_TRUE;

     timeout = L7_Random();
     join_timeout = timeout * my_port->join_timeout;

     if (my_port->application->app == GARP_GVRP_APP)
     {
       garpTimerAdd (0, my_port->port_no,
                     join_timeout,
                     &(my_port->join_timer_instance), GARP_TIMER_GVRP_JOIN);
     }
     else
     {
       garpTimerAdd ((L7_uint32)my_port->application->vlan_id, my_port->port_no,
                     join_timeout,
                     &(my_port->join_timer_instance), GARP_TIMER_GMRP_JOIN);
     }
   }

   if (my_port->cstart_leave_timer && (!my_port->leave_timer_running))
   {
     if ((my_port->application->app == GARP_GVRP_APP) &&
         (my_port->application->last_gid_used > 0))
     {
       /* make sure that we only start the Leave timer if there are any
        * dynamically registered attributes on the current port.
        * Otherwise, starting Leave timer is useless -- it won't
        * do anything except wasting processor cycles.
        */
       if (L7_TRUE == isAttributeRegistered(my_port->machines,
                                          my_port->application->last_gid_used-1))
       {
           /* uncomment the line below for debugging print statements */
           /*garpTraceInternalEvents("\nGVRP Leave timer Port=%u, last_gid_used=%u\n",
                        my_port->port_no, my_port->application->last_gid_used);*/

           garpTimerAdd(0, my_port->port_no,
                      (L7_uint32)(my_port->leave_timeout),
                      &(my_port->leave_timer_instance), GARP_TIMER_GVRP_LEAVE);

           my_port->leave_timer_running = L7_TRUE;
       }
     }
     else
     if ((my_port->application->app == GARP_GMRP_APP) &&
         (my_port->application->last_gid_used > 1))
     {
       if (L7_TRUE == isAttributeRegistered(my_port->machines,
                                          my_port->application->last_gid_used-1))
       {
           /* uncomment the line below for debugging print statements */
           /* sysapiPrintf("\nGMRP Leave timer Port=%u, last_gid_used=%u",
               my_port->port_no, my_port->application->last_gid_used);**/

           garpTimerAdd(my_port->application->vlan_id, my_port->port_no,
                      (L7_uint32)(my_port->leave_timeout),
                      &(my_port->leave_timer_instance), GARP_TIMER_GMRP_LEAVE);

           my_port->leave_timer_running = L7_TRUE;
       }
     }
   }

   my_port->cstart_leave_timer  = L7_FALSE;
}

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
void gid_leave_timer_expired(L7_uint32 port_no)
{
    Garp      *application;
    Gid       *my_port, *to_port;
    L7_int32  gid_index;
    L7_BOOL   any_expired_attributes = L7_FALSE;

    /* for debug*/
    Vlan_id       key;

    GARP_TRACE_PROTOCOL_EVENTS("gid_leave_timer_expired:  port %d\n", port_no);

    application = &gvrp_app->g;

    if (gid_find_port(application->gid, port_no, (void*)&my_port))
    {
        /* turn off the "Leave Timer"-running indication flag*/
        my_port->leave_timer_running = L7_FALSE;


        /* check if there are any dynamically-registered attributes
         * whose Registrar(s) in Leaving state (Lv) */
        for (gid_index = (my_port->application->last_gid_used-1);
            gid_index >= 0; gid_index--)
        {
            if ((L7_TRUE == my_port->machines[gid_index].incoming_port)
             &&(Lv == my_port->machines[gid_index].registrar))
            {
                /* the my_port->machines[gid_index] dynamic attribute has just
                 * expired;  propagate the ReqLeave to other ports */
                gip_propagate_leave(my_port, gid_index);
                any_expired_attributes = L7_TRUE;
            }
        }

        if (L7_FALSE == any_expired_attributes)
        {
            /* if no dynamic attributes expired, there is nothing to do.
             * This scenario is possible if JoinIn message(s) was received
             * after the Leave timer was started: after receiving the JoinIn,
             * the attribute(s) transitioned to Registered (Inn) state.*/
             return;
        }

        to_port = my_port;

        /* go through all connected ports and transmit the LeaveEmpty message
         * for the attribute(s) that just expired.  Note that we don't start
         * JoinIn timer, we just send out the message right here -- and this
         * behavior is still standard-compliant. */
        while ((to_port = to_port->next_in_connected_ring) != my_port)
        {
            to_port->tx_pending = L7_TRUE;
            to_port->application->transmit_fn(to_port->application, to_port);
        }


        /* for all dynamic attributes that expired, the leave_indication_fn()
         * removes the entry from the database (if the attribute
         * is registered on one port only and no static VLAN with the same
         * VID exists) */
         for (gid_index = (my_port->application->last_gid_used-1);
              gid_index >= 0; gid_index--)
        {
            if ((L7_TRUE == my_port->machines[gid_index].incoming_port) &&
               (Lv == my_port->machines[gid_index].registrar))
            {
                my_port->application->leave_indication_fn(my_port->application,
                                                           my_port, gid_index);
            }
            else
            {
              (void) gvd_get_key(gid_index, &key);
              /* uncomment this line for debug */
              /*GARP_TRACE_INTERNAL_EVENTS("%s:port: %d gid_index:%d Vlan Id %u incoming port %d registrar %d\n",
                     __FUNCTION__,my_port->port_no, gid_index,key, my_port->machines[gid_index].incoming_port,
                     my_port->machines[gid_index].registrar);  */
             }
        }
    }

    return;
}


/*********************************************************************
* @purpose  leave timer expiry function.
*
* @param    vid       vlan id
* @param    port_no   port number
*
* @returns  none
*
* @notes    The instance of GMRP will be validated using the VLAN ID
*
* @notes
*
* @end
*********************************************************************/
void gid_gmr_leave_timer_expired(L7_uint32 vid, L7_uint32 port_no)
{
    Gmr *gmr_inst;
    Garp *gmr_app;
    Gid  *my_port, *to_port;
    L7_uint32  gid_index;
    L7_BOOL any_expired_attributes = L7_FALSE;

    gmr_inst = gmrpInstanceGet(vid);

    GARP_TRACE_PROTOCOL_EVENTS("gid_gmr_leave_timer_expired:  vlan %d, port %d\n",
                               vid, port_no);

    if (gmr_inst == L7_NULLPTR)
    {
        /* This failure is OK.  The instance of GMRP has aged after the
           timer popped but before the timer was serviced.  */
        return;
    }

    gmr_app = &gmr_inst->g;

    if (gid_find_port(gmr_app->gid, port_no, (void*)&my_port))
    {

        /* turn off the "Leave Timer"-running indication flag*/
        my_port->leave_timer_running = L7_FALSE;


        /* check if there are any dynamically-registered attributes
         * whose Registrar(s) in Leaving state (Lv) */
        for (gid_index = (my_port->application->last_gid_used-1);
             gid_index >= Number_of_legacy_controls; gid_index--)
        {
            if ((L7_TRUE == my_port->machines[gid_index].incoming_port) &&
               (Lv == my_port->machines[gid_index].registrar))
            {
                /* the my_port->machines[gid_index] dynamic attribute has just
                 * expired;  propagate the ReqLeave to other ports */
                gip_propagate_leave(my_port, gid_index);
                any_expired_attributes = L7_TRUE;
            }
        }

        if (L7_FALSE == any_expired_attributes)
        {
            /* if no dynamic attributes expired, there is nothing to do.
             * This scenario is possible if JoinIn message(s) was received
             * after the Leave timer was started: after receiving the JoinIn,
             * the attribute(s) transitioned to Registered (Inn) state.*/
            return;
        }

        to_port = my_port;


        /* go through all connected ports and transmit the LeaveEmpty message
         * for the attribute(s) that just expired.  Note that we don't start
         * JoinIn timer, we just send out the message right here -- and this
         * behavior is still standard-compliant. */
        while ((to_port = to_port->next_in_connected_ring) != my_port)
        {
            to_port->tx_pending = L7_TRUE;
            to_port->application->transmit_fn(to_port->application,to_port);
        }

        /* for all dynamic attributes that expired, the leave_indication_fn()
         * removes the entry from the databases (if the attribute
         * is registered on one port only) */
        for (gid_index = (my_port->application->last_gid_used-1);
             gid_index >= Number_of_legacy_controls; gid_index--)
        {
            if ((L7_TRUE == my_port->machines[gid_index].incoming_port) &&
                (Lv == my_port->machines[gid_index].registrar))
            {
                my_port->application->leave_indication_fn(my_port->application,
                                                          my_port, gid_index);
            }
        }
  }

  return;
}


/*********************************************************************
* @purpose  Leave all timer expiry function.
*
* @param    L7_uint32     port number
*
* @returns  none
*
* @notes
*
* @end
*********************************************************************/
void gid_leaveall_timer_expired(L7_uint32 port_no)
{
  Garp *application;
  Gid  *my_port;
  L7_double64 timeout;
  L7_uint32   join_timeout = 0;

  application = &gvrp_app->g;

  GARP_TRACE_PROTOCOL_EVENTS("gid_leaveall_timer_expired: port %d\n",
                             port_no);

  if (gid_find_port(application->gid, port_no, (void*)&my_port))
  {
      /* send the leave all to all machines */
      gid_leaveall(my_port);

      my_port->transmit_leaveall = L7_TRUE;
      my_port->cstart_join_timer = L7_TRUE;

      /* transmit the LeaveAll message */
      application->transmit_fn(application, my_port);


        /* For the sake of efficiency, we will start Join In timer
         * after expiration of Leave All timers only if
         *
         *    1) the port is enabled and Join Timer event was either lost
         *       by gid_join_timer_expired_to_queue() or
         *       Join Timer is not running on the current port, and
         *
         *    2) The current port is either connected, or there are
         *       dynamically registered attributes on the port that
         *       need to expire.
         *
         *  In other cases starting Join In timer is senseless.
         *  For example, if the port is enabled but not connected and there
         *  is only default VLAN registered on that port, it makes
         *  no sense to start Join In -- it doesn't do anything.
         */
        if ((!(my_port->join_timer_running) || (failed_timer[port_no].gvrp_join_in))
            && (my_port->is_enabled)
            && ((my_port->is_connected)
            || isAttributeRegistered(my_port->machines,my_port->application->last_gid_used-1)))
      {
          my_port->join_timer_running = L7_TRUE;
          failed_timer[port_no].gvrp_join_in = L7_FALSE;
          timeout = L7_Random();
          join_timeout = timeout * my_port->join_timeout;

          garpTimerAdd ( 0, my_port->port_no,
                         join_timeout,
                         &(my_port->join_timer_instance), GARP_TIMER_GVRP_JOIN);

            /* uncomment the line below for debugging print statements */
            /*sysapiPrintf("\n Join In timer Port=%u", my_port->port_no);*/
        }

        if (my_port->cstart_leave_timer && (!my_port->leave_timer_running))
        {
            if (my_port->application->last_gid_used > 0)
            {
                /* make sure that we only start the Leave timer if there are any
                 * dynamically registered attributes on the current port.
                 * Otherwise, starting Leave timer is useless -- it won't
                 * do anything except wasting processor cycles.
                 */
                if (L7_TRUE == isAttributeRegistered(my_port->machines,
                                       my_port->application->last_gid_used-1))
                {
                    /* uncomment the line below for debugging print statements */
                    /*sysapiPrintf("\nGVRP Leave timer Port=%u, last_gid_used=%u\n",
                                 my_port->port_no, my_port->application->last_gid_used);*/

                    garpTimerAdd(0, my_port->port_no,
                               (L7_uint32)(my_port->leave_timeout),
                               &(my_port->leave_timer_instance), GARP_TIMER_GVRP_LEAVE);

                    my_port->leave_timer_running = L7_TRUE;
                    my_port->cstart_leave_timer  = L7_FALSE;
                }
            }
        }
    }

    return;
}

/*********************************************************************
* @purpose  Leave all timer expiry function for GMRP
*
* @param    L7_uint32     VLAN Id
* @param    Gid           pointer to port structure
* @param    Gmr           pointer GMR instance structure
*
* @returns  none
*
* @notes
*
* @end
*********************************************************************/
void gid_gmr_leaveall_timer_expired(L7_uint32 vid, Gid *my_port, Gmr *gmr_inst)
{
  L7_double64 timeout;
  L7_uint32   join_timeout = 0;

  if (((Gid *)L7_NULL == my_port) || ((Gmr *)L7_NULL == gmr_inst))
  {
      return;
  }

  osapiSemaTake(gmr_inst->gmrSema, L7_WAIT_FOREVER);

  {
      /* Debug Block */
      L7_dll_member_t *local_member;
      L7_dll_t      *local_gmd;
      gmrp_index_t *local_index;
      L7_ushort16   vlanId = 0;
      L7_uchar8     macAddr[L7_MAC_ADDR_LEN];
      L7_uchar8     buf[32];
      L7_uint32     i;

      local_gmd = gmr_inst->gmd;
      if (DLLFirstGet(local_gmd,&local_member)==L7_SUCCESS)
      {
        local_index = (gmrp_index_t *)local_member->data;
        /*need to delete these entries from the the mfdb too!!!*/
        memcpy((void *)&vlanId,(void *)local_index->vlanIdmacAddress,2);
        memcpy((void *)macAddr,(void *)&local_index->vlanIdmacAddress[2],L7_MAC_ADDR_LEN);

        memset(buf, 0,sizeof(buf));
        for (i = 0;i < L7_MAC_ADDR_LEN ;i++)
        {
            sprintf(buf,"%02x:",macAddr[i]);
        }
      }

      GARP_TRACE_PROTOCOL_EVENTS("gid_gmr_leaveall_timer_expired: vid %d,vlan %d, port %d, address: %s\n",
                                 vid, vlanId, my_port->port_no, buf);
  }


  /* send the leave all to all machines */
  gid_leaveall(my_port);

  my_port->cstart_join_timer = L7_TRUE;

  /* transmit the LeaveAll message */
  gmr_inst->g.transmit_fn(&(gmr_inst->g), my_port);

  /* For the sake of efficiency, we will start Join In timer
   * after expiration of Leave All timers only if
   *
   *    1) the port is enabled and Join Timer is not running
   *       on the current port, and
   *
   *    2) The current port is either connected, or there are
   *       dynamically registered attributes on the port that
   *       need to expire.
   *
   *  In other cases starting Join In timer is senseless.
   *  For example, if the port is enabled but not connected and there
   *  is no dynamic regiatrations on that port/GMRP instance, it makes
   *  no sense to start Join In timer -- it doesn't do anything.
   */
  if (!(my_port->join_timer_running) && (my_port->is_enabled)
      && ((my_port->is_connected)
      || isAttributeRegistered(my_port->machines,my_port->application->last_gid_used-1)))
  {
      my_port->join_timer_running = L7_TRUE;
      timeout = L7_Random();
      join_timeout = timeout * my_port->join_timeout;

      garpTimerAdd (vid, my_port->port_no, join_timeout,
                    &(my_port->join_timer_instance), GARP_TIMER_GMRP_JOIN);
  }


  if (my_port->cstart_leave_timer && (!my_port->leave_timer_running))
  {
      if (my_port->application->last_gid_used > 1)
      {
          if (L7_TRUE == isAttributeRegistered(my_port->machines,
                                         my_port->application->last_gid_used-1))
          {
              /* uncomment the line below for debugging print statements */
              /* sysapiPrintf("\nGMRP Leave timer Port=%u, last_gid_used=%u",
                  my_port->port_no, my_port->application->last_gid_used);**/

              garpTimerAdd(my_port->application->vlan_id, my_port->port_no,
                         (L7_uint32)(my_port->leave_timeout),
                         &(my_port->leave_timer_instance), GARP_TIMER_GMRP_LEAVE);

              my_port->leave_timer_running = L7_TRUE;
              my_port->cstart_leave_timer  = L7_FALSE;
          }
      }
  }

  osapiSemaGive(gmr_inst->gmrSema);
  return;
}

/*********************************************************************
* @purpose  Join timer expiry function.
*
* @param    L7_uint32     port number
*
* @returns  none
*
* @notes
*
* @end
*********************************************************************/
void gid_join_timer_expired(L7_uint32 port_no)
{
  Gid  *my_port;
  Garp *application;


  GARP_TRACE_PROTOCOL_EVENTS("gid_join_timer_expired: port %d, \n", port_no);

  application = &gvrp_app->g;

  if (gid_find_port(application->gid, port_no, (void*)&my_port))
  {
    my_port->join_timer_running   = L7_FALSE;

    if (my_port->is_enabled)
    {
        application->transmit_fn(application, my_port);

        gid_do_actions(my_port);
    }

  }

  return;
}

/*********************************************************************
* @purpose  GMRP join timer expiry function
*
* @param    L7_uint32     VLAN Id
* @param    L7_uint32     port number
*
* @returns  none
*
* @notes    The instance of GMRP will be validated using the VLAN ID
*
* @end
*********************************************************************/
void gid_gmr_join_timer_expired(L7_uint32 vid, L7_uint32 port_no)
{
  Gmr  *gmr_inst;
  Garp *gmr_app;
  Gid  *my_port;

  GARP_TRACE_PROTOCOL_EVENTS("gid_gmr_join_timer_expired: vid %d, port %d, \n",
                             vid, port_no);

  gmr_inst = gmrpInstanceGet(vid);

  if (gmr_inst == L7_NULLPTR)
  {
      /* This failure is OK.  The instance of GMRP has aged after the
         timer popped but before the timer was serviced.  */
      return;
  }

  gmr_app = &gmr_inst->g;

  if (gid_find_port(gmr_app->gid, port_no, (void*)&my_port))
  {
    my_port->join_timer_running   = L7_FALSE;

    if (my_port->is_enabled)
    {
        gmr_app->transmit_fn(gmr_app, my_port);

        gid_do_actions(my_port);
    }
  }

  return;
}


