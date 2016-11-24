/*
 * Product: EmStack/Stack
 * Release: R6_2_0
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION OF VIRATA CORPORATION
 * THE EMWEB SOFTWARE ARCHITECTURE IS PATENTED (US PATENT #5,973,696)
 * AND OTHER US AND INTERNATIONAL PATENTS PENDING.
 * 'EMWEB' AND 'EMSTACK' ARE TRADEMARKS OF VIRATA CORPORATION
 * 
 * Notice to Users of this Software Product:
 * 
 * This software product of Virata Corporation ("Virata"), 5 Clock Tower
 * Place, Suite 400, Maynard, MA  01754 (e-mail: info@virata.com) in 
 * source and object code format embodies valuable intellectual property 
 * including trade secrets, copyrights and patents which are the exclusive
 * proprietary property of Virata. Access, use, reproduction, modification
 * disclsoure and distribution are expressly prohibited unless authorized
 * in writing by Virata.  Under no circumstances may you copy this
 * software or distribute it to anyone else by any means whatsoever except in
 * strict accordance with a license agreement between Virata and your
 * company.  This software is also protected under patent, trademark and 
 * copyright laws of the United States and foreign countries, as well as 
 * under the license agreement, against any unauthorized copying, modification,
 * or distribution.
 * Please see your supervisor or manager regarding any questions that you may
 * have about your right to use this software product.  Thank you for your
 * cooperation.
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2001 Virata Corporation
 * All Rights Reserved
 * 
 * Virata Corporation
 * 5 Clock Tower Place
 * Suite 400
 * Maynard, MA  01754
 * 
 * Voice: +1 (978) 461-0888
 * Fax:   +1 (978) 461-2080
 * 
 * http://www.emweb.com/
 *   support@agranat.com
 *     sales@agranat.com
 *
 * EmWeb/EmStack Buffer Configuration Definitions
 *
 */

#ifndef _EB_CONFIG_H_
#define _EB_CONFIG_H_


/*
 * EB_DEV_GATHER_SND - define if your device layer can gather data from
 *                        a chain of buffers to be sent as a single packet
 *                        without performing a data copy
 */
#define EB_DEV_GATHER_SND

/* See comments in ../lib/buffer/ew_buffer.c */

#ifndef EB_MAX_BUFFER_DESCR
#define EB_MAX_BUFFER_DESCR 1024 // increased from 512
#endif
#ifndef EB_MAX_BUFFER_SEGS
/* increased EB_MAX_BUFFER_SEGS from 512, so that EB_MAX_BUFFER_DESCR and EB_MAX_BUFFER_SEGS are same and we use the same number to allocate the buffers during initalization so that all the segements and buffers are available in free pool */
#define EB_MAX_BUFFER_SEGS  1024
#endif
#define EB_SYS_SEG_DYNAMIC
#ifndef EB_SYS_SEGMENT_SIZE
#define EB_SYS_SEGMENT_SIZE 1024 // increased from 256
#endif

#include <stdlib.h>             /* to get malloc and free only */

/*
 * Minimum First Buffer Shortcut Control
 *
 *   If the device layer ensures that the first buffer in any chain passed
 *   to IP is large enough to contain the entire IP and TCP headers
 *   (usually TBD bytes), then set EMWEB_SYS_CONTIGUOUS_HEADERS to 1.
 *   This saves some data copies.
 */
/* #define EB_SYS_CONTIGUOUS_HEADERS 1 */

/* ################################################################
 * System environment Definitions
 *################################################################ */

#include "commdefs.h"
#include "osapi.h"

#define ebAlloc(x) osapiMalloc(L7_CLI_WEB_COMPONENT_ID, x)
#define ebFree(x)  osapiFree(L7_CLI_WEB_COMPONENT_ID,x)

#define EMBUF_TRACE(x) /* printf x */
#define EMBUF_WARN(x) /* printf x */
#define EMBUF_ERROR(x) { printf x; fatal_trap( __FILE__, __LINE__ ); }


/*
 *** Local Variables: ***
 *** mode: c ***
 *** tab-width: 4 ***
 *** End: ***
 */
#endif /* _EB_CONFIG_H_ */
