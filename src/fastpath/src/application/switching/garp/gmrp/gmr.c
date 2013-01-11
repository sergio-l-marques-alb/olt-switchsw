/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    gmr.c
* @purpose     GMR defintions and functions
* @component   GARP
* @comments    none
* @create      02/23/2001
* @author      Hassan
* @author
* @end
*
**********************************************************************/
/*************************************************************

*************************************************************/

#include "string.h"
#include "l7_common.h"
#include "defaultconfig.h"
#include "osapi.h"
#include "sysapi.h"
#include "log.h"
#include "nimapi.h"
#include "dtlapi.h"
#include "dot1q_api.h"
#include "garpapi.h"
#include "garpctlblk.h"
#include "garpcfg.h"
#include "gidapi.h"
#include "gipapi.h"
#include "gmrapi.h"
#include "gmrp_api.h"
#include "gmf.h"
#include "gmd.h"
#include "osapi.h"
#include "dot1s_api.h"
#include "comm_mask.h"
#include "mfdb_api.h"
#include "cnfgr.h"
#include "gidtt.h"
#include "garp_debug.h"
#include "garpcfg.h"
#include "dll_api.h"

/*----------------------------------------------------------------------------*/
/* GMR : GARP MULTICAST REGISTRATION APPLICATION : IMPLEMENTATION SIZING      */
/*----------------------------------------------------------------------------*/


#define Gmr_Unused_index           Gmr_Number_of_gid_machines

static L7_uchar8 tx_pdu[GARP_MAX_PDU_SIZE];

#define GMRP_MAX_MSG_SIZE    8

extern void *globalGmd;
extern gmrpInfo_t *gmrpInfo;
extern gmrpInstance_t *gmrpInstance;
extern GARPCBptr GARPCB;
extern GarpInfo_t garpInfo;

static const L7_uchar8 Garp_gmrp_mac_address[6] =
{
  0x01, 0x80, 0xc2, 0x00, 0x00, 0x20
};

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
* @notes    Creates a new instance of GMR, allocating and initialising a control
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
L7_BOOL gmr_create_gmr(L7_uint32 process_id, L7_ushort16 vlan_id, void **gmr)
{
  Gmr      *my_gmr = (Gmr *)osapiMalloc(L7_GMRP_COMPONENT_ID, (L7_uint32)sizeof(Gmr));
  L7_BOOL  returnVal = L7_FALSE;
  L7_BOOL rc;


  GARP_TRACE_INTERNAL_EVENTS("GMRP: Create GMR request for vlan_id %d\n", vlan_id);

  if (my_gmr != L7_NULL)
  {
    my_gmr->g.process_id = (L7_int32)process_id;
    my_gmr->g.gid        = L7_NULL;
    /* Account for 1 based array of machines */
    if (gip_create_gip((Gmr_Number_of_gid_machines +1), &my_gmr->g.gip) == L7_TRUE)
    {
      my_gmr->g.max_gid_index = Gmr_Number_of_gid_machines - 1;
      my_gmr->g.last_gid_used = Number_of_legacy_controls/* - 2*/;/*SK-Service Req*/

      my_gmr->g.join_indication_fn  = gmr_join_indication;
      my_gmr->g.leave_indication_fn = gmr_leave_indication;
      my_gmr->g.join_propagated_fn  = gmr_join_propagated;
      my_gmr->g.leave_propagated_fn = gmr_leave_propagated;
      my_gmr->g.transmit_fn         = gmr_tx;
      my_gmr->g.added_port_fn       = gmr_added_port;
      my_gmr->g.removed_port_fn     = gmr_removed_port;

      my_gmr->vlan_id = vlan_id;

      rc = gmd_create_gmd(&my_gmr->gmd);
      my_gmr->number_of_gmd_entries = Max_multicasts;
      my_gmr->last_gmd_used_plus1   = 0;

      *gmr = my_gmr;

      my_gmr->g.app = GARP_GMRP_APP;
      my_gmr->g.vlan_id = vlan_id;

      my_gmr->gmrSema = osapiSemaBCreate( OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
      if(my_gmr->gmrSema == L7_NULL)
         return L7_FALSE;

      returnVal = L7_TRUE;
    }
    else
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GARP_COMPONENT_ID,
              "GMRP: Failed to create GIP for vlan_id %d\n", vlan_id);
      osapiFree(L7_GMRP_COMPONENT_ID, my_gmr);
    }
  }
  else
  {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GARP_COMPONENT_ID,
              "GMRP: Failed to allocate Gmr for vlan_id %d\n", vlan_id);
  }

  return returnVal;
}

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
void gmr_destroy_gmr(void *my_gmr)
{
  Gid *my_port;
  Gmr *local_gmr = (Gmr*)my_gmr;
  L7_uint32 index;
  L7_RC_t rc;
  L7_BOOL rc1;

  GARP_TRACE_INTERNAL_EVENTS("Destroying GMR instance %d for vlan %d\n",
                             local_gmr->vlan_id);

  rc = osapiSemaTake(local_gmr->gmrSema,L7_WAIT_FOREVER);


  /* Moved  gmrpEntryDelete from gmd_destroy_gmd to here. */
  gmr_remove_gmrp_entry(local_gmr->gmd);
  while ((my_port = local_gmr->g.gid) != L7_NULL)
  {
    rc1 = gid_destroy_port(&local_gmr->g, my_port->port_no);
  }
  gmd_destroy_gmd(local_gmr->gmd);

  gip_destroy_gip(local_gmr->g.gip);

  rc = gmrpInstanceIndexFind(local_gmr->vlan_id, &index);

  gmrpInstance[index].inuse = L7_FALSE;
  gmrpInstance[index].gmr = L7_NULLPTR;

  rc = osapiSemaGive(local_gmr->gmrSema);
  rc = osapiSemaFlush(local_gmr->gmrSema);
  rc = osapiSemaDelete(local_gmr->gmrSema);
  bzero((char*)local_gmr,(L7_int32)sizeof(Gmr));
  osapiFree(L7_GMRP_COMPONENT_ID, my_gmr);
}

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
*          if (my_gmr->vlan_id == Lan)
*          {
*              if stp_forwarding(port_no) gmr_connect_port(port_no);
*          }
*          else if vlan_forwarding(vlan_id, port_no)
*          {
*              gmr_connect_port(port_no);
*          }
*
*          As the system continues to run it should invoke gmr_disconnect_port()
*          and gmr_connect_port() as required to maintain the required connectivity.
*
* @end
*********************************************************************/
void gmr_added_port(void *my_gmr, L7_uint32 port_no)
{
  /*L7_RC_t rc;*/
   /*
  * Provide any management intialization of legacy control or multicast
  * attributes from templates here for the new port.
  */
  /*rc = dtlDot1dGMRPEnable(port_no);*/
}


