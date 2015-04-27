/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_usl_bcm_debug.h
*
* @purpose    Handle debug facilities for usl
*
* @component  HAPI
*
* @comments   none
*
* @create     11/19/2003
*
* @author     mbaucom
* @end
*
**********************************************************************/
#ifndef _L7_USL_BCM_DEBUG_H
#define _L7_USL_BCM_DEBUG_H

#include "l7_common.h"
#include "bcmx/l2.h"

extern void usl_log_msg_fmt(L7_char8 * file, L7_ulong32 line, L7_char8 * msg,L7_BOOL overRide);
extern void usl_print_log(L7_BOOL set);
extern void usl_lplist_print(bcmx_lplist_t *list);
extern L7_uint32 usl_allowed_log_types_get(void);
extern void usl_allowed_log_types_set(L7_uint32 types);
#endif
