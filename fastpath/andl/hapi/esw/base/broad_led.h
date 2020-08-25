/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2008
*
**********************************************************************
*
* @filename  broad_led.h
*
* @purpose   
*
* @component hapi
*
* @comments
*
* @create    8/14/08
*
* @author    Sudhir.M
*
* @end
*
**********************************************************************/

#ifndef INCLUDE_BROAD_LED_H
#define INCLUDE_BROAD_LED_H

#include "l7_common.h"
#include "broad_common.h"

#if L7_FEAT_CUSTOM_LED_BLINK
/* If this feature is defined then the following function must exist in a
   customer specific file. */
#define HAPI_BROAD_LED_BLINK_ENABLE_DISABLE(unit,blink) hapiBroadLedBlinkEnableDisable(blink)
#else
#define HAPI_BROAD_LED_BLINK_ENABLE_DISABLE(unit,blink) (BCM_E_UNAVAIL)
#endif

#endif
