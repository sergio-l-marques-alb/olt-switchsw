/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename radius.c
*
* @purpose RADIUS Client base protocol
*
* @component radius
*
* @comments none
*
* @create 03/28/2003
*
* @author spetriccione
*
* @end
*
**********************************************************************/

#include "radius_include.h"
#include "l7_ip_api.h"
#include "rto_api.h"
#include "simapi.h"

extern L7_uint32 radius_sm_bp_id;
extern L7_uint32 radius_lg_bp_id;

static void radiusCopyDecrMSKey2attr(radiusPacket_t *packet,
                                     L7_uchar8 *sendKey,
                                     L7_uchar8 sendKeyLen,
                                     L7_uchar8 *recvKey,
                                     L7_uchar8 recvKeyLen);

/*********************************************************************
*
* @purpose Calculate the Message digest on the packet with the autheticator
*          as the corresponding request authenticator and secret.
*
* @param serverEntry @b((input)) current RADIUS server
* @param packet @b((input)) response received from the server
* @param request @b((input)) original Access-Request
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusResponseAuthenticatorValidate(radiusServerEntry_t *serverEntry,
                                            radiusPacket_t *packet,
                                            L7_char8 *requestAuthenticator)
{
  L7_uchar8 responseAuthenticator[RADIUS_AUTHENTICATOR_LENGTH];
  L7_uchar8 digest[RADIUS_PASSWORD_BLOCK_LENGTH];
  L7_MD5_CTX_t context;
  L7_uint32 pktLength = osapiNtohs(packet->header.msgLength);
  L7_uint32 secretLen = (L7_uint32)strlen(serverEntry->serverConfig.radiusServerConfigSecret);

  /*
  ** Save the received Response-Authenticator and overwrite it with the
  ** original Request-Authenticator
  */
  bcopy(packet->header.authenticator, responseAuthenticator, sizeof(responseAuthenticator));
  bcopy(requestAuthenticator, packet->header.authenticator, sizeof(packet->header.authenticator));

  /*
  ** Append the secret in the extra space allocated at rx time.
  */
  bcopy(serverEntry->serverConfig.radiusServerConfigSecret,
        (L7_char8 *)packet + pktLength, secretLen);

  /*
  ** Calculate the MD5 digest on the received packet as per RFC
  */
  l7_md5_init(&context);
  l7_md5_update(&context, (L7_uchar8 *)packet, pktLength + secretLen);
  l7_md5_final(digest, &context);

  if (memcmp(digest, responseAuthenticator, RADIUS_PASSWORD_BLOCK_LENGTH) == 0)
  {
     return L7_SUCCESS;
  }

  /* Did not calculate same Response-Authenticator */
  if (serverEntry->serverConfig.radiusServerConfigServerType == RADIUS_SERVER_TYPE_AUTH)
  {
    serverEntry->radiusServerStats_t.authStats.radiusAuthClientBadAuthenticators++;
  }
  else
  {
    serverEntry->radiusServerStats_t.acctStats.radiusAcctClientBadAuthenticators++;
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Calculate the Message-Authenticator digest on the received
*          packet with using the original request authenticator and
*          the secret as the hmac key.
*
* @param serverEntry @b((input)) current RADIUS server
* @param packet @b((input)) response received from the server
* @param request @b((input)) original Access-Request
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusMessageAuthenticatorValidate(radiusServerEntry_t *serverEntry,
                                           radiusPacket_t *packet,
                                           radiusRequest_t *request)
{
  L7_uint32 pktLength = osapiNtohs(packet->header.msgLength);
  L7_uchar8 digest[RADIUS_AUTHENTICATOR_LENGTH];

  /*
  ** Overwrite the Response-Authenticator with the original Request-Authenticator
  */
  bcopy(request->authenticator, packet->header.authenticator, sizeof(packet->header.authenticator));

  /*
  ** Verify the Message-Authenticator (a one way HMAC MD5 hash of the packet
  ** with the Request-Authenticator and the Message-Authenticator field being
  ** octets of zero)
  */
  L7_hmac_md5((L7_uchar8 *)&packet->header,
              pktLength,
              (L7_uchar8 *)serverEntry->serverConfig.radiusServerConfigSecret,
              (L7_uint32)strlen(serverEntry->serverConfig.radiusServerConfigSecret),
              digest);


  if (memcmp(digest, request->messageAuthenticator, RADIUS_AUTHENTICATOR_LENGTH) == 0)
  {
    return L7_SUCCESS;
  }

  /* Did not calculate same Message-Authenticator */
  serverEntry->radiusServerStats_t.authStats.radiusAuthClientBadAuthenticators++;

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Set the unique authenticator for the Radius Request
*
* @param authenticator @b((input)) location to put authenticator
* @param authenticatorLength @b((input)) size of authenticator
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
void radiusAuthenticatorSet(L7_char8 *authenticator,
                            L7_uint32 authenticatorLength)
{
  L7_uint32 randomNumber;
  L7_uint32 i = 0;
  L7_double64 randVal;

  while (i < authenticatorLength)
  {
    randVal = L7_Random();
    randomNumber = (L7_uint32)(randVal * RADIUS_RANDOM_NUMBER_SEED);
    memcpy(authenticator, &randomNumber, sizeof(L7_uint32));
    authenticator += sizeof(L7_uint32);
    i += sizeof(L7_uint32);
  }

  return;
}

/*********************************************************************
*
* @purpose Calculate the digest on the password using a MD5 one way hash.
*
* @param digest @b((output)) location to put digest result
* @param secret @b((input)) pointer to secret
* @param secretLength @b((input)) size of secret
* @param authVector @b((input)) stream of data
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments The digest is calculated using a concatenation of the secret
*           and authVector.
*
* @end
*
*********************************************************************/
L7_RC_t radiusDigestCalculate(L7_char8 *digest,
                              L7_char8 *secret,
                              L7_uint32 secretLength,
                              L7_char8 *authVector)
{
   L7_uchar8 buffer[RADIUS_MAX_PASSWORD_LENGTH];
   L7_MD5_CTX_t context;

   bzero(buffer, sizeof(buffer));

   if (secret != L7_NULL)
   {
     bcopy(secret, buffer, secretLength);
   }

   bcopy(authVector, buffer + secretLength, RADIUS_AUTHENTICATOR_LENGTH);

   l7_md5_init(&context);
   l7_md5_update(&context, buffer, secretLength + RADIUS_AUTHENTICATOR_LENGTH);
   l7_md5_final((L7_uchar8 *)digest, &context);

   return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Encrypts the UserPassword using the Request-Authenticator
*          and MD5 hash functions as specified in the RADIUS RFC.
*
* @param serverEntry @b((input)) current RADIUS server
* @param request @b((input)) original Access-Request
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusUserPasswordEncrypt(radiusServerEntry_t *serverEntry,
                                  radiusRequest_t *request)
{
  L7_uint32 i, j;
  L7_char8 digest[RADIUS_PASSWORD_BLOCK_LENGTH];
  L7_char8 *authVector, *encPwd;
  L7_uint32 secretLen = (L7_uint32)strlen(serverEntry->serverConfig.radiusServerConfigSecret);

  request->encryptedPasswordLength = L7_NULL;
  bzero(request->encryptedPassword, RADIUS_MAX_PASSWORD_LENGTH);

  if (request->requestInfo->userPasswordLength > RADIUS_MAX_PASSWORD_LENGTH)
  {
    request->requestInfo->userPasswordLength = RADIUS_MAX_PASSWORD_LENGTH;
  }

  bcopy(request->requestInfo->userPassword, request->encryptedPassword,
        request->requestInfo->userPasswordLength);
  encPwd = request->encryptedPassword;

  if (serverEntry->serverConfig.radiusServerIsSecretConfigured == L7_TRUE)
  {
    (void)radiusDigestCalculate(digest,
                                serverEntry->serverConfig.radiusServerConfigSecret,
                                secretLen,
                                request->authenticator);
  }
  else
  {
    /* No secret has been configured */
    (void)radiusDigestCalculate(digest,
                                L7_NULL,
                                L7_NULL,
                                request->authenticator);
  }

  for(i = L7_NULL; i < request->requestInfo->userPasswordLength; i += RADIUS_PASSWORD_BLOCK_LENGTH)
  {
    /*
    ** XOR the digest with the password
    */
    authVector = encPwd;

    for(j = L7_NULL; j < RADIUS_PASSWORD_BLOCK_LENGTH; j++)
    {
      *encPwd ^= digest[j];
      encPwd++;
    }

    /*
    ** Update the encrypted password length and calculate the next digest
    */
    request->encryptedPasswordLength += RADIUS_PASSWORD_BLOCK_LENGTH;

    if ((i + RADIUS_PASSWORD_BLOCK_LENGTH) < request->requestInfo->userPasswordLength)
    {
      if (serverEntry->serverConfig.radiusServerIsSecretConfigured == L7_TRUE)
      {
        (void)radiusDigestCalculate(digest,
                                    serverEntry->serverConfig.radiusServerConfigSecret,
                                    secretLen,
                                    authVector);
      }
      else
      {
        /* No secret has been configured */
        (void)radiusDigestCalculate(digest,
                                    L7_NULL,
                                    L7_NULL,
                                    request->authenticator);
      }
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Convert the RADIUS attributes based upon the secret to the
*          new secret for the next server.
*
* @param nextServerEntry @b((input)) pointer to RADIUS server
* @param requestList @b((input)) pointer to request linked list
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
void radiusSecretConvert(radiusServerEntry_t *nextServerEntry,
                         radiusRequest_t *requestList)
{
  L7_RC_t rc;
  radiusRequest_t *request;
  radiusPacket_t *oldPacket, *newPacket;
  L7_uint32 pktLength;
  radiusAttr_t *oldAttr, *newAttr;
  L7_uchar8 *pMsgAuth;
  L7_uint32 oldEncryptedPasswordLength;

  /*
  ** Loop through the list of requests
  */
  for (request = requestList; request != L7_NULL; request = request->nextRequest)
  {
    oldEncryptedPasswordLength = request->encryptedPasswordLength;

    /* Calculate the nes encrypted password based upon the new secret */
    rc = radiusUserPasswordEncrypt(nextServerEntry, request);

    /*
    ** Update the packet length based upon the old packet with the
    ** possible change in the encrypted password length.
    */
    pktLength = ((osapiNtohs(request->requestMessage->header.msgLength) -
                 oldEncryptedPasswordLength) +
                 request->encryptedPasswordLength);

    /*
    ** Copy the old attributes into the new packet
    */
    if ((newPacket = (radiusPacket_t *)osapiMalloc(L7_RADIUS_COMPONENT_ID, pktLength)) != L7_NULL)
    {
      oldPacket = request->requestMessage;

      pMsgAuth = L7_NULL;

      /* Fill in the header */
      newPacket->header.code = oldPacket->header.code;
      newPacket->header.id = oldPacket->header.id;
      newPacket->header.msgLength = osapiHtons((L7_ushort16)pktLength);

      bcopy(request->authenticator, newPacket->header.authenticator, sizeof(request->authenticator));

      /* Point to the attributes */
      newAttr = (radiusAttr_t *)newPacket->attributes;
      oldAttr = (radiusAttr_t *)oldPacket->attributes;

      /*
      ** Loop through the attributes in this request
      */
      while (oldAttr < (radiusAttr_t *)((L7_uchar8 *)request->requestMessage +
             osapiNtohs(request->requestMessage->header.msgLength)))
      {
        if (oldAttr->type == RADIUS_ATTR_TYPE_USER_PASSWORD)
        {
          /* Store the User-Password based upon the new secret */
          newAttr->type = oldAttr->type;
          newAttr->length = sizeof(radiusAttr_t) - 1 + request->encryptedPasswordLength;
          bcopy(request->encryptedPassword, newAttr->value, request->encryptedPasswordLength);
        }
        else if (oldAttr->type == RADIUS_ATTR_TYPE_MESSAGE_AUTHENTICATOR)
        {
          newAttr->type = oldAttr->type;
          newAttr->length = sizeof(radiusAttr_t) - 1 + sizeof(request->messageAuthenticator);

          /* Zero out the Message-Authenticator before calculation, as per spec. */
          bzero(request->messageAuthenticator, sizeof(request->messageAuthenticator));
          bzero(newAttr->value, sizeof(request->messageAuthenticator));

          /* Save a pointer to the octets of zero */
          pMsgAuth = (L7_uchar8 *)newAttr + sizeof(radiusAttr_t) - 1;
        }
        else
        {
          /* Copy any other attribute */
          bcopy((L7_uchar8 *)oldAttr, (L7_uchar8 *)newAttr, oldAttr->length);
        }

        /* Advance pointer to next attribute */
        newAttr = (radiusAttr_t *)((L7_uchar8 *)newAttr + newAttr->length);
        oldAttr = (radiusAttr_t *)((L7_uchar8 *)oldAttr + oldAttr->length);

      } /* end while loop of attributes */

      if (pMsgAuth != L7_NULL)
      {
        /*
        ** Generate the unique Message-Authenticator based upon the new secret
        **
        ** Note: This calculation must be last.
        */
        L7_hmac_md5((L7_uchar8 *)newPacket,
                    pktLength,
                    (L7_uchar8 *)nextServerEntry->serverConfig.radiusServerConfigSecret,
                    (L7_uint32)strlen(nextServerEntry->serverConfig.radiusServerConfigSecret),
                    (L7_uchar8 *)request->messageAuthenticator);

        bcopy(request->messageAuthenticator, pMsgAuth, sizeof(request->messageAuthenticator));
      }

      /*
      ** Free the old and Link the new packet to the request.
      */
      osapiFree(L7_RADIUS_COMPONENT_ID, oldPacket);
      request->requestMessage = newPacket;

    } /* end if packet not null */
  } /* end for loop of requests */

  return;
}

/*********************************************************************
*
* @purpose Create the Access-Request packet with the mandatory objects
*          for a user authentication.
*
* @param serverEntry @b((input)) pointer to RADIUS server
* @param request @b((input)) pointer to request data
*
* @returns A pointer to a radiusPacket or L7_NULL if a packet is not
*          created.
*
* @comments Also link the created packet to the request which can be
*           used for retransmission, in case of timeouts.
*
* @end
*
*********************************************************************/
radiusPacket_t *radiusUserAuthenticateCreate(radiusServerEntry_t *serverEntry,
                                             radiusRequest_t *request)
{
  L7_RC_t rc;
  L7_uint32 pktLength;
  L7_uchar8 *nextAttr;
  radiusPacket_t *packet;
  radiusAttr_t *attr;
  L7_uchar8 *pMsgAuth = L7_NULLPTR;
  L7_uchar8 nasId[L7_RADIUS_NAS_IDENTIFIER_SIZE + 1];
  L7_uint32 nasIdSize;

  /*
  ** Generate the unique request authenticator and encrypt the password
  */
  radiusAuthenticatorSet(request->authenticator, RADIUS_AUTHENTICATOR_LENGTH);
  rc = radiusUserPasswordEncrypt(serverEntry, request);

  /* Get the NAS-Identifier */
  radiusNASIdentifierGet((L7_uchar8 *)&nasId, &nasIdSize);

  /*
  ** Calculate the packet length : RadiusHeader + Attributes
  */
  pktLength = sizeof(radiusHeader_t) +
    (sizeof(radiusAttr_t) - 1) + request->requestInfo->userNameLength +
    (sizeof(radiusAttr_t) - 1) + request->encryptedPasswordLength +
    ((request->requestInfo->stateLength > L7_NULL) ?
      ((sizeof(radiusAttr_t) - 1) + request->requestInfo->stateLength) : L7_NULL) +
    (sizeof(radiusAttr_t) - 1) + nasIdSize +
    ((serverEntry->serverConfig.incMsgAuthMode == L7_ENABLE) ?
      ((sizeof(radiusAttr_t) - 1) + sizeof(request->messageAuthenticator)) : L7_NULL);

  /*
  ** Fill in the packet
  */
  if ((packet = (radiusPacket_t *)osapiMalloc(L7_RADIUS_COMPONENT_ID, pktLength)) != L7_NULL)
  {
    /*
    ** Fill in the header
    */
    packet->header.code = RADIUS_CODE_ACCESS_REQUEST;
    packet->header.id = request->requestId;

    packet->header.msgLength = osapiHtons((L7_ushort16)pktLength);

    bcopy(request->authenticator, packet->header.authenticator, sizeof(request->authenticator));
    nextAttr = packet->attributes;

    /*
    ** Fill in the UserName attribute (mandatory)
    */
    attr = (radiusAttr_t *)nextAttr;
    attr->type = RADIUS_ATTR_TYPE_USER_NAME;
    attr->length = sizeof(radiusAttr_t) - 1 + request->requestInfo->userNameLength;
    bcopy(request->requestInfo->userName, attr->value, request->requestInfo->userNameLength);
    nextAttr += attr->length;

    /*
    ** Fill in the User Password attribute (mandatory)
    */
    attr = (radiusAttr_t *)nextAttr;
    attr->type = RADIUS_ATTR_TYPE_USER_PASSWORD;
    attr->length = sizeof(radiusAttr_t) - 1 + request->encryptedPasswordLength;
    bcopy(request->encryptedPassword, attr->value, request->encryptedPasswordLength);
    nextAttr += attr->length;

    /*
    ** Fill in the State Type attribute (mandatory if available)
    */
    if (request->requestInfo->stateLength != L7_NULL)
    {
      attr = (radiusAttr_t *)nextAttr;
      attr->type = RADIUS_ATTR_TYPE_STATE;
      attr->length = sizeof(radiusAttr_t) - 1 + request->requestInfo->stateLength;
      bcopy(request->requestInfo->state, attr->value, request->requestInfo->stateLength);
      nextAttr += attr->length;
    }

    /*
    ** Fill in the NAS Identifier (optionally mandatory)
    */
    attr = (radiusAttr_t *)nextAttr;
    attr->type = RADIUS_ATTR_TYPE_NAS_IDENTIFIER;
    attr->length = sizeof(radiusAttr_t) - 1 + nasIdSize;
    bcopy((L7_char8 *)&nasId, attr->value, nasIdSize);
    nextAttr += attr->length;

    /*
    ** Zero in the Message-Authenticator (optionally mandatory)
    */
    if (serverEntry->serverConfig.incMsgAuthMode == L7_ENABLE)
    {
      bzero(request->messageAuthenticator, sizeof(request->messageAuthenticator));

      attr = (radiusAttr_t *)nextAttr;
      attr->type = RADIUS_ATTR_TYPE_MESSAGE_AUTHENTICATOR;
      attr->length = sizeof(radiusAttr_t) - 1 + sizeof(request->messageAuthenticator);

      /* Save a pointer to the octets of zero */
      pMsgAuth = nextAttr + sizeof(radiusAttr_t) - 1;

      nextAttr += attr->length;
    }


    /*
    ** Generate the unique Message-Authenticator (a one way MD5 hash of the packet
    ** with Message-Autheticator field being octets of zero) and fill the packet with it.
    **
    ** Note: This calculation must be last.
    */
    if (serverEntry->serverConfig.incMsgAuthMode == L7_ENABLE)
    {
      L7_hmac_md5((L7_uchar8 *)&packet->header,
                  pktLength,
                  (L7_uchar8 *)serverEntry->serverConfig.radiusServerConfigSecret,
                  (L7_uint32)strlen(serverEntry->serverConfig.radiusServerConfigSecret),
                  (L7_uchar8 *)request->messageAuthenticator);

      bcopy(request->messageAuthenticator, pMsgAuth, sizeof(request->messageAuthenticator));
    }

    /*
    ** Link the packet to the request.
    */
    request->requestMessage = packet;

    return packet;
 }

 return L7_NULL;
}

/*********************************************************************
*
* @purpose Create the RADIUS request packet with any supplied objects.
*
* @param serverEntry @b((input)) current RADIUS server
* @param request @b((input)) Access-Request
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments Also link the created packet to the request which can be
*           used for retransmission, in case of timeouts.
*
* @end
*
*********************************************************************/
radiusPacket_t *radiusRequestPacketCreate(radiusServerEntry_t *serverEntry,
                                          radiusRequest_t *request)
{
  L7_uint32 pktLength;
  L7_uchar8 *nextAttr;
  radiusPacket_t *packet;
  radiusAttr_t *attr;
  radiusValuePair_t *vp;
  L7_uint32 tmpValue;
  L7_BOOL eapMessagePresent = L7_FALSE;
  L7_BOOL addEncryptedPW = L7_FALSE;
  L7_uchar8 *pMsgAuth = L7_NULL;

  /*
  ** Generate the unique request authenticator
  */
  radiusAuthenticatorSet(request->authenticator, RADIUS_AUTHENTICATOR_LENGTH);

  /*
  ** Find any attributes that need updating based upon the secret
  */
  for (vp = request->requestInfo->vpList; vp; vp = vp->nextPair)
  {
    /* Check for a User-Password included in the vp list. If it is
    ** present it is assumed to be a clear text password and needs to
    ** encrypted using the secret.
    */
    if (vp->attrId == RADIUS_ATTR_TYPE_USER_PASSWORD)
    {
      bcopy(vp->strValue, request->requestInfo->userPassword, vp->intValue);
      request->requestInfo->userPasswordLength = vp->intValue;

      if (radiusUserPasswordEncrypt(serverEntry, request) == L7_SUCCESS)
      {
        addEncryptedPW = L7_TRUE;

        /* Delete the unencrypted password */
        radiusAttrValuePairDel(&request->requestInfo->vpList,
                               vp->attrId,
                               L7_NULL);
      }
    }

    /* Check for a EAP-Message included in the vp list. A message
    ** authenticator based upon the secret is required to be contained
    ** within any request that contains an EAP message for proper
    ** authentication at the server. It will have to be added.
    */
    if (vp->attrId == RADIUS_ATTR_TYPE_EAP_MESSAGE)
    {
      eapMessagePresent = L7_TRUE;
    }

    /* Check for a Message-Authenticators included in the vp list.
    ** Delete any existing as they can not be correct as they will
    ** not be based upon the secret.
    **/
    if (vp->attrId == RADIUS_ATTR_TYPE_MESSAGE_AUTHENTICATOR)
    {
      radiusAttrValuePairDel(&request->requestInfo->vpList,
                             vp->attrId,
                             L7_NULL);
    }

    /* Check the NAS-IP address */
    if(vp->attrId == RADIUS_ATTR_TYPE_NAS_IP_ADDRESS)
    {
      if(vp->intValue == L7_NULL)
      {
#ifdef L7_ROUTING_PACKAGE
        L7_uint32 rtrAddr = 0;
        L7_uint32 nextHop;
        L7_uint32 intIfNum;
        L7_routeEntry_t route;
        /* Get the local IP address in the same subnet as the next hop
         * toward the RADIUS server. */
        /* First, get the best route to the server. */
        if (rtoBestRouteLookup(serverEntry->serverConfig.radiusServerConfigIpAddress,
                               &route, L7_FALSE) == L7_SUCCESS)
        {
          intIfNum = route.ecmpRoutes.equalCostPath[0].arpEntry.intIfNum;
          nextHop = route.ecmpRoutes.equalCostPath[0].arpEntry.ipAddr;

          /* Now find the local address on this interface in the same subnet
           * as the next hop address (Next hop address could be a secondary.) */
          ipMapLocalAddrToNeighbor(nextHop, intIfNum, &rtrAddr);
        }
        if (rtrAddr)
        {
            vp->intValue = rtrAddr;
        }
        else
#endif
        {
          /* get the system ip address */
          vp->intValue = simGetSystemIPAddr();
        }
      }
    }

  }

  /* Add the encrypted User-Password if required */
  if (addEncryptedPW == L7_TRUE)
  {
    (void)radiusAttrValuePairAdd(&request->requestInfo->vpList,
                                RADIUS_ATTR_TYPE_USER_PASSWORD,
                                L7_NULL,
                                request->encryptedPassword,
                                request->encryptedPasswordLength);
  }

  /* Add the Message-Authenticator if required as octets of zero */
  if ((eapMessagePresent == L7_TRUE) ||
      (serverEntry->serverConfig.incMsgAuthMode == L7_ENABLE))
  {
    bzero(request->messageAuthenticator, sizeof(request->messageAuthenticator));

    (void)radiusAttrValuePairAdd(&request->requestInfo->vpList,
                                RADIUS_ATTR_TYPE_MESSAGE_AUTHENTICATOR,
                                L7_NULL,
                                request->messageAuthenticator,
                                (L7_uint32)sizeof(request->messageAuthenticator));
  }

  /*
  ** Calculate the packet length
  */

  /* Start with the RadiusHeader */
  pktLength = (L7_uint32)sizeof(radiusHeader_t);

  /* Then add the length of attributes supplied by user through ValuePairs */
  for (vp = request->requestInfo->vpList; vp; vp = vp->nextPair)
  {
    if (vp->vsAttrId)
    {
      pktLength += MIN_RADIUS_VSA_LENGTH;
    }

    pktLength += (L7_uint32)sizeof(radiusAttr_t) - 1;

    if (vp->attrType == RADIUS_ATTR_VALUE_TYPE_STRING)
    {
      pktLength += vp->intValue;
    }
    else
    {
      pktLength += 4; /* TBD */
    }
  }

  /*
  ** Fill in the packet
  */
  if ((packet = (radiusPacket_t *)osapiMalloc(L7_RADIUS_COMPONENT_ID, pktLength)) != L7_NULL)
  {
    /*
    ** Fill in the header
    */
    packet->header.code = RADIUS_CODE_ACCESS_REQUEST;

    /* Put the identifier into the packet */
    packet->header.id = request->requestId;

    /* Put the request authenticator into the packet */
    bcopy(request->authenticator, packet->header.authenticator, sizeof(request->authenticator));

    packet->header.msgLength = osapiHtons((L7_ushort16)pktLength);

    nextAttr = packet->attributes;

    /*
    ** Add the attributes supplied by user through ValuePairs
    */
    for (vp = request->requestInfo->vpList; vp; vp = vp->nextPair)
    {
      attr = (radiusAttr_t *) nextAttr;

      if (vp->vsAttrId)
      {
        L7_uint32 vsAttrLength;

        vsAttrLength = (vp->attrType == RADIUS_ATTR_VALUE_TYPE_STRING ? vp->intValue : 4);
        attr->type = vp->attrId;
        tmpValue = (L7_uint32)osapiHtonl(vp->vendorCode);
        bcopy((L7_char8 *)&tmpValue, attr->value, 4);
        attr->length = sizeof(radiusAttr_t) - 1 + 4 + vsAttrLength;

        nextAttr += MIN_RADIUS_VSA_LENGTH;
        attr = (radiusAttr_t *) nextAttr;
        attr->type = vp->vsAttrId;
      }
      else
      {
        attr->type = vp->attrId;
      }

      if (vp->attrType == RADIUS_ATTR_VALUE_TYPE_STRING)
      {
        attr->length = sizeof(radiusAttr_t) - 1 + vp->intValue;
        bcopy(vp->strValue, attr->value, vp->intValue);
      }
      else
      {
        attr->length = sizeof(radiusAttr_t) - 1 + 4;
        tmpValue = (L7_uint32)osapiHtonl(vp->intValue);
        bcopy((L7_char8 *)&tmpValue, attr->value, 4);
      }

      if (vp->attrId == RADIUS_ATTR_TYPE_MESSAGE_AUTHENTICATOR)
      {
        /* Save a pointer to the octets of zero */
        pMsgAuth = nextAttr + sizeof(radiusAttr_t) - 1;
      }

      nextAttr += attr->length;
    }

    /* Add the Message Authenticator if required */
    if ((eapMessagePresent == L7_TRUE) ||
        (serverEntry->serverConfig.incMsgAuthMode == L7_ENABLE))
    {
      /*
      ** Generate the unique Message-Authenticator (a one way MD5 hash of the packet
      ** with Message-Autheticator field being octets of zero) and fill the packet with it.
      **
      ** Note: This calculation must be last.
      */
      L7_hmac_md5((L7_uchar8 *)&packet->header,
                  pktLength,
                  (L7_uchar8 *)serverEntry->serverConfig.radiusServerConfigSecret,
                  (L7_uint32)strlen(serverEntry->serverConfig.radiusServerConfigSecret),
                  (L7_uchar8 *)request->messageAuthenticator);

      if (L7_NULL != pMsgAuth)
      {
        bcopy(request->messageAuthenticator, pMsgAuth, sizeof(request->messageAuthenticator));
      }
    }

    /*
    ** Link the packet to the request.
    */
    request->requestMessage = packet;

    return packet;
 }

 return L7_NULL;
}

/*********************************************************************
*
* @purpose Create the Accounting-Request packet with the mandatory objects,
*          and any optional objects.
*
* @param serverEntry @b((input)) current RADIUS server
* @param request @b((input)) Account-Request
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments Also link the created packet to the request which can be
*           used for retransmission, in case of timeouts.
*
* @end
*
*********************************************************************/
radiusPacket_t *radiusAccountingPacketCreate(radiusServerEntry_t *serverEntry,
                                             radiusRequest_t *request)
{
  L7_uint32 pktLength;
  L7_uchar8 *nextAttr;
  radiusPacket_t *packet;
  radiusAttr_t *attr;
  radiusValuePair_t *vp;
  L7_uint32 tmpValue;
  L7_MD5_CTX_t context;
  L7_uint32 secretLen = (L7_uint32)strlen(serverEntry->serverConfig.radiusServerConfigSecret);
  L7_uchar8 nasId[L7_RADIUS_NAS_IDENTIFIER_SIZE + 1];
  L7_uint32 nasIdSize;

  /* Get the NAS-Identifier */
  radiusNASIdentifierGet((L7_uchar8 *)&nasId, &nasIdSize);

  /*
  ** Calculate the packet length : RadiusHedaer + attributes
  */
  pktLength = (L7_uint32)sizeof(radiusHeader_t);
  pktLength += (L7_uint32)(2 * (sizeof(radiusAttr_t) - 1) + 4 + nasIdSize);

  /*
  ** Add the length of attributes supplied by user through ValuePairs
  */
  for (vp = request->requestInfo->vpList; vp; vp = vp->nextPair)
  {
     if (vp->vsAttrId)
     {
       pktLength += MIN_RADIUS_VSA_LENGTH;
     }

     pktLength += (L7_uint32)sizeof(radiusAttr_t) - 1;

     if (vp->attrType == RADIUS_ATTR_VALUE_TYPE_STRING)
     {
       pktLength += vp->intValue;
     }
     else
     {
       pktLength += 4;
     }
   }

   /*
   ** Fill in the packet
   */
   if ((packet = (radiusPacket_t *)osapiMalloc(L7_RADIUS_COMPONENT_ID, pktLength + secretLen)) != L7_NULL)
   {
      /*
      ** Fill in the header, except the authenticator
      */
      packet->header.code = RADIUS_CODE_ACCOUNT_REQUEST;
      packet->header.id = request->requestId;
      packet->header.msgLength = osapiHtons((L7_ushort16)pktLength);
      nextAttr = packet->attributes;

      /*
      ** Fill in the Acct-Status-Type attribute (mandatory)
      */
      attr = (radiusAttr_t *) nextAttr;
      attr->type = RADIUS_ATTR_TYPE_ACCT_STATUS_TYPE;
      attr->length = sizeof(radiusAttr_t) - 1 + 4;
      tmpValue = (L7_uint32)osapiHtonl(request->requestInfo->acctStatusType);
      bcopy((L7_char8 *)&tmpValue, attr->value, 4);
      nextAttr += attr->length;

      /*
      ** Fill in the NAS Identifier (optionally mandatory)
      */
      attr = (radiusAttr_t *) nextAttr;
      attr->type = RADIUS_ATTR_TYPE_NAS_IDENTIFIER;
      attr->length = sizeof(radiusAttr_t) - 1 + nasIdSize;
      bcopy((L7_char8 *)&nasId, attr->value, nasIdSize);
      nextAttr += attr->length;

      /*
      ** Add the attributes supplied by user through ValuePairs
      */
      for (vp = request->requestInfo->vpList; vp; vp = vp->nextPair)
      {
         attr = (radiusAttr_t *) nextAttr;

         if (vp->vsAttrId)
         {
           L7_uint32 vsAttrLength;

           vsAttrLength = (vp->attrType == RADIUS_ATTR_VALUE_TYPE_STRING ? vp->intValue : 4);
           attr->type = vp->attrId;
           tmpValue = (L7_uint32)osapiHtonl(vp->vendorCode);
           bcopy((L7_char8 *)&tmpValue, attr->value, 4);
           attr->length = sizeof(radiusAttr_t) - 1 + 4 + vsAttrLength;

           nextAttr += MIN_RADIUS_VSA_LENGTH;
           attr = (radiusAttr_t *) nextAttr;
           attr->type = vp->vsAttrId;
         }
         else
         {
           attr->type = vp->attrId;
         }

         if (vp->attrType == RADIUS_ATTR_VALUE_TYPE_STRING)
         {
           attr->length = sizeof(radiusAttr_t) - 1 + vp->intValue;
           bcopy(vp->strValue, attr->value, vp->intValue);
         }
         else
         {
           attr->length = sizeof(radiusAttr_t) - 1 + 4;
           tmpValue = (L7_uint32)osapiHtonl(vp->intValue);
           bcopy((L7_char8 *)&tmpValue, attr->value, 4);
         }

         nextAttr += attr->length;
      }

      /*
      ** Generate the unique request authenticator (a one way MD5 hash of the packet
      ** with autheticator field being 0 octets) and fill the packet with it.
      */
      bcopy(serverEntry->serverConfig.radiusServerConfigSecret,
            (L7_char8 *)packet + pktLength, secretLen);
      l7_md5_init(&context);
      l7_md5_update(&context, (L7_uchar8 *)packet, pktLength + secretLen);
      l7_md5_final((L7_uchar8 *)request->authenticator, &context);
      bcopy(request->authenticator, packet->header.authenticator,
            sizeof(request->authenticator));

      /*
      ** Link the packet to the request.
      */
      request->requestMessage = packet;

      return packet;
   }

   return L7_NULL;
}

/*********************************************************************
*
* @purpose Process the Accounting-Response packet recieved from the RADIUS
*          server.
*
* @param serverEntry @b((input)) current RADIUS server
* @param packet @b((input)) response received from the server
* @param request @b((input)) original Account-Request
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
void radiusAccountResponseProcess(radiusServerEntry_t *serverEntry,
                                  radiusPacket_t *packet,
                                  radiusRequest_t *request)
{
  if (radiusResponseAuthenticatorValidate(serverEntry, packet, request->authenticator) == L7_SUCCESS)
  {
    RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Successfully processed Account-Response.",
                     __FUNCTION__);

    /*
    ** Accounting-Response received for an outstanding request
    */
    radiusRequestDelink(serverEntry, request);
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE,  L7_RADIUS_COMPONENT_ID, "RADIUS: Accounting-Response failed to "
                    "validate, id=%u. The RADIUS Client received an invalid message from the server.\n",
                    packet->header.id);
  }

  bufferPoolFree(radius_lg_bp_id, (L7_uchar8 *)packet);

  return;
}

/*********************************************************************
*
* @purpose Process the Access-Challenge packet recieved from the RADIUS
*          server.
*
* @param serverEntry @b((input)) current RADIUS server
* @param packet @b((input)) response received from the server
* @param request @b((input)) original Access-Request
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
void radiusAccessChallengeProcess(radiusServerEntry_t *serverEntry,
                                  radiusPacket_t *packet,
                                  radiusRequest_t *request)
{
  RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Started Processing Access-Challenge, validation in progress.",
                     __FUNCTION__);

  if (radiusResponseAuthenticatorValidate(serverEntry, packet, request->authenticator) == L7_SUCCESS)
  {
    /*
    ** Validate any attributes that require special handling
    */
    if (radiusResponseAttributesValidate(serverEntry, packet, request) == L7_FAILURE)
    {
      RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Attributes failed validation, returning..",
                     __FUNCTION__);
      bufferPoolFree(radius_lg_bp_id, (L7_uchar8 *)packet);
      return;
    }

    if (strlen((L7_char8 *)request->requestInfo->userName) != L7_NULL)
    {
      RADIUS_DLOG(RD_LEVEL_INFO,"%s(): User (%s) needs to respond for challenge\n",
                     __FUNCTION__, (L7_char8 *)request->requestInfo->userName);
    }

    /*
    ** Access-Challenge received for an outstanding request
    */
    request->requestInfo->status = RADIUS_STATUS_CHALLENGED;
    RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Successfully processed Access-Challenge.",
                     __FUNCTION__);

    radiusResponseNotify(request->requestInfo, packet);

    radiusRequestDelink(serverEntry, request);
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE,  L7_RADIUS_COMPONENT_ID, "RADIUS: Access-Challenge failed to validate, id=%u."
                        " The RADIUS Client received an invalid message from the server.\n", packet->header.id);
    bufferPoolFree(radius_lg_bp_id, (L7_uchar8 *)packet);
  }

  return;
}

