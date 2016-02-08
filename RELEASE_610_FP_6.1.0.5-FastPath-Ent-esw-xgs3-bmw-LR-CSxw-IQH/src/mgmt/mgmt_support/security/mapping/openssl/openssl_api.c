/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename     openssl_api.c
*
* @purpose      OPENSSL API functions
*
* @component    openssl
*
* @comments     none
*
* @create       11/15/2006
*
* @author       jshaw
*
* @end
*
**********************************************************************/

#include "openssl_include.h"
#include "defaultconfig.h"
#include "sslt_exports.h"
#ifdef L7_WIRELESS_PACKAGE
#include "wireless_commdefs.h"
#include "wireless_defaultconfig.h"
#endif /* L7_WIRELESS_PACKAGE */

#define OPENSSL_CIPHER_LIST        "ALL:!aNULL:!eNULL:!ADH:!EXPORT40"

static L7_BOOL dhGenerateInProgress = L7_FALSE;
extern void *opensslDhSema;
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
* @notes Lists of cipher suites can be combined in a single cipher string using the + character.
*        This is used as a logical "AND" operation. For example SHA1+DES represents all cipher
*        suites containing the SHA1 and the DES algorithms. Each cipher string can be optionally
*        preceded by the characters !, - or +. If ! is used then the ciphers are permanently
*        deleted from the list. The ciphers deleted can never reappear in the list even if
*        they are explicitly stated.  If - is used then the ciphers are deleted from the list,
*        but some or all of the ciphers can be added again by later options. If + is used then
*        the ciphers are moved to the end of the list. This option doesn't add any new ciphers
*        it just moves matching existing ones.  If none of these characters is present then the
*        string is just interpreted as a list of ciphers to be appended to the current preference
*        list. If the list includes any ciphers already present they will be ignored: that is they
*        will not moved to the end of the list.
*
* @end
*
*********************************************************************/
L7_RC_t opensslCipherListGet(L7_char8 *cipher_list)
{
  L7_char8 tempList[256];
  osapiStrncpy(tempList, OPENSSL_CIPHER_LIST, sizeof(tempList));

#if (L7_SECURITY_SUPPORT_DES == 0)
  osapiStrncat(tempList, ":!DES", sizeof(tempList));
#endif
#if (L7_SECURITY_SUPPORT_3DES == 0)
  osapiStrncat(tempList, ":!3DES", sizeof(tempList));
#endif
#if (L7_SECURITY_SUPPORT_AES == 0)
  osapiStrncat(tempList, ":!AES", sizeof(tempList));
#endif
#if (L7_SECURITY_SUPPORT_RC4 == 0)
  osapiStrncat(tempList, ":!RC4", sizeof(tempList));
#endif
#if (L7_SECURITY_SUPPORT_RC2 == 0)
  osapiStrncat(tempList, ":!RC2", sizeof(tempList));
#endif
#if (L7_SECURITY_SUPPORT_IDEA == 0)
  osapiStrncat(tempList, ":!IDEA", sizeof(tempList));
#endif

#if (L7_SECURITY_SUPPORT_SSLV2 == 0)
  osapiStrncat(tempList, ":!SSLv2", sizeof(tempList));
#endif
#if (L7_SECURITY_SUPPORT_SSLV3 == 0)
  osapiStrncat(tempList, ":!SSLv3", sizeof(tempList));
#endif
#if (L7_SECURITY_SUPPORT_TLSV1 == 0)
  osapiStrncat(tempList, ":!TLSv1", sizeof(tempList));
#endif

  osapiStrncat(tempList, ":@STRENGTH", sizeof(tempList));
  osapiStrncpy(cipher_list, tempList, sizeof(tempList));

  return L7_SUCCESS;
}
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
void opensslSeedPrng(L7_char8 *randomFile)
{
  L7_char8 randomSeed[] = "1968336945jsdoi983unf09ljf903jfeo9uj309jfed09ujkldgnf-u3099\
106019d;ljf093ufa09jfm3209fadg90uafbd809h32r08fhdfg0[hfbn= xkd09ewofjq09ugjfd700700246\
61874616dlkfasldjf9usadfasd9f8usdalfjasd09fsadjf09fj0293 udfoivklglhae[0uwnfa9s0dfdlxh\
62075903908ur;ojdf09qurj3qf09ewuf;jq390ru3lkfm09ewurj32;09qcj0923qrdf;md09j29270573100\
04684008546840949070940110464064685123170613687316136873394323749731694613496310090248\
as;dfj09qetnfaewp9v8haweg980qhgt nf89hv[qrg'jdzfpv9ha fuewqofdas5487228442508575825967\
607808616078654ejkjf09u3;09r09asdjfkl9032r934urfg98qh3gnthbg9h]4rufj[0ajfq]we9u0934uj0\
60878103204946532119870431790464797987981713722467847190777959352265343379306519410125\
091684065719406451309u8rklja09u3j09rq093jr0923re09ff8guyj39um09ajef0329urergf093q66409\
06389794lkasjdf092u3rjd09url;gj09gu20990K)(UJ)(j093j09ufed09gj';e9ou3j09ladj09fug06653\
601715975189520189103sldkjf0932urnds0923072680137298017313444030079107190kkj3fde381257";

  if (randomFile == L7_NULLPTR)
  {
    LOG_MSG("OPENSSL: random file is null, exiting\n");
    return;
  }

  /* Seed with any unpredictable data */
  if (RAND_load_file(randomFile, -1) == 0)
  {
    /* Create the random file and try to load it again */
    if (((osapiFsWrite((L7_char8 *)randomFile,
                                 (L7_char8 *)&randomSeed,
                                 sizeof(randomSeed))) != L7_SUCCESS) ||
        ((RAND_load_file(randomFile, -1) == 0)))
    {
      LOG_MSG("OPENSSL: Error loading random file, %s\n", randomFile);
    }
  }

  /* Create new random data file for next time */
  if (RAND_write_file(randomFile) == -1)
  {
    LOG_MSG("OPENSSL: Error creating NEW random file %s\n", randomFile);
  }

  return;
}

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
void opensslSecureBlockingSet(SSL *ssl)
{
  L7_int32 ssl_rfd, ssl_wfd;

  /* Set the SSL read connection to Blocking */
  if ((ssl_rfd = SSL_get_rfd(ssl)) > 0)
  {
    if (osapiSocketNonBlockingModeSet(ssl_rfd,L7_FALSE) != L7_SUCCESS)
    {
      LOG_MSG("OPENSSL: failed to set read socket %d to blocking\n", ssl_rfd);
    }
  }

  /* Set the SSL write connection to Blocking */
  if ((ssl_wfd = SSL_get_wfd(ssl)) > 0)
  {
    if (osapiSocketNonBlockingModeSet(ssl_wfd,L7_FALSE) != L7_SUCCESS)
    {
      LOG_MSG("OPENSSL: failed to set write socket %d to blocking\n", ssl_wfd);
    }
  }

  return;
}

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
void opensslSecureNonBlockingSet(SSL *ssl)
{
  L7_int32 ssl_rfd, ssl_wfd;

  /* Set the SSL read connection to Non-Blocking */
  if ((ssl_rfd = SSL_get_rfd(ssl)) > 0)
  {
    if (osapiSocketNonBlockingModeSet(ssl_rfd,L7_TRUE) != L7_SUCCESS)
    {
      LOG_MSG("OPENSSL: failed to set read socket %d to non-blocking\n", ssl_rfd);
    }
  }

  /* Set the SSL write connection to Non-Blocking */
  if ((ssl_wfd = SSL_get_wfd(ssl)) > 0)
  {
    if (osapiSocketNonBlockingModeSet(ssl_wfd,L7_TRUE) != L7_SUCCESS)
    {
      LOG_MSG("OPENSSL: failed to set write socket %d to non-blocking\n", ssl_wfd);
    }
  }

  return;
}

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
void opensslPemPassPhrasePrompt(void)
{

  LOG_MSG("OPENSSL: The Server SSL PEM file should not be pass phrase protected\n");

  return;
}
/*********************************************************************
*
* @purpose  Read server key from PEM file
*
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
L7_RC_t
opensslServerKeyRead(L7_char8 *keyFile, EVP_PKEY **pkey)
{
  FILE        *fp = L7_NULLPTR;

  if (keyFile == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  fp = opensslFileOpen(keyFile);

  if (fp == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  /*
  PEM_read_PrivateKey reads private/public key, the public key is a
  subset of the information in the private key, so this reads both
  */
  *pkey = PEM_read_PrivateKey(fp, L7_NULLPTR, ((void *)opensslPemPassPhrasePrompt), L7_NULLPTR);

  fclose(fp);

  if ((*pkey) == L7_NULLPTR)
  {
    LOG_MSG("OPENSSL_SSL: Error loading private key from keyFile\n");
    return L7_FAILURE;
  }

  return L7_SUCCESS;

} /* opensslServerKeyRead */


