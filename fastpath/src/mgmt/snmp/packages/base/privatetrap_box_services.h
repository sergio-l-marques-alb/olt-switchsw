/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2008
*
**********************************************************************
*
* Name: privatetrap_box_services.h
*
* Purpose: Box Services trap functions
*
* Created by: vkozlov
*
* Component: SNMP
*
*********************************************************************/

#ifndef _PRIVATETRAP_BOX_SERVICES_H
#define _PRIVATETRAP_BOX_SERVICES_H

#include "commdefs.h"


#include "l7_common.h"
        
/* Begin Function Definitions */

L7_RC_t snmp_boxsFanStateChangeTrapSend( L7_uint32 fanNum, L7_uint32  fanEventType);
L7_RC_t snmp_boxsTemperatureChangeTrapSend( L7_uint32 sensorNum, L7_uint32  tempEventType);
L7_RC_t snmp_boxsPowSupplyStateChangeTrapSend( L7_uint32 psmNum, L7_uint32  psmEventType);


/* End Function Definitions*/

#endif /* _PRIVATETRAP_BOX_SERVICES_H */