/*********************************************************************
*
* @purpose Process the Access-Reject packet recieved from the RADIUS
*          server.
*
* @param serverEntry @b((input)) pointer to RADIUS server
* @param packet @b((input)) packet received from server
* @param request @b((input)) origional request for this response
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
void radiusAccessRejectProcess(radiusServerEntry_t *serverEntry,
                               radiusPacket_t *packet,
                               radiusRequest_t *request)
{

  RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Started Processing Access-Reject, validation in progress.",
                     __FUNCTION__);

  if (radiusResponseAuthenticatorValidate(serverEntry, packet, request->authenticator) == L7_SUCCESS)
  {
    /*
    ** Validate any attributes that require special handling
    */
    if (radiusResponseAttributesValidate(serverEntry, packet, request) == L7_FAILURE)
    {
      RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Attributes failed validation, returning..",
                     __FUNCTION__);

      bufferPoolFree(radius_lg_bp_id, (L7_uchar8 *)packet);
      return;
    }

    /*
    ** Access-Reject received for an outstanding request
    */
    request->requestInfo->status = RADIUS_STATUS_AUTHEN_FAILURE;

    RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Successfully processed Access-Reject.",
                     __FUNCTION__);

    radiusResponseNotify(request->requestInfo, packet);

    radiusRequestDelink(serverEntry, request);
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "RADIUS: Access-Reject failed validation, id=%u\n", packet->header.id);
    bufferPoolFree(radius_lg_bp_id, (L7_uchar8 *)packet);
  }

  return;
}

