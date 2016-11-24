/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename openssl_api.h
*
* @purpose SSL Tunnel API header
*
* @component openssl
*
* @comments none
*
* @create  11/15/2006
*
* @author jshaw
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_OPENSSL_API_H
#define INCLUDE_OPENSSL_API_H

#include <openssl/ssl.h>

#define OPENSSL_PEM_CERT_HEADER          "-----BEGIN CERTIFICATE-----\n"
#define OPENSSL_PEM_CERT_FOOTER          "\n-----END CERTIFICATE-----\n"
/*********************************************************************
*
* @purpose Get the openssl cipher list
*
* @returns L7_SUCCESS
*
* @comments
*
*  Cipher Suite Selection from http://www.openssl.org/docs/apps/ciphers.html
*
*  ALL - Include every available combination (all suites except the eNULL ciphers
*        which must be explicitly enabled)
*  TLSv1 - Transport Layer Security version 1 cipher suites
*  SSLv3 - Secure Sockets Layer version 3 cipher suites
*  !SSLv2 - Remove Secure Sockets Layer version 2 cipher suites
*  AES - Advanced Encryption Standard ciphers suite
*  3DES- Include Triple DES three stage ciphers suite
*  DES- Include Data Encryption Standard ciphers suite
*  RC4- Include RC4 ciphers suite
*  RC2- Include RC2 ciphers suite
*  SHA - ciphers suites using SHA
*  IDEA - ciphers suites using IDEA
*  MD5 - Allow MD5 even though it has weaknesses and is "nearly-broken"
*  !aNULL - Remove the cipher suites offering no authentication
*  !eNULL - Remove the cipher suites offering no encryption
*  !ADH - Remove Anonymous ciphers
*  !EXPORT40 - Remove Export-Crippled 40 bit ciphers
*  EXPORT56 - Export-Crippled 56 bit ciphers
*  EXPORT - Export-Crippled 40 and 56 bit ciphers
*  HIGH -  'high' encryption cipher suites. This currently means those with key lengths larger than 128 bits,
*          and some cipher suites with 128-bit keys.
*  MEDIUM - 'medium' encryption cipher suites, currently some of those using 128 bit encryption.
*  LOW - 'low' encryption cipher suites, currently those using 64 or 56 bit encryption algorithms
*        but excluding export cipher suites.
*  @STRENGTH - Sort list by their strength and select most secure
*
* @end
*
*********************************************************************/
L7_RC_t opensslCipherListGet(L7_char8 *cipher_list);

/*********************************************************************
*
* @purpose Seed the OpenSSL Random Number Generator
*
* @param L7_char8 *randomFile @b{(input)} pointer file name to read/write
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
void opensslSeedPrng(L7_char8 *randomFile);

/*********************************************************************
*
* @purpose Set the SSL connection to Blocking
*
* @param ssl @b{(input)} pointer to the SSL connection object
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
void opensslSecureBlockingSet(SSL *ssl);

/*********************************************************************
*
* @purpose Set the SSL connection to Non-Blocking
*
* @param ssl @b{(input)} pointer to the SSL connection object
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
void opensslSecureNonBlockingSet(SSL *ssl);

/*********************************************************************
*
* @purpose Prompt for a PEM pass phrase
*
* @returns void
*
* @comments FASTPATH does not support interactive password prompting
*
* @end
*
*********************************************************************/
void opensslPemPassPhrasePrompt(void);


