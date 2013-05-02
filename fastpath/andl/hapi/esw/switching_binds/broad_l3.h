/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename broad_l3.h
*
* @purpose This file includes all the messages for L3 and prototypes
*
* @component hapi
*
* @comments none
*
* @create 5/07/2002
*
* @author Andrey Tsigler
* @end
*
**********************************************************************/
#ifndef BROAD_L3_H_INCLUDED
#define BROAD_L3_H_INCLUDED
#include "dapi.h"

L7_RC_t hapiBroadL3PortInit(DAPI_PORT_t *dapiPortPtr);
L7_RC_t hapiBroadL3Init(DAPI_t *dapi_g);
L7_RC_t hapiBroadL3CardInit(L7_ushort16 unitNum,L7_ushort16 slotNum,DAPI_t *dapi_g);
L7_RC_t hapiBroadL3RouterCardRemove(DAPI_USP_t *usp, DAPI_CMD_t cmd,
                                   void *data, DAPI_t *dapi_g);
L7_RC_t hapiBroadL3UpdateMacLearn(mac_addr_t macAddr, L7_ushort16 vlanId, 
                                  DAPI_USP_t *usp, DAPI_t *dapi_g);
L7_RC_t hapiBroadL3UpdateMacAge(mac_addr_t macAddr, L7_ushort16 vlanId, 
                                DAPI_t *dapi_g);
L7_RC_t hapiBroadL3RoutingSend(DAPI_USP_t *usp, DAPI_FRAME_TYPE_t *frameType, 
                               L7_uchar8 *pktData, DAPI_t *dapi_g);
L7_RC_t hapiBroadL3PortVlanNotify(DAPI_USP_t  *usp, L7_ushort16 vlanId,
                                  L7_BOOL addDel, DAPI_t *dapi_g);
#endif
