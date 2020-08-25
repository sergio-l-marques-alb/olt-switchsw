/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename  util_keying.h
* @purpose   Utility function headers are included in this file
* @component keying
* @comments  none
* @create    02/17/2004
* @author    Suhel Goel
* @end
*             
**********************************************************************/
#ifndef CLI_UTIL_KEYING_H
#define CLI_UTIL_KEYING_H

void featureKeyingMaskFillLeftMostBits(ComponentMask *keyingMask, L7_uint32 currCompId);

void featureKeyingMaskFLMaskBit(ComponentMask *keyingMask, L7_uint32 *nextCompId);

#endif