/*********************************************************************
* @purpose  process a GMR command
*
* @param    my_gmr        pointer to gmrp
* @param    port_no       port number
*
* @returns  void
*
* @notes    The system has removed and destroyed the GID port. This function should
*           provide any application specific cleanup required.
*
* @end
*********************************************************************/
void gmr_removed_port(void *my_gmr, L7_uint32 port_no)
{
  /*L7_RC_t rc; */
  /*
  * Provide any GMR specific cleanup or management alert functions for the
  * removed port.
  */
  /*rc = dtlDot1dGMRPDisable(port_no);*/
}

/*---------------------------------------------------------------------------*/
/* GMR : GARP MULTICAST REGISTRATION APPLICATION : JOIN, LEAVE INDICATIONS   */
/*---------------------------------------------------------------------------*/
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
*         is in Legacy mode B (Gmr_Forward_unregistered set (registered) for that port,
*         but not Gmr_Forward_all) then registration of a multicast address on this
*         port can cause it to be filtered on that other port. This is handled by
*         gmr_join_propagated() for the other ports which may be effected. It will
*         be called as a consequence of the GIP propagation of the newly registered
*         attribute (multicast address).
*
* @end
*********************************************************************/
void gmr_join_indication(void *my_gmr, void *my_port, L7_uint32 joining_gid_index)
{
  L7_ushort16     gmd_index;
  /*L7_ushort16     gid_index;*/
  L7_uchar8       key[L7_MAC_ADDR_LEN];
  Gmr             *local_gmr = (Gmr*)my_gmr;
  Gid             *local_port = (Gid*)my_port;
  L7_BOOL         rc1;
  L7_RC_t         rc;

  if (gid_registered_here(local_port, Gmr_Forward_all) == L7_FALSE)
  {
    if ((joining_gid_index == Gmr_Forward_all) ||
        (joining_gid_index == Gmr_Forward_unregistered))
    {
#ifdef GMRP_SERVICE
      gmd_index = 0; gid_index = gmd_index + Number_of_legacy_controls;

      while (gmd_index < local_gmr->last_gmd_used_plus1)
      {
        if (!gid_registered_here(local_port, gid_index))
        {
          if (joining_gid_index == Gmr_Forward_all)
          {
            gmd_get_key(local_gmr->gmd, gmd_index, key);

          }
          else if (!gip_propagates_to(local_port, gid_index))
          {     /*(joining_gid_index == Gmr_Forward_unregistered) */
            gmd_get_key(local_gmr->gmd, gmd_index, key);
          }
        }
        gmd_index ++;
        gid_index ++;
      }


#endif /* GMRP_SERVICE*/
    }
    else /* Multicast Attribute */
    {
      gmd_index = joining_gid_index - Number_of_legacy_controls;
      rc1 = gmd_get_key(local_gmr->gmd, gmd_index, key);

      /*put this port to fwd this valn id group mac address*/
      rc = gmrpFwdPortAdd(local_gmr->vlan_id,key,local_port->port_no);

      {
          /* Debug Block */
          L7_uchar8     buf[32];
          L7_ushort16   vlanId;
          L7_uint32     i;

          memcpy((void *)&vlanId,(void *)key,2);
          memset(buf, 0,sizeof(buf));
          for (i = 2;i < L7_MAC_ADDR_LEN ;i++)
          {
              sprintf(buf,"%02x:",key[i]);
          }
          GARP_TRACE_PROTOCOL_EVENTS("gmr_join_indication: port %d, vlan %d, address: %s\n",
                                     local_port->port_no, vlanId, buf);
      }  /* end debug block */

    }
  }
}

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
void gmr_join_propagated(void *my_gmr, void *my_port, L7_uint32 joining_gid_index)
{
  L7_ushort16     gmd_index;
  L7_uchar8       key[L7_MAC_ADDR_LEN];
  Gmr             *local_gmr = (Gmr*)my_gmr;
  Gid             *local_port = (Gid*)my_port;
  L7_BOOL         rc1;

  if (joining_gid_index >= Number_of_legacy_controls)
  {
    /* Multicast attribute */
    if ((!gid_registered_here(local_port, Gmr_Forward_all)) &&
        (gid_registered_here(local_port, Gmr_Forward_unregistered)) &&
        (!gid_registered_here(local_port, joining_gid_index)))
    {
      gmd_index = joining_gid_index - Number_of_legacy_controls;
      rc1 = gmd_get_key(local_gmr->gmd, gmd_index, key);

      {
          /* Debug Block */
          L7_uchar8     buf[32];
          L7_ushort16   vlanId;
          L7_uint32     i;

          memcpy((void *)&vlanId,(void *)key,2);
          memset(buf, 0,sizeof(buf));
          for (i = 2;i < L7_MAC_ADDR_LEN ;i++)
          {
              sprintf(buf,"%02x:",key[i]);
          }
          GARP_TRACE_PROTOCOL_EVENTS("gmr_join_propagated: port %d, vlan %d, address: %s\n",
                                     local_port->port_no, vlanId, buf);
      }  /* end debug block */

    }
  }
}

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
void gmr_leave_indication(void *my_gmr, void *my_port, L7_uint32 leaving_gid_index)
{
  L7_ushort16     gmd_index;
  L7_ushort16     gid_index = 0;
  L7_BOOL         mode_a;
  L7_BOOL         mode_c;
  L7_uchar8       key[L7_MAC_ADDR_LEN];
  Gmr             *local_gmr /*= (Gmr*)my_gmr*/;
  Gid             *local_port = (Gid*)my_port;
  Garp          *local_garp = (Garp*)my_gmr;
  L7_uchar8       vidMac[2+L7_MAC_ADDR_LEN];
  Gid           *to_port;
  L7_BOOL       morePorts = L7_FALSE;
  L7_uint32     j;
  L7_uint32     vlanId;
  L7_BOOL       rc1;
  L7_RC_t       rc;
  L7_INTF_MASK_t forwardingMask;
  L7_BOOL       anotherFwdLink = L7_FALSE;
  L7_ushort16   vlanIdShort;
  mfdbUserInfo_t userInfo;

  vlanId = local_garp->vlan_id;
  vlanIdShort = ((L7_ushort16)vlanId);

  local_gmr = gmrpInstanceGet(vlanId);
  if(local_gmr == L7_NULLPTR)
    return;

  mode_a =  gid_registered_here(local_port, Gmr_Forward_all);
  mode_c = !gid_registered_here(local_port, Gmr_Forward_unregistered);

  if ((leaving_gid_index == Gmr_Forward_all) ||
      ((!mode_a) && (leaving_gid_index == Gmr_Forward_unregistered)))
  {
#ifdef GMRP_SERVICE
    gmd_index = 0; gid_index = gmd_index + Number_of_legacy_controls;
    while (gmd_index < local_gmr->last_gmd_used_plus1)
    {
      if (!gid_registered_here(local_port, gid_index))
      {
        if (mode_c || gip_propagates_to(local_port, gid_index))
        {
          gmd_get_key(local_gmr->gmd, gmd_index, key);

        }
      }

      gmd_index ++;
      gid_index ++;
    }


#endif /*GMRP_SERVICE*/
  }
  else if (!mode_a)
  {
    if (mode_c || gip_propagates_to(local_port, gid_index))
    {
      /* Multicast Attribute */
      gmd_index = leaving_gid_index - Number_of_legacy_controls;

      rc1 = gmd_get_key(local_gmr->gmd, gmd_index, key);

      rc = gmrpFwdPortDelete(local_gmr->vlan_id,key,local_port->port_no);


      {
          /* Debug Block */
          L7_uchar8     buf[32];
          L7_ushort16   vlanId;
          L7_uint32     i;

          memcpy((void *)&vlanId,(void *)key,2);
          memset(buf, 0,sizeof(buf));
          for (i = 2;i < L7_MAC_ADDR_LEN ;i++)
          {
              sprintf(buf,"%02x:",key[i]);
          }
          GARP_TRACE_PROTOCOL_EVENTS("gmr_leave_indication: port %d, vlan %d, address: %s\n",
                                     local_port->port_no, vlanId, buf);
      }  /* end debug block */
    }

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
          /* yes, its registered over one more port */
          morePorts = L7_TRUE;
          break;
        }
      }
      /* advance to next in the ring */
      to_port = to_port->next_in_connected_ring;
    } while (to_port != local_port);


    /* delete the attribute if its not registered over more than one port */
    /* and there is only one port left in the ring                        */
    if (morePorts == L7_FALSE)
    {
      /* delete the attribute from Database */
      if(leaving_gid_index >=2) /*SK-Service Req*/
      {
        rc1 = gmd_get_key(local_gmr->gmd, leaving_gid_index-Number_of_legacy_controls, key);
        if (rc1 != L7_TRUE)
        {
          return;
        }
        rc = gmrpFwdPortDelete(local_gmr->vlan_id, key, local_port->port_no);
        if (rc != L7_SUCCESS)
        {
          return;
        }
        /* Before deleting the entry make sure that this entry does not have
         * any other forwarding ports. Need to check again even though this is
         * encased in a loop that checks for other ports in the ring, the loop
         * does not ensure if the port has been disabled for this vlanId.
         */
        memset(&forwardingMask, 0x00, sizeof(L7_INTF_MASK_t));
        memcpy((char *)&vidMac[0],&vlanIdShort,L7_FDB_IVL_ID_LEN);
        memcpy((char *)&vidMac[2], key, L7_MAC_ADDR_LEN);
        rc = mfdbEntryProtocolInfoGet(vidMac, L7_MFDB_PROTOCOL_GMRP, &userInfo);
        if (rc == L7_SUCCESS)
        {
          /* If there are no more forwarding ports for this entry, then delete
             the entry from mfdb */
          L7_INTF_NONZEROMASK(userInfo.fwdPorts, anotherFwdLink);
          if (anotherFwdLink == L7_FALSE)
          {
            rc1 = gmd_delete_entry(local_gmr->gmd, leaving_gid_index-Number_of_legacy_controls);
            rc = gmrpEntryDelete(local_gmr->vlan_id,key);

            to_port = local_port;
            do
            {
              for (j = leaving_gid_index ; j < (L7_uint32)local_gmr->g.last_gid_used ; j++)
              {
                to_port->machines[j].applicant = to_port->machines[j+1].applicant;
                to_port->machines[j].registrar = to_port->machines[j+1].registrar;
                to_port->machines[j].incoming_port = to_port->machines[j+1].incoming_port;
                to_port->application->gip[j] = to_port->application->gip[j+1];
              }

              if ((to_port->last_transmitted >= leaving_gid_index) &&
                  (to_port->last_transmitted > 2))/*SK-Service Req*/
              {
                to_port->last_transmitted--;
              }

              to_port = to_port->next_in_port_ring;
            } while (to_port != local_port);
            if (to_port->application->last_gid_used > 2)/*SK-Service Req*/
            {
              to_port->application->last_gid_used--;
            }

            if (local_gmr->last_gmd_used_plus1 > 0)
            {
              local_gmr->last_gmd_used_plus1--;
            }
          } /*if (anotherFwdLink == L7_FALSE)*/
        } /*if (rc == L7_SUCCESS)*/
      } /*if(leaving_gid_index >=2)*/
    } /* end morePorts == L7_FALSE */
    else if (morePorts == L7_TRUE)
    {
      to_port = local_port;

      to_port->machines[leaving_gid_index].incoming_port = L7_FALSE;

      /* the registration on to_port has expired, so set Registrar to Mt (Empty).
       * Don't change the Applicant (which is in Qa state most likely), since
       * other port(s) still has/have this attribute registered, and the Applicant
       * should send JoinEmpty messages from the current port
       * upon Leave All events. */
      to_port->machines[leaving_gid_index].registrar     = Mt;
    }
  }}
}

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
void gmr_leave_propagated(void *my_gmr, void *my_port, L7_uint32 leaving_gid_index)
{
  L7_ushort16     gmd_index;
  L7_uchar8       key[L7_MAC_ADDR_LEN];
  Gmr             *local_gmr = (Gmr*)my_gmr;
  Gid             *local_port = (Gid*)my_port;
  L7_BOOL         rc1;

  if (leaving_gid_index >= Number_of_legacy_controls)
  {
    /* Multicast attribute */
    if ((!gid_registered_here(local_port, Gmr_Forward_all)) &&
        (gid_registered_here(local_port, Gmr_Forward_unregistered)) &&
        (!gid_registered_here(local_port, leaving_gid_index)))
    {
      gmd_index = leaving_gid_index - Number_of_legacy_controls;
      rc1 = gmd_get_key(local_gmr->gmd, gmd_index, key);

      {
          /* Debug Block */
          L7_uchar8     buf[32];
          L7_ushort16   vlanId;
          L7_uint32     i;

          memcpy((void *)&vlanId,(void *)key,2);
          memset(buf, 0,sizeof(buf));
          for (i = 2;i < L7_MAC_ADDR_LEN ;i++)
          {
              sprintf(buf,"%02x:",key[i]);
          }
          GARP_TRACE_PROTOCOL_EVENTS("gmr_leave_propagated: port %d, vlan %d, address: %s\n",
                                     local_port->port_no, vlanId, buf);
      }  /* end debug block */


    }
  }
}

