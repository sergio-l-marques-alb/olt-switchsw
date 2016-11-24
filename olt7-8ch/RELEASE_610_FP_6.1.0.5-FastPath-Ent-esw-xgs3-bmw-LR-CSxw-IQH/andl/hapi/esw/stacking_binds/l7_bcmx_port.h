
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2008
*
**********************************************************************
*
* @filename   l7_bcmx_port.c
*
* @purpose    New bcmx layer that issues commands for port configuration
*
* @component  HAPI
*
* @comments   none
*
* @create     3/31/2008
*
* @author     nshrivastav
* @end
*
**********************************************************************/

#ifndef L7_BCMX_PORT_H
#define L7_BCMX_PORT_H

#include "l7_common.h"
#include "osapi.h"
#include "trace_api.h"
#include "l7_resources.h"

#include "broad_utils.h"


/*********************************************************************
*
* @purpose Add list of vlans to a port
*
* @param   port         - The LPORT    
* @param   vlanCmd      - Pointer to vlan membership custom bcmx command   
*
* @returns BCMX Error Code
*
* @notes This is a wrapper function around the BCMX api. The attribute
*        is committed to the hardware by calling BCMX api if the Card  
*        to which the port belongs is not being initialized/un-configured. 
*        During Initialization/Un-configuration, the attribute value is 
*        committed to the hardware later once application has finished 
*        initialization/un-configuration of all the ports in a Card. 
*
* @end
*
*********************************************************************/
int l7_bcmx_port_vlan_member_add(bcmx_lport_t port,
                                 HAPI_BROAD_CUSTOM_BCMX_VLAN_CMD_t *vlanCmd);

/*********************************************************************
*
* @purpose Remove the vlans from a port
*
* @param   port         - The LPORT    
* @param   vlanCmd      - Pointer to vlan membership custom bcmx command   
*
* @returns BCMX Error Code
*
* @notes This is a wrapper function around the BCMX api. The attribute
*        is committed to the hardware by calling BCMX api if the Card  
*        to which the port belongs is not being initialized/un-configured. 
*        During Initialization/Un-configuration, the attribute value is 
*        committed to the hardware later once application has finished 
*        initialization/un-configuration of all the ports in a Card. 
*
* @end
*
*********************************************************************/
int l7_bcmx_port_vlan_member_delete(bcmx_lport_t port,
                                    HAPI_BROAD_CUSTOM_BCMX_VLAN_CMD_t *vlanCmd);


/*********************************************************************
*
* @purpose Set the pvid for a port
*
* @param   port    -  The LPORT 
* @param   vid     -  PVID
*
* @returns BCMX Error Code
*
* @notes This is a wrapper function around the BCMX api. The attribute
*        is committed to the hardware by calling BCMX api if the Card  
*        to which the port belongs is not being initialized/un-configured. 
*        During Initialization/Un-configuration, the attribute value is 
*        committed to the hardware later once application has finished 
*        initialization/un-configuration of all the ports in a Card. 
*
* @end
*
*********************************************************************/
int l7_bcmx_port_untagged_vlan_set(bcmx_lport_t port, bcm_vlan_t vid);

/*********************************************************************
*
* @purpose Set the Ingress filtering mode for a port
*
* @param   port    -  The LPORT 
* @param   flags   -  Filtering mode
*
* @returns BCMX Error Code
*
* @notes This is a wrapper function around the BCMX api. The attribute
*        is committed to the hardware by calling BCMX api if the Card  
*        to which the port belongs is not being initialized/un-configured. 
*        During Initialization/Un-configuration, the attribute value is 
*        committed to the hardware later once application has finished 
*        initialization/un-configuration of all the ports in a Card. 
*
* @end
*
*********************************************************************/
int l7_bcmx_port_vlan_member_set(bcmx_lport_t port, uint32 flags);

/*********************************************************************
*
* @purpose Get the Ingress filtering mode for a port
*
* @param   port    -  The LPORT 
* @param   flags   -  Filtering mode
*
* @returns BCMX Error Code
*
* @notes This is a wrapper function around the BCMX api. 
*
* @end
*
*********************************************************************/
int l7_bcmx_port_vlan_member_get(bcmx_lport_t port, uint32 *flags);

/*********************************************************************
*
* @purpose Set the Acceptable frame type mode for a port
*
* @param   port    -  The LPORT 
* @param   mode    -  Acceptable frame type mode
*
* @returns BCMX Error Code
*
* @notes This is a wrapper function around the BCMX api. The attribute
*        is committed to the hardware by calling BCMX api if the Card  
*        to which the port belongs is not being initialized/un-configured. 
*        During Initialization/Un-configuration, the attribute value is 
*        committed to the hardware later once application has finished 
*        initialization/un-configuration of all the ports in a Card. 
*
* @end
*
*********************************************************************/
int l7_bcmx_port_discard_set(bcmx_lport_t port, int mode);

