/*
 * ptin_cfg.h
 *
 * Created on: 2010/04/08
 * Author: Andre Brizido
 * 
 * Modified on 2011/06/14 
 * By: Alexandre Santos (alexandre-r-santos@ptinovacao.pt) 
 * Notes: 
 */

#ifndef _PTIN_CFG_H
#define _PTIN_CFG_H

#include "ptin_include.h"


/***************************************************************************** 
 * Functions Prototypes
 *****************************************************************************/

/**
 * Gets Network Connectivity configuration
 * 
 * @param ntwConn Pointer to the output data (mask defines what to read)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_cfg_ntw_connectivity_get(ptin_NtwConnectivity_t *ntwConn);

/**
 * Sets Network Connectivity configuration
 * 
 * @param ntwConn Pointer to the config data (mask defines what to set)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_cfg_ntw_connectivity_set(ptin_NtwConnectivity_t *ntwConn);

/**
 * Returns the external inBand VLAN ID
 * 
 * @return L7_uint16 inBand VID (zero if not configured)
 */
extern inline L7_uint16 ptin_cfg_inband_vlan_get(void);

/**
 * Creates a bridge between dtl0 interface and a virtual interface eth0.4093 
 *  
 * NOTE: 
 *  1. virtual interface eth0.4093 is created here 
 *  2. all operations are accomplished through an external shell script 
 *     '/usr/local/ptin/scripts/startBridge.sh'
 * 
 * @author alex (4/10/2012)
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_cfg_inband_bridge_set(void);

/**
 * Creates a bridge between dtl0.2048 interface and a virtual 
 * interface eth0.2048
 *  
 * NOTE: 
 *  1. virtual interface eth0.2048 is created here 
 *  2. all operations are accomplished through an external shell script 
 *     '/usr/local/ptin/scripts/startPcapBridge.sh' 
 * 
 * @author joaom (12/16/2015)
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_cfg_pcap_bridge_set(void);

#endif /* _PTIN_CFG_H */
