/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_usl_bcm.h
*
* @purpose    Private datastructures and prototypes for USL
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
#ifndef L7_USL_BCM_H
#define L7_USL_BCM_H

#include "l7_common.h"
#include "log.h"
#include "l7_usl_bcm_debug.h"

#define USL_LOG_MSG(__type__,__format__, __args__...)  {\
    l7_logf((__type__ & USL_INFO_LOG)?L7_LOG_SEVERITY_DEBUG:L7_LOG_SEVERITY_ERROR,\
            L7_DRIVER_COMPONENT_ID, __FILE__, __LINE__, __format__, ##__args__);\
    }

#define USL_LOG_ERROR(format, args...) {\
		L7_uchar8 usl_log_buf[LOG_MSG_MAX_MSG_SIZE*2];\
	    if (sprintf (usl_log_buf, format, ##args) > (LOG_MSG_MAX_MSG_SIZE - 1))\
          usl_log_buf[LOG_MSG_MAX_MSG_SIZE-1] = (L7_uchar8)0x00;\
      usl_log_msg_fmt(__FILE__,__LINE__,usl_log_buf,L7_TRUE);\
    LOG_ERROR(666); \
    }

#endif /* L7_USL_BCM_H */
