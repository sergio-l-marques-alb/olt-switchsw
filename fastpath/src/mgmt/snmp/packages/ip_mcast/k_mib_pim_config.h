/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src\mgmt\snmp\snmp_sr\snmpd\unix\k_mib_pim.c
*
* @purpose  Provide interface to switch between PIM-SM and PIM-DM mode
*
* @component SNMP
*
* @comments none
*
* @create 11/23/2002
*
* @author Colin Verne
* @end
*
**********************************************************************/
/********************************************************************
 *
 *******************************************************************/
 
#ifndef K_MIB_PIM_CONFIG_H
#define K_MIB_PIM_CONFIG_H

#include "usmdb_mib_pim_rfc5060_api.h"

typedef enum
{
  L7_SNMP_PIM_MODE_SPARSE,
  L7_SNMP_PIM_MODE_DENSE
} snmp_pim_mode_t;
  
extern L7_uint32 snmp_pim_mode;


#endif /* K_MIB_PIM_CONFIG_H */
