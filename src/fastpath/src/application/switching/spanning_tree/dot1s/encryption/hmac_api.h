/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  hmac_api.h
*
* @purpose 	 HMAC with MD5
*
* @component Encryption
*
* @comments  from RFC 2104
*
* @create    9/19/2002
*
* @author    skalyanam
*
* @end
*             
**********************************************************************/
#ifndef HMAC_API_H
#define HMAC_API_H

#include "l7_common.h"

extern void L7_hmac_md5(L7_uchar8	*text, 
			            L7_uint32 text_len, 
			            L7_uchar8 *key, 
			            L7_uint32 key_len, 
			            L7_uchar8 *digest);
#endif /*HMAC_API_H*/