/*----------------------------------------------------------------------------*/
/* GMR : GARP MULTICAST REGISTRATION APPLICATION : RECEIVE MESSAGE PROCESSING */
/*----------------------------------------------------------------------------*/
/*********************************************************************
* @purpose  gmr DB is full
*
* @param    my_gmr              pointer to gmrp
* @param    my_port             pointer to port structure
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
static void gmr_db_full(Gmr *my_gmr, Gid *my_port)
{  /*
  * If it is desirable to be able to operate correctly with an undersized
  * database add code here. The best approach seems to be to use GID
  * management controls to configure the attribute for the Legacy mode
  * control Gmr_Forward_all to be Registration fixed on all ports on which join
  * messages have been discarded because their keys are not in the database.
  * Then start a retry timer which attempts to scavenge space from the
  * database at a later time, and if it suceeds waits for a few Leaveall
  * times before switching Gmr_Forward_all back to Normal_registration.
  */
}

/*********************************************************************
* @purpose  Process one received message.
*
* @param    my_gmr              pointer to gmrp
* @param    my_port             pointer to port structure
* @param    msg                 pointer to received message
*
* @returns  void
*
* @notes    Dispatch messages by message event, and by attribute type (legacy mode
*           control, or multicast address) except in the case of the LeaveAll
*           message event which applies equally to all attributes.
*
*           A LeaveAll message never causes an indication (join or leave directly),
*           even for the point to point link protocol enhancements (where an
*           ordinary Leave does). No further work is needed here.
*
*           A LeaveAllRange message is currently treated exactly as a LeaveAll
*           (i.e. the range is ignored).
*
*           All the remaining messages refer to a single attribute (i.e. a single
*           registered group address). Try to find a matching entry in the MCD
*           database. If one is found dispatch the message to a routine which will
*           handle both the local GID effects and the GIP propagation to other ports.
*
*           If no entry is found Leave and Empty messages can be discarded, but
*           JoinIn and JoinEmpty messages demand further treatment. First, an attempt
*           is made to create a new entry using free space (in the database, which
*           corresponds to a free GID machine set). If this fails an attempt may be
*           made to recover space from a machine set which is in an unused or less
*           significant state. Finally the database is consider full and the received
*           message is discarded.
*
*           Once (if) an entry is found, Leave, Empty, JoinIn, and JoinEmpty are
*           all submitted to GID (gid_rcv_msg()).
*
*           JoinIn and JoinEmpty may cause Join indications, which are then propagated
*           by GIP.
*
*           On a shared medium, Leave and Empty will not give rise to indications
*           immediately. However this routine does test for and propagate
*           Leave indications so that it can be used unchanged with a point to point
*           protocol enhancement.
*
* @end
*********************************************************************/
static void gmr_rcv_msg(Gmr *my_gmr, Gid *my_port, Gmf_msg *msg)
{
  L7_uint32 gmd_index = Gmr_Unused_index;
  L7_uint32 gid_index = Gmr_Unused_index;
  L7_uchar8 key[8];
  L7_uint32 gmrp_entries;
  L7_BOOL gmrp_database_is_full;
  L7_BOOL gmr_registration_failed;

  memcpy((void *)key,(void *)&my_gmr->vlan_id,2);
  memcpy((void *)&key[2],(void *)msg->key1,GmrpMacAddressSize);

  if ((msg->event == Gid_rcv_leaveall) ||
      (msg->event == Gid_rcv_leaveall_range))
  {
    gid_gmr_rcv_leaveall(my_gmr, my_port);
  }
  else
  { if (msg->attribute == Gmr_Legacy_attribute)
    {
    #ifdef GMRP_SERVICE
      gid_index = msg->legacy_control;
      my_port->application->last_gid_used = msg->legacy_control+1;
      my_port->application->gip[msg->legacy_control+1] = 0;
    #endif
    }
    else if (!gmd_find_entry(my_gmr->gmd, key, &gmd_index))
    {/*  && (msg->attribute == Gmr_Multicast_attribute) */

      if ((msg->event == Gid_rcv_joinin) ||
          (msg->event == Gid_rcv_joinempty) ||
          (msg->event == Gid_normal_registration) ||
          (msg->event == Gid_fix_registration) ||
          (msg->event == Gid_forbid_registration))
      {
          /* Check whether the database is full before attempting to
             add to the databases */
          gmrp_database_is_full     = L7_TRUE;
          gmr_registration_failed   = L7_TRUE;

          if (garpInfo.infoCounters.gmrpRegistationCount < L7_MAX_GROUP_REGISTRATION_ENTRIES)
          {
              /* Extra sanity check: No need to process a new entry
                 if MFDB database is full from other components. */
              if (mfdbCurrEntriesGet(&gmrp_entries) == L7_SUCCESS)
              {
                  if (gmrp_entries < L7_MFDB_MAX_MAC_ENTRIES)
                  {
                      gmrp_database_is_full = L7_FALSE;
                  }
              }
          }

          if (gmrp_database_is_full != L7_TRUE)
          {
              if (gmd_create_entry(my_gmr->gmd, key, &gmd_index) == L7_TRUE)
              {
                  gmr_registration_failed = L7_FALSE;
              }
          }

        if ( (gmrp_database_is_full == L7_TRUE) ||(gmr_registration_failed == L7_TRUE) )
        {
          {
            gmr_db_full(my_gmr, my_port);
            gmrpInfo->failedReg[my_port->port_no]++;
            gmd_index = Gmr_Unused_index;
          }
        }
        else
        {
          my_gmr->last_gmd_used_plus1++;
          /*my_port->application->last_gid_used++;*/
          my_port->application->last_gid_used = gmd_index + Number_of_legacy_controls+1;
          my_port->application->gip[gmd_index + Number_of_legacy_controls] = 0;
        }
      }  /* msg->event */
    }  /* !gmd_find_entry */

    if (gmd_index != Gmr_Unused_index)
    {
      gid_index = gmd_index + Number_of_legacy_controls;
    }

    if (gid_index != Gmr_Unused_index)
    {
      gid_gmr_rcv_msg(my_gmr, my_port, gid_index, msg->event);
    }
  }
}