/*********************************************************************
*
* @purpose  Write server key to PEM file
*
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
L7_RC_t
opensslServerKeyWrite(L7_char8 *keyFile, EVP_PKEY *pkey)
{
  L7_uint32   fd = 0;
  FILE        *fp = L7_NULLPTR;

  if (keyFile == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  fd = osapiFsOpen(keyFile);

  if (fd == 0)
  {
    if ((osapiFsFileCreate(keyFile, &fd) != L7_SUCCESS) || (fd == 0))
    {
      return L7_FAILURE;
    }
  }

  fp = fdopen((int)fd, "w");

  if (fp == L7_NULLPTR)
  {
    osapiFsClose(fd);
    return L7_FAILURE;
  }

  if (!PEM_write_PrivateKey(fp, pkey, L7_NULLPTR, L7_NULLPTR, 0, 0, L7_NULLPTR))
  {
    fclose(fp);
    return L7_FAILURE;
  }

  fclose(fp);

  return L7_SUCCESS;

} /* opensslServerKeyWrite */


/*********************************************************************
*
* @purpose  Write server certificate to PEM file
*
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
L7_RC_t
opensslServerCertWrite(L7_char8 *certFile, X509 *cert)
{
  L7_uint32   fd = 0;
  FILE        *fp = L7_NULLPTR;

  if (certFile == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  fd = osapiFsOpen(certFile);

  if (fd == 0)
  {
    return L7_FAILURE;
  }

  fp = fdopen((int)fd, "w");

  if (fp == L7_NULLPTR)
  {
    osapiFsClose(fd);
    return L7_FAILURE;
  }

  if (!PEM_write_X509(fp, cert))
  {
    fclose(fp);
    return L7_FAILURE;
  }

  fclose(fp);

  return L7_SUCCESS;

} /* opensslServerCertWrite */


