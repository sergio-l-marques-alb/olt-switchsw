/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename sslt.h
*
* @purpose SSL Tunnel base headers
*
* @component sslt
*
* @comments none
*
* @create 07/15/2003
*
* @author spetriccione
*
* @end
*
**********************************************************************/

#include "sslt_exports.h"

#ifndef INCLUDE_SSLT_H
#define INCLUDE_SSLT_H

/* SSL Tunnel Buffer
 * Temporarily changing SSL Tunnel buffer to 16k.
 * The last chunk of data in IE7 and IE8 is always comes with 8k.
 * This causes issue code download through https session in IE7 and IE8.
 * With 16k buffer it works as IE7 and IE8 sends data chunk with 16k-33 i.e. 16531.
 */
#define SSLT_BUF_SIZE 16384 

#define SSLT_ADDRESS_BUFFER_MAX       64
#define SSLT_PEM_CERT_HEADER          "-----BEGIN CERTIFICATE-----\n"
#define SSLT_PEM_CERT_FOOTER          "\n-----END CERTIFICATE-----\n"
#define SSLT_PEM_CERT_REQ_HEADER      "-----BEGIN CERTIFICATE REQUEST-----\n"
#define SSLT_PEM_CERT_REQ_FOOTER      "\n-----END CERTIFICATE REQUEST-----\n"
#define SSLT_DER_CERT_REQ_SIZE_MAX (L7_SSLT_PEM_BUFFER_SIZE_MAX - \
                                    strlen(SSLT_PEM_CERT_REQ_HEADER) - \
                                    strlen(SSLT_PEM_CERT_REQ_FOOTER) - 1)

/*
** Internal function prototypes
*/

L7_uint32 ssltConnectionLoop(SSL *ssl,
                             L7_int32 sock_fd,
                             ssltSecureTypes_t secureType);

void ssltServerUnSecureDataSend(L7_uint32 sock_fd,
                                L7_uchar8 *buf,
                                L7_uint32 size);

L7_RC_t ssltServerSecureDataSend(SSL *ssl,
                                 L7_uchar8 *buf,
                                 L7_uint32 size);

#endif /* INCLUDE_SSLT_H */

