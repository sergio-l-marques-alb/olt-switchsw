#ifndef L7_USL_BCM_VLAN_MAC_H
#define L7_USL_BCM_VLAN_MAC_H
#include "bcmx/vlan.h"
#include "bcmx/port.h"

bcm_error_t usl_bcmx_vlan_mac_add(bcm_mac_t mac, bcm_vlan_t vid, int prio);
bcm_error_t usl_bcmx_vlan_mac_delete(bcm_mac_t mac);
bcm_error_t usl_mac_bcmx_vlan_control_port_set(bcmx_lport_t port,
                bcm_vlan_control_port_t type, int arg);

#endif