/*********************************************************************
* @purpose  Process an entire received pdu for this instance of GMR.
*
* @param    my_gmr              pointer to gmrp
* @param    port_no             port number
* @param    my_pdu                 pointer to received pdu
*
* @returns  void
*
* @notes    Process an entire received pdu for this instance of GMR: initialise
*           the Gmf pdu parsing routine, and, while messages last, read and process
*           them one at a time.
*
* @end
*********************************************************************/
void gmr_rcv(void *gmr, L7_uint32 port_no, void *my_pdu)
{
  Gmf      gmf;
  Gmf_msg  msg;
  Gid     *my_port;
  GarpPdu *pdu = (GarpPdu*)my_pdu;
  Gmr     *my_gmr;
  L7_RC_t rc;
  L7_BOOL valid_event_attribute_rcvd = L7_FALSE;

  my_gmr = (Gmr *)gmr;
  rc = osapiSemaTake(my_gmr->gmrSema, L7_WAIT_FOREVER);
  if (gid_find_port(my_gmr->g.gid,port_no,(void*)&my_port))
  {
    if (my_port->is_enabled && my_port->is_connected)
    {
      if(L7_TRUE == gmf_rdmsg_init(pdu, &gmf))
      {
        while (gmf_rdmsg(&gmf, &msg, &valid_event_attribute_rcvd))
        {
          if(L7_TRUE == valid_event_attribute_rcvd)
          {
            gmr_rcv_msg(my_gmr, my_port, &msg);
          }
        }
        gip_do_actions(my_port);
      }
    }
  }
  rc = osapiSemaGive(my_gmr->gmrSema);
}

