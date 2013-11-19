#ifndef _PTIN_HAPI_H
#define _PTIN_HAPI_H

#include "l7_common.h"
#include "ptin_structs.h"
#include "ptin_globaldefs.h"
#include "dapi_struct.h"
#include "broad_common.h"

/********************************************************************
 * TYPES DEFINITION
 ********************************************************************/
typedef struct {
  DAPI_USP_t *usp;
  DAPI_t     *dapi_g;
} ptin_dapi_port_t;

typedef struct {
//  L7_int        ptin_port;
  bcmx_lport_t  lport;
  bcm_trunk_t   trunk_id;
  bcm_port_t    bcm_port;
  L7_uint32     class_port;
} ptin_hapi_intf_t;

/********************************************************************
 * DEFINES
 ********************************************************************/


/********************************************************************
 * MACROS AND INLINE FUNCTIONS
 ********************************************************************/

#define DAPIPORT_SET(dapiPort,usp_ref,dapi_g_ref) \
{                                         \
  (dapiPort)->usp    = usp_ref;               \
  (dapiPort)->dapi_g = dapi_g_ref;            \
}

#define DAPIPORT_GET_PTR(dapiPort,dapiPortPtr,hapiPortPtr)            \
{                                                                     \
  dapiPortPtr = DAPI_PORT_GET( (dapiPort)->usp, (dapiPort)->dapi_g ); \
  hapiPortPtr = HAPI_PORT_GET( (dapiPort)->usp, (dapiPort)->dapi_g ); \
}

/********************************************************************
 * EXTERNAL VARIABLES
 ********************************************************************/

extern L7_int bcm_unit;


/********************************************************************
 * EXTERNAL FUNCTIONS PROTOTYPES
 ********************************************************************/

/**
 * Initializes PTin HAPI data structures
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t hapi_ptin_data_init(void);

/**
 * Initializes PTin HAPI configurations
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t hapi_ptin_config_init(void);

/**
 * Initialize Switch control parameters
 * 
 * @author mruas (5/31/2012)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_hapi_switch_init(void);

/**
 * Initialize PHY control parameters
 * 
 * @author asantos (07/02/2013)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_hapi_phy_init(void);

/** 
 * Get bcm unit id for this switch. 
 * Normally is ZERO, but nervertheless it's better to be sure 
 * 
 * @param bcm_unit: switch unit id
 * 
 * @return L7_RC_t: always L7_SUCCESS
 */
extern L7_RC_t hapi_ptin_bcmUnit_get(L7_int *bcm_unit);

/**
 * Get sdk port reference
 * 
 * @param port: PTin port id 
 *        bcm_port: Pointer to the location where the bcm_port
 *        will be stored
 *  
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t hapi_ptin_bcmPort_get(L7_int port, L7_int *bcm_port);

/**
 * Get port if, from the sdk port reference
 * 
 * @param bcm_port: SDK port number 
 *        port: PTin port id
 *  
 * @return L7_RC_t: 
 *        L7_SUCCESS if found
 *        L7_FAILURE otherwise
 */
extern L7_RC_t hapi_ptin_port_get(L7_int bcm_port, L7_int *port);

/**
 * Get port bitmap in pbmp_t format for all physical ports
 * 
 * @param pbmp_mask : port bitmap
 */
extern void hapi_ptin_allportsbmp_get(pbmp_t *pbmp_mask);

