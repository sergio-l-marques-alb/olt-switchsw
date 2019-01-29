/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename clustering_defaultconfig.h
*
* @purpose  To hold all clustering system default configuration values.
*
* @component none (yet)
*
* @comments This file contains default values for end user configuration
*           parameters.  These values are used to build default
*           configuration files for components.
*
* @create 12/06/2007
*
* @author dcaugherty
* @end
*
**********************************************************************/
#ifndef CLUSTERING_DEFAULTCONFIG_H
#define CLUSTERING_DEFAULTCONFIG_H

#include "datatypes.h"
#include "l7_common.h"
#include "l7_productconfig.h"       

#define FD_CLUSTER_MAX_PEER_SWITCHES   L7_WIRELESS_MAX_PEER_SWITCHES

#define FD_CLUSTER_PRIORITY            CLUSTER_MIN_PRIORITY

#define FD_CLUSTER_RELIABLE_MSG_SIZE   CLUSTER_MAX_RELIABLE_MSG_SIZE   
#define FD_CLUSTER_DATAGRAM_MSG_SIZE   CLUSTER_MAX_DATAGRAM_MSG_SIZE   

#endif /* CLUSTERING_DEFAULTCONFIG_H */
