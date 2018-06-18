/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename usmdb_sslt.c
*
* @purpose SSL Tunnel USMDB API functions
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
#include "l7_common.h"
#include "usmdb_sslt_api.h"
#include "sslt_api.h"

/*********************************************************************
*
* @purpose Set the Admin Mode of the SSL Tunnel server.
*
* @param unitIndex @b((input)) the unit for this operation
* @param mode @b{(input)} value of new Admin mode setting,L7_ENABLE or
*                         L7_DISABLE
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbssltAdminModeSet(L7_uint32 unitIndex,
                              L7_uint32 mode)
{
  return ssltAdminModeSet(mode);
}

/*********************************************************************
*
* @purpose Get the Admin Mode of the SSL Tunnel server.
*
* @param unitIndex @b((input)) the unit for this operation
* @param mode @b{(input)} location to store the Admin mode setting
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbssltAdminModeGet(L7_uint32 unitIndex,
                              L7_uint32 *mode)
{
  return ssltAdminModeGet(mode);
}

/*********************************************************************
*
* @purpose Set the Auxhilary Mode of the SSL Tunnel server.
*
* @param mode @b{(input)} value of new Aaux mode setting
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbssltAuxModeSet(L7_uint32 mode)
{
  return ssltAuxModeSet(mode);
}

/*********************************************************************
*
* @purpose Get the Auxilary Mode of the SSL Tunnel server.
*
* @param mode @b{(input)} location to store the Aux mode setting
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbssltAuxModeGet(L7_uint32 *mode)
{
  return ssltAuxModeGet(mode);
}

/*********************************************************************
*
* @purpose Set the secure port number to listen on for SSLT connections.
*
* @param unitIndex @b((input)) the unit for this operation
* @param number @b{(input)} value of the new port setting
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbssltSecurePortSet(L7_uint32 unitIndex,
                               L7_uint32 number)
{
  return ssltSecurePortSet(number);
}

/*********************************************************************
*
* @purpose Get the secure port number that the SSLT connection is using.
*
* @param unitIndex @b((input)) the unit for this operation
* @param number @b{(input)} location to store the value of the port number
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbssltSecurePortGet(L7_uint32 unitIndex,
                               L7_uint32 *number)
{
  return ssltSecurePortGet(number);
}

/*********************************************************************
*
* @purpose Set the first auxiliary port number to listen on for SSLT connections.
*
* @param number @b{(input)} value of the new port setting
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbssltAuxSecurePort1Set(L7_uint32 unitIndex,
                                  L7_uint32 number)
{
  return ssltAuxSecurePort1Set(number);
}

/*********************************************************************
*
* @purpose Get the first auxiliary port number that the SSLT connection is using.
*
* @param number @b{(input)} location to store the value of the port number
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbssltAuxSecurePort1Get(L7_uint32 unitIndex,
                                  L7_uint32 *number)
{
  return ssltAuxSecurePort1Get(number);
}

/*********************************************************************
*
* @purpose Set the second auxiliary port number to listen on for SSLT connections.
*
* @param number @b{(input)} value of the new port setting
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbssltAuxSecurePort2Set(L7_uint32 unitIndex,
                                  L7_uint32 number)
{
  return ssltAuxSecurePort2Set(number);
}

/*********************************************************************
*
* @purpose Get the second auxiliary port number that the SSLT connection is using.
*
* @param number @b{(input)} location to store the value of the port number
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbssltAuxSecurePort2Get(L7_uint32 unitIndex,
                                  L7_uint32 *number)
{
  return ssltAuxSecurePort2Get(number);
}

/*********************************************************************
*
* @purpose Set the unsecure port number to open for SSLT connections.
*
* @param unitIndex @b((input)) the unit for this operation
* @param number @b{(input)} value of the new port setting
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbssltUnSecurePortSet(L7_uint32 unitIndex,
                                 L7_uint32 number)
{
  return ssltUnSecurePortSet(number);
}

/*********************************************************************
*
* @purpose Get the unsecure port number that the SSLT connection is using.
*
* @param unitIndex @b((input)) the unit for this operation
* @param number @b{(input)} location to store the value of the port number
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbssltUnSecurePortGet(L7_uint32 unitIndex,
                                 L7_uint32 *number)
{
  return ssltUnSecurePortGet(number);
}

/*********************************************************************
*
* @purpose Set the unsecure server IP address to open for SSLT connections.
*
* @param unitIndex @b((input)) the unit for this operation
* @param number @b{(input)} value of the new server IP address
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbssltUnSecureServerAddrSet(L7_uint32 unitIndex,
                                       L7_uint32 number)
{
  return ssltUnSecureServerAddrSet(number);
}

/*********************************************************************
*
* @purpose Get the unsecure server IP address that the SSLT connection
*          is using.
*
* @param unitIndex @b((input)) the unit for this operation
* @param number @b{(input)} location to store the value of the server
*                           IP address
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbssltUnSecureServerAddrGet(L7_uint32 unitIndex,
                                       L7_uint32 *number)
{
  return ssltUnSecureServerAddrGet(number);
}

/*********************************************************************
*
* @purpose Set the Pass Phrase string used to access the PEM-encoded
*          (Privacy Enhanced Mail) certificates and key files required
*          for the SSLT connections.
*
* @param unitIndex @b((input)) the unit for this operation
* @param string @b{(input)} pointer to the new PEM Pass Phrase string
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbssltPemPassPhraseSet(L7_uint32 unitIndex,
                                  L7_uchar8 *string)
{
  return ssltPemPassPhraseSet(string);
}

/*********************************************************************
*
* @purpose Find out if the PEM Pass Phrase has been set or not
*
* @param unitIndex @b((input)) the unit for this operation
* @param phraseIsSet @b{(input)} location to set with boolean result
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbssltPemPassPhraseCheck(L7_uint32 unitIndex,
                                    L7_BOOL *phraseIsSet)
{
  return ssltPemPassPhraseCheck(phraseIsSet);
}

/*********************************************************************
*
* @purpose Set the SSL Protocol Level to be used by SSL Tunnel code
*
* @param unitIndex @b((input)) the unit for this operation
* @param protocolId @b{(input)} ID of SSLT supported protocol
* @param mode @b{(input)} value of new mode setting
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbssltProtocolLevelSet(L7_uint32 uintIndex,
                                  L7_uint32 protocolId,
                                  L7_uint32 mode)
{
  return ssltProtocolLevelSet(protocolId, mode);
}

/*********************************************************************
*
* @purpose Get the SSL Protocol Level in use by the SSL Tunnel code
*
* @param unitIndex @b((input)) the unit for this operation
* @param protocolId @b{(input)} ID of SSLT supported protocol
* @param mode @b{(input)} location to store the mode setting
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbssltProtocolLevelGet(L7_uint32 uintIndex,
                                  L7_uint32 protocolId,
                                  L7_uint32 *mode)
{
  return ssltProtocolLevelGet(protocolId, mode);
}


/*********************************************************************
*
* @purpose Set the Hard Timeout (in hours) for SSLT sessions.
*
* @param number @b{(input)} value of the new timeout setting
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbssltSessionHardTimeOutSet(L7_uint32 val)
{
  return ssltSessionHardTimeOutSet(val);
}

/*********************************************************************
*
* @purpose Get the Hard Timeout (in hours) for SSLT sessions.
*
* @param val @b{(input)} location to store the value of the timeout
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbssltSessionHardTimeOutGet(L7_uint32 *val)
{
  return ssltSessionHardTimeOutGet(val);
}

/*********************************************************************
*
* @purpose Set the Soft Timeout (in minutes) for SSLT sessions.
*
* @param number @b{(input)} value of the new timeout setting
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbssltSessionSoftTimeOutSet(L7_uint32 val)
{
  return ssltSessionSoftTimeOutSet(val);
}

/*********************************************************************
*
* @purpose Get the Soft Timeout (in minutes) for SSLT sessions.
*
* @param val @b{(input)} location to store the value of the timeout
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbssltSessionSoftTimeOutGet(L7_uint32 *val)
{
  return ssltSessionSoftTimeOutGet(val);
}

/*********************************************************************
*
* @purpose Set the number of allowable SSLT sessions.
*
* @param number @b{(input)} value of the number of sslt sessions
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbssltNumSessionsSet(L7_uint32 val)
{
  return ssltNumSessionsSet(val);
}

/*********************************************************************
*
* @purpose Get the number of allowable SSLT sessions.
*
* @param val @b{(input)} location to store the value of the number
*                        of sessions
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbssltNumSessionsGet(L7_uint32 *val)
{
  return ssltNumSessionsGet(val);
}

/*********************************************************************
*
* @purpose Determine if self-signed server certificate exists
*
* @param L7_uint32  number     @b{(input)} certificate number (1-2)
*
* @returns L7_SUCCESS, certificate and RSA key pair exists
* @returns L7_FAILURE  if generation in progress
* @returns L7_NOT_EXIST  if cert not present
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbssltCertificateExists(L7_uint32 number)
{
  return ssltCertificateExists(number);
}

/*********************************************************************
*
* @purpose Determine if Diffie Hellman parameters have been generated
*
* @param void
*
* @returns L7_SUCCESS, params exist and are loaded
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbssltDHParametersExist(void)
{
  return ssltDHParametersExist();
}

/*********************************************************************
*
* @purpose Generate a self-signed server certificate for SSL tunnel
*
* @param L7_uint32  number     @b{(input)} certificate number
* @param L7_uint32  keyLength  @b{(input)} RSA key length, 0 will use existing key.
* @param L7_char8  *commonName @b((input)) subject DN common name field
* @param L7_char8  *orgName    @b((input)) subject DN organization name field
* @param L7_char8  *orgUnit    @b((input)) subject DN organization unit field
* @param L7_char8  *location   @b((input)) subject DN location field
* @param L7_char8  *state      @b((input)) subject DN state field
* @param L7_char8  *country    @b((input)) subject DN country field
* @param L7_char8  *email      @b((input)) subject DN email field
* @param L7_uint32  days       @b((input)) days certificate will be valid
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments  If certificate does not exist, a keyLength must be specified
*             to generate an RSA key pair on the device.
*            If keyLength == 0, certificate will use existing RSA key pair.
*            If specified, keyLength must be >= L7_SSLT_KEY_LENGTH_MIN and
*                                           <= L7_SSLT_KEY_LENGTH_MAX.
*            If DN fields are not specified, defaults will be applied.
*            DN field lengths must be >= L7_SSLT_SUBJECT_DN_FIELD_MIN and
*                                     <= L7_SSLT_SUBJECT_DN_FIELD_MAX.
*            country length must be L7_SSLT_SUBJECT_COUNTRY_SIZE.
*            If days == 0, default valid days will be applied.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbssltCertificateGenerate(L7_uint32 number,
                                     L7_uint32 keyLength,
                                     L7_char8 *commonName,
                                     L7_char8 *orgName,
                                     L7_char8 *orgUnit,
                                     L7_char8 *location,
                                     L7_char8 *state,
                                     L7_char8 *country,
                                     L7_char8 *email,
                                     L7_uint32 days)
{
  return ssltCertificateGenerate(number, keyLength, commonName, orgName, orgUnit,
                                 location, state, country, email, days);
}

/*********************************************************************
*
* @purpose Set the active certificate for the SSL tunnel
*
* @param L7_uint32  number     @b{(input)} certificate number (1-2)
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE, certificate does not exist
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbssltCertificateActiveSet(L7_uint32 number)
{
        return ssltCertificateActiveSet(number);
}

/*********************************************************************
*
* @purpose Import a signed certificate for SSL tunnel
*
* @param L7_uint32  number      @b{(input)} certificate number
* @param L7_char8  *cert        @b((input)) signed cert X509 PEM format
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE, failed to import certificate
*
* @comments  This will replace self-signed certificate (number),
*             use usmDbssltCertificateGet to read certificate info.
*            If RSA key does not match key for certificate (number),
*             import will fail.
*            cert buffer cannot be larger than L7_SSLT_PEM_FILE_SIZE_MAX
*
* @end
*
*********************************************************************/
L7_RC_t usmDbssltCertificateImport(L7_uint32  number,
                                   L7_char8  *cert)
{
        return ssltCertificateImport(number, cert);
}