/*-----------------------------------------------------------------------------*/
/* GMR : GARP MULTICAST REGISTRATION APPLICATION : TRANSMIT PROCESSING         */
/*-----------------------------------------------------------------------------*/

/*********************************************************************
* @purpose  process a GMR command
*
* @param    my_gmr              pointer to gmrp
* @param    gid_index           index in the gmrp DB
* @param    msg                 pointer to received message
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
static void gmr_tx_msg(Gmr *my_gmr, L7_ushort16 gid_index, Gmf_msg *msg)
{
  L7_ushort16 gmd_index;
  L7_BOOL     rc1;

  if (msg->event == Gid_tx_leaveall)
  {
    msg->attribute      = Gmr_Multicast_attribute;
    /*this needs to be a valid attribute type, the attribute type should be ignored on receipt*/
    /*and just the leave_all event should be processed as the leave_all event applies equally*/
    /*to the group attribute and legacy attribute*/
  }
  else if (gid_index == Gmr_Forward_all)
  {
    msg->attribute      = Gmr_Legacy_attribute;
    msg->legacy_control = Gmr_Forward_all;
  }
  else if (gid_index == Gmr_Forward_unregistered)
  {
    msg->attribute      = Gmr_Legacy_attribute;
    msg->legacy_control = Gmr_Forward_unregistered;
  }
  else /* index for Gmr_Multicast_attribute */
  {
    msg->attribute = Gmr_Multicast_attribute;

    gmd_index = gid_index - Number_of_legacy_controls;
    rc1 = gmd_get_key(my_gmr->gmd, gmd_index, msg->key1);

  }
}