/*********************************************************************
*
* @purpose Process the Access-Accept packet recieved from the RADIUS
*          server.
*
* @param serverEntry @b((input)) pointer to RADIUS server
* @param packet @b((input)) response packet received from server
* @param request @b((input)) origional outstanding request
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
void radiusAccessAcceptProcess(radiusServerEntry_t *serverEntry,
                               radiusPacket_t *packet,
                               radiusRequest_t *request)
{
  RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Started Processing Access Accept, validation in progress.",
                     __FUNCTION__);

  if (radiusResponseAuthenticatorValidate(serverEntry, packet, request->authenticator) == L7_SUCCESS)
  {
    /*
    ** Validate any attributes that require special handling
    */
    if (radiusResponseAttributesValidate(serverEntry, packet, request) == L7_FAILURE)
    {
      RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Attributes failed validation, returning..",
                     __FUNCTION__);
      bufferPoolFree(radius_lg_bp_id, (L7_uchar8 *)packet);
      return;
    }

    /*
    ** Access-Accept received for an outstanding request
    */
    request->requestInfo->status = RADIUS_STATUS_SUCCESS;
    RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Successfully processed Access-Accept.",
                     __FUNCTION__);

    radiusResponseNotify(request->requestInfo, packet);

    radiusRequestDelink(serverEntry, request);
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE,  L7_RADIUS_COMPONENT_ID, "RADIUS: Access-Accpet failed "
                 "to validate, id=%u. The RADIUS Client received an invalid message from the "
                 "server.\n", packet->header.id);
    bufferPoolFree(radius_lg_bp_id, (L7_uchar8 *)packet);
  }

  return;
}