/*********************************************************************
*
* @purpose  Read server key from PEM file
*
* @param    L7_char8 *keyFile   @b((input)) ptr to file name
* @param    EVP_PKEY **pkey     @b((output)) server key
*
*
* @returns  L7_SUCCESS, key returned
* @returns  L7_FAILURE, indicated key file corrupt or does not exist
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t opensslServerKeyRead(L7_char8 *keyFile, EVP_PKEY **pkey);


/*********************************************************************
*
* @purpose  Write server key to PEM file
*
* @param    L7_char8 *keyFile   @b((input)) ptr to file name
* @param    EVP_PKEY **pkey     @b((output)) server key
*
*
* @returns  L7_SUCCESS, file written
* @returns  L7_FAILURE, failed to write key file
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t opensslServerKeyWrite(L7_char8 *keyFile, EVP_PKEY *pkey);

/*********************************************************************
*
* @purpose  Write server certificate to PEM file
*
* @param    L7_char8 *certFile @b((input)) ptr to file name
* @param    X509      *cert    @b((input))  server certificate
*
*
* @returns  L7_SUCCESS, file written
* @returns  L7_FAILURE, failed to write certificate file
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t opensslServerCertWrite(L7_char8 *certFile, X509 *cert);


/*********************************************************************
*
* @purpose  Read server certificate from PEM file
*
* @param    L7_char8 *certFile @b((input)) ptr to file name
* @param    L7_char8  *certPEM @b((output)) server certificate PEM format
* @param    X509      *cert    @b((output)) server certificate internal format
*
*
* @returns  L7_SUCCESS,
* @returns  L7_FAILURE, failed to read cert file
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t opensslServerCertRead(L7_char8 *certFile, L7_char8 *certPEM, X509 **certX509);

/*********************************************************************
*
* @purpose  Write DH params to PEM file
*
* @param    L7_char8 *file   @b{(input)}
* @param    DH       *dh    @b((input))  diffie hellman parameters
*
*
* @returns  L7_SUCCESS, file written
* @returns  L7_FAILURE, failed to write pem file
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t opensslDiffieHellmanParamsWrite(L7_char8 *file, DH *dh);

/*********************************************************************
*
* @purpose  Print ASN1 time string to a buffer
*
* @param    ASN1_TIME *tm @b((input)) pointer to ASN1 time object
* @param    L7_char8  *bp @b{(output)} pointer to output buffer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments wrap the functions copied from openssl above.
*
* @end
*
*********************************************************************/
L7_RC_t opensslASN1TimePrint(ASN1_TIME *tm, L7_char8 *bp);

/*********************************************************************
*
* @purpose  Print MD5 certificate fingerprint to a buffer
*
* @param    X509      *cert @b((input)) pointer to ASN1 time object
* @param    L7_char8  *bp   @b{(output)} pointer to output buffer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments buffer must be at least L7_WIRELESS_SSL_FINGERPRINT_MD5_SIZE_MAX
*
* @end
*
*********************************************************************/
L7_RC_t opensslX509DigestPrint(X509 *cert, L7_char8 *bp);

/*********************************************************************
*
* @purpose  Open file, return open FILE *
*
* @param    L7_char8  *filename
*
*
* @returns  FILE *, L7_NULLPTR if open failed
*
* @comments
*
* @end
*
*********************************************************************/
FILE *opensslFileOpen(L7_char8 *filename);

/*********************************************************************
*
* @purpose Initialize parameters for the Diffie-Hellman Key
*          exchange protocol.
*
* @param    L7_char8  *dhWeakFile @b{(input)} pointer to weak dh file name
* @param    L7_char8  *dhWeakFile @b{(input)} pointer to strong dh file name
* @param    DH       **dh512      @b{(output)} double pointer to weak dh structure
* @param    DH       **dh1024     @b{(output)} double pointer to strong dh structure
*
* @returns L7_SUCCESS, parameters read
*          L7_FAILURE, failed to read DH pem files.
*
* @comments  called during startup, will use sema to assign static globals
*
* @end   cert semaphore should be held for this call
*
*********************************************************************/
L7_RC_t opensslDiffieHellmanParamsInit(L7_char8 *dhWeakFile, L7_char8 *dhStrongFile,
                                       DH **dh512P, DH **dh1024P);

/*********************************************************************
*
* @purpose Free parameters for the Diffie-Hellman Key
*          exchange protocol.
*
* @param    DH       **dh512      @b{(input)} double pointer to weak dh structure
* @param    DH       **dh1024     @b{(input)} double pointer to strong dh structure
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
void    opensslDiffieHellmanParamsFree(DH **dh512P, DH **dh1024P);

/*********************************************************************
*
* @purpose  Initialize parameters for the Diffie-Hellman Key
*           exchange protocol.
*
* @param    void      *semaP      @b{(input)} pointer to semaphore to use to protect dh params
* @param    DH       **dh512      @b{(output)} double pointer to weak dh structure
* @param    DH       **dh1024     @b{(output)} double pointer to strong dh structure
*
* @returns  void
*
* @comments  This function will generate the parameters if required
*            it should always be called on a separate task, these
*            take a long time, will use sema to assign static globals.
*
* @end
*
*********************************************************************/
void    opensslDiffieHellmanParamsCreate(void **semaP, DH **dh512P, DH **dh1024P);
#endif /* INCLUDE_OPENSSL_API_H */