/*********************************************************************
*
* @purpose Set the administrative mode for a port
*
* @param   port    -  The LPORT 
* @param   enable  -  Enable/Disable the port
*
* @returns BCMX Error Code
*
* @notes This is a wrapper function around the BCMX api. The attribute
*        is committed to the hardware by calling BCMX api if the Card  
*        to which the port belongs is not being initialized/un-configured. 
*        During Initialization/Un-configuration, the attribute value is 
*        committed to the hardware later once application has finished 
*        initialization/un-configuration of all the ports in a Card. 
*
* @end
*
*********************************************************************/
int l7_bcmx_port_enable_set(bcmx_lport_t port, int enable);

/*********************************************************************
*
* @purpose Set the broadcast rate threshold for a port
*
* @param   limit    -  Rate 
* @param   flags    
* @param   port     - The LPORT    
*
* @returns BCMX Error Code
*
* @notes This is a wrapper function around the BCMX api. The attribute
*        is committed to the hardware by calling BCMX api if the Card  
*        to which the port belongs is not being initialized/un-configured. 
*        During Initialization/Un-configuration, the attribute value is 
*        committed to the hardware later once application has finished 
*        initialization/un-configuration of all the ports in a Card. 
*
* @end
*
*********************************************************************/
int l7_bcmx_rate_bcast_set(int limit, int flags, bcmx_lport_t port);

/*********************************************************************
*
* @purpose Set the multicast rate threshold for a port
*
* @param   limit    -  Rate 
* @param   flags    
* @param   port     - The LPORT    
*
* @returns BCMX Error Code
*
* @notes This is a wrapper function around the BCMX api. The attribute
*        is committed to the hardware by calling BCMX api if the Card  
*        to which the port belongs is not being initialized/un-configured. 
*        During Initialization/Un-configuration, the attribute value is 
*        committed to the hardware later once application has finished 
*        initialization/un-configuration of all the ports in a Card. 
*
* @end
*
*********************************************************************/
int l7_bcmx_rate_mcast_set(int limit, int flags, bcmx_lport_t port);

/*********************************************************************
*
* @purpose Set the DLF rate threshold for a port
*
* @param   limit    -  Rate 
* @param   flags    
* @param   port     - The LPORT    
*
* @returns BCMX Error Code
*
* @notes This is a wrapper function around the BCMX api. The attribute
*        is committed to the hardware by calling BCMX api if the Card  
*        to which the port belongs is not being initialized/un-configured. 
*        During Initialization/Un-configuration, the attribute value is 
*        committed to the hardware later once application has finished 
*        initialization/un-configuration of all the ports in a Card. 
*
* @end
*
*********************************************************************/
int l7_bcmx_rate_dlfbc_set(int limit, int flags, bcmx_lport_t port);

/*********************************************************************
*
* @purpose Set the Egress rate for a port
*
* @param   port         - The LPORT    
* @param   kbits_sec    - Rate 
* @param   kbits_burst  - Burst rate    
*
* @returns BCMX Error Code
*
* @notes This is a wrapper function around the BCMX api. The attribute
*        is committed to the hardware by calling BCMX api if the Card  
*        to which the port belongs is not being initialized/un-configured. 
*        During Initialization/Un-configuration, the attribute value is 
*        committed to the hardware later once application has finished 
*        initialization/un-configuration of all the ports in a Card. 
*
* @end
*
*********************************************************************/
int l7_bcmx_port_rate_egress_set(bcmx_lport_t port, uint32 kbits_sec, 
                                 uint32 kbits_burst);

/*********************************************************************
*
* @purpose Set the learning mode for a port
*
* @param   port     - The LPORT    
* @param   flags    
*
* @returns BCMX Error Code
*
* @notes This is a wrapper function around the BCMX api. The attribute
*        is committed to the hardware by calling BCMX api if the Card  
*        to which the port belongs is not being initialized/un-configured. 
*        During Initialization/Un-configuration, the attribute value is 
*        committed to the hardware later once application has finished 
*        initialization/un-configuration of all the ports in a Card. 
*
* @end
*
*********************************************************************/
int l7_bcmx_port_learn_set(bcmx_lport_t port, uint32 flags);

/*********************************************************************
*
* @purpose Set the max frame size for a port
*
* @param   port     - The LPORT    
* @param   size     - The max frame size    
*
* @returns BCMX Error Code
*
* @notes This is a wrapper function around the BCMX api. The attribute
*        is committed to the hardware by calling BCMX api if the Card  
*        to which the port belongs is not being initialized/un-configured. 
*        During Initialization/Un-configuration, the attribute value is 
*        committed to the hardware later once application has finished 
*        initialization/un-configuration of all the ports in a Card. 
*
* @end
*
*********************************************************************/
int l7_bcmx_port_frame_max_set(bcmx_lport_t port, int size);

