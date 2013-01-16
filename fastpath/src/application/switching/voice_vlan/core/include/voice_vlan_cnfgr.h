/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
* @filename  voice_vlan_cnfgr.h
*
* @purpose   Voice Vlan configurator file
*
* @component voiceVlan
*
* @comments 
*
* @create    
*
* @author    
*
* @end
*             
**********************************************************************/

#ifndef VOICE_VLAN_CNFGR_H
#define VOICE_VLAN_CNFGR_H

#include "l7_cnfgr_api.h"

#define VOICE_VLAN_IS_READY \
  (((voiceVlanPortCnfgrState == VOICE_VLAN_PHASE_WMU) || \
    (voiceVlanPortCnfgrState == VOICE_VLAN_PHASE_INIT_3) || \
    (voiceVlanPortCnfgrState == VOICE_VLAN_PHASE_EXECUTE) || \
    (voiceVlanPortCnfgrState == VOICE_VLAN_PHASE_UNCONFIG_1)) \
   ? (L7_TRUE) : (L7_FALSE))

                  typedef enum
{
  VOICE_VLAN_PHASE_INIT_0 = 0,
  VOICE_VLAN_PHASE_INIT_1,
  VOICE_VLAN_PHASE_INIT_2,
  VOICE_VLAN_PHASE_WMU,
  VOICE_VLAN_PHASE_INIT_3,
  VOICE_VLAN_PHASE_EXECUTE,
  VOICE_VLAN_PHASE_UNCONFIG_1,
  VOICE_VLAN_PHASE_UNCONFIG_2,
} voiceVlanCnfgrState_t;


/* Begin Function Prototypes */

void voiceVlanApiCnfgrCommand(L7_CNFGR_CMD_DATA_t * pCmdData);  

/* End Function Prototypes */
#endif /* VOICE_VLAN_CNFGR_H */