/*********************************************************************
* @purpose  Send Gmr to DTLmessage
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
void SendGMRPPktToDtl(L7_ushort16 length,
                      L7_uint32   port_no,
                      L7_uchar8 * origP,
                      L7_uint32 vlanId)
{
  DTL_CMD_TX_INFO_t dtlCmd;
  L7_uchar8          *dataStart;
  L7_netBufHandle   bufHandle = 0;
  L7_RC_t           rc=L7_FAILURE;
  L7_uint32         activeState = L7_INACTIVE;
  L7_uint32         mode;
  L7_uint32         mstid;
  L7_uint32         portState;
  L7_ushort16       len;

  if (GARP_GMRP_IS_ENABLED == L7_TRUE)
  {
    /* Have to make sure that this port is a member of the vlan context
     * and the port is forwarding in the instance of MSTP that this vlan
     * is associated with if MSTP is supported and enabled. If either dot1d
     * is present or STP (dot1s or dot1d) is disabled, NIM maintains a active
     * list of ports so query NIM.
     */
    rc = dot1qOperVlanMemberGet(vlanId, port_no, &mode);
    if (rc == L7_SUCCESS && mode == L7_DOT1Q_FIXED)
    {
      if (dot1sModeGet() == L7_TRUE)
      {
        mstid = dot1sVlanToMstiGet(vlanId);
        portState = dot1sMstiPortStateGet(mstid, port_no);
        /* The check for manual forwarding takes care of the case when MSTP
         * is disabled for this port
         */
        if (portState == L7_DOT1S_FORWARDING ||
            portState == L7_DOT1S_MANUAL_FWD)
        {
          activeState = L7_ACTIVE;
        }
      }
      else
      {
        /* Either dot1s is not supported or is disabled
         * In both cases NIM will have the active state for this port
         */
        if ( (nimGetIntfActiveState(port_no, &activeState) != L7_SUCCESS))
          return;
      }
      if (activeState == L7_ACTIVE)
      {

        len = length - GARP_802_3_MAC_HEADER_LEN;

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
        bzero((char *)&dtlCmd,(L7_int32)sizeof(DTL_CMD_TX_INFO_t));
        dtlCmd.intfNum = port_no;
        dtlCmd.priority = 1;
        dtlCmd.typeToSend = DTL_NORMAL_UNICAST;

        /*Now in vlan context we need to allow egress processing*/
        dtlCmd.cmdType.L2.domainId = vlanId;

        rc = dtlPduTransmit(bufHandle, DTL_CMD_TX_L2, &dtlCmd);
        if(rc==L7_SUCCESS)
          /* increment stats */
          gmrpInfo->send[port_no]++;
      }/* if activeState == L7_ACTIVE */
    } /* end if port member of vlan */
  } /* end if GARP_GMRP_IS_ENABLED */
}