/*********************************************************************
*
* @purpose Process attributes returned in a access response packet that may
*          require special handling.
*
* @param serverEntry @b((input)) current RADIUS server
* @param packet @b((input)) response received from the server
* @param request @b((input)) original Access-Request
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusResponseAttributesValidate(radiusServerEntry_t *serverEntry,
                                         radiusPacket_t *packet,
                                         radiusRequest_t *request)
{
  L7_uint32 attrsLength;
  radiusAttr_t *attr = (radiusAttr_t *)packet->attributes;
  L7_BOOL eapMessagePresent = L7_FALSE;
  L7_BOOL messageAuthenticatorPresent = L7_FALSE;
  L7_BOOL msKeyAttrPresent = L7_FALSE;
  L7_uint32  vendorId=0;
  radiusAttr_t  *msKey;
  L7_uint32  len = 0;
  L7_uchar8  *keyPtr = L7_NULLPTR;
  L7_uchar8  *key = L7_NULLPTR;
  L7_uchar8  *keyLen = L7_NULLPTR;
  L7_uchar8  sendKey[RADIUS_MS_KEY_SIZE];
  L7_uchar8  recvKey[RADIUS_MS_KEY_SIZE];
  L7_uchar8  sendKeyLen = 0;
  L7_uchar8  recvKeyLen = 0;

  attrsLength = osapiNtohs(packet->header.msgLength) - sizeof(radiusHeader_t);

  while (attrsLength > L7_NULL)
  {
    /*
    ** Look for an EAP message
    */
    if (attr->type == RADIUS_ATTR_TYPE_EAP_MESSAGE)
    {
      eapMessagePresent = L7_TRUE;
    }

    /*
    ** Look for a Message-Authenticator
    */
    if (attr->type == RADIUS_ATTR_TYPE_MESSAGE_AUTHENTICATOR)
    {
      messageAuthenticatorPresent = L7_TRUE;
      bcopy(attr->value, request->messageAuthenticator, sizeof(request->messageAuthenticator));
      bzero(attr->value, sizeof(request->messageAuthenticator));
    }

    /*
    ** Look for vendor specific attribute Microsoft key
    */

    if (attr->type == RADIUS_ATTR_TYPE_VENDOR)
    {
       /*
       ** ------------------------------------------------------------------------------------------------
       *  | Vendor | Vendor   | Vendor  | MS-MPPE  | MS-MPPE | Encrypted | MS-MPPE  | MS-MPPE | Encrypted |
       *  | Type   | Attr len | ID      | Key-Type | Key-len | Key       | Key-Type | Key-len | Key       |
       *  | 1 byte | 1 byte   | 4 bytes | 1 byte   | 1 byte  | XX bytes  | 1 byte   | 1 byte  | XX bytes  |                                                                 |
       *  ------------------------------------------------------------------------------------------------
       */

       /*
       ** Extract the ms key attribute
       */
        memcpy(&vendorId,attr->value,RADIUS_VENDOR_ID_SIZE);

        if( osapiNtohl(vendorId) == RADIUS_VENDOR_ID_MICROSOFT)
        {

           /*
           ** Point to encrypted key attribute, after vendor ID attribute
           */
           msKey = (radiusAttr_t *)(((L7_uchar8 *)attr) +
                                    sizeof(radiusAttr_t) - 1 +
                                    RADIUS_VENDOR_ID_SIZE);

           /*
           ** MS key length
           */
           len  =  msKey->length - sizeof(radiusAttr_t) + 1;

           if (msKey->type == RADIUS_VENDOR_ATTR_MS_MPPE_SEND_KEY )
           {
               key = sendKey;
               keyLen = &sendKeyLen;
           }
           else if (msKey->type == RADIUS_VENDOR_ATTR_MS_MPPE_RECV_KEY)
           {
               key = recvKey;
               keyLen = &recvKeyLen;
           }
           else
           {
               L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                       "RADIUS: MS attribute type =%d\n",msKey->type);
           }

            if ((msKey->type == RADIUS_VENDOR_ATTR_MS_MPPE_SEND_KEY)||
                (msKey->type == RADIUS_VENDOR_ATTR_MS_MPPE_RECV_KEY))
            {

               /*
               ** Point to encrypted key value, after vendor ID, key type and key
               *  length
               */

               keyPtr = (((L7_uchar8 *)attr) + RADIUS_VENDOR_ID_SIZE +
                         (2*(sizeof(radiusAttr_t)- 1)));

               /*
               ** Decrypt the MS key
               */
               if (radiusDecryptMSKey(keyPtr,len,request->authenticator,
                   (L7_uchar8 *)serverEntry->serverConfig.radiusServerConfigSecret,
                   (L7_uint32)strlen(serverEntry->serverConfig.radiusServerConfigSecret),
                    key, keyLen) == L7_SUCCESS)
               {
                  msKeyAttrPresent = L7_TRUE;
               }
            }
        }
    }

    attrsLength -= attr->length;
    attr = (radiusAttr_t *) ((L7_char8 *)attr + attr->length);
  }

  /*
  ** Validate the Message-Authenticator if present
  */
  if ((messageAuthenticatorPresent == L7_TRUE) &&
      (radiusMessageAuthenticatorValidate(serverEntry, packet, request) != L7_SUCCESS))
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE,  L7_RADIUS_COMPONENT_ID, "RADIUS: Failed to validate "
               "Message-Authenticator, id=%u. The RADIUS Client received an invalid message "
               "from the server.\n", packet->header.id);
    return L7_FAILURE;
  }

  /*
  ** A Message-Authenticator must be present with an EAP-Message
  */
  if ((eapMessagePresent == L7_TRUE) &&
      (messageAuthenticatorPresent == L7_FALSE))
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE,  L7_RADIUS_COMPONENT_ID, "RADIUS: Response is missing "
                "Message-Authenticator, id=%u. The RADIUS Client received an invalid message"
                " from the server.\n", packet->header.id);
    return L7_FAILURE;
  }

  /*
  ** Replace encrypted key with decrypted key in MS key attribute value
  */
  if(msKeyAttrPresent == L7_TRUE)
  {
    radiusCopyDecrMSKey2attr(packet, sendKey, sendKeyLen,
                             recvKey, recvKeyLen);
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Determine the type of the request, create the corresponding
*          Radius packet, queue the packet for timeouts/response
*          matching, send the packet to the RADIUS server.
*
*
* @param requestInfo @b((input))
*
* @returns
*
* @comments The input Attr-Value pair list must be freed.
*
* @end
*
*********************************************************************/
L7_RC_t radiusRequestInfoProcess(radiusRequestInfo_t *requestInfo)
{
  L7_RC_t rv = 0;
  radiusRequest_t *request = L7_NULL;
  radiusPacket_t *packet = L7_NULL;
  radiusServerEntry_t *serverEntry = L7_NULL, *currentEntry = L7_NULL;
  radiusClient_t *pClient = radiusClientGet();
  L7_RADIUS_SERVER_USAGE_TYPE_t usageType;
  L7_char8 serverName[L7_RADIUS_SERVER_NAME_LENGTH+1];
  L7_RC_t rc;
  L7_char8 name[L7_COMPONENT_NAME_MAX_LEN];

  RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Entered in to the function..",
                     __FUNCTION__);

  if ((L7_NULL == requestInfo) || (L7_NULL == requestInfo->serverNameIndex))
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "RADIUS (): request is dropped as it has no reference to current server\n");

    return L7_FAILURE;
  }

  if ((rc = cnfgrApiComponentNameGet(requestInfo->componentID, name)) != L7_SUCCESS)
  {
    osapiStrncpySafe(name, "Unknown", 8);
  }

  if (bufferPoolAllocate(radius_sm_bp_id, (L7_uchar8 **)&request) == L7_SUCCESS)
  {
    (void)memset((void *)request, L7_NULL, RADIUS_SM_BUF_SIZE);

    /* Create the appropriate RADIUS packet */
    request->requestInfo = requestInfo;

    /*
    ** Look for an access request with a value pair list present
    */
    if ((requestInfo->requestType == RADIUS_REQUEST_TYPE_AUTH_VP))
    {
      RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Forming Access-Request message with Value-Pair list.",
                     __FUNCTION__);

      /* revert to the standard type for further processing */
      requestInfo->requestType = RADIUS_REQUEST_TYPE_AUTH;

      /*
      ** Only process authorization requests if enabled
      */
      if ((pClient != L7_NULL) && (pClient->authOperMode == L7_ENABLE))
      {
        if (requestInfo->componentID == L7_DOT1X_COMPONENT_ID)
        {
          usageType = L7_RADIUS_SERVER_USAGE_DOT1X;
        }
        else if (requestInfo->componentID == L7_USER_MGR_COMPONENT_ID)
        {
          usageType = L7_RADIUS_SERVER_USAGE_LOGIN;
        }
        else
        {
          usageType = L7_RADIUS_SERVER_USAGE_ALL;
        }
        (void)radiusServerAuthCurrentEntryGet(requestInfo->serverNameIndex, usageType, &currentEntry);
        if ((currentEntry != L7_NULLPTR) && 
           (currentEntry->serverConfig.radiusServerDnsHostName.hostAddrType
                 == L7_IP_ADDRESS_TYPE_DNS) &&
           (currentEntry->pendingRequests == NULL))
        {
          rc =  radiusResolveServerIPAddress(currentEntry);
          if(rc == L7_SUCCESS)
          {
            RADIUS_DLOG(RD_LEVEL_INFO,"%s(): DNS entry resolved-: dnsName: %s IP: %s.",
                       __FUNCTION__,currentEntry->radiusServerDnsHostName.host.
                       hostName,
                       osapiInet_ntoa(currentEntry->serverConfig.
                       radiusServerConfigIpAddress));

            currentEntry->serverConfig.radiusServerConfigRowStatus =
                                RADIUS_SERVER_ACTIVE;
          }
          else
          {
          radiusServerClose(currentEntry, currentEntry->serverNameIndex);
          currentEntry->serverConfig.radiusServerConfigRowStatus =
                          RADIUS_SERVER_NOTREADY;
          currentEntry = L7_NULLPTR; 
          }
        }
        if( L7_NULLPTR != currentEntry )
        {
          radiusResolveAllServerIPAddresses(RADIUS_SERVER_TYPE_AUTH);
          serverEntry = currentEntry;
        }
        else
        {
          if ((rv = radiusServerNameEntryGet(requestInfo->serverNameIndex, serverName)) != L7_FAILURE)
          {
            RADIUS_DLOG(RD_LEVEL_INFO, "%s(): Current Server not found for server name:%s, reselecting the current server",
                      __FUNCTION__, serverName); 
          }
          radiusResolveAllServerIPAddresses(RADIUS_SERVER_TYPE_AUTH);
          serverEntry = radiusServerSelect(RADIUS_SERVER_TYPE_AUTH, usageType, requestInfo->serverNameIndex);

          if(serverEntry != L7_NULL)
          {
            rv = radiusServerOpen(serverEntry);
            if(rv != L7_SUCCESS)
            {
              serverEntry = L7_NULL;
            }
          }
          /*If current server is not ready by now, function returns FAIL
          after appropriate house keeping.  Note that creation of packet
          and calling send() happens only when serverEntry != L7_NULL..*/
        }
        if(serverEntry != L7_NULL)
        {
          request->tryAnotherServer =
                 radiusAuthActiveServerCountGet(requestInfo->serverNameIndex, usageType) - 1;

          request->requestId = ++serverEntry->requestId;
          packet = radiusRequestPacketCreate(serverEntry, request);
        }
        else
        {
          L7_LOGF (L7_LOG_SEVERITY_NOTICE, L7_RADIUS_COMPONENT_ID,
                   "%s: Radius server not selected. Request Type: %d  Requestor: %d, %s",
                   __func__, requestInfo->requestType, requestInfo->componentID, name);
        }
      }
    }
    /*
    ** Look for a simple user access request
    */
    else if ((requestInfo->requestType == RADIUS_REQUEST_TYPE_AUTH))
    {
      RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Forming sumple user Access-Request message.",
                     __FUNCTION__);

      /*
      ** Only process authorization requests if enabled
      */
      if ((pClient != L7_NULL) && (pClient->authOperMode == L7_ENABLE))
      {
        if (requestInfo->componentID == L7_DOT1X_COMPONENT_ID)
        {
          usageType = L7_RADIUS_SERVER_USAGE_DOT1X;
        }
        else if (requestInfo->componentID == L7_USER_MGR_COMPONENT_ID)
        {
          usageType = L7_RADIUS_SERVER_USAGE_LOGIN;
        }
        else
        {
          usageType = L7_RADIUS_SERVER_USAGE_ALL;
        }
        (void)radiusServerAuthCurrentEntryGet(requestInfo->serverNameIndex, usageType, &currentEntry);
        if ((currentEntry != L7_NULLPTR) && 
            (currentEntry->serverConfig.radiusServerDnsHostName.hostAddrType
              == L7_IP_ADDRESS_TYPE_DNS) &&
            (currentEntry->pendingRequests == NULL))
        {
          rc =  radiusResolveServerIPAddress(currentEntry);
          if(rc == L7_SUCCESS)
          {
            RADIUS_DLOG(RD_LEVEL_INFO,"%s(): DNS entry resolved-: dnsName: %s IP: %s.",
                       __FUNCTION__,currentEntry->radiusServerDnsHostName.host.
                       hostName,
                       osapiInet_ntoa(currentEntry->serverConfig.
                       radiusServerConfigIpAddress));

            currentEntry->serverConfig.radiusServerConfigRowStatus =
                                RADIUS_SERVER_ACTIVE;
          }
          else
          {
             radiusServerClose(currentEntry, currentEntry->serverNameIndex);
             currentEntry->serverConfig.radiusServerConfigRowStatus =
                          RADIUS_SERVER_NOTREADY;
             currentEntry = L7_NULLPTR; 
          }
        }
        if( L7_NULLPTR != currentEntry )
        {
          radiusResolveAllServerIPAddresses(RADIUS_SERVER_TYPE_AUTH);
          serverEntry = currentEntry;
        }
        else
        {
          if ((rv = radiusServerNameEntryGet(requestInfo->serverNameIndex, serverName)) != L7_FAILURE)
          {
            RADIUS_DLOG(RD_LEVEL_INFO, "%s(): Current Server not found for server name:%s, reselecting the current server",
                        __FUNCTION__, serverName);
          }

          radiusResolveAllServerIPAddresses(RADIUS_SERVER_TYPE_AUTH);
          serverEntry = radiusServerSelect(RADIUS_SERVER_TYPE_AUTH, usageType, requestInfo->serverNameIndex);

          if(serverEntry != L7_NULL)
          {
            rv = radiusServerOpen(serverEntry);
            if(rv != L7_SUCCESS)
            {
              serverEntry = L7_NULL;
            }
          }
          /*If current server is not ready by now, function returns FAIL
            after appropriate house keeping.  Note that creation of packet
            and calling send() happens only when serverEntry != L7_NULL..*/
        }
        if(serverEntry != L7_NULL)
        {
          request->tryAnotherServer =
                 radiusAuthActiveServerCountGet(requestInfo->serverNameIndex, usageType) - 1;

          request->requestId = ++serverEntry->requestId;
          packet = radiusUserAuthenticateCreate(serverEntry, request);
        }
        else
        {
          L7_LOGF (L7_LOG_SEVERITY_NOTICE, L7_RADIUS_COMPONENT_ID,
                   "%s: Radius server not selected. Request Type: %d  Requestor: %d, %s",
                   __func__, requestInfo->requestType, requestInfo->componentID, name);
        }
      }
    }
    /*
    ** Look for an accounting request
    */
    else if (requestInfo->requestType == RADIUS_REQUEST_TYPE_ACCT )
    {
      RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Forming Acct-Request message.",
                     __FUNCTION__);

      /*
      ** Only process accounting requests if accounting mode is enabled
      */
      if ((pClient != L7_NULL) && (pClient->acctAdminMode == L7_ENABLE))
      {
        radiusServerAcctCurrentEntryGet(requestInfo->serverNameIndex,&currentEntry);
        if ((currentEntry != L7_NULLPTR) && 
            (currentEntry->serverConfig.radiusServerDnsHostName.hostAddrType
              == L7_IP_ADDRESS_TYPE_DNS) &&
            (currentEntry->pendingRequests == NULL))
        {
          rc =  radiusResolveServerIPAddress(currentEntry);
          if(rc == L7_SUCCESS)
          {
            RADIUS_DLOG(RD_LEVEL_INFO,"%s(): DNS entry resolved-: dnsName: %s IP: %s.",
                        __FUNCTION__,currentEntry->radiusServerDnsHostName.host.
                        hostName,
                        osapiInet_ntoa(currentEntry->serverConfig.
                        radiusServerConfigIpAddress));

           currentEntry->serverConfig.radiusServerConfigRowStatus =
                                 RADIUS_SERVER_ACTIVE;
          }
          else
          {
           radiusServerClose(currentEntry, currentEntry->serverNameIndex);
           currentEntry->serverConfig.radiusServerConfigRowStatus =
                          RADIUS_SERVER_NOTREADY;
           currentEntry = L7_NULLPTR; 
          }
        }
        if( L7_NULLPTR != currentEntry )
        {
          serverEntry = currentEntry;
        }
        else
        {
          if ((rv = radiusAcctServerNameEntryGet(requestInfo->serverNameIndex, serverName)) != L7_FAILURE)
          {
            RADIUS_DLOG(RD_LEVEL_INFO, "%s(): Current Server not found for server name:%s !",
                        __FUNCTION__, serverName);
          }

          radiusResolveAllServerIPAddresses(RADIUS_SERVER_TYPE_ACCT);
          /* After dns client resolution, configured accounting server
             may become active, hence we can open socket, now...*/
            serverEntry = radiusServerSelect(RADIUS_SERVER_TYPE_ACCT, L7_RADIUS_SERVER_USAGE_ALL,
                     requestInfo->serverNameIndex);
          if(serverEntry != L7_NULL)
          {
            rv = radiusServerOpen(serverEntry);
            if(rv != L7_SUCCESS)
            {
              serverEntry = L7_NULL;
            }
          }

        }
        if(serverEntry != L7_NULL)
        {
          /* Accounting server shall not h'v backup servers.*/
          request->tryAnotherServer = 0 ;
          request->requestId = ++serverEntry->requestId;
          packet = radiusAccountingPacketCreate(serverEntry, request);
        }
        else
        {
          L7_LOGF (L7_LOG_SEVERITY_NOTICE, L7_RADIUS_COMPONENT_ID,
                   "%s: Radius server not selected. Request Type: %d  Requestor: %d, %s",
                   __func__, requestInfo->requestType, requestInfo->componentID, name);
        }
      }
    }

    /*
    ** Was a RADIUS packet generated
    */
    if (packet != L7_NULL)
    {
      RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Sending request packet with ID: %d.",
                     __FUNCTION__,request->requestId);

      /* Send the request packet */
      rv = radiusPacketSend(serverEntry, request);

      if(rv == L7_SUCCESS) {

        /* Insert the new request at the front of the request queue */
        request->nextRequest = serverEntry->pendingRequests;

        if (serverEntry->pendingRequests != L7_NULL)
        {
          serverEntry->pendingRequests->prevRequest = request;
        }

        serverEntry->pendingRequests = request;

        /* Update tx stats */
      if (serverEntry->serverConfig.radiusServerConfigServerType == RADIUS_SERVER_TYPE_AUTH)
      {
           serverEntry->radiusServerStats_t.authStats.radiusAuthClientAccessRequests++;
           serverEntry->radiusServerStats_t.authStats.radiusAuthClientPendingRequests++;
        }
        else
        {
           serverEntry->radiusServerStats_t.acctStats.radiusAcctClientRequests++;
           serverEntry->radiusServerStats_t.acctStats.radiusAcctClientPendingRequests++;
        }

        /* Check for a supplied value pairs list and free it if present */
        if (requestInfo->vpList != L7_NULL)
        {
          radiusAttrValuePairListFree(requestInfo->vpList);
        }

        return L7_SUCCESS;
      }
    }
    else
    {
       L7_LOGF(L7_LOG_SEVERITY_NOTICE,  L7_RADIUS_COMPONENT_ID,"RADIUS: Server Entry is Null or Could not allocate Radius Packet \n");
    }
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "RADIUS: Could not allocate a request buffer\n");
  }

  /*
  ** Check for a supplied value pairs list and free it if present
  */
  if ((requestInfo != L7_NULL) && (requestInfo->vpList != L7_NULL))
  {
    radiusAttrValuePairListFree(requestInfo->vpList);
  }

  /*
  ** For auth requests, notify the user of the failure
  */
  if ((requestInfo->requestType == RADIUS_REQUEST_TYPE_AUTH) ||
      (requestInfo->requestType == RADIUS_REQUEST_TYPE_AUTH_VP))
  {
    requestInfo->status = RADIUS_STATUS_COMM_FAILURE;
    radiusResponseNotify(requestInfo, L7_NULL);
  }

  if (requestInfo != L7_NULL)
  {
    bufferPoolFree(radius_sm_bp_id, (L7_uchar8 *)requestInfo);
  }

  if (request != L7_NULL)
  {
    request->requestInfo = L7_NULL;
    bufferPoolFree(radius_sm_bp_id, (L7_uchar8 *)request);
  }

  L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
          "RADIUS: radiusRequestInfoProcess(): Could not "
                     "successfully process the request\n");
  return L7_FAILURE;
}