/*********************************************************************
*
* @purpose Enable/Disable flow-control on a port
*
* @param   port         - The LPORT    
* @param   pauseCmd     - Pause configuration for the port
*
* @returns BCMX Error Code
*
* @notes This is a wrapper function around the BCMX api. The attribute
*        is committed to the hardware by calling BCMX api if the Card  
*        to which the port belongs is not being initialized/un-configured. 
*        During Initialization/Un-configuration, the attribute value is 
*        committed to the hardware later once application has finished 
*        initialization/un-configuration of all the ports in a Card. 
*
* @end
*
*********************************************************************/
int l7_bcmx_port_flow_control_set(bcmx_lport_t port, HAPI_BROAD_CUSTOM_BCMX_PAUSE_CMD_t pauseCmd);

/*********************************************************************
*
* @purpose Set the default port priority of a port
*
* @param   port         - The LPORT    
* @param   priority     
*
* @returns BCMX Error Code
*
* @notes This is a wrapper function around the BCMX api. The attribute
*        is committed to the hardware by calling BCMX api if the Card  
*        to which the port belongs is not being initialized/un-configured. 
*        During Initialization/Un-configuration, the attribute value is 
*        committed to the hardware later once application has finished 
*        initialization/un-configuration of all the ports in a Card. 
*
* @end
*
*********************************************************************/
int l7_bcmx_port_untagged_priority_set(bcmx_lport_t port, int priority);


/*********************************************************************
*
* @purpose Set a port cosq configuration 
*
* @param   port         - The LPORT    
* @param   mode     
* @param   weights    
* @param   delay     
* @param   minKbps
* @param   maxKbps
*
* @returns BCMX Error Code
*
* @notes This is a wrapper function around the BCMX api. The attribute
*        is committed to the hardware by calling BCMX api if the Card  
*        to which the port belongs is not being initialized/un-configured. 
*        During Initialization/Un-configuration, the attribute value is 
*        committed to the hardware later once application has finished 
*        initialization/un-configuration of all the ports in a Card. 
*
* @end
*
*********************************************************************/
int l7_bcmx_cosq_port_sched_set(bcmx_lport_t port,
				                int mode,
				                const int weights[BCM_COS_COUNT],
				                int delay,
                                const int minKbps[BCM_COS_COUNT],
                                const int maxKbps[BCM_COS_COUNT]);

/*********************************************************************
*
* @purpose Set the dvlan mode of a port
*
* @param   port         - The LPORT    
* @param   mode     
*
* @returns BCMX Error Code
*
* @notes This is a wrapper function around the BCMX api. The attribute
*        is committed to the hardware by calling BCMX api if the Card  
*        to which the port belongs is not being initialized/un-configured. 
*        During Initialization/Un-configuration, the attribute value is 
*        committed to the hardware later once application has finished 
*        initialization/un-configuration of all the ports in a Card. 
*
* @end
*
*********************************************************************/
int l7_bcmx_port_dtag_mode_set(bcmx_lport_t port, int mode);

/*********************************************************************
*
* @purpose Set the dvlan ethertype for a port
*
* @param   port         - The LPORT    
* @param   tpid         - Ethertype    
*
* @returns BCMX Error Code
*
* @notes This is a wrapper function around the BCMX api. The attribute
*        is committed to the hardware by calling BCMX api if the Card  
*        to which the port belongs is not being initialized/un-configured. 
*        During Initialization/Un-configuration, the attribute value is 
*        committed to the hardware later once application has finished 
*        initialization/un-configuration of all the ports in a Card. 
*
* @end
*
*********************************************************************/
int l7_bcmx_port_tpid_set(bcmx_lport_t port, uint16 tpid);

/*********************************************************************
*
* @purpose Set the speed/duplex of a port
*
* @param   port         - The LPORT    
* @param   tpid         - Ethertype    
*
* @returns BCMX Error Code
*
* @notes This is a wrapper function around the BCMX api. The attribute
*        is committed to the hardware by calling BCMX api if the Card  
*        to which the port belongs is not being initialized/un-configured. 
*        During Initialization/Un-configuration, the attribute value is 
*        committed to the hardware later once application has finished 
*        initialization/un-configuration of all the ports in a Card. 
*
* @end
*
*********************************************************************/
int l7_bcmx_port_medium_config_set(bcmx_lport_t port,
                                   bcm_port_medium_t medium,
                                   bcm_phy_config_t  *config);

/*********************************************************************
*
* @purpose Add/Remove a port from the vlan in the usl cache
*
* @param   unit         - FP unit number
* @param   slot         - FP slot number
* @param   bcmUnit      - bcmUnit of the port
* @param   bcmPort      - bcmPort of the port
* @param   vlanId       - Vlan-id
* @param   tagged       - port is tagged member of vlan
* @param   vlanCmd      - L7_TRUE: Add port to the vlan
*                         L7_FALSE: Remove port from the vlan
*
*********************************************************************/
void l7_usl_port_cache_vlan_entry_set(L7_uint32 unit, L7_uint32 slot, L7_int32 bcmUnit, L7_int32 bcmPort, L7_uint32 vlanId, L7_BOOL tagged, L7_BOOL vlanCmd);

#endif
