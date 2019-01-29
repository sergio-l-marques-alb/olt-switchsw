/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   pimdm_map_rto.h
*
* @purpose    PIM-DM Unicast Routing Table Access Related Function
*             Definitions
*
* @component  PIM-DM
*
* @comments   none
*
* @create     25-Apr-05
*
* @author     Krishna Mohan CS
* @end
*
**********************************************************************/
#ifndef _PIMDM_MAP_RTO_H
#define _PIMDM_MAP_RTO_H

#include "l7_common.h"

L7_RC_t
pimdmMapRTOBestRouteClientRegister (L7_uint32 addrFamily);

L7_RC_t
pimdmMapRTOBestRouteClientDeRegister (L7_uint32 addrFamily);

#endif /* _PIMDM_MAP_RTO_H */
