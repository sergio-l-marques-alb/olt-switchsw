/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
* @filename  protected_port_include.h
*
* @purpose   Protected Port include header
*
* @component protectedPort
*
* @comments
*
* @create    6/5/2005
*                                                         
* @author    ryadagiri 
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_PROTECTED_PORT_INCLUDE_H
#define INCLUDE_PROTECTED_PORT_INCLUDE_H

/*
 ***********************************************************************
 *                            COMMON INCLUDES
 ***********************************************************************
 */

#include "l7_common.h"
#include "osapi.h"
#include "log.h"
#include "nvstoreapi.h"
#include "defaultconfig.h"
#include "default_cnfgr.h"
#include "comm_mask.h"
#include "dtlapi.h"

/*
 **********************************************************************
 *                            HEADER FILES
 **********************************************************************
 */
#include "protected_port_cfg.h"
#include "protected_port_cnfgr.h"
#include "protected_port_util.h"
#include "protected_port_migrate.h"
#include "protected_port_api.h"

/*
 **********************************************************************
 *                            EXTERNAL DATA STRUCTURES
 **********************************************************************
 */
extern osapiRWLock_t protectedPortCfgRWLock;
extern protectedPortCfg_t *protectedPortCfg;
extern protectedPortCnfgrState_t protectedPortCnfgrState;

/* Operational info of the Protected port interface */
extern protectedPortIntfInfo_t     *protectedPortIntfInfo;

#endif /*INCLUDE_PROTECTED_PORT_INCLUDE_H */