/*****************************************************************************
*
* @purpose Replace encrypted MS-MPPE-Send-Key/Recv-Key with decrypted keys.
*
* @param packet     @b((input))  Pointer to buffer containing radius packet
* @param sendKey    @b((output)) Pointer to MS-MPPE-Send key
* @param sendkeyLen @b((output)) MS-MPPE-Send key len
* @param recvKey    @b((output)) Pointer to MS-MPPE-Recv key
* @param recvkeyLen @b((output)) MS-MPPE-Recv key len
*
* @returns
*
* @comments
*
* @end
*
******************************************************************************/
static void radiusCopyDecrMSKey2attr(radiusPacket_t *packet,
                                     L7_uchar8 *sendKey,
                                     L7_uchar8 sendKeyLen,
                                     L7_uchar8 *recvKey,
                                     L7_uchar8 recvKeyLen)
{
  L7_uint32    attrsLength;
  radiusAttr_t *attr = (radiusAttr_t *)packet->attributes;
  L7_uchar8    *pos=L7_NULLPTR;
  L7_uint32    vendorId=0;
  L7_uchar8    keyAttrLen=0;
  L7_uchar8    keyLen=0;

  attrsLength = osapiNtohs(packet->header.msgLength) - sizeof(radiusHeader_t);

  while (attrsLength > L7_NULL)
  {
    /*
    ** Look for vendor specific attribute Microsoft key
    */

    if (attr->type == RADIUS_ATTR_TYPE_VENDOR)
    {
       /*
       ** Extract the ms key attribute and decrypt the key
       */
        memcpy(&vendorId,attr->value, RADIUS_VENDOR_ID_SIZE);

        if( osapiNtohl(vendorId) == RADIUS_VENDOR_ID_MICROSOFT)
        {
           /*
           ** Advace to after vendor ID
           */
           pos = (L7_uchar8 *)attr + sizeof(radiusAttr_t)-1 + RADIUS_VENDOR_ID_SIZE;

           keyAttrLen = *(pos+1);

           /*
           ** Print MS key attribtues
           */
           radiusDebugPktDump(RD_LEVEL_CLUSTER_RX_PKT, pos, keyAttrLen);

           /*
           ** clear the encrypted key and add decrypted key, but key length
            * remains unchanged
            */
           keyLen = (keyAttrLen - sizeof(radiusAttr_t)+ 1);

           /*
           ** MS-MPPE encrypted key value is cleared and replaced with decrypted key value.
           *  The length of MS-MPPE key attribtue is unchanged, in the the key value first
           *  byte is decrypted key length followed by decrypted key and the remaining length
           *  is padded with zeros.
            */

           /*
           ** ------------------------------------------------------------------------------------------------------------------------
           *  | Vendor | Vendor   | Vendor  | MS-MPPE  | MS-MPPE | Decrypted | Decrypted | MS-MPPE  | MS-MPPE | Decrypted | Decrypted |
           *  | Type   | Attr len | ID      | Key-Type | Key-len | key len   | Key       | Key-Type | Key-len | key len   | Key       |
           *  | 1 byte | 1 byte   | 4 bytes | 1 byte   | 1 byte  | 1 byte    | XX bytes  | 1 byte   | 1 byte  | 1 byte    | XX bytes  |                                                                 |
           *  ------------------------------------------------------------------------------------------------------------------------
           */

           if ( *(pos) == RADIUS_VENDOR_ATTR_MS_MPPE_SEND_KEY )
           {
               memset( (pos+sizeof(radiusAttr_t)-1), 0, keyLen);
               memcpy(pos+sizeof(radiusAttr_t), sendKey, sendKeyLen);
               *(pos+sizeof(radiusAttr_t)-1) = sendKeyLen;
           }
           else if (*(pos) == RADIUS_VENDOR_ATTR_MS_MPPE_RECV_KEY)
           {
               memset( (pos+sizeof(radiusAttr_t)-1), 0, keyLen);
               memcpy(pos+sizeof(radiusAttr_t), recvKey, recvKeyLen);
               *(pos+sizeof(radiusAttr_t)-1) = recvKeyLen;
           }

           /*
           ** Print MS key attribtues
           */
           radiusDebugPktDump(RD_LEVEL_CLUSTER_RX_PKT, pos, keyAttrLen);
        }
    }
    attrsLength -= attr->length;
    attr = (radiusAttr_t *) ((L7_char8 *)attr + attr->length);
  }
}

