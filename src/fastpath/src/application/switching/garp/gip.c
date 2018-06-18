/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    gip.c
* @purpose     GIP API internatal to GARP applications
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
#include "string.h"
#include "l7_common.h"
#include "osapi.h"
#include "log.h"
#include "gipapi.h"
#include "garp_debug.h"


/******************************************************************************
 * GIP : GARP INFORMATION PROPAGATION : CREATION, DESTRUCTION
 ******************************************************************************
 */

#define GIP_INSTANCE_NOT_USED     0


multipleGIP_t   *multipleGIP;

/*********************************************************************
* @purpose  initialize multiple GIP data strucyure.
*
* @param    void
*
* @returns  L7_FAILURE
* @returns  L7_SUCCESS
*
* @notes   initialize the multiple GIP instance data structure
*
*
* @end
*********************************************************************/
L7_RC_t gipInitMultipleGIP()
{
    memset(multipleGIP,0x00,sizeof(multipleGIP_t)*L7_NUM_STP_INSTANCES);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  create a GIP instance.
*
* @param    instanceID instance ID
*
* @returns  L7_FAILURE
* @returns  L7_SUCCESS
*
* @notes   create a GIP instance corresponding to the STP instance
*
*
* @end
*********************************************************************/
L7_RC_t gipCreateGIPInstance(L7_uint32 instanceID)
{
    L7_uint32  counter;

    GARP_TRACE_INTERNAL_EVENTS("Creating Gip Instance for MST %d\n",instanceID);

    for (counter = 0; counter < L7_NUM_STP_INSTANCES; counter++)
    {
        if (multipleGIP[counter].inUse == L7_FALSE)
        {
            multipleGIP[counter].inUse = L7_TRUE;
            multipleGIP[counter].intstanceID = instanceID;
            multipleGIP[counter].ringOfPorts = L7_NULL;
            return L7_SUCCESS;
        }
    }
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  delete a GIP instance.
*
* @param    instanceId instance ID
*
* @returns  L7_FAILURE
* @returns  L7_SUCCESS
*
* @notes   delete a GIP instance corresponding to the STP instance
*
*
* @end
*********************************************************************/
L7_RC_t gipDeleteGIPInstance(L7_uint32 instanceID)
{
  L7_uint32 counter;
  portInRing_t *ringOfPorts;
  portInRing_t *nextRingOfPorts;

  GARP_TRACE_INTERNAL_EVENTS("Deleting Gip Instance for MST %d\n",instanceID);

  for (counter = 0; counter < L7_NUM_STP_INSTANCES; counter++)
  {
    if (multipleGIP[counter].intstanceID == instanceID) 
    {
      multipleGIP[counter].inUse = L7_FALSE;
      multipleGIP[counter].intstanceID = GIP_INSTANCE_NOT_USED;
      ringOfPorts = multipleGIP[counter].ringOfPorts;
      while (ringOfPorts != L7_NULL)
      {
        nextRingOfPorts = ringOfPorts->next;
        osapiFree(L7_GARP_COMPONENT_ID, ringOfPorts);
        ringOfPorts = nextRingOfPorts;
      }
      multipleGIP[counter].ringOfPorts = L7_NULL;
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  add a port to a GIP instance.
*
* @param    instanceId    instance ID
* @param    portNumber    port number
*
* @returns  L7_FAILURE
* @returns  L7_SUCCESS
*
* @notes   add a port to a GIP instance corresponding to the STP instance
*
*
* @end
*********************************************************************/
L7_RC_t gipInsertPortGIPInstance(L7_uint32 instanceID, L7_uint32 portNumber)
{
    L7_uint32    counter;
    portInRing_t *firstPort;
    portInRing_t *newRingOfPorts;

    /* if we already have added the port, exit */
    for (counter = 0; counter < L7_NUM_STP_INSTANCES; counter++) 
    {
        if (multipleGIP[counter].intstanceID == instanceID) 
        {
            firstPort = multipleGIP[counter].ringOfPorts;
            while (firstPort != L7_NULL) 
            {
                if (firstPort->portNumber == portNumber) 
                {
                    return L7_SUCCESS;
                }
                firstPort = firstPort->next;
            }
            break;
        }
    }

    GARP_TRACE_INTERNAL_EVENTS("Inserting Port Gip Instance for MST %d and port %d\n",
                               instanceID, portNumber);
    /* allocate memeory for the new port in the ring */
    newRingOfPorts = (portInRing_t *)osapiMalloc(L7_GARP_COMPONENT_ID, (L7_uint32)sizeof(portInRing_t));
    newRingOfPorts->portNumber = portNumber;
    newRingOfPorts->next = L7_NULL;

    for (counter = 0; counter < L7_NUM_STP_INSTANCES; counter++)
    {
        if (multipleGIP[counter].intstanceID == instanceID) 
        {
            firstPort = multipleGIP[counter].ringOfPorts;
            
            if (firstPort == L7_NULL)
            {
                /* this is the first port in the ring */
                multipleGIP[counter].ringOfPorts = newRingOfPorts;
            }
            else
            {
                /* there is more ports in the ring */
                /* always add the port to begining */
                newRingOfPorts->next = firstPort;
                multipleGIP[counter].ringOfPorts = newRingOfPorts;
            }
            
            return L7_SUCCESS;
        }
    }

    return L7_FAILURE;
}


/*********************************************************************
* @purpose  delete port from a GIP instance.
*
* @param    instanceID    instance ID
* @param    portNumber    port number
*
* @returns  L7_FAILURE
* @returns  L7_SUCCESS
*
* @notes   delete a port from a GIP instance corresponding to the STP instance
*
*
* @end
*********************************************************************/
L7_RC_t gipDeletePortGIPInstance(L7_uint32 instanceID,L7_uint32 portNumber)
{
    L7_uint32 counter;
    portInRing_t *currentRingOfPorts;
    portInRing_t *previousRingOfPorts;

    GARP_TRACE_INTERNAL_EVENTS("Deleting Port Gip Instance for MST %d and port %d\n",
                               instanceID, portNumber);

    for (counter = 0; counter < L7_NUM_STP_INSTANCES; counter++)
    {
        if (multipleGIP[counter].intstanceID == instanceID)
        {
            currentRingOfPorts = multipleGIP[counter].ringOfPorts;
            previousRingOfPorts = currentRingOfPorts;
            
            if(currentRingOfPorts != L7_NULL)
            {
                do {

                    if (currentRingOfPorts->portNumber == portNumber)
                    {
                        /* the instance and the port are found */
                        
                        /* if there is only one port in the ring */
                        if ((currentRingOfPorts->next == L7_NULL) &&
                            (previousRingOfPorts == currentRingOfPorts))
                        {
                            multipleGIP[counter].ringOfPorts = L7_NULL;
                        }
                        /* this is the first port in the ring */
                        else if (currentRingOfPorts == multipleGIP[counter].ringOfPorts)
                        {
                            multipleGIP[counter].ringOfPorts = currentRingOfPorts->next;
                        }
                        /* if this is the last port */
                        else if (currentRingOfPorts->next == L7_NULL)
                        {
                            previousRingOfPorts->next = L7_NULL;
                        }
                        else
                        {
                            previousRingOfPorts->next = currentRingOfPorts->next;
                        }

                        osapiFree(L7_GARP_COMPONENT_ID, currentRingOfPorts);
                        return L7_SUCCESS;                        
                    }

                    previousRingOfPorts = currentRingOfPorts;
                    currentRingOfPorts =  previousRingOfPorts->next;

                } while (currentRingOfPorts != L7_NULL);
            }
            /* looks like we found the instance, but not the port */
            return L7_FAILURE;
        }
    }
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  check if a port is participating in a certain instance.
*
* @param    instanceID    instance ID
* @param    portNumber    port number
*
* @returns  L7_FAILURE
* @returns  L7_SUCCESS
*
* @notes   check if a port is forwarding in a MSTP instance
*
*
* @end
*********************************************************************/
L7_RC_t gipIsPortBelongToInstance(L7_uint32 instanceID, L7_uint32 portNumber)
{
    L7_uint32 counter;
    portInRing_t *currentRingOfPorts;


    for (counter = 0; counter < L7_NUM_STP_INSTANCES; counter++)
    {
        if (multipleGIP[counter].intstanceID == instanceID)
        {
            currentRingOfPorts = multipleGIP[counter].ringOfPorts;
            
            while (currentRingOfPorts != L7_NULL)
            {
                if (currentRingOfPorts->portNumber == portNumber)
                {
                    return L7_SUCCESS;
                }

                currentRingOfPorts =  currentRingOfPorts->next;
            }
        }
    }

    return L7_FAILURE;
}

/*********************************************************************
* @purpose  create GIP.
*
* @param    void
*
* @returns  L7_FALSE
* @returns  L7_TRUE
*
* @notes   Creates a new instance of GIP, allocating space for propagation counts
*          for up to max_attributes.
*
*          Returns True if the creation suceeded together with a pointer to the
*          GIP information. This pointer is passed to gid_create_port() for ports
*          using this instance of GIP. and is saved along with GID information.
*
* @end
*********************************************************************/
L7_BOOL gip_create_gip(L7_uint32 max_attributes, L7_uint32 **gip)
{  
    /*
	* GIP maintains a set of propagation counts for up to max attributes.
	* It currently maintains no additional information, so the GIP instance
	* is represented directly by a pointer to the array of propagation counts.
	*/
	
  L7_uint32 *my_gip;

  my_gip = (L7_uint32 *)osapiMalloc(L7_GARP_COMPONENT_ID, sizeof(L7_uint32)*max_attributes);
  if (my_gip != L7_NULLPTR)
  {
    *gip = my_gip;
  }
  else
    return L7_FALSE;

   return L7_TRUE;
}

/*********************************************************************
* @purpose  destroy GIP
*
* @param    void *    pointer to GIP
*
* @returns  None
*
* @notes   Destroys the instance of GIP, releasing previously allocated space.
*
* @end
*********************************************************************/
void gip_destroy_gip(void *gip)
{  /*
	*
	*/

	osapiFree(L7_GARP_COMPONENT_ID, gip);
}

/******************************************************************************
 * GIP : GARP INFORMATION PROPAGATION : CONNECT, DISCONNECT PORTS
 ******************************************************************************
 */

/*********************************************************************
* @purpose  connect port  to ring
*
* @param    Garp *    pointer to application
*
* @returns  None
*
* @notes   	Finds the port, checks that it is not already connected, and connects
*           it into the GIP propagation ring which uses GIP field(s) in GID control
*           blocks to link the source port to the ports to which the information is
*           to be propagated.
*
*           Propagates joins from and to the other already connected ports as
*           necessary.
*
* @end
*********************************************************************/
static void gip_connect_into_ring(Gid *my_port)
{
	Gid *first_connected, *last_connected;

    /*LOG_MSG("GIP: Connecting Ports #%x to ring.\n",my_port->port_no);*/
	
    my_port->is_connected           = L7_TRUE;
	my_port->next_in_connected_ring = my_port;

	first_connected = my_port;

	do {
          first_connected = first_connected->next_in_port_ring;
    }while  (!first_connected->is_connected);
      
	

	my_port->next_in_connected_ring = first_connected;

    last_connected = first_connected;

	while (last_connected->next_in_connected_ring != first_connected)
    {
       last_connected = last_connected->next_in_connected_ring;
    }

	last_connected->next_in_connected_ring = my_port;
}


/*********************************************************************
* @purpose  disconnect port from ring
*
* @param    Garp *    pointer to application
*
* @returns  None
*
* @notes   	Checks to ensure that the port is connected, and then disconnects it from
*           the GIP propagation ring. Propagates leaves to the other ports which
*           remain in the ring and causes leaves to my_port as necessary.
*
* @end
*********************************************************************/
static void gip_disconnect_from_ring(Gid *my_port)
{
	Gid *first_connected, *last_connected;

    /*LOG_MSG("GIP: Disconnecting Ports #%x from ring.\n",my_port->port_no);*/
	
    first_connected					= my_port->next_in_connected_ring;
	my_port->next_in_connected_ring = my_port;
	my_port->is_connected           = L7_FALSE;

    last_connected = first_connected;

	while (last_connected->next_in_connected_ring != my_port)
    {
        last_connected = last_connected->next_in_connected_ring;
    }
		   
	last_connected->next_in_connected_ring = first_connected;
}


/*********************************************************************
* @purpose  connect port
*
* @param    Garp *    pointer to application
* @param    L7_uint32 port number
*
* @returns  None
*
* @notes   	Finds the port, checks that it is not already connected, and connects
*           it into the GIP propagation ring which uses GIP field(s) in GID control
*           blocks to link the source port to the ports to which the information is
*           to be propagated.
*
*           Propagates joins from and to the other already connected ports as
*           necessary.
*
* @end
*********************************************************************/
void gip_connect_port(Garp *application, L7_uint32 port_no)
{  /*
	* If a GID instance for this application and port number is found, is
	* enabled, and is not already connected, then connect that port into the
	* GIP propagation ring.
	*
	* Propagate every attribute that has been registered (i.e. the Registrar
	* appears not to be Empty) on any other connected port, and which has in
	* consequence a non-zero propagation count, to this port, generating a join
	* request.
	*
	* Propagate every attribute that has been registered on this port and not
	* on any others (having a propagation count of zero prior to connecting this
	* port) to all the connected ports, updating propagation counts.
	*
	* Action any timers required. Mark the port as connected.
	*
	*/
	Gid      *my_port;
	L7_uint32  gid_index;

    GARP_TRACE_INTERNAL_EVENTS("Entering gip_connect_port: app: %d, port_no %d\n",
                               application->app, port_no);


	if (gid_find_port(application->gid, port_no, (void*)&my_port))
	{
        if ((my_port->is_enabled) && (!my_port->is_connected))
        {
            GARP_TRACE_INTERNAL_EVENTS("Connecting port into ring: app: %d, port_no %d\n",
                                       application->app, port_no);

            gip_connect_into_ring(my_port);

            gid_index = (application->app == GARP_GVRP_APP) ? 0 : 2;

            for (; gid_index <= (L7_uint32)(application->last_gid_used); gid_index++)
            {
                if (gip_propagates_to(my_port, gid_index))
                {
                    gid_join_request(my_port, gid_index);
                }

                if (gid_registered_here(my_port, gid_index))
                {
                    gip_propagate_join(my_port, gid_index);
                }
                    
            }

            gip_do_actions(my_port);
            my_port->is_connected = L7_TRUE;
        }             
	}
}


/*********************************************************************
* @purpose  disconnect port
*
* @param    Garp *    pointer to application
* @param    L7_uint32 port number
*
* @returns  None
*
* @notes   	Checks to ensure that the port is connected, and then disconnects it from
*           the GIP propagation ring. Propagates leaves to the other ports which
*           remain in the ring and causes leaves to my_port as necessary.
*
* @end
*********************************************************************/
void gip_disconnect_port(Garp *application, L7_uint32 port_no)
{  /*
	* Reverses the operations performed by gip_connect_port().
	*/
	Gid      *my_port;
	L7_uint32  gid_index;

    GARP_TRACE_INTERNAL_EVENTS("Entering gip_disconnect_port: app: %d, port_no %d\n",
                               application->app, port_no);


	if (gid_find_port(application->gid, port_no, (void*)&my_port))
	{
		if ((my_port->is_enabled) && (my_port->is_connected))
        {
            gid_index = (application->app == GARP_GVRP_APP) ? 0 : 2;

		    for (; gid_index <= (L7_uint32)(application->last_gid_used); gid_index++)
            {
                if (gip_propagates_to(my_port, gid_index))
                {
                    gid_leave_request(my_port, gid_index);
                }

                if (gid_registered_here(my_port, gid_index))
                {
                    gip_propagate_leave(my_port, gid_index);
                }                  
            }

            gip_do_actions(my_port);
            gip_disconnect_from_ring(my_port);
            my_port->is_connected = L7_FALSE;
        }
	}
}


/******************************************************************************
 * GIP : GARP INFORMATION PROPAGATION : PROPAGATE SINGLE ATTRIBUTES
 ******************************************************************************
 */

/*********************************************************************
* @purpose  propagate join
*
* @param    Gid *     pointer to port
* @param    L7_uint32 index
*
* @returns  None
*
* @notes   Propagates a join indication for a single attribute (identified
*          by a combination of its attribute class and index) from my_port to other
*          ports, causing join requests to those other ports if required.
*
*          GIP maintains a joined membership count for the connected ports for each
*          attribute (in a given context) so that leaves are not caused when joins
*          from other ports would maintain membership.
*
*          Because this count is maintained by "dead-reckoning" it is important
*          that this function only be called when there is a change indication for
*          the source port and index.
*
* @end
*********************************************************************/
void gip_propagate_join(Gid *my_port, L7_uint32 gid_index)
{  /*
	* Propagates a join indication, causing join requests to other ports
	* if required.
	*
	* The join needs to be propagated if either (a) this is the first port in
	* the connected group to register membership, or (b) there is one other port
	* in the group registering membership, but no further port which would cause
	* a join request to that port.
	*
	*/
	Gid      *to_port;
    
    GARP_TRACE_PROTOCOL_EVENTS("Propagate Join Request: app: %d, port_no %d, index %d\n",
                               my_port->application->app, my_port->port_no, gid_index);


	if (my_port->is_connected)
	{
        to_port = my_port;
		while ((to_port = to_port->next_in_connected_ring) != my_port)
		{
            gid_join_request(to_port, gid_index);
			to_port->application->join_propagated_fn(my_port->application,
													 my_port, gid_index);
        } /* end while */
    }
} /* end gip_propagate_join */


/*********************************************************************
* @purpose  propagate leave
*
* @param    Gid *     pointer to port
* @param    L7_uint32 index
*
* @returns  None
*
* @notes   Propagates a leave indication for a single attribute, causing leave
*          requests to those other ports if required.
*
*          See the comments for gip_propagate_join() before reading further.
*          This function decrements the "dead reckoning" membership count.
*
* @end
*********************************************************************/
void gip_propagate_leave(Gid *my_port, L7_uint32 gid_index)
{
   /* Propagates a leave indication for a single attribute, causing leave
	* requests to those other ports if required.
	*
	* See the comments for gip_propagate_join() before reading further.
	* This function decrements the "dead reckoning" membership count.
	*
	* The first step is to check that this port is connected to any others, if
	* not the leave indication should not be propagated, nor should the joined
	* membership be decremented. Otherwise, the leave will need to be propagated
	* if this is either (a) the last port in the connected group to register
	* membership, or (b) there is one other port in the group registering
	* membership, in which case the leave request needs to be sent to that
	* port alone.
	*/
	Gid        *to_port;
    L7_uint32  remaining_members;

    GARP_TRACE_PROTOCOL_EVENTS("Propagate Leave Request: app: %d, port_no %d, index %d\n",
                               my_port->application->app, my_port->port_no, gid_index);

	
    if (my_port->is_connected)
	{
        to_port = my_port;
        if (to_port->machines[gid_index].incoming_port == L7_TRUE)
        {
            if (to_port->application->gip[gid_index] > 0)
            {
                to_port->application->gip[gid_index] -= 1;
            }
        }        


        remaining_members = to_port->application->gip[gid_index];

/** djohnson 12678: removed, causes ports to get stuck in VLANs
        if (remaining_members > 0)
        {
           to_port->machines[gid_index].incoming_port = L7_FALSE;
        }
*/
        if (remaining_members <= 1)
        {
            while ((to_port = to_port->next_in_connected_ring) != my_port)
            {
               if ((remaining_members == 0) ||
                   (gid_registered_here(to_port, gid_index)))
               {
                   gid_leave_request(to_port, gid_index);
                   to_port->application->leave_propagated_fn(my_port->application,
                                                             my_port, gid_index);
               }
            } /* end while */
        }
    }
} /* end gip_propagate_leave */


/*********************************************************************
* @purpose  propagate to port
*
* @param    Gid *     pointer to port
* @param    L7_uint32 index
*
* @returns  L7_FALSE
* @returns  L7_TRUE
*
* @notes   True if any other port is propagating the attribute associated 
*          with index to my_port.
*
* @end
*********************************************************************/
L7_BOOL gip_propagates_to(Gid *my_port, L7_uint32 gid_index)
{
	L7_BOOL returnVal = L7_FALSE;

    if ((my_port->is_connected) &&
		((my_port->application->gip[gid_index] == 2) ||
		 ((my_port->application->gip[gid_index] == 1) &&
		  (!gid_registered_here(my_port, gid_index)))))
    {
        returnVal = L7_TRUE;
    }

	return returnVal;
}

/******************************************************************************
 * GIP : GARP INFORMATION PROPAGATION : ACTION TIMERS
 ******************************************************************************
 */

/*********************************************************************
* @purpose  Gip managment
*
* @param    Gid *     pointer to port
*
* @returns  L7_FALSE
* @returns  L7_TRUE
*
* @notes   	Calls GID to carry out GID 'scratchpad' actions accumulated during this
*           invocation of GARP for all the ports in the GIP propagation list,
*           including the source port.
*
* @end
*********************************************************************/
void gip_do_actions(Gid *my_port)
{  /*
	* Calls GID to carry out GID 'scratchpad' actions accumulated during this
	* invocation of GARP for all the ports in the GIP ring, including my port.
	*/
	Gid *this_port = my_port;

	do {
        gid_do_actions(this_port);
    } while ((this_port = this_port->next_in_connected_ring) != my_port  );
         
}

/*********************************************************************
* @purpose  connect port to active topology
*
* @param    Garp *    pointer to application
* @param    L7_uint32 port number
*
* @returns  None
*
* @notes   	Finds the port, checks that it is not already connected, and connects
*           it into the GIP propagation ring which uses GIP field(s) in GID control
*           blocks to link the source port to the ports to which the information is
*           to be propagated.
*
*           Propagates joins from and to the other already connected ports as
*           necessary.
*
* @end
*********************************************************************/
extern void gip_connect_port_to_active_topology(Garp *application, L7_uint32 port_no)
{
	Gid      *my_port;

	if (gid_find_port(application->gid, port_no, (void*)&my_port))
	{
        if (my_port->is_connected == L7_FALSE)
        {
            gip_connect_into_ring(my_port);
            my_port->is_connected = L7_TRUE;
        }
    }

}


/*********************************************************************
* @purpose  disconnect port from active toplogy
*
* @param    Garp *    pointer to application
* @param    L7_uint32 port number
*
* @returns  None
*
* @notes   	Checks to ensure that the port is connected, and then disconnects it from
*           the GIP propagation ring. Propagates leaves to the other ports which
*           remain in the ring and causes leaves to my_port as necessary.
*
* @end
*********************************************************************/
extern void gip_disconnect_port_from_active_tolpolgy(Garp *application, L7_uint32 port_no)
{
	Gid      *my_port;

	if (gid_find_port(application->gid, port_no, (void*)&my_port))
	{
        if (my_port->is_connected == L7_TRUE)
        {
            gip_disconnect_from_ring(my_port);
            my_port->is_connected = L7_FALSE;
        }
    }

}

