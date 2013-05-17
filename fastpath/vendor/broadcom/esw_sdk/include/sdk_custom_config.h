/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  sdk_custom_config.h
*
* @purpose   Overrides for constants defined by the ESW SDK. 
*
* @component Driver
*
* @comments  This file is specific to FASTPATH/SmartPATH. 
*
* @create    3/24/2009
*
* @author    cmutter
*
* @end
*
**********************************************************************/
#ifndef _SDK_CUSTOM_CONFIG_H
#define _SDK_CUSTOM_CONFIG_H

#include "flex.h"

#define BCM_RLINK_RX_REMOTE_MAX_DEFAULT {10, 10, 10, 80, 80, 0, 0, 0}
#define BCM_RLINK_L2_REMOTE_MAX_DEFAULT 0

#ifdef L7_PRODUCT_SMARTPATH

#define SOC_MAX_NUM_SWITCH_DEVICES 2
#define SAL_THREAD_STKSZ 16384

#else /* Not SmartPATH */

#define SAL_THREAD_STKSZ 32768

#endif /* Not SmartPATH */

#if defined(L7_PRODUCT_SMB) || defined(L7_PRODUCT_SMARTPATH)

#define BCM_RX_POOL_COUNT_DEFAULT 96

#else /* Not SMB or SmartPATH */

#define BCM_RX_POOL_COUNT_DEFAULT 384

#endif /* Not SMB or SmartPATH */

#if defined(L7_PRODUCT_SMARTPATH)
#define BCM_LOCAL_UNITS_MAX     8
#else
#define BCM_LOCAL_UNITS_MAX     18
#endif

#if (defined(L7_PRODUCT_SMB) || defined(L7_PRODUCT_SMARTPATH))
#define CT_TUNNEL_QUEUE_SIZE 64
#else
#define CT_TUNNEL_QUEUE_SIZE 256
#endif

#ifdef PC_LINUX_HOST
#define ATP_RETRY_TIMEOUT_DEFAULT 10000000
#else
#define ATP_RETRY_TIMEOUT_DEFAULT 1000000
#endif
#define ATP_RETRY_COUNT_DEFAULT   20

  /*
  **       The RPC timeout interval is chosen to be twice the overall ATP 
  **       timeout. Since transaction requires two ATP transfers plus some work on 
  **       remote unit, the timeout should theoretically be twice the ATP 
  **       timeout plus some processing time, but since we use a very long ATP
  **       timeout we ignore the processing time delay.
  */
#define RPC_REPLY_TIMEOUT (2 * ATP_RETRY_TIMEOUT_DEFAULT * ATP_RETRY_COUNT_DEFAULT)


#if L7_FEAT_CUSTOM_MMU
#include "brd_e2e_overrides.h"
#endif

#endif /* _SDK_CUSTOM_CONFIG_H */
