/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename sslt_util.h
*
* @purpose SSL Tunnel Utilities header
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
#ifndef INCLUDE_SSLT_UTIL_H
#define INCLUDE_SSLT_UTIL_H

#include <openssl/ssl.h>

/* 
** Internal function prototypes
*/
void ssltSeedPrng(void);

L7_RC_t ssltDiffieHellmanParamsExist(void);
L7_RC_t ssltDiffieHellmanParamsInit(void);
void ssltDiffieHellmanParamsFree(void);
void ssltDiffieHellmanParamsCreate(void);

DH *ssltDiffieHellmanCallback(SSL *ssl, 
                              int is_export, 
                              int keylength);

SSL_CTX *ssltContextCreate(void);
L7_RC_t ssltListenTaskRefresh(ssltSecureTypes_t secureType);

void ssltCheckAndListenTaskCleanup(void);
void ssltListenTaskCleanup(ssltSecureTypes_t secureType);

void ssltConnTaskCleanup(SSL *ssl);

L7_RC_t ssltDebugLevelSet(L7_uint32 level);

L7_RC_t ssltServerCertFileGet(L7_uint32  number,
                              L7_char8  *filename);

L7_RC_t ssltServerKeyFileGet(L7_uint32 number,
                             L7_char8 *filename);

L7_RC_t ssltServerCertRead(L7_uint32  number,
                           L7_char8  *certPEM,
                           X509     **certX509);

L7_RC_t ssltServerCredentialsCreate(L7_uint32 number,
                                    L7_uint32 keyLength,
                                    L7_char8 *commonName,
                                    L7_char8 *orgName, 
                                    L7_char8 *orgUnit,
                                    L7_char8 *location,
                                    L7_char8 *state,
                                    L7_char8 *country,
                                    L7_char8 *email,
                                    L7_uint32 days);


L7_RC_t ssltServerCredentialsImport(L7_uint32 number,
                                    L7_char8 *cert);

L7_RC_t ssltServerCertReqCreate(L7_uint32  number, 
                                L7_char8  *commonName, 
                                L7_char8  *orgName, 
                                L7_char8  *orgUnit, 
                                L7_char8  *location, 
                                L7_char8  *state, 
                                L7_char8  *country,
                                L7_char8  *email,
                                L7_char8  *req);

L7_RC_t ssltCertGenerate(L7_uint32 number,
                         L7_uint32 keyLength,
                         L7_char8 *commonName,
                         L7_char8 *orgName,
                         L7_char8 *orgUnit,
                         L7_char8 *location,
                         L7_char8 *state,
                         L7_char8 *country,
                         L7_char8 *email,
                         L7_uint32 days);

L7_RC_t ssltASN1TimePrint(ASN1_TIME *tm,
                          L7_char8  *bp);

L7_RC_t ssltX509DigestPrint(X509      *cert,
                            L7_char8  *bp);

#endif /* INCLUDE_SSLT_UTIL_H */

