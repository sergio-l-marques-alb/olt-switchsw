
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: snmpcttask.h
*
* Purpose: API interface for starting the SNMP Cable Test task
* 	
*
* Created by: Colin Verne 09/26/2002
*
* Component: SNMP
*
*********************************************************************/
/*************************************************************
                    
*************************************************************/

#ifndef SNMPCTTASK_H
#define SNMPCTTASK_H

#include "snmp_ct_api.h"

extern snmpCtCfg_t snmpCtCfg;
extern L7_BOOL snmpCtReady;

/*********************************************************************
*
* @purpose  Starts SNMP Cable Test task function
*
* @notes    none 
*
* @end
*********************************************************************/
void 
L7_snmp_ct_task ();

#endif /* SNMPVCTTASK_H */
