/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    gipapi.h
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
#ifndef INCLUDE_GIP_API_H_
#define INCLUDE_GIP_API_H_

#include "gidapi.h"

#define L7_NUM_STP_INSTANCES      (L7_MAX_MULTIPLE_STP_INSTANCES + 1)

typedef struct portInRing_s
{
    L7_uint32            portNumber;
    struct portInRing_s  *next;
} portInRing_t;

typedef struct multipleGIP_s
{
    L7_BOOL      inUse;
    L7_uint32    intstanceID;
    portInRing_t *ringOfPorts;
} multipleGIP_t;



/******************************************************************************
 * GIP : GARP INFORMATION PROPAGATION : CREATION, DESTRUCTION
 ******************************************************************************
 */

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
L7_BOOL gip_create_gip(L7_uint32 max_attributes, L7_uint32 **gip);

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
extern void gip_destroy_gip(void *gip);

/******************************************************************************
 * GIP : GARP INFORMATION PROPAGATION : PROPAGATION FUNCTIONS
 ******************************************************************************
 */

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
extern void gip_connect_port(Garp *application, L7_uint32 port_no);


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
extern void gip_disconnect_port(Garp *application, L7_uint32 port_no);
   
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
extern void gip_propagate_join(Gid *my_port, L7_uint32 index);
   

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
extern void gip_propagate_leave(Gid *my_port, L7_uint32 index);

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
extern L7_BOOL gip_propagates_to(Gid *my_port, L7_uint32 index);

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
extern void gip_do_actions(Gid *my_port);


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
extern void gip_connect_port_to_active_topology(Garp *application, L7_uint32 port_no);


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
extern void gip_disconnect_port_from_active_tolpolgy(Garp *application, L7_uint32 port_no);

#endif