/*********************************************************************
* @purpose  Transmit a pdu for this instance of GMR.
*
* @param    my_gmr              pointer to gmrp
* @param    gid_index           index in the gmrp DB
*
* @returns  void
*
* @notes    Get and prepare a pdu for the transmission, if one is not available
*           simply return, if there is more to transmit GID will reschedule a call
*           to this function.
*
*           Get messages to transmit from GID and pack them into the pdu using Gmf
*           (MultiCast pdu Formatter).
*
* @end
*********************************************************************/
void gmr_tx(void *my_garp, void *my_port)
{
  /*GarpPdu         *pdu;*/
  Gmf             gmf;
  Gmf_msg         msg;
  Gid_event       tx_event;
  L7_uint32       gid_index;
  L7_uchar8       *p, * origP;
  L7_ushort16     length = 0;
  L7_uchar8       tempLength = 0;
  L7_uchar8       bridge_id[6];
  Gmr             *local_gmr/* = (Gmr*)my_gmr*/;
  Gid             *local_port = (Gid*)my_port;
  Garp            *local_garp = (Garp*)my_garp;
  L7_uint32       mode;
  L7_RC_t         rc;
  L7_BOOL         rc1;

  local_gmr = (Gmr*)gmrpInstanceGet(local_garp->vlan_id);
  if(local_gmr==L7_NULLPTR)
    return;

  /*check if this port is still in the assigned vlan*/
  rc = dot1qOperVlanMemberGet(local_garp->vlan_id,local_port->port_no,&mode);
  if(mode != L7_DOT1Q_FIXED)/*this port is not a member of this vlan*/
    return;

  /* initilize the buffer */
  memset(&tx_pdu[0],0x00,GARP_MAX_PDU_SIZE);

  /* initilize the data pointers */
  p = origP = tx_pdu;

  /* GMRP DESTINATION (Always 01:80:C2:00:00:20) */
  memcpy (p,Garp_gmrp_mac_address,6);
  /* increment the pointer by the size of the address */
  p += 6;

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


    /*check for validity of gmr*/
    local_gmr = (Gmr*)gmrpInstanceGet(local_garp->vlan_id);
    if(local_gmr==L7_NULLPTR)
    {
      /*no need to give the sema as it has been destroyed*/
      return;
    }
    if ((tx_event = gid_gmr_next_tx(local_gmr, local_port, &gid_index)) != Gid_null)
    {
      rc1 = gmf_wrmsg_init(&gmf, (void *)p, local_gmr->vlan_id);

      /* add the protocol id to the length */
      length += 2;

      do
      {
        msg.event = tx_event;

        gmr_tx_msg(local_gmr, (L7_ushort16)gid_index, &msg);

        if ((tempLength = gmf_wrmsg(&gmf, &msg)) == 0)
        {
          gid_untx(my_port);
          break;
        }

        /* add the length for gmr message */
        length += tempLength;

        /* get next event */
        tx_event  = gid_gmr_next_tx(local_gmr, my_port, &gid_index);

        if ((length > (GARP_MAX_PDU_SIZE - GMRP_MAX_MSG_SIZE - 2)) &&
            (tx_event != Gid_null))
        {
          gmf_close_wrmsg(&gmf);

          /* increment length for the the endmark */
          length += 2;

          /* send the packet */
          SendGMRPPktToDtl(length,
                           local_port->port_no,
                           origP,
                           local_gmr->vlan_id);


          /* initilize length */
          length = GARP_802_3_FRAME_HEADER_LEN;

          /* initilize the buffer */
          memset(&tx_pdu[length],0x00,GARP_MAX_PDU_SIZE-length);

          /* start the message from the protocol id again */
          p = &tx_pdu[GARP_802_3_FRAME_HEADER_LEN];

          rc1 = gmf_wrmsg_init(&gmf, (void *)p, local_gmr->vlan_id);

          /* add the protocol id to the length */
          length += 2;

        }

      } while (tx_event != Gid_null);

      gmf_close_wrmsg(&gmf);

      /* increment length for the the endmark */
      length += 2;

      /* send the packet */
      SendGMRPPktToDtl(length,
                       local_port->port_no,
                       origP,
                       local_gmr->vlan_id);
    }
    /*printf("about to give gmr_tx sema\n");*/
  /*printf("gave up gmr_tx sema\n");*/

  }
}

