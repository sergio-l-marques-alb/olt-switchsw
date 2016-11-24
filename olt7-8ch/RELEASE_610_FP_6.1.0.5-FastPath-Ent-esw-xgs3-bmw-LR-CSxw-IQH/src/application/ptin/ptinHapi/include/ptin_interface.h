#ifndef _PTIN_INTERFACE__H
#define _PTIN_INTERFACE__H

#include "datatypes.h"
#include "dtlinclude.h"

#define PHY_MAX_MAXFRAME              2048

#define PHY_PORT_MEDIA_ELECTRIC       0
#define PHY_PORT_MEDIA_OPTICAL        1
#define PHY_PORT_MEDIA_INTERNAL       2

#define PHY_PORT_AUTONEG              0
#define PHY_PORT_10_MBPS              1
#define PHY_PORT_100_MBPS             2
#define PHY_PORT_1000_MBPS            3
#define PHY_PORT_2500_MBPS            4
#define PHY_PORT_10000_MBPS           5

extern L7_BOOL ptin_interface_qos_valid(L7_uint32 port, L7_uint32 *qos_list);

extern L7_RC_t ptin_phy_config_set(L7_uint8 port, L7_uint8 enable, L7_uint8 speed, L7_uint8 autoneg, L7_uint8 full_duplex, L7_uint16 frame_max, L7_uint8 lb, L7_uint8 macLearn);

extern L7_RC_t ptin_phy_config_get(L7_uint8 port, L7_uint8 *enable, L7_uint8 *speed, L7_uint8 *autoneg, L7_uint8 *full_duplex, L7_uint16 *frame_max, L7_uint8 *lb, L7_uint8 *macLearn);

extern L7_RC_t ptin_phy_state_get(L7_uint8 port, L7_uint8 *linkup, L7_uint8 *autoneg_complete);

extern L7_RC_t ptin_readCounters(L7_HWEthRFC2819_PortStatistics *stat);

extern L7_RC_t ptin_clearCounters(L7_uint32 port);

extern L7_RC_t dtlPTinIntfCallback(DAPI_USP_t *ddusp, 
                                   DAPI_FAMILY_t family, 
                                   DAPI_CMD_t cmd, 
                                   DAPI_EVENT_t event,
                                   void *dapiEventInfo);

#endif