/**
 * Get port descriptor from ddUsp interface
 * 
 * @param ddUsp : unit, slot and port reference
 * @param dapi_g
 * @param intf_desc : interface descriptor with lport, bcm_port 
 *                  (-1 if not physical) and trunk_id (-1 if not
 *                  trunk)
 * @param pbmp : If is a physical port, it will be ADDED to this
 *             port bitmap.
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_hapi_portDescriptor_get(DAPI_USP_t *ddUsp, DAPI_t *dapi_g, ptin_hapi_intf_t *intf_desc, pbmp_t *pbmp);

/**
 * Apply a linkscan to this port
 * 
 * @param dapiPort: Port
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_hapi_linkscan_set(DAPI_USP_t *usp, DAPI_t *dapi_g, L7_uint8 enable);

/**
 * Get Egress port type definition
 * 
 * @param dapiPort  : Physical interface
 * @param port_type : Port type (PROMISCUOUS/COMMUNITY/ISOLATED)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t hapi_ptin_egress_port_type_get(ptin_dapi_port_t *dapiPort, L7_int *port_type);

/**
 * Egress port type definition
 * 
 * @param dapiPort  : Physical or logical interface
 * @param port_type : Port type (PROMISCUOUS/COMMUNITY/ISOLATED)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t hapi_ptin_egress_port_type_set(ptin_dapi_port_t *dapiPort, L7_int port_type);

/**
 * Attribute L2 learning priority to specified port
 * 
 * @param dapiPort : Physical or logical interface
 * @param macLearn_enable : Enable MAC Learning 
 * @param stationMove_enable : Enable L2 Station Move 
 * @param stationMove_prio   : L2 Station Move priority
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t hapi_ptin_l2learn_port_set(ptin_dapi_port_t *dapiPort, L7_int macLearn_enable, L7_int stationMove_enable, L7_int stationMove_prio, L7_int stationMove_samePrio);

/**
 * Get L2 learning attributes
 * 
 * @param dapiPort : Physical or logical interface
 * @param macLearn_enable    : Enable MAC Learning (output)
 * @param stationMove_enable : Enable L2 Station Move (output)
 * @param stationMove_prio   : L2 Station Move priority (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t hapi_ptin_l2learn_port_get(ptin_dapi_port_t *dapiPort, L7_int *macLearn_enable, L7_int *stationMove_enable, L7_int *stationMove_prio, L7_int *stationMove_samePrio);

/**
 * Read counters (physical interfaces)
 *  
 * Note: currently masks are ignored, meaning that all values are read
 * 
 * @param portStats Structure with port# and masks (port# must be set)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t hapi_ptin_counters_read(ptin_HWEthRFC2819_PortStatistics_t *portStats);

/**
 * Clears counters from a physical interface
 * 
 * @param port Port # (physical interface)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t hapi_ptin_counters_clear(L7_uint phyPort);

/**
 * Get counters activity (physical interfaces)
 *  
 * Note: at the moment, masks are ignored, therefore all values 
 * are read for all ports)
 * 
 * @param portsActivity Structure with port# and masks (port# must be set)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t hapi_ptin_counters_activity_get(ptin_HWEth_PortsActivity_t *portsActivity);

#if 0
/**
 * Add/remove a rate limiter to a particular traffic type
 * 
 * @param dapiPort : port 
 * @param enable   : Add or remove rate limit
 * @param rateLimit : rate limit profile
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t hapi_ptin_rateLimit_set(ptin_dapi_port_t *dapiPort, L7_BOOL enable, ptin_pktRateLimit_t *rateLimit);
#endif

/**
 * Configures storm control
 * 
 * @param dapiPort : port 
 * @param enable   : Enable or diable
 * @param stormControl : storm control data
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t hapi_ptin_stormControl_set(ptin_dapi_port_t *dapiPort, L7_BOOL enable, ptin_stormControl_t *stormControl);

#if 1
/**
 * Add port to storm control policies
 * 
 * @param dapiPort 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t hapi_ptin_stormControl_port_add(ptin_dapi_port_t *dapiPort);

/**
 * Remove port to storm control policies
 * 
 * @param dapiPort 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t hapi_ptin_stormControl_port_remove(ptin_dapi_port_t *dapiPort);
#endif

/**
 * Show trapped packets (to CPU) according to the configured 
 * trap rule 
 * 
 * @param bcm_port : Input port (bcm representation)
 * @param ovlan : Outer vlan
 * @param ivlan : Inner vlan
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_debug_trap_packets_show( L7_int bcm_port, L7_uint16 ovlan, L7_uint16 ivlan, L7_uchar8 *packet_data );

#endif /*_PTIN_HAPI_H */