/*********************************************************************
*
* @purpose Generate a certificate request for the device.
*
* @param L7_uint32  number     @b{(input)}  certificate number
* @param L7_char8  *commonName @b((input))  DN common name field
* @param L7_char8  *orgUnit    @b((input))  DN organization unit field
* @param L7_char8  *location   @b((input))  DN location field
* @param L7_char8  *state      @b((input))  DN state field
* @param L7_char8  *country    @b((input))  DN country field
* @param L7_char8  *request    @b((output)) cert request X509 PEM format
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE, self-signed certificate does not exist.
*
* @comments  The certificate request will use the RSA key pair
*             generated for self-signed certificate (number).
*            DN field lengths must be >= L7_SSLT_SUBJECT_DN_FIELD_MIN and
*                                     <= L7_SSLT_SUBJECT_DN_FIELD_MAX
*            country length must be L7_SSLT_SUBJECT_COUNTRY_SIZE
*            request buffer MUST be L7_SSLT_PEM_BUFFER_SIZE_MAX
*
* @end
*
*********************************************************************/
L7_RC_t usmDbssltCertificateRequestGenerate(L7_uint32 number,
                                            L7_char8 *commonName,
                                            L7_char8 *orgName,
                                            L7_char8 *orgUnit,
                                            L7_char8 *location,
                                            L7_char8 *state,
                                            L7_char8 *country,
                                            L7_char8 *email,
                                            L7_char8 *request)
{
        return ssltCertificateRequestGenerate(number, commonName, orgName, orgUnit,
                                        location, state, country, email, request);
}

