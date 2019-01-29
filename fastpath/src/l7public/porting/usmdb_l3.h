

/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  usmdb_l3.h
*
* @purpose USMDB layer Common Include - Layer 3 package
*
* @component unitmgr
*
* @comments   none
*
* @create     08/06/2001
*
* @author     wjacobs
*
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#ifndef USMDB_L3_H

#define USMDB_L3_H
 
#include "l3_defaultconfig.h"

#include "usmdb_ospf_api.h"
#include "usmdb_mib_ospf_api.h"

#include "usmdb_ospfv3_api.h"
#include "usmdb_mib_ospfv3_api.h"

#include "usmdb_ip_api.h"
#include "usmdb_mib_ipfwd_api.h"

#include "usmdb_mib_ripv2_api.h"

#include "usmdb_mib_vrrp_api.h"

#include "usmdb_trap_layer3_api.h"

#include "usmdb_mib_relay_api.h"
                           
#include "usmdb_rtrdisc_api.h"
                           
#define USMDB_OSPF_AREA_DEFAULT_ADVERTISE_AREA  FD_OSPF_AREA_DEFAULT_ADVERTISE_AREA
#define USMDB_OSPF_AREA_DEFAULT_AREA_RANGE_DEFAULT_LSDBTYPE  FD_OSPF_AREA_DEFAULT_AREA_RANGE_LSDBTYPE

#endif                           
