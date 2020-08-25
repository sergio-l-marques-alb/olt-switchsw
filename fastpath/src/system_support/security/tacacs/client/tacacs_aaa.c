/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename tacacs_aaa.c
*
* @purpose This file contains all functions common to authentication,
*          authorization and accounting.
*
* @component tacacs+
*
* @comments none
*
* @create 03/17/2005
*
* @author gkiran
*         dfowler 06/23/05
*
* @end
*
**********************************************************************/
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include "l7_common.h"
#include "md5_api.h"
#include "tacacs_api.h"
#include "tacacs_cfg.h"
#include "tacacs_aaa.h"
#include "osapi.h"
#include "osapi_support.h"
#include "tacacs_exports.h"

extern tacacsCfg_t     *tacacsCfg;
extern tacacsOprData_t *tacacsOprData;

/*********************************************************************
*
* @purpose To get the server entry index for the given server IP address.
*
* @param   L7_uint32   ip       @b{(input)}   server ip address.
* @param   L7_ushort16 *index   @b{(output)}  configuration index.
*
* @returns L7_SUCCESS, index set to server entry
* @returns L7_FAILURE, not found
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t tacacsServerIndexGet(L7_uint32    ip,
                             L7_uint32   *index)
{
  L7_ushort16 i = 0;

  if (index == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  for (i = 0; i < L7_TACACS_MAX_SERVERS; i++)
  {
    if (tacacsCfg->servers[i].ip == ip)
    {
      *index = i;
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose To get the server entry index for the given server host address.
*
* @param   L7_uchar8   serverAddress @b{(input)} the Hostname or 
*                                                IP address of the server.
* @param   L7_ushort16 *index        @b{(output)}  configuration index. 
*
* @returns L7_SUCCESS, index set to server entry
* @returns L7_FAILURE, not found
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t tacacsServerHostIndexGet(L7_uchar8   *serverAddress,
                                 L7_uint32   *index)
{
  L7_ushort16 srvIndex = 0;
  L7_uint32  length;

  if (index == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  for (srvIndex = 0; srvIndex < L7_TACACS_MAX_SERVERS; srvIndex++)
  {
    if((strlen(serverAddress) > strlen(tacacsCfg->servers[srvIndex].hostName.host.hostName)) ? 
       (length = strlen(serverAddress)) : (length = strlen(tacacsCfg->servers[srvIndex].hostName.host.hostName)))
    {
       if (osapiStrncmp(tacacsCfg->servers[srvIndex].hostName.host.hostName, serverAddress, length) == L7_NULL)
       {
         *index = srvIndex;
         return L7_SUCCESS;
       }
    }
    
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose To get the server entry index for the given server IP address or
*          Hostname.
*
* @param   L7_IP_ADDRESS_TYPE_t type @b{(input)} Address type DNS or IP address
* @param   L7_uchar8   serverAddress @b{(input)} the Hostname or 
*                                                IP address of the server.
* @param   L7_ushort16 *index        @b{(output)}configuration index. 
*
* @returns L7_SUCCESS, index set to server entry
* @returns L7_FAILURE, not found
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t tacacsServerIPHostNameIndexGet(L7_IP_ADDRESS_TYPE_t type,
                                       L7_uchar8   *serverAddress,
                                       L7_uint32   *index)
{
  L7_RC_t rc = L7_FAILURE;
  
  if (type == L7_IP_ADDRESS_TYPE_IPV4)
  {
    rc = tacacsServerIndexGet(osapiInet_addr(serverAddress), index);
  }
  else
  {
    rc = tacacsServerHostIndexGet(serverAddress, index);
  }
  return rc;
}

/*********************************************************************
*
* @purpose Open and bind socket for a configured server
*
* @param tacacsServerCfg_t *server @((input)) pointer to server cfg
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments  This function does not do the connect, it creates the fd
*            and does the bind.
*
* @end
*
*********************************************************************/
L7_RC_t tacacsServerSocketOpen(tacacsServerCfg_t *server,
                               L7_uint32         *socket)
{
  L7_sockaddr_in_t s_addr;
  L7_uint32        s = 0;

  memset(&s_addr, 0, sizeof(L7_sockaddr_in_t));

  /* create the socket */
  if (osapiSocketCreate(L7_AF_INET, 
                        L7_SOCK_STREAM, 
                        L7_NULL, &s) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  s_addr.sin_family = L7_AF_INET;
  s_addr.sin_port = osapiHtons(L7_NULL);
  s_addr.sin_addr.s_addr = osapiHtonl(server->sourceIP);
  if (osapiSocketBind(s, (L7_sockaddr_t *)&s_addr, 
                      sizeof(s_addr)) != L7_SUCCESS)
  {
    osapiSocketClose(s);
    return L7_FAILURE;
  }

  *socket = s;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose To generate session identifier randomly.
*
* @param pSessionId @b{(output)} the generated Session Identifier.
*
* @returns none
*
* @comments none
*
* @end
*
*********************************************************************/
L7_uint32 tacacsSessionIdGenerate(void)
{
  /* dfowler - FIX THIS, need to ENSURE random, clock may not do it */
  L7_uint32 id = 0;
  L7_clocktime randomSeed;

  /*Get the UTC time and 
   * use this as seed to generate random engine */

  osapiUTCTimeGet(&randomSeed);

  srand(randomSeed.seconds);

  while ((id = rand()) == 0);
  return id;
}

/*********************************************************************
*
* @purpose To calculate MD5 message digest hash for given input.
*
* @param pDigestOutput @b{(output)} The MD5 message digested Output.
* @param pDigestInput  @b{(input)}  The MD5 message digested input string.
* @param inputLen      @b{(input)}  The length of the input string.
*
* @returns none
*
* @comments none
*
* @end
*
*********************************************************************/
static void tacacsMD5DigestCalculate(L7_uchar8 *pDigestOutput,
                                     L7_uchar8 *pDigestInput, 
                                     L7_uchar8 inputLen)
{
  L7_MD5_CTX_t context;

  l7_md5_init(&context);
  l7_md5_update(&context, pDigestInput, inputLen);
  l7_md5_final(pDigestOutput, &context);

  return;
}

/*********************************************************************
*
* @purpose To encrypt/decrypt the TACACS+ packet. The packet is encrypted
*          for clear text input string. The packet is decrypted for
*          encrypted packet input string.
*
* @param L7_uchar8 *packet    @b((input/output)) pointer to packet buffer
* @param L7_uint32  length    @b{(input)}        packet length - header
* @param L7_char8  *key       @b{(input)}        encryption key
*
* @returns none
*
* @comments none
*
* @end
*
*********************************************************************/
void tacacsEncryptDecrypt(L7_uchar8 *packet,
                          L7_uint32  length,  /* excludes packet header */
                          L7_char8  *key)
{
  L7_char8     keyLength = strlen(key);
  L7_char8     inputLength = L7_NULL;
  L7_short16   msgLength = L7_NULL;
  L7_short16   pktBytes = L7_NULL;
  L7_short16   md5OutBytes = L7_NULL;
  L7_char8     md5Input[L7_TACACS_MD5_INPUT_LEN];
  L7_char8     md5Output[L7_TACACS_MD5_OUTPUT_LEN];

  memset(md5Input, L7_NULL, sizeof(md5Input));
  memset(md5Output, L7_NULL, sizeof(md5Output));

  /* Constructing the first MD5 hash as per standard */
  /* Copying session id in to input string */
  memcpy(&(md5Input[msgLength]), (packet + L7_TACACS_PKT_SESSION_ID_OFFSET), 
         L7_TACACS_SESSION_ID_BYTES);
  msgLength += L7_TACACS_SESSION_ID_BYTES;

  /* Copying secret key in to input string */
  memcpy(&(md5Input[msgLength]), key, keyLength);
  msgLength += keyLength;

  /* Copying version number in to input string */
  md5Input[msgLength] = *(packet + L7_TACACS_PKT_VERSION_OFFSET);
  msgLength += L7_TACACS_PKT_VERSION_BYTES;

  /* Copying sequence number in to input string */
  md5Input[msgLength] = *(packet + L7_TACACS_PKT_SEQ_NO_OFFSET);
  msgLength += L7_TACACS_SEQ_NO_BYTES;

  inputLength = msgLength; /* for first MD5 hash */

  while (pktBytes < length)
  {
    tacacsMD5DigestCalculate(md5Output, md5Input, inputLength);
    for (md5OutBytes = L7_NULL; 
         md5OutBytes < L7_TACACS_MD5_OUTPUT_LEN;
         md5OutBytes++)
    {
      /* XOR-ing byte wise with pseudo random pad*/
      packet[L7_TACACS_BODY_OFFSET + pktBytes] ^= md5Output[md5OutBytes];
      pktBytes++;
      if (pktBytes >= length)
      {
        /* If the length of the body is exceeding, truncate!!!*/
        break;
      }
    } /* end of for loop */
    memcpy(&(md5Input[msgLength]), md5Output, L7_TACACS_MD5_OUTPUT_LEN);
    inputLength = msgLength + L7_TACACS_MD5_OUTPUT_LEN;
  } /* end of while packet bytes */

  return;
}

/*********************************************************************
*
* @purpose To get the next appropriate server to contact.
*
* @param   L7_uint32  ip        @b{(input)}  current server ip, 0 returns first to contact
* @param   L7_uint32 *serverIdx @b{(output)} next server configuration index
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments ip == 0, return first server to contact.
*
* @end
*
*********************************************************************/
L7_RC_t tacacsServerToContactNextGet(L7_uint32  ip,
                                     L7_uint32 *serverIdx)
{
  L7_uint32 index = 0;
  L7_uint32 priority = L7_TACACS_PRIORITY_MAX + 1;  /* max is LOWEST priority */
  L7_BOOL   found = L7_FALSE;
  L7_uint32 currentIdx = 0;
  L7_uint32 i = 0;

  if ((ip == 0) ||
      (tacacsServerIndexGet(ip, &currentIdx) != L7_SUCCESS))
  {
    /* no previous server, just find the first highest priority server */
    for (i = 0; i < L7_TACACS_MAX_SERVERS; i++)
    {
      if ((tacacsCfg->servers[i].ip != 0) &&
          (tacacsCfg->servers[i].priority < priority))    
      {
        index = i;
        priority = tacacsCfg->servers[i].priority;
        found = L7_TRUE;
      }
    }
  } else
  {
    /* find the next configured server at same or lower priority,
       if priority is the same then contact that server next, 
       if priority is lower, then search the beginning of the list 
       and check priority against any lower priority we skipped before, 
       since the servers are stored in order configured, 
       assume that we have already attempted servers at beginning 
       of list with the same or higher priority than the currentIdx */
    for (i = currentIdx+1; i < L7_TACACS_MAX_SERVERS; i++)
    {
      if ((tacacsCfg->servers[i].ip != 0) &&
          (tacacsCfg->servers[i].priority >= tacacsCfg->servers[currentIdx].priority))    
      {
        if (tacacsCfg->servers[i].priority == tacacsCfg->servers[currentIdx].priority)
        {
          *serverIdx = i;
          return L7_SUCCESS;
        }
        if (tacacsCfg->servers[i].priority < priority)
        {
          index = i;
          priority = tacacsCfg->servers[i].priority;
          found = L7_TRUE;
        }
      }
    }

    for (i = 0; i < currentIdx; i++)
    {
      if ((tacacsCfg->servers[i].priority > tacacsCfg->servers[currentIdx].priority) &&
          (tacacsCfg->servers[i].priority < priority))
      {
        index = i;
        priority = tacacsCfg->servers[i].priority;
        found = L7_TRUE;
      }
    }
  }

  if (found == L7_TRUE)
  {
    *serverIdx = index;
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}