/*********************************************************************
* @purpose  Process a GMR command
*
* @param    Gmr          application
* @param    void*         pointer to command
*
* @returns  event
*
* @notes
*
*
* @end
*********************************************************************/
extern L7_BOOL gmr_proc_command(void *command)
{
  L7_BOOL returnVal = L7_TRUE;
  GARPCommand *cmd;
  Gmf_msg     mesg;
  Gmr *application;
  Gid *my_port = L7_NULLPTR;/* = application->g.gid;*/
  static const L7_uint32 legacy_mask = (L7_uint32)(GARP_GMRP_FWDALL|GARP_GMRP_FWDUN);
  L7_uint32 index;
  L7_RC_t rc = L7_FAILURE;
  L7_BOOL rc1;

  L7_uint32 mode;

  cmd  = (GARPCommand *)command;

  garpTraceProcessGmrCommand(cmd);

  if(cmd->vlan_id != L7_NULL)
  {
    rc = gmrpInstanceIndexFind(cmd->vlan_id, &index);
    if (rc == L7_FAILURE)
    {
      return L7_FALSE;
    }
    application = gmrpInstance[index].gmr;

    if(application->g.gid != L7_NULL)
    {
      if (cmd->port!=0)
      {
        rc1 = gid_find_port(application->g.gid, cmd->port, (void *) &my_port);
      }
      else
      {
        my_port = application->g.gid;
      }
    }



    if (cmd->flags & legacy_mask)
    {
      mesg.attribute = Gmr_Legacy_attribute;
      if (cmd->flags & GARP_GMRP_FWDALL)
      {
        mesg.legacy_control  = Gmr_Forward_all;
      }
      else  /* cmd->flags & GARP_GMRP_FWDUN */
      {
        mesg.legacy_control = Gmr_Forward_unregistered;
      }
    }
    else
    {
      mesg.attribute = Gmr_Multicast_attribute;
      memcpy( mesg.key1, cmd->data, GmrpMacAddressSize );
    }

    switch ( cmd->command )
    {
    case GARP_JOIN:
      mesg.event = Gid_join;
      break;
    case GARP_LEAVE:
      mesg.event = Gid_leave;
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
      /*must check to see if this port is in this vlan*/
      rc = dot1qOperVlanMemberGet(cmd->vlan_id,cmd->port,&mode);
      if(mode == L7_DOT1Q_FIXED)/*indicates include*/
        {rc = osapiSemaTake(application->gmrSema, L7_WAIT_FOREVER);
         rc1 = gid_create_port(&application->g,cmd->port);
         rc = osapiSemaGive(application->gmrSema);
         return L7_TRUE;
        }
      else
        returnVal = L7_FALSE;
      break;
    case GARP_DISABLE_PORT:
      rc = osapiSemaTake(application->gmrSema, L7_WAIT_FOREVER);
      rc1 = gid_destroy_port(&application->g,cmd->port);
      rc = osapiSemaGive(application->gmrSema);
      return L7_TRUE;
      break;
   default:
     returnVal = L7_FALSE;
      break;
   }

  /* If the command can be processed, and the given port  */
  /*  is enabled, go ahead and process the command.       */
    if (returnVal == L7_TRUE)
    {
     if ((my_port!=NULL)&&(my_port->is_enabled))
      {
       gmr_rcv_msg(application,my_port, &mesg);
     }
   }

  }

  return returnVal;
}
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
void gmr_remove_gmrp_entry(void *gmd)
{
  L7_dll_member_t *local_member;
  L7_dll_t *local_gmd;
  gmrp_index_t *local_index;
  L7_ushort16 vlanId;
  L7_uchar8 macAddr[L7_MAC_ADDR_LEN];
  L7_RC_t rc_dll = L7_FAILURE;

  local_gmd = (L7_dll_t *)gmd;
  rc_dll =  DLLFirstGet(local_gmd,&local_member);
  while(rc_dll == L7_SUCCESS)
  {
    local_index = (gmrp_index_t *)local_member->data;
    /*need to delete these entries from the the mfdb too!!!*/
    memcpy((void *)&vlanId,(void *)(local_index->vlanIdmacAddress),2);
    memcpy((void *)macAddr,(void *)&(local_index->vlanIdmacAddress[2]),L7_MAC_ADDR_LEN);
    if (gmrpEntryDelete((L7_uint32)vlanId,macAddr) != L7_SUCCESS)
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GARP_COMPONENT_ID,
                "Error deleting GMRP Entry");
    rc_dll = DLLNextGet(&local_member);
  }
  return;
}

