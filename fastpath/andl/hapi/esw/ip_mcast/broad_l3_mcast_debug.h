/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename broad_l3_mcast.h
*
* @purpose This file includes all the messages for L3 IP Multicast and prototypes
*
* @component hapi
*
* @comments none
*
* @create
*
* @author
* @end
*
**********************************************************************/
#ifndef INCLUDE_MD_L3_MCAST_H
#define INCLUDE_MD_L3_MCAST_H

#include "dapi.h"

/*********************************************************************
*
* @purpose Init debug routines for IP Mcast package
*
* @param   
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadL3McastDebugInit();

/*********************************************************************
*
* @purpose enable/diables the mcast in the silicon
*
* @param  enable   - enable/diables the mcast in the silicon 
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiDebugMcastEnable(int enable);

/*********************************************************************
*
* @purpose enable/diables the MLD in the silicon
*
* @param  family   - IPv6 or IPv4 family
* @param  enable   - enable/diables the MLD in the silicon
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiDebugMcastMldEnable(int family, int enable);

/*********************************************************************
*
* @purpose modify the TTL value for the interface
*
* @param  inIntIfNum     - interface number
* @param  mcastTtlValue  - TTL value
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiDebugMcastTTLSet(L7_uint32 inIntIfNum, L7_uint32 mcastTtlValue);
#endif