#ifdef L7_ROUTING_PACKAGE
/*********************************************************************
* @purpose  This is the callback function RADIUS registers with IP MAP
*           for notification of routing events.
*
* @param    intIfnum @b{(input)}  internal interface number
* @param    event    @b{(input)}  an event listed in L7_RTR_EVENT_CHANGE_t
* @param    pData    @b{(input)}  unused
* @param    response @b{(input)}  provides parameters used to acknowledge
*                                 processing of the event
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t radiusRoutingEventChangeCallBack (L7_uint32 intIfNum,
                                          L7_uint32 event,
                                          void *pData,
                                          ASYNC_EVENT_NOTIFY_INFO_t *response)
{
    L7_RC_t rc = L7_SUCCESS;
    ASYNC_EVENT_COMPLETE_INFO_t event_completion;

    memset(&event_completion, 0, sizeof( ASYNC_EVENT_COMPLETE_INFO_t) );

    if (event == L7_RTR_INTF_ENABLE)
    {
      rc = radiusIssueCmd(RADIUS_ROUTING_INTF_ENABLE, L7_NULL, (void *)&intIfNum);
    }
    else if (event == L7_RTR_INTF_DISABLE_PENDING)
    {
      rc = radiusIssueCmd(RADIUS_ROUTING_INTF_DISABLE, L7_NULL, (void *)&intIfNum);
    }

    if ((response != L7_NULLPTR) &&
        (response->handlerId != 0) &&
        (response->correlator != 0))
    {
      /* acknowledge RADIUS processing of the event */
      /* Purposely overloading componentID with L7_IPMAP_REGISTRANTS_t */
      event_completion.componentId = L7_IPRT_RADIUS;
      event_completion.handlerId = response->handlerId;
      event_completion.correlator = response->correlator;
      event_completion.async_rc.rc = rc;
      event_completion.async_rc.reason = ASYNC_EVENT_REASON_RC_SUCCESS;
      asyncEventCompleteTally(&event_completion);
    }
    return rc;
}
#endif

