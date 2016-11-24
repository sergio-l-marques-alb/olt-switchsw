#ifndef L7_USL_BCM_VLAN_IPSUBNET_H
#define L7_USL_BCM_VLAN_IPSUBNET_H
#include "bcmx/vlan.h"
#include "bcmx/port.h"

bcm_error_t usl_bcmx_vlan_ip4_add(bcm_ip_t ipaddr, bcm_ip_t netmask, bcm_vlan_t vid, int prio);
bcm_error_t usl_bcmx_vlan_ip4_delete(bcm_ip_t ipaddr, bcm_ip_t netmask);
bcm_error_t usl_ip_bcmx_vlan_control_port_set(bcmx_lport_t port,
                               bcm_vlan_control_port_t type, int arg);

#endif