/*********************************************************************
*
* @purpose  Read server certificate from PEM file
*
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
L7_RC_t
opensslServerCertRead(L7_char8 *certFile, L7_char8 *certPEM, X509 **certX509)
{
  L7_char8  buffer[L7_SSLT_PEM_FILE_SIZE_MAX];
  L7_char8  *p = L7_NULLPTR;
  FILE      *fp = L7_NULLPTR;

  if (certFile == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  /* Read entire file to return PEM format */
  if (osapiFsRead(certFile, buffer, L7_SSLT_PEM_FILE_SIZE_MAX) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Copy PEM format into buffer, only copy server certificate */

  memset(certPEM, 0, L7_SSLT_PEM_BUFFER_SIZE_MAX);
  p = strstr((const char *)buffer, OPENSSL_PEM_CERT_FOOTER);

  if (p == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  p += strlen(OPENSSL_PEM_CERT_FOOTER);
  memcpy(certPEM, buffer, p - buffer);

  /* Read PEM file into internal format */

  fp = opensslFileOpen(certFile);

  if (fp == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  (*certX509) = PEM_read_X509(fp, L7_NULLPTR, L7_NULLPTR, L7_NULLPTR);
  fclose(fp);

  if ((*certX509) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;

} /* opensslServerCertRead */

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
L7_RC_t
opensslDiffieHellmanParamsWrite(L7_char8 *file, DH *dh)
{
    L7_uint32   size = 0;
    L7_uint32   fd = 0;
    FILE        *fp = L7_NULLPTR;

    if (osapiFsFileSizeGet(file, &size) == L7_SUCCESS)
    {
        if (osapiFsDeleteFile(file) != L7_SUCCESS)
        {
            LOG_MSG("OPENSSL: File %s delete failed\n", file);

            return L7_FAILURE;
        }
    }

    if ((osapiFsFileCreate(file, &fd) != L7_SUCCESS) || (fd == 0))
    {
        LOG_MSG("OPENSSL: Could not create %s\n", file);
        return L7_FAILURE;
    }

    fp = fdopen((int)fd, "w");

    if (fp == L7_NULLPTR)
    {
        LOG_MSG("OPENSSL: Could not get file pointer for %s\n", file);
        osapiFsClose(fd);
        return L7_FAILURE;
    }

    if (!PEM_write_DHparams(fp, dh))
    {
        LOG_MSG("OPENSSL: PEM DH params could not be written\n");
        fclose(fp);
        return L7_FAILURE;
    }

    fclose(fp);

    return L7_SUCCESS;

} /* opensslDiffieHellmanParamsWrite */

static const char *mon[12]=
{
    "Jan","Feb","Mar","Apr","May","Jun",
    "Jul","Aug","Sep","Oct","Nov","Dec"
};

/*********************************************************************
*
* @purpose  Print generalized ASN1 time string to a buffer
*
* @param    char                 *bp @b{(output)} pointer to output buffer
* @param    ASN1_GENERALIZEDTIME *tm @b((input))  pointer to ASN1 time object
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  This was copied directly from ASN1_GENERALIZEDTIME_print,
*            which prints to an open BIO, we want to avoid using BIOs that
*            allocate memory on the fly and openssl does not provide another
*            method, the only change here is to print to a buffer,
*            everything else is left the same so we can refer back to
*            the original function if needed.
*
* @end
*
*********************************************************************/
static int
openssl_ASN1_GENERALIZEDTIME_print(char *bp, ASN1_GENERALIZEDTIME *tm)
{
    char    *v;
    int     gmt=0;
    int     i;
    int     y=0, M=0, d=0, h=0, m=0, s=0;

    i = tm->length;
    v = (char *)tm->data;

    if (i < 12) return(0);

    if (v[i-1] == 'Z') gmt=1;

    for (i=0; i<12; i++)
    {
        if ((v[i] > '9') || (v[i] < '0')) return(0);
    }

    y = (v[0]-'0')*1000+(v[1]-'0')*100 + (v[2]-'0')*10+(v[3]-'0');
    M = (v[4]-'0')*10+(v[5]-'0');

    if ((M > 12) || (M < 1)) return(0);

    d = (v[6]-'0')*10+(v[7]-'0');
    h = (v[8]-'0')*10+(v[9]-'0');
    m = (v[10]-'0')*10+(v[11]-'0');

    if ((v[12] >= '0') && (v[12] <= '9') && (v[13] >= '0') && (v[13] <= '9'))
    {
        s = (v[12]-'0')*10+(v[13]-'0');
    }

    if (osapiSnprintf(bp,24,"%s %2d %02d:%02d:%02d %d%s", mon[M-1],d,h,m,s,y,(gmt)?" GMT":"") <= 0)
    {
        return(0);
    }

    return(1);

} /* openssl_ASN1_GENERALIZEDTIME_print */

/*********************************************************************
*
* @purpose  Print UTC ASN1 time string to a buffer
*
* @param    char          *bp @b{(output)} pointer to output buffer
* @param    ASN1_UTCTIME  *tm @b((input))  pointer to ASN1 time object
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  This was copied directly from ASN1_UTCTIME_print,
*            which prints to an open BIO, we want to avoid using BIOs that
*            allocate memory on the fly and openssl does not provide another
*            method, the only change here is to print to a buffer,
*            everything else is left the same so we can refer back to
*            the original function if needed.
*
* @end
*
*********************************************************************/
static int
openssl_ASN1_UTCTIME_print(char *bp, ASN1_UTCTIME *tm)
{
    char    *v;
    int     gmt=0;
    int     i;
    int     y=0, M=0, d=0, h=0, m=0, s=0;

    i = tm->length;
    v = (char *)tm->data;

    if (i < 10) return(0);

    if (v[i-1] == 'Z') gmt=1;

    for (i=0; i<10; i++)
    {
        if ((v[i] > '9') || (v[i] < '0')) return(0);
    }

    y = (v[0]-'0')*10+(v[1]-'0');

    if (y < 50) y+=100;

    M = (v[2]-'0')*10+(v[3]-'0');

    if ((M > 12) || (M < 1)) return(0);

    d = (v[4]-'0')*10+(v[5]-'0');
    h = (v[6]-'0')*10+(v[7]-'0');
    m = (v[8]-'0')*10+(v[9]-'0');

    if ((v[10] >= '0') && (v[10] <= '9') && (v[11] >= '0') && (v[11] <= '9'))
    {
        s = (v[10]-'0')*10+(v[11]-'0');
    }

    if (osapiSnprintf(bp,24,"%s %2d %02d:%02d:%02d %d%s", mon[M-1],d,h,m,s,y+1900,(gmt)?" GMT":"") <= 0)
    {
        return(0);
    }

    return(1);

} /* openssl_ASN1_UTCTIME_print */

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
L7_RC_t
opensslASN1TimePrint(ASN1_TIME *tm, L7_char8 *bp)
{
    int rc = 0;

    if (tm->type == V_ASN1_UTCTIME)
    {
        rc = openssl_ASN1_UTCTIME_print(bp, tm);
    }

    if (tm->type == V_ASN1_GENERALIZEDTIME)
    {
        rc = openssl_ASN1_GENERALIZEDTIME_print(bp, tm);
    }

    if (rc == 0)
    {
        return L7_FAILURE;
    }

    return L7_SUCCESS;

} /* opensslASN1TimePrint */

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
* @comments buffer must be at least L7_SSLT_FINGERPRINT_MD5_SIZE_MAX
*
* @end
*
*********************************************************************/
L7_RC_t
opensslX509DigestPrint(X509 *cert, L7_char8 *bp)
{
    L7_uint32   i = 0;
    L7_uint32   n = 0;
    L7_uchar8   md[EVP_MAX_MD_SIZE];
    L7_uchar8   *p = L7_NULLPTR;

    if (!X509_digest(cert, EVP_md5(), md, &n))
    {
        return L7_FAILURE;
    }

    p = bp;

    for (i = 0; i < n; i++)
    {
        p += osapiSnprintf(p, 2, "%02X", md[i]);
    }

    *p = '\0';

    return L7_SUCCESS;

} /* opensslX509DigestPrint */

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
FILE
*opensslFileOpen(L7_char8 *filename)
{
  L7_uint32   fd = 0;
  FILE        *fp = L7_NULLPTR;
  L7_uint32   size = 0;

  if ((osapiFsFileSizeGet(filename, &size) != L7_SUCCESS) || (size == 0))
  {
    LOG_MSG("OPENSSL: File %s size = %d\n", filename, size);
    return L7_NULLPTR;
  }

  fd = osapiFsOpen(filename);

  if (fd == 0)
  {
    LOG_MSG("OPENSSL: Could not open %s\n", filename);
    return L7_NULLPTR;
  }

  fp = fdopen((int)fd, "r");

  if (fp == L7_NULLPTR)
  {
    LOG_MSG("OPENSSL: Could not get file pointer for %s\n", filename);
    osapiFsClose(fd);
    return L7_NULLPTR;
  }

  return fp;

} /* opensslFileOpen */

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
                                       DH **dh512P, DH **dh1024P)
{
    DH  *dhWeak = L7_NULLPTR;
    DH  *dhStrong = L7_NULLPTR;

    osapiSemaTake(opensslDhSema, L7_WAIT_FOREVER);

    opensslDiffieHellmanParamsFree(dh512P, dh1024P);

    /*
    Make sure we don't assign the global values read by the
    calback function until we are done without errors
    */

    if ((opensslDiffieHellmanParamsRead(dhWeakFile,
                                            &dhWeak) == L7_SUCCESS) &&
        (opensslDiffieHellmanParamsRead(dhStrongFile,
                                            &dhStrong) == L7_SUCCESS))

    {
        *dh512P = dhWeak;
        *dh1024P = dhStrong;
        osapiSemaGive(opensslDhSema);
        return L7_SUCCESS;
    }

    osapiSemaGive(opensslDhSema);

    /* Make sure these are freed on any failure */

    if (dhWeak != L7_NULLPTR)
    {
        DH_free(dhWeak);
    }

    if (dhStrong != L7_NULLPTR)
    {
        DH_free(dhStrong);
    }

    LOG_MSG("DH params not read\n");
    return L7_FAILURE;

} /* opensslDiffieHellmanParamsInit */

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
void opensslDiffieHellmanParamsFree(DH **dh512P, DH **dh1024P)
{
    /* Make sure these are freed on any failure */

    if (*dh512P != L7_NULLPTR)
    {
        DH_free(*dh512P);
        *dh512P = L7_NULLPTR;
    }

    if (*dh1024P != L7_NULLPTR)
    {
        DH_free(*dh1024P);
        *dh1024P = L7_NULLPTR;
    }

    return;

} /* opensslDiffieHellmanParamsFree */

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
void opensslDiffieHellmanParamsCreate(void **semaP, DH **dh512P, DH **dh1024P)
{
    DH  *dhWeak = L7_NULLPTR;
    DH  *dhStrong = L7_NULLPTR;

    /* Generate DH parameters one time only */

    /*
    Make sure we don't assign the global values read by the
    calback function until we are done without errors
    */

    if (dhGenerateInProgress == L7_TRUE)
    {
      /* DH generation in progress by some other task, wait for completion and return */
      osapiSemaTake(opensslDhSema, L7_WAIT_FOREVER);
      osapiSemaGive(opensslDhSema);
      return;
    }

    do
    {
        int codes = 0;

        osapiSemaTake(opensslDhSema, L7_WAIT_FOREVER);
        dhGenerateInProgress = L7_TRUE;

        dhWeak = DH_generate_parameters(512, DH_GENERATOR_5, L7_NULLPTR, L7_NULLPTR);

        if ((dhWeak == L7_NULLPTR) || (DH_check(dhWeak, &codes) == 0))
        {
            LOG_MSG("DH Weak generate failed\n");
            break;
        }

        if ((codes & (DH_CHECK_P_NOT_PRIME | DH_CHECK_P_NOT_SAFE_PRIME |
                      DH_NOT_SUITABLE_GENERATOR | DH_UNABLE_TO_CHECK_GENERATOR)) != 0)
        {
            LOG_MSG("DH Weak generate bogus\n");
            break;
        }

        LOG_MSG("OPENSSL: Diffie Hellman weak parameters created.\n");

        dhStrong = DH_generate_parameters(1024, DH_GENERATOR_5, L7_NULLPTR, L7_NULLPTR);

        if ((dhStrong == L7_NULLPTR) || (DH_check(dhStrong, &codes) == 0))
        {
            LOG_MSG("DH Strong generate failed\n");
            break;
        }

        if ((codes & (DH_CHECK_P_NOT_PRIME | DH_CHECK_P_NOT_SAFE_PRIME |
                      DH_NOT_SUITABLE_GENERATOR | DH_UNABLE_TO_CHECK_GENERATOR)) != 0)
        {
            LOG_MSG("DH Strong generate bogus\n");
            break;
        }

        LOG_MSG("OPENSSL: Diffie Hellman strong parameters created.\n");


        osapiSemaTake(*semaP, L7_WAIT_FOREVER);
        /* Save separate copies for wireless and sslt */
        if (opensslDiffieHellmanParamsWrite(L7_SSLT_DHWEAK_PEM,
                                                dhWeak) != L7_SUCCESS)
        {
            LOG_MSG("OPENSSL: Failed to save %s.\n", L7_SSLT_DHWEAK_PEM);
        }

        if (opensslDiffieHellmanParamsWrite(L7_SSLT_DHSTRONG_PEM,
                                                dhStrong) != L7_SUCCESS)
        {
            LOG_MSG("OPENSSL: Failed to save %s.\n", L7_SSLT_DHSTRONG_PEM);
        }
#ifdef L7_WIRELESS_PACKAGE
        /* store a copy for wireless to use */
        if (opensslDiffieHellmanParamsWrite(L7_WIRELESS_SSL_DHWEAK_PEM,
                                                dhWeak) != L7_SUCCESS)
        {
            LOG_MSG("OPENSSL: Failed to save %s.\n", L7_WIRELESS_SSL_DHWEAK_PEM);
        }

        if (opensslDiffieHellmanParamsWrite(L7_WIRELESS_SSL_DHSTRONG_PEM,
                                                dhStrong) != L7_SUCCESS)
        {
            LOG_MSG("OPENSSL: Failed to save %s.\n", L7_WIRELESS_SSL_DHWEAK_PEM);
        }
#endif /* L7_WIRELESS_PACKAGE */

        *dh512P = dhWeak;
        *dh1024P = dhStrong;

        osapiSemaGive(*semaP);

        dhGenerateInProgress = L7_FALSE;
        osapiSemaGive(opensslDhSema);

        return;

    } while (0);

    dhGenerateInProgress = L7_FALSE;
    osapiSemaGive(opensslDhSema);

    /* Make sure these are freed on any failure */

    if (dhWeak != L7_NULLPTR)
    {
        DH_free(dhWeak);
    }

    if (dhStrong != L7_NULLPTR)
    {
        DH_free(dhStrong);
    }

    LOG_MSG("OPENSSL: Failed to create Diffie Hellman parameters.\n");

    return;

} /* opensslDiffieHellmanParamsCreate */