/*********************************************************************
*
* @purpose Get the active certificate for the SSL tunnel
*
* @param L7_uint32  *number     @b{(output)} certificate number (1-2)
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE, no certificates exist
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbssltCertificateActiveGet(L7_uint32 *number)
{
        return ssltCertificateActiveGet(number);
}

/*********************************************************************
*
* @purpose Get SSL tunnel server certificate information.
*
* @param L7_uint32  number      @b{(input)}  certificate number
* @param L7_BOOL   *active      @b((output)) this is the active certificate
* @param L7_char8  *issuerCN    @b((output)) certificate signing authority (Issued By)
* @param L7_char8  *validFrom   @b((output)) formatted time string
* @param L7_char8  *validTo     @b((output)) formatted time string
* @param L7_char8  *subjectCN   @b((output)) subject DN common name (Issued To)
* @param L7_char8  *subject     @b((output)) subject DN one-line format
* @param L7_char8  *fingerprint @b((output)) fingerprint hex format
* @param L7_char8  *cert        @b((output)) cert in X509 PEM format
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE, certificate number does not exist
*
* @comments  cert buffer must be L7_SSLT_PEM_BUFFER_SIZE_MAX
*            DN buffers must be L7_SSLT_SUBJECT_DN_FIELD_MAX
*            subject line buffer must be L7_SSLT_SUBJECT_LINE_MAX
*            fingerprint buffer must be L7_SSLT_FINGERPRINT_MD5_SIZE_MAX
*            date buffer must be L7_SSLT_CERT_DATE_SIZE_MAX
*
* @end
*
*********************************************************************/
L7_RC_t usmDbssltCertificateGet(L7_uint32  number,
                                L7_BOOL   *active,
                                L7_char8  *issuerCN,
                                L7_char8  *validFrom,
                                L7_char8  *validTo,
                                L7_char8  *subjectCN,
                                L7_char8  *subject,
                                L7_char8  *fingerprint,
                                L7_char8  *cert)
{
        return ssltCertificateGet(number, active, issuerCN, validFrom, validTo,
                            subjectCN, subject, fingerprint, cert);
}

