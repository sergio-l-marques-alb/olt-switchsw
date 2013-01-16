/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  dot1x_outcalls.h
*
* @purpose   dot1x outcalls include file
*
* @component dot1x
*
* @comments  none
*
* @create    04/08/2003
*
* @author    mfiorito
*
* @end
*             
**********************************************************************/

#ifndef INCLUDE_DOT1X_OUTCALLS_H
#define INCLUDE_DOT1X_OUTCALLS_H

extern L7_RC_t dot1xOutcallsSysInit();
extern L7_RC_t dot1xTxKey(L7_uint32 intIfNum);
extern L7_RC_t dot1xProcessKey(L7_uint32 intIfNum, L7_netBufHandle bufHandle);

#endif /* INCLUDE_DOT1X_OUTCALLS_H */
