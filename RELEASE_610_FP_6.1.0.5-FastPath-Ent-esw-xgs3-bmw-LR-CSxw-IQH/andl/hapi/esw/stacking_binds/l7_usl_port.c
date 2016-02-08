/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2008
*
**********************************************************************
*
* @filename   l7_usl_port.c
*
* @purpose    l7_BCMX api's for port configuration 
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

#include "l7_common.h"
#include "osapi.h"
#include "dapi.h" 
#include "broad_utils.h"

#include "bcmx/port.h"
#include "bcmx/rate.h"
#include "bcmx/cosq.h"

/*********************************************************************
*
* @purpose Add list of vlans to a port
*
* @param   port         - The LPORT    
* @param   vlanCmd      - Pointer to vlan membership custom bcmx command   
*
* @returns BCMX Error Code
*
* @notes This is a wrapper function around the BCMX api. 
*
* @end
*
*********************************************************************/
int l7_bcmx_port_vlan_member_add(bcmx_lport_t port,
                                 HAPI_BROAD_CUSTOM_BCMX_VLAN_CMD_t *vlanCmd)
{
  return customx_port_vlan_member_add(port, vlanCmd);
}

/*********************************************************************
*
* @purpose Remove the vlans from a port
*
* @param   port         - The LPORT    
* @param   vlanCmd      - Pointer to vlan membership custom bcmx command   
*
* @returns BCMX Error Code
*
* @notes This is a wrapper function around the BCMX api. 
*
* @end
*
*********************************************************************/
int l7_bcmx_port_vlan_member_delete(bcmx_lport_t port,
                                    HAPI_BROAD_CUSTOM_BCMX_VLAN_CMD_t *vlanCmd)
{
  return customx_port_vlan_member_delete(port, vlanCmd);
}

/*********************************************************************
*
* @purpose Set the pvid for a port
*
* @param   port    -  The LPORT 
* @param   vid     -  PVID
*
* @returns BCMX Error Code
*
* @notes This is a wrapper function around the BCMX api. 
*
* @end
*
*********************************************************************/
int l7_bcmx_port_untagged_vlan_set(bcmx_lport_t port, bcm_vlan_t vid)
{
  return bcmx_port_untagged_vlan_set(port, vid);
}

/*********************************************************************
*
* @purpose Set the Ingress filtering mode for a port
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
int l7_bcmx_port_vlan_member_set(bcmx_lport_t port, uint32 flags)
{
  return bcmx_port_vlan_member_set(port, flags);
}

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
int l7_bcmx_port_vlan_member_get(bcmx_lport_t port, uint32 *flags)
{
  return bcmx_port_vlan_member_get(port, flags);
}

/*********************************************************************
*
* @purpose Set the Acceptable frame type mode for a port
*
* @param   port    -  The LPORT 
* @param   mode    -  Acceptable frame type mode
*
* @returns BCMX Error Code
*
* @notes This is a wrapper function around the BCMX api.
*
* @end
*
*********************************************************************/
int l7_bcmx_port_discard_set(bcmx_lport_t port, int mode)
{
  return bcmx_port_discard_set(port, mode);
}

/*********************************************************************
*
* @purpose Set the administrative mode for a port
*
* @param   port    -  The LPORT 
* @param   enable  -  Enable/Disable the port
*
* @returns BCMX Error Code
*
* @notes This is a wrapper function around the BCMX api.
*
* @end
*
*********************************************************************/
int l7_bcmx_port_enable_set(bcmx_lport_t port, int enable)
{
  return bcmx_port_enable_set(port, enable);
}

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
* @notes This is a wrapper function around the BCMX api.
*
* @end
*
*********************************************************************/
int l7_bcmx_rate_bcast_set(int limit, int flags, bcmx_lport_t port)
{
  return bcmx_rate_bcast_set(limit, flags, port);
}

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
* @notes This is a wrapper function around the BCMX api. 
*
* @end
*
*********************************************************************/
int l7_bcmx_rate_mcast_set(int limit, int flags, bcmx_lport_t port)
{
  return bcmx_rate_mcast_set(limit, flags, port);
}

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
* @notes This is a wrapper function around the BCMX api.
*
* @end
*
*********************************************************************/
int l7_bcmx_rate_dlfbc_set(int limit, int flags, bcmx_lport_t port)
{
  return bcmx_rate_dlfbc_set(limit, flags, port);
}

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
* @notes This is a wrapper function around the BCMX api. 
*
* @end
*
*********************************************************************/
int l7_bcmx_port_rate_egress_set(bcmx_lport_t port, uint32 kbits_sec, 
                                 uint32 kbits_burst)
{
  return bcmx_port_rate_egress_set(port, kbits_sec, kbits_burst);
}

