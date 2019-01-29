/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  dot1x_local.h
*
* @purpose   dot1x local authentication routines include file
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
#ifndef INCLUDE_DOT1X_LOCAL_H
#define INCLUDE_DOT1X_LOCAL_H

extern L7_RC_t dot1xLocalAuthResponseProcess(dot1xLogicalPortInfo_t *logicalPortInfo, L7_netBufHandle bufHandle);
extern L7_RC_t dot1xLocalAuthMd5ResponseValidate(dot1xLogicalPortInfo_t *logicalPortInfo, L7_uchar8 *response);
extern void dot1xLocalAuthChallengeGenerate(L7_uchar8 *challenge, L7_uint32 challengeLen);
extern void dot1xLocalMd5Calc(L7_uchar8 *inBuf, L7_uint32 inLen, L7_uchar8 *outBuf);

#endif /* INCLUDE_DOT1X_LOCAL_H */