/*********************************************************************
*
* @purpose Set the learning mode for a port
*
* @param   port     - The LPORT    
* @param   flags    
*
* @returns BCMX Error Code
*
* @notes This is a wrapper function around the BCMX api.
*
* @end
*
*********************************************************************/
int l7_bcmx_port_learn_set(bcmx_lport_t port, uint32 flags)
{
  return bcmx_port_learn_set(port, flags);
}

/*********************************************************************
*
* @purpose Set the max frame size for a port
*
* @param   port     - The LPORT    
* @param   size     - The max frame size    
*
* @returns BCMX Error Code
*
* @notes This is a wrapper function around the BCMX api. 
*
* @end
*
*********************************************************************/
int l7_bcmx_port_frame_max_set(bcmx_lport_t port, int size)
{
  return bcmx_port_frame_max_set(port, size);
}

/*********************************************************************
*
* @purpose Enable/Disable flow-control on a port
*
* @param   port         - The LPORT    
* @param   pauseCmd     - Pause configuration for the port
*
* @returns BCMX Error Code
*
* @notes This is a wrapper function around the BCMX api.
*
* @end
*
*********************************************************************/
int l7_bcmx_port_flow_control_set(bcmx_lport_t port, HAPI_BROAD_CUSTOM_BCMX_PAUSE_CMD_t pauseCmd)
{
  return customx_port_flow_control_set(port, pauseCmd);
}

/*********************************************************************
*
* @purpose Set the default port priority of a port
*
* @param   port         - The LPORT    
* @param   priority     
*
* @returns BCMX Error Code
*
* @notes This is a wrapper function around the BCMX api.
*
* @end
*
*********************************************************************/
int l7_bcmx_port_untagged_priority_set(bcmx_lport_t port, int priority)
{
  return bcmx_port_untagged_priority_set(port, priority);
}

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
* @notes This is a wrapper function around the BCMX api. 
*
* @end
*
*********************************************************************/
int l7_bcmx_cosq_port_sched_set(bcmx_lport_t port,
				                int mode,
				                const int weights[BCM_COS_COUNT],
				                int delay,
                                const int minKbps[BCM_COS_COUNT],
                                const int maxKbps[BCM_COS_COUNT])
{
  int rv;
  int cosq;
  bcmx_lplist_t  cosq_port_list;

  rv = bcmx_lplist_init(&cosq_port_list, L7_MAX_INTERFACE_COUNT, 0);
  if (L7_BCMX_OK(rv) == L7_TRUE) {
    rv = bcmx_lplist_last_insert(&cosq_port_list, port);
  }

  if (L7_BCMX_OK(rv) == L7_TRUE) {
    rv = bcmx_cosq_port_sched_set(cosq_port_list, mode, weights, delay);
  }

  bcmx_lplist_free(&cosq_port_list);

  if (L7_BCMX_OK(rv) == L7_TRUE)
  {
    for (cosq = 0; cosq < BCM_COS_COUNT; cosq++)
    {
      //printf("%s(%d) cosq=%u, min=%u, max=%u\r\n", __FUNCTION__, __LINE__,cosq,minKbps[cosq],maxKbps[cosq]);
      rv = bcmx_cosq_port_bandwidth_set(port, cosq, minKbps[cosq], maxKbps[cosq], 0);
      if (rv == BCM_E_UNAVAIL)
      {
        printf("%s(%d) Feature not supported\r\n", __FUNCTION__, __LINE__);
        /* Silently ignore errors if this feature is not supported. */
        rv = BCM_E_NONE;
        break;
      }
      if (L7_BCMX_OK(rv) != L7_TRUE)
        break;
    }
  }

  return rv;
}

/*********************************************************************
*
* @purpose Set the dvlan mode of a port
*
* @param   port         - The LPORT    
* @param   mode     
*
* @returns BCMX Error Code
*
* @notes This is a wrapper function around the BCMX api.
*
* @end
*
*********************************************************************/
int l7_bcmx_port_dtag_mode_set(bcmx_lport_t port, int mode)
{
  return bcmx_port_dtag_mode_set(port, mode);
}

/*********************************************************************
*
* @purpose Set the dvlan ethertype for a port
*
* @param   port         - The LPORT    
* @param   tpid         - Ethertype    
*
* @returns BCMX Error Code
*
* @notes This is a wrapper function around the BCMX api.
*
* @end
*
*********************************************************************/
int l7_bcmx_port_tpid_set(bcmx_lport_t port, uint16 tpid)
{
  return bcmx_port_tpid_set(port, tpid);
}

/*********************************************************************
*
* @purpose Set the speed/duplex of a port
*
* @param   port         - The LPORT    
* @param   tpid         - Ethertype    
*
* @returns BCMX Error Code
*
* @notes This is a wrapper function around the BCMX api. 
*
* @end
*
*********************************************************************/
int l7_bcmx_port_medium_config_set(bcmx_lport_t port,
                                   bcm_port_medium_t medium,
                                   bcm_phy_config_t  *config)
{
  return bcmx_port_medium_config_set(port, medium, config);
}


