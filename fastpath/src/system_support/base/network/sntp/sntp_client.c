/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2009
*
**********************************************************************
* @filename  sntp_client.c
*
* @purpose   SNTP Client main implementation
*
* @component SNTP
*
* @comments  none
*
* @create    20/01/09
*
* @author    kvijayan
*
* @end
*
**********************************************************************/
#include <netinet/in.h>
#include <time.h>
#include "l7_common.h"
#include "sysapi.h"
#include "osapi.h"
#include "osapi_support.h"
#include "l7_cnfgr_api.h"
#include "default_cnfgr.h"
#include "sysapi_hpc.h"
#include "sntp_api.h"
#include "sntp_client.h"
#include "sntp_cfg.h"
#include "simapi.h"
#include "l7_socket.h"
#include "dns_client_api.h"
#include "md5_api.h"
#include "dtlapi.h"

#if defined(L7_IPV6_PACKAGE)
#include "l7_ip6_api.h"
#endif
static int sntpDebug=0;

#define SNTP_MD5_DIGEST_LENGTH 16

/* Enable Debug Code */
#define SNTP_CLIENT_DEBUG 1
/* Enable UT code */
#define SNTP_CLIENT_UNIT_TEST	 1

#define SNTP_PRINTF   if (sntpDebug) sysapiPrintf

/* Task level defines */
/* Our task name */
#define L7_SNTP_TASK                     "SNTP"

/* Packet Length for broadcasting time to stack members */
#define L7_SNTP_STACK_PACKET_LEN          16

/* Our clock stratum - This is fixed. We can never be anything else. */
#define L7_SNTP_DEFAULT_STRATUM              15

/* Maximum acceptable delay - Discard packets with more than this RTT */
#define SNTP_MAX_DELAY         6

/** Maximum acceptable dispersion */
#define SNTP_MAX_DISPERSION  16

/* Packet parse/format defines */

/** NTP authentication constants */                                                                            

#define	SNTP_PKT_HEADER_LEN	        12 * sizeof(L7_uint32) /* minimum header length */
#define SNTP_PKT_AUTH_MD5_LEN	    sizeof(L7_uint32) + SNTP_MD5_DIGEST_LENGTH  /* MD5 authenticator, keyid + digest */
#define SNTP_PKT_MAX_EXT_LEN	    1024	            /* maximum NTPv4 extension field size */

/* The maximum length of a poll packet. */
#define SNTP_POLL_PACKET_LEN        SNTP_PKT_HEADER_LEN + SNTP_PKT_AUTH_MD5_LEN

/** NTP Packet offsets (from 0) */
#define SNTP_DELAY_OFFSET         4     /** Offset of delay field */
#define SNTP_DISPERSION_OFFSET    8     /** Offset of dispersion field */
#define SNTP_REFERENCE_ID_OFFSET  12    /** Offset of reference id */
#define SNTP_REFERENCE_OFFSET     16    /** Offset of reference timestamp */
#define SNTP_ORIGINATE_OFFSET     24    /** Offset of originate timestamp */
#define SNTP_RECEIVE_OFFSET       32    /** Offset of receive timestamp */
#define SNTP_TRANSMIT_OFFSET      40    /** Offset of transmit timestamp */

/** NTP packet manifest constants. */
#define SNTP_REFERENCE_ID_LEN  4        /** Length of reference id */

/** Maximum acceptable distance */
#define SNTP_MAX_DISTANCE         1

/*
 * Structure to hold interesting data in a NTP packet
 */
typedef struct sntpPacket_s
{
  L7_uchar8     li;                  /* Leap indicator */
  L7_uchar8     version;             /* NTP version */
  L7_uchar8     mode;                /* Server mode */
  L7_uchar8     stratum;             /* Claimed server stratum */
  L7_uchar8     poll;                /* Polling interval */
  L7_char8      precision;           /* Precision of server clock - Signed value. */
  L7_int32      referenceId;         /* Ref id. */
  L7_uint32     reference;           /* Reference Timestamp */
  L7_uint32     reference_fraction;  /* Fraction part of Timestamp */
  L7_uint32     originate;           /* Originate Timestamp */
  L7_uint32     originate_fraction;  /* Fraction part of Timestamp */
  L7_uint32     receive;             /* Receive Timestamp */
  L7_uint32     receive_fraction;    /* Fraction part of Timestamp */
  L7_uint32     transmit;            /* Transmit Timestamp */
  L7_uint32     transmit_fraction;   /* Fraction part of Timestamp */
  L7_uint32     current;             /* Current Timestamp */
  L7_uint32     current_fraction;    /* Fraction part of Timestamp */
  L7_uchar8    *authenticator;       /* Authenticator Information */
} sntpPacket_t;

/*
 * Structure to hold SNTP status and statistics
 */
typedef struct sntpStatusData_s
{
  /* Timestamps (in local time - UTC) of events */
  L7_uint32 sntpLastUpdateTime;  /* Last update of system clock */
  L7_uint32 sntpLastAttemptTime; /* Last SNTP request or receipt of unsolicited message */

  /* Status of the last SNTP request or unsolicited message */
  L7_SNTP_PACKET_STATUS_t sntpLastAttemptStatus;

  /* Information from the last received valid packet */
  L7_inet_addr_t sntpServerIpAddress;                /* Server IP Address */
  L7_SNTP_ADDRESS_TYPE_t sntpServerAddressType; /* Server Address Type */
  L7_uchar8 sntpServerStratum;                  /* Server Stratum */
  L7_uchar8 sntpServerVersion;                  /* Server Version */
  L7_uint32 sntpServerReferenceClockId;         /* Server Reference Clock ID */
  L7_SNTP_SERVER_MODE_t sntpServerMode;         /* Server Mode */

  /* Computational information from the last received valid packet */
  L7_int32  peerOffset;                         /* Clock offset relative to server */
  L7_uint32 peerUpdate;                         /* Packet received Time */
  
  /* Counters */
  L7_uint32 sntpBroadcastCount;  /* Unsolicited Broadcast messages processed while in Broadcast mode */
  L7_uint32 sntpMulticastCount;  /* Unsolicited Broadcast messages processed while in Multicast mode */
} sntpStatusData_t;

/*
 * Pointers to copies of the config & status data.
 * Initialized by sntpClientInit().
 */
static struct sntpCfgData_s     *sntpCfgData = L7_NULL;
static struct sntpStatusData_s  *sntpStatusData = L7_NULL;
static struct sntpServerStats_s *serverStats = L7_NULL;

/* Semaphore Lock to protect access to shared data structures. */
static void *sntpTaskSyncSemaphore = L7_NULL;


/* Task id */
static L7_int32  sntpClientTaskId = 0;

/* Active server index (0..L7_SNTP_MAX_SERVERS-1) */
static L7_int32  sntpActiveServer = NO_SERVER_SELECTED;

/** Our socket. We flag a closed socket with a -1. */
static L7_int32 ucastFd = -1;

/* This socket is defined to address SNTP over IPv6 broadcast mode.
 * Another new Socket FD(mcastFd6) is declared to handle SNTP v6 Broadcast messages 
 */
static L7_int32 mcastFd6 = -1;

/* Client local IP address. */
static L7_uint32 localAddr;

/** Current server IP address.*/ 
static L7_inet_addr_t sntpServerAddress;

/** Server address from last received packet.*/ 
static L7_inet_addr_t recvSntpServerAddrs;

/* Number of current retries per unicast server */
static L7_uint32 retryCount = 0;

/* We declare this data structure static so we don't have to fill it out
* every time we want to send a query. We just change a few fields
* just prior to transmission. TODO - We might also look at actually
* formatting a request packet and just changing the individual timestamps,
* version and poll interval directly in the packet.
*/

static sntpPacket_t sntpQuery =
{
  0,
  L7_SNTP_DEFAULT_VERSION,
  L7_SNTP_SERVER_MODE_CLIENT,
  L7_SNTP_DEFAULT_STRATUM,
  L7_SNTP_DEFAULT_POLL_INTERVAL,
  L7_SNTP_LOCAL_CLOCK_PRECISION,
  (int)0x4252434D,    /* BRCM in hex */
  0,
  0,
  0,
  0,
  0
};

/* Format timestamp data into an SNTP packet. */
#define FORMAT_TIMESTAMP(data, timestamp)                   \
{                                                           \
  L7_int32 offset;                                          \
  for (offset = 3; offset >= 0; offset--)                   \
  {                                                         \
    (data)[3-offset] = ((timestamp >> (offset*8)) & 0xff);  \
  }                                                         \
}

/* Extract timestamp from SNTP packet */
#define PARSE_TIMESTAMP(data, timestamp)                \
{                                                       \
  L7_uint32 offset;                                     \
  L7_uint32 t = 0;                                      \
  for (offset = 0; offset < 4; offset++)                \
    t = (t * 256) + (data)[offset];                     \
  timestamp = t;                                        \
}


/**********************************************************************
* @purpose  Force reselection of the currently active server (if any).
*
* @end
*********************************************************************/
void sntpActiveServerReselect()
{
  L7_uint32     cnt = 0;

  sntpActiveServer = NO_SERVER_SELECTED;

  for (cnt = 0; cnt < L7_SNTP_MAX_SERVERS; cnt++)
  {
    sntpCfgData->server[cnt].isRequestSent = L7_FALSE; 
  }
}

/**********************************************************************
* @purpose  Get the index of the currently active server, if any.
*
* @returns  index in range [0..l&_SNTP_MAX_SERVERS-1] or NO_SERVER_SELECTED
*
* @end
*********************************************************************/
L7_uint32 sntpActiveServerGet()
{
  return sntpActiveServer;
}

/**********************************************************************
* @purpose  Attempt to obtain the SNTP client mutex
*
* @returns  L7_SUCCESS if lock obtained.
* @returns  L7_FAILURE
*
* @notes    This is a blocking call.
* @notes    Caller must NOT hold the SNTP client mutex.
*
* @end
*********************************************************************/
L7_RC_t sntpTaskLock()
{
  return osapiSemaTake(sntpTaskSyncSemaphore, L7_WAIT_FOREVER);
}

/**********************************************************************
* @purpose  Release the client task mutex
*
* @returns  L7_SUCCESS if lock returned
* @returns  L7_FAILURE on failure.
*
* @notes    Caller must hold the SNTP client mutex
*
* @end
******************************************************************t***/
L7_RC_t sntpTaskUnlock()
{
  return osapiSemaGive(sntpTaskSyncSemaphore);
}

/**********************************************************************
* @purpose  Format an SNTP poll packet.
*
* @param    data @b{(input)} Pointer to the packet data to write.
* @param    length @b{(input)} Length of the packet data.
* @param    packet @b{(input)} Pointer to sntpPacket data structure
*           (where the poll fields are stored)
*
* @returns  None.
*
* @notes    We only fill in fields in the packet that are necessary
*           for SNTP polls. This is NOT a general purpose SNTP packet
*           formatter.
*
* @end
*********************************************************************/
static void sntpPacketFormat(L7_uchar8 * data, L7_int32 length,
                              sntpPacket_t * packet)
{
  memset(data, '\0', length);
  data[0] = (packet->li << 6) | (packet->version << 3) | packet->mode;
  data[1] = packet->stratum;
  data[2] = packet->poll;
  data[3] = packet->precision;

  FORMAT_TIMESTAMP(data + SNTP_ORIGINATE_OFFSET, packet->originate);
  FORMAT_TIMESTAMP(data + SNTP_RECEIVE_OFFSET, packet->originate);
  FORMAT_TIMESTAMP(data + SNTP_TRANSMIT_OFFSET, packet->transmit);
}

/**********************************************************************
* @purpose  Parse an SNTP packet and validate the incoming data.
*
* @param    data @b{(input)} Pointer to sntpPacket data structure (where the parsed
*           data is stored)
* @param    packet @b{(input)} Pointer to the packet.
* @param    length @b{(input)} Length of the packet data.
*
* @returns  sntpPacketStatus - the status of the parsed packet.
*
* @notes  The packet is completely parsed and then the error conditions
*         are applied. If this routine returns L7_SUCCESS then the
*         results may be used to supply a timestamp to the system.
*         If not, then various fields will need to be examined to
*         determine the actions to take.
*         Note that this routine only validates the incoming packet
*         - not the semantics of the packet mode, i.e. receiving
*         a broadcast packet in unicast mode or vice versa is an
*         error not caught by this routine.
*
* @end
*********************************************************************/
static L7_SNTP_PACKET_STATUS_t sntpPacketParse(sntpPacket_t * data,
                          L7_uchar8 * packet, L7_int32 length)
{

  L7_int32 i;
  L7_int32 rootDelay = 0;
  L7_int32 rootDispersion = 0;
  L7_char8 kissCode[5];

  /** It's best if this comes first -
  *  It makes our local processing time
  *  part of the RTT calculation.
  */
  data->current = (L7_uint32)osapiNTPTimeGet();
  data->li = packet[0] >> 6;
  data->version = (packet[0] >> 3) & 0x07;
  data->mode = packet[0] & 0x07;
  data->stratum = packet[1];
  data->poll = packet[2];
  data->precision = packet[3];

  for (i = 0; i < 4; i++)
  {
    data->referenceId = (data->referenceId << 8) |
          packet[SNTP_REFERENCE_ID_OFFSET + i];
  }

  /** Get the delay - note that this field is only 4 bytes and is signed */
  /* Ignore the fraction part of the delay value */
  rootDelay = (L7_int32)(packet[SNTP_DELAY_OFFSET] & 0x7f);
  rootDelay = rootDelay * 256 + packet[SNTP_DELAY_OFFSET + 1];

  if (packet[SNTP_DELAY_OFFSET] && 0x80)  /** Handle negative values */
    rootDelay = -rootDelay;

  /** Get the dispersion - note that this field is only 4 bytes and is unsigned */
  rootDispersion = (L7_int32)(packet[SNTP_DISPERSION_OFFSET] & 0x7f);
  rootDispersion = rootDispersion * 256 + packet[SNTP_DISPERSION_OFFSET+1];

  PARSE_TIMESTAMP(packet + SNTP_REFERENCE_OFFSET, data->reference);
  PARSE_TIMESTAMP(packet + SNTP_ORIGINATE_OFFSET, data->originate);
  PARSE_TIMESTAMP(packet + SNTP_RECEIVE_OFFSET, data->receive);
  PARSE_TIMESTAMP(packet + SNTP_TRANSMIT_OFFSET, data->transmit);

  if (length < SNTP_PKT_HEADER_LEN)
  {
    return L7_SNTP_STATUS_OTHER;       
  }

  /* check for message authenticator extensions, we are only
     going to support MD5 authentication for now, parse as
     indicated in drafts for NTPv4 extensions so we are 
     compatible with both NTPv3 and NTPv4 for authentication */

  data->authenticator = L7_NULLPTR;
  if ((sntpCfgData->sntpAuthMode == L7_TRUE) &&
      (length > SNTP_PKT_HEADER_LEN))
  {
    L7_int32  tmpLen = 0;
    L7_uint32 extLen = 0;
    L7_uint32 authLen = SNTP_PKT_HEADER_LEN;

    extLen = length - authLen;
    while (extLen > 0) {
      if (((extLen % 4) != 0) ||
          (extLen < SNTP_PKT_AUTH_MD5_LEN))  /* not implementing DES or autokey */
      {
        return L7_SNTP_STATUS_OTHER;
      }
      if (extLen == SNTP_PKT_AUTH_MD5_LEN) 
      {
        data->authenticator = packet + authLen;
        break;
      }
      /* extLength > SNTP_PKT_AUTH_MD5_LEN, skip over NTPv4 extensions */
      tmpLen = osapiNtohl(*(L7_uint32 *)(packet + authLen)) & 0xffff;
      if ((tmpLen <= 0) || ((tmpLen % 4) != 0) ||
          (tmpLen > SNTP_PKT_MAX_EXT_LEN)) 
      {
        return L7_SNTP_STATUS_OTHER; /* bad MAC length */
      }
      authLen += tmpLen;
      extLen  -= tmpLen;
    }
  }

  if (data->stratum > SNTP_STRATUM_MAX)     /** Bizarre? */
     return L7_SNTP_STATUS_OTHER;

  if (data->transmit < L7_SNTP_DISCARD_TIME)
    return L7_SNTP_STATUS_BAD_DATE_ENCODED;

  if (data->stratum < SNTP_STRATUM_MIN)    /** Kiss of death */
  {
      for (i = 0; i < 4; i++)
        kissCode[i] = packet[SNTP_REFERENCE_ID_OFFSET + i];
	  kissCode[4] = '\0';
      L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_SNTP_COMPONENT_ID,"sntpPacketParse: Received kiss of death packet : Stratum = %d, Kiss Code = %s\n", 
                data->stratum, kissCode);
      return L7_SNTP_STATUS_KISS_OF_DEATH;
  }


  /** This chunk of code filters out all invalid modes
   * as well as checks that the packet matches our configured mode.
   * See table in RFC 2030.
   */
  if ((sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_BROADCAST) ||
      (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_MULTICAST))
  {
    if (data->mode != L7_SNTP_SERVER_MODE_BROADCAST)
      return L7_SNTP_STATUS_OTHER;
  }
  else if (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_UNICAST)
  {
    if ((data->mode !=  L7_SNTP_SERVER_MODE_SERVER) &&
        (data->mode != L7_SNTP_SERVER_MODE_SYMMETRIC_PASSIVE))
      return L7_SNTP_STATUS_OTHER;
  }
  else
  {
    /** We're not running? Must have just changed modes... */
    return L7_SNTP_STATUS_OTHER;
  }

  /** Leap indicator of 3 means server whacked */
  if (data->li == 3)
    return L7_SNTP_STATUS_SERVER_UNSYNCHRONIZED;

  /** Invalid version                         */
  if ((data->version < SNTP_VERSION_MIN) ||
      (data->version > SNTP_VERSION_MAX))
    return L7_SNTP_STATUS_VERSION_NOT_SUPPORTED;

  /* Positive root delay within range. */
  if ((rootDelay < 0) ||
      (rootDelay > SNTP_MAX_DELAY) ||
      (rootDispersion < 0) ||
      (((rootDelay / 2) + rootDispersion) >= SNTP_MAX_DISPERSION))
    return L7_SNTP_STATUS_OTHER;

  /** Update the system status */
  sntpStatusData->sntpServerMode = data->mode;
  sntpStatusData->sntpServerReferenceClockId = 0;
  sntpStatusData->sntpServerStratum = data->stratum;
  sntpStatusData->sntpServerVersion = data->version;

  /* In Version 3 secondary servers and clients, sntpServerReferenceClockId
     * contains the 32-bit IPv4 address of the synchronization source. In Version 4 
     * secondary servers and clients, it contains the low order 32 bits (hence the index
     * in the fol loop below starts from 4) of the last transmit
     * timestamp received from the synchronization source. */
  if((sntpStatusData->sntpServerVersion == 4) &&
     (sntpStatusData->sntpServerStratum >= 2))
  {
    for (i = 4; i < 8; i++)
    {
      sntpStatusData->sntpServerReferenceClockId = (sntpStatusData->sntpServerReferenceClockId << 8) |
            packet[SNTP_TRANSMIT_OFFSET + i];
    }
  }
  else
  {
    sntpStatusData->sntpServerReferenceClockId = data->referenceId;
  }

  SNTP_PRINTF("SNTP Packet:\n");
  SNTP_PRINTF(" delay=%d\n", rootDelay);
  SNTP_PRINTF(" dispersion=%d\n", rootDispersion);
  SNTP_PRINTF(" li=%x\n", data->li);
  SNTP_PRINTF(" version=%x\n", data->version);
  SNTP_PRINTF(" mode=%x\n", data->mode);
  SNTP_PRINTF(" stratum=%d\n", data->stratum);
  SNTP_PRINTF(" poll=%d\n", data->poll);
  SNTP_PRINTF(" precision=%x\n", data->precision);
  SNTP_PRINTF(" referenceId=%d\n", data->referenceId);
  SNTP_PRINTF(" reference=%u\n", data->reference);
  SNTP_PRINTF(" originate=%u\n", data->originate);
  SNTP_PRINTF(" receive=%u\n", data->receive);
  SNTP_PRINTF(" transmit=%u\n", data->transmit);
  SNTP_PRINTF(" current=%u\n", data->current);

  return L7_SNTP_STATUS_SUCCESS;
}

/**********************************************************************
* @purpose  Record a validated timestamp and return index of record
*           just written.
*
* @param    packet @b{(input)} Pointer to sntpPacket data structure
*             (contains anything of interest)
* @param    delay @b{(input)} Pointer to place to store the calculated delay
* @param    offset @b{(input)} Pointer to place to store the local clock offset
*
* @returns  None
*
* @notes  From RFC 2030:
*
*   When the server reply is received, the client determines a
*   Destination Timestamp variable as the time of arrival according to
*   its clock in SNTP timestamp format. The following table summarizes the
*   four timestamps.
*
*      Timestamp Name          ID   When Generated
*      ------------------------------------------------------------
*      Originate Timestamp     T1   time request sent by client
*      Receive Timestamp       T2   time request received by server
*      Transmit Timestamp      T3   time reply sent by server
*      Destination Timestamp   T4   time reply received by client
*
*   The roundtrip delay d and local clock offset t are defined as
*
*      d = (T4 - T1) - (T2 - T3)     t = ((T2 - T1) + (T3 - T4)) / 2.
*
* @end
*********************************************************************/
static void sntpRTTCalculate(sntpPacket_t * packet, L7_int32 * delay,
                                        L7_int32 * offset)
{
  L7_uint32 t1 = packet->originate;
  L7_uint32 t2 = packet->receive;
  L7_uint32 t3 = packet->transmit;
  L7_uint32 t4 = packet->current;

  *delay = 0;
  if ((sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_BROADCAST) ||
      (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_MULTICAST))
  {
    if(t3 > t4)
      *offset = (t3 - t4);
    else
      *offset = -1 * (t4 - t3);
    SNTP_PRINTF("sntpRTTCalculate: broadcast/multicast mode - delay %d offset %d\n",
                    *delay, *offset);
  }
  else if (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_UNICAST)
  {
    L7_int32 a, b, asign, bsign;
    /* a = diff(t2,t1), asign = cmp(t2,t1), b = diff(t3,t4), bsign = cmp(t3,t4) */
    if(t2 > t1)
    {
      a = t2 - t1;
      asign = 1;
    }
    else
    {
      a = t1 - t2;
      asign = -1;
    }
    if(t3 > t4)
    {
      b = t3 - t4;
      bsign = 1;
    }
    else
    {
      b = t4 - t3;
      bsign = -1;
    }
    /* Have to divide these components before adding them to avoid
     * overflow in signed integer */    
    *offset = (asign * a)/2 + (bsign * b)/2 + ((asign * (a%2)) + (bsign * (b%2)))/2;
    SNTP_PRINTF("sntpRTTCalculate: unicast mode - delay %d offset %d\n",
                    *delay, *offset);
  }
}

/**
* SECTION: Socket Handling
*
* This section contains all of the UDP socket manipulation
* routines, i.e. open, close, address resolution, server
* selection and multicast group registration.
*
*
*/

/**********************************************************************
* @purpose  This routine closes the local socket.
*
* @returns  L7_SUCCESS
*
* @notes    If the socket is already closed, this routine doesn't do much.
*           The "global" file descriptor is set to -1.
*           The remote server address is also cleared.
*
*
* @end
*********************************************************************/
L7_RC_t sntpLocalSocketClose()
{
  struct L7_ip_mreq_s  ipMreq;
  localAddr = L7_INADDR_ANY;

  if (ucastFd >= 0)
  {
    /* Close out all multicast/broadcast associations */
    /** fill in the argument structure to leave the multicast group */
    /** initialize the multicast address to leave */
    ipMreq.imr_multiaddr.s_addr = osapiHtonl(osapiInet_addr(SNTP_MULTICAST_GROUP_ADDRESS));
    /** unicast interface addr from which to receive the multicast packets */
    ipMreq.imr_interface.s_addr = osapiHtonl(localAddr);
    /** set the socket option to join the MULTICAST group */
    (void)osapiSetsockopt (ucastFd, IPPROTO_IP, L7_IP_DROP_MEMBERSHIP, (char *)&ipMreq, sizeof(ipMreq));

    osapiShutdown(ucastFd, L7_SHUT_RDWR); /** Mark it for complete closure */
    osapiSocketClose(ucastFd);
    ucastFd = -1;

    memset(&sntpServerAddress,0x00,sizeof(sntpServerAddress));
    memset(&recvSntpServerAddrs,0x00,sizeof(recvSntpServerAddrs));
    SNTP_PRINTF("socket close - socket %d\n", ucastFd);
  }
  if (mcastFd6 >= 0)
  {
    osapiShutdown(mcastFd6, L7_SHUT_RDWR); /** Mark it for complete closure */
    osapiSocketClose(mcastFd6);
    mcastFd6 = -1;
    SNTP_PRINTF("socket close - socket %d\n", mcastFd6);
  }
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Resolve an address into it's internal representation.
*
* @param    atype @b{(input)} address type (unknown, IPv4, DNS,IPv6)
* @param    address @b{(input)} address string to resolve
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes    Returns 0 if unable to resolve address.
*           Potentially invokes DNS to resolve address
*
* @end
*********************************************************************/

static L7_RC_t sntpResolveAddress(L7_SNTP_ADDRESS_TYPE_t atype,
                                  L7_char8 * address)
{
  L7_char8 hostname[DNS_DOMAIN_NAME_SIZE_MAX];
  dnsClientLookupStatus_t  status;
  L7_uint32 ipAddr =0;
  L7_inet_addr_t destIp[2];

  /* Return Failure if the address or serverAddr pointers are null */
  if(address == L7_NULL)
    return L7_FAILURE;

  memset(&sntpServerAddress,0x00,sizeof(sntpServerAddress));
  memset(&destIp[0],0x00,sizeof(L7_inet_addr_t));
  memset(&destIp[1],0x00,sizeof(L7_inet_addr_t));

  if (atype == L7_SNTP_ADDRESS_DNS)
  {
    /* First try to resolve for a IPv4 address and if that fails then proceed further
     * with a IPv6 DNS resolution. Log an error if both of the methods fails
     */

    if (dnsClientInetNameLookup(L7_AF_INET,address,&status,hostname,destIp) != L7_SUCCESS)
    {
      #if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
      memset(&destIp,0x00,sizeof(L7_inet_addr_t));
      if (dnsClientInetNameLookup(L7_AF_INET6,address,&status,hostname,destIp) == L7_SUCCESS)
      {
        sntpServerAddress.family = L7_AF_INET6;
        memcpy(&(sntpServerAddress.addr.ipv6),&(destIp[0].addr.ipv6),sizeof(destIp[0].addr.ipv6));
      }
      else
      #endif
      {
        SNTP_PRINTF("Failed to Resolve DNS address for Server %s status:%d\r\n",\
                    address,status);
        L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_SNTP_COMPONENT_ID,"Failed to Resolve DNS address for Server %s \r\n",address);
        return L7_FAILURE; 
      }
    }
    else
    {
       sntpServerAddress.family = L7_AF_INET;
       inetAddressGet(L7_AF_INET, &destIp[0], &ipAddr); 
       sntpServerAddress.addr.ipv4.s_addr = ipAddr; 
    }
  }
  else if (atype == L7_SNTP_ADDRESS_IPV4)
  {
    sntpServerAddress.family = L7_AF_INET;
    osapiInetPton(L7_AF_INET,address,(L7_uchar8 *)&(destIp[0].addr.ipv4.s_addr));
    inetAddressGet(L7_AF_INET, &destIp[0], &ipAddr); 
    sntpServerAddress.addr.ipv4.s_addr = ipAddr; 
  }
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  else if(atype == L7_SNTP_ADDRESS_IPV6)
  {
    sntpServerAddress.family = L7_AF_INET6;
    osapiInetPton(L7_AF_INET6,address,(L7_uchar8 *)&(sntpServerAddress.addr.ipv6));
  }
#endif
  else
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  This routine opens a Unicast Local Socket and binds to it.
*
* @param    none
* @returns  Status - L7_ERROR if socket could not be opened.
*                   L7_SUCCESS if socket was successfully opened.
*
* @notes    If the socket is already open, this routine will close
*           it first.
*
*
* @end
*********************************************************************/
static L7_RC_t sntpLocalUcastSocketOpen()
{
  L7_sockaddr_union_t baddr;
  L7_uint32 domain;
  L7_uint32 off = 0;

  localAddr = L7_INADDR_ANY;
  /* Reset the baddr union */
  memset(&baddr,0x00,sizeof(baddr));  

  /* Resolve the new IP Address(IPv4 or IPv6) */ 
  if(sntpResolveAddress(sntpCfgData->server[sntpActiveServer].addressType,
                       sntpCfgData->server[sntpActiveServer].address) != L7_SUCCESS)
  {
      SNTP_PRINTF("Address Resolution Failed for ActiveServer %d\n", sntpActiveServer);
      return L7_FAILURE;
  }  
  domain = sntpServerAddress.family;
  
  /** Open a socket and begin querying the server. */
  if (osapiSocketCreate(domain, L7_SOCK_DGRAM, 0, &ucastFd) != L7_SUCCESS)
  {
    SNTP_PRINTF("socket open failed - errno %d\n", osapiErrnoGet());
    return L7_ERROR;
  }
  SNTP_PRINTF("socket open - socket %d\n", ucastFd);
  if(domain == L7_AF_INET)
  {
    /** Bind to local port */
    baddr.u.sa4.sin_family = L7_AF_INET;
    baddr.u.sa4.sin_addr.s_addr = osapiHtonl(localAddr);

    /** Unicast mode, bind to local unicast address */
    baddr.u.sa4.sin_port = L7_SNTP_DEFAULT_UNICAST_CLIENT_PORT;
    if (osapiSocketBind(ucastFd, (L7_sockaddr_t *)&(baddr.u.sa4),sizeof(baddr.u.sa4)) != L7_SUCCESS)
    {
      SNTP_PRINTF("Can't bind socket to local address 0x%lx port %d - errno %d\n",
                   localAddr, sntpCfgData->sntpClientPort, osapiErrnoGet());
      sntpLocalSocketClose();
      return L7_ERROR;
    }
    /* Do  a set sock opt only for unicast socket */
    if (osapiSetsockopt(ucastFd, L7_SOL_SOCKET, L7_SO_BROADCAST,(L7_char8 *) &off, sizeof(off))== L7_FAILURE)
    {
      L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_SNTP_COMPONENT_ID,"sntpLocalSocketOpen: osapiSetsockopt SO_BROADCAST off failed");
      SNTP_PRINTF("sntpLocalSocketOpen: osapiSetsockopt SO_BROADCAST off failed-errno %d\n", osapiErrnoGet());
      sntpLocalSocketClose();
      return L7_ERROR;
    }
  }
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  else if (domain == L7_AF_INET6)
  {
    /* bind IN6ADDR_ANY */
    memset(&baddr.u.sa6, 0, sizeof(baddr.u.sa6));
    baddr.u.sa6.sin6_family = L7_AF_INET6;
    /** Unicast mode, bind to local unicast address */
    baddr.u.sa6.sin6_port = L7_SNTP_DEFAULT_UNICAST_CLIENT_PORT;
    
    if (osapiSocketBind(ucastFd, (L7_sockaddr_t *)&(baddr.u.sa6),sizeof(baddr.u.sa6)) != L7_SUCCESS)
    {
       SNTP_PRINTF("Can't bind IPv6 socket to local address %d port %d - errno %d\n",
           baddr.u.sa6.sin6_addr.in6.addr8, sntpCfgData->sntpClientPort, osapiErrnoGet());
       sntpLocalSocketClose();
       return L7_ERROR;
    }
  }    
#endif
  else
  {
    SNTP_PRINTF("\r\nInvalid Family in sntpLocalUcastSocketOpen");
    return L7_ERROR;
  }
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  This routine opens multicast Local Socket and binds to it.
*
* @param    none
* @returns  Status - L7_ERROR if socket could not be opened.
*                   L7_SUCCESS if socket was successfully opened.
*
* @notes    If the socket is already open, this routine will close
*           it first.
*
*
* @end
*********************************************************************/
static L7_RC_t sntpLocalMcastSocketOpen()
{
  L7_uint32 on = 1;
  L7_sockaddr_union_t baddr;
  localAddr = L7_INADDR_ANY;

  struct L7_ip_mreq_s  ipMreq;
     
  /** Open a socket and begin querying the server. */
  if (osapiSocketCreate(L7_AF_INET, L7_SOCK_DGRAM, 0, &ucastFd) != L7_SUCCESS)
  {
    ucastFd = -1;
    SNTP_PRINTF("socket open failed - errno %d\n", osapiErrnoGet());
    return L7_ERROR;
  }
  SNTP_PRINTF("socket open - socket %d\n", ucastFd);
     
  if (osapiSetsockopt(ucastFd, L7_SOL_SOCKET, L7_SO_BROADCAST,(L7_char8 *) &on, sizeof(on))!= L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_SNTP_COMPONENT_ID,"sntpLocalSocketOpen: osapiSetsockopt SO_BROADCAST on failed");
    return L7_ERROR;
  }
  /** fill in the argument structure to join the multicast group */
  /** initialize the multicast address to join */
  ipMreq.imr_multiaddr.s_addr = osapiHtonl(osapiInet_addr(SNTP_MULTICAST_GROUP_ADDRESS));
  
  /** unicast interface addr from which to receive the multicast packets */
  ipMreq.imr_interface.s_addr = osapiHtonl(localAddr);
  
  /** set the socket option to join the MULTICAST group */
  if (osapiSetsockopt (ucastFd, IPPROTO_IP, L7_IP_ADD_MEMBERSHIP,
                       (char *)&ipMreq, sizeof(ipMreq)) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_SNTP_COMPONENT_ID,\
        "SNTP: Can't join multicast group - errno %d\n", \
              osapiErrnoGet());
    return L7_ERROR;
  }
  
  /** Unicast mode, bind to local unicast address */
  baddr.u.sa4.sin_port = L7_SNTP_DEFAULT_UNICAST_CLIENT_PORT;
  
  if (osapiSocketBind(ucastFd, (L7_sockaddr_t *)&(baddr.u.sa4),sizeof(baddr.u.sa4)) != L7_SUCCESS)
  {
     SNTP_PRINTF("Can't bind socket to local address 0x%lx port %d - errno %d\n",
         localAddr, sntpCfgData->sntpClientPort, osapiErrnoGet());
     return L7_ERROR;
  }
  return L7_SUCCESS;
}

#if defined(L7_IPV6_MGMT_PACKAGE) || defined(L7_IPV6_PACKAGE)
/**********************************************************************
* @purpose Group Membership Registration for IPv6 broadcast mode for
*          SNTP client on network port and service port if present.
*
* @param none
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
static void sntpIPv6GroupMembershipMgmtPortAdd()
{
  struct L7_ip_mreq6_s mreq6;
  L7_int32 sockopt;
  L7_in6_addr_t mcastAddress;

#if defined (L7_IPV6_PACKAGE) 
  L7_uint32 intIfNum;
  L7_uint32 mode;
  L7_RC_t rc = L7_FAILURE;
#endif    
 
  osapiInetPton(L7_AF_INET6, SNTP_IPV6_LINK_LOCAL_SCOPED_ADDRESS, (L7_uchar8 *)&(mcastAddress)); 
  memcpy(&mreq6.imr6_multiaddr, &mcastAddress, L7_IP6_ADDR_LEN);
  mreq6.imr6_intIfNum = 0;

  sockopt = L7_IPV6_ADD_MEMBERSHIP;
  memset(mreq6.imr6_ifName, 0, sizeof(mreq6.imr6_ifName));

#if SERVICE_PORT_PRESENT
  /* Join the ipv6 sntp multicast link-local address "ff02::101" on service port */
  osapiSnprintf(mreq6.imr6_ifName, sizeof(mreq6.imr6_ifName), "%s%d", bspapiServicePortNameGet(), bspapiServicePortUnitGet());
  osapiSetsockopt(mcastFd6, IPPROTO_IPV6, sockopt,
                  (L7_uchar8 *) &mreq6, sizeof(struct L7_ip_mreq6_s));
#endif

  memset(mreq6.imr6_ifName, 0, sizeof(mreq6.imr6_ifName));
  /* Join the ipv6 sntp multicast link-local address "ff02::101" on network port */
  osapiSnprintf(mreq6.imr6_ifName, sizeof(mreq6.imr6_ifName), "%s0", L7_DTL_PORT_IF);
  if (osapiSetsockopt(mcastFd6, IPPROTO_IPV6, sockopt, (L7_uchar8 *) &mreq6, sizeof(struct L7_ip_mreq6_s)) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_SNTP_COMPONENT_ID, "osapiSetsockopt failed");
  }

 /* Also Join on enabled routing interfaces the user could have changed the mode
  * from Unicast to Multicast 
  */
#if defined (L7_IPV6_PACKAGE) 
  rc = nimFirstValidIntfNumber(&intIfNum);
  while (rc == L7_SUCCESS)
  {
    if( (ip6MapRtrIntfOperModeGet(intIfNum, &mode) == L7_SUCCESS) && (mode == L7_ENABLE))
    {
      sntpGroupMembershipAdd(intIfNum, L7_TRUE);
    }
    rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
  }
#endif    
   
}
#endif

/**********************************************************************
* @purpose  This routine opens a broadcast local socket and binds to it.
*
* @param    none
* @returns  Status - L7_ERROR if socket could not be opened.
*                   L7_SUCCESS if socket was successfully opened.
*
* @notes    If the socket is already open, this routine will close
*           it first.
*
*
* @end
*********************************************************************/
static L7_RC_t sntpLocalBcastSocketOpen()
{
  L7_uint32 on = 1;
  L7_sockaddr_union_t baddr;
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  L7_sockaddr_union_t baddr6;
  L7_uint32      opt = 1;
#endif

  localAddr = L7_INADDR_ANY;

  /** Open a socket and begin querying the server. */
  if (osapiSocketCreate(L7_AF_INET, L7_SOCK_DGRAM, 0, &ucastFd) != L7_SUCCESS)
  {
    SNTP_PRINTF("socket open failed - errno %d\n", osapiErrnoGet());
    return L7_ERROR;
  }
  SNTP_PRINTF("socket open - socket %d\n", ucastFd);
   
  /** Broadcast mode, bind to local broadcast address */
  baddr.u.sa4.sin_family = L7_AF_INET;
  baddr.u.sa4.sin_addr.s_addr = osapiHtonl(localAddr);
  baddr.u.sa4.sin_port = osapiHtons(sntpCfgData->sntpClientPort);
  if (osapiSocketBind(ucastFd, (L7_sockaddr_t *)&(baddr.u.sa4),sizeof(baddr.u.sa4)) != L7_SUCCESS)
  {
     SNTP_PRINTF("Can't bind socket to local address 0x%lx port %d - errno %d\n",
        localAddr, sntpCfgData->sntpClientPort, osapiErrnoGet());
     sntpLocalSocketClose();
     return L7_ERROR;
  }
  if (osapiSetsockopt(ucastFd, L7_SOL_SOCKET, L7_SO_BROADCAST,(L7_char8 *) &on, sizeof(on))== L7_FAILURE)
  {
    SNTP_PRINTF("\r\n sntpLocalBcastSocketOpen: osapiSetsockopt SO_BROADCAST on failed for IPv4 socket -errno %d\r\n", osapiErrnoGet()); 
    L7_LOGF(L7_LOG_SEVERITY_WARNING,L7_SNTP_COMPONENT_ID,"sntpLocalBcastSocketOpen: osapiSetsockopt SO_BROADCAST on failed");
    sntpLocalSocketClose();
    return L7_ERROR;
  }

  if (osapiSetsockopt(ucastFd, L7_SOL_SOCKET, L7_SO_REUSEADDR,
            (L7_uchar8 *)&on, sizeof(on)) != L7_SUCCESS)
  {
     SNTP_PRINTF("\r\n osapiSetsockopt with SO_REUSEADDR failed for IPv4 socket errorno %d\n", osapiErrnoGet());
     L7_LOGF( L7_LOG_SEVERITY_WARNING,L7_SNTP_COMPONENT_ID, \
             "osapiSetsockopt with SO_REUSEADDR failed errorno %d\n", osapiErrnoGet());
     sntpLocalSocketClose();
     return L7_ERROR;
  }
 
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  /** Open a IPv6 socket and start listening to the server. */
  if (osapiSocketCreate (L7_AF_INET6, L7_SOCK_DGRAM, 0, &mcastFd6) != L7_SUCCESS)
  {
    SNTP_PRINTF("socket open failed - errno %d\n", osapiErrnoGet());
    return L7_ERROR;
  }

  if (osapiSetsockopt(mcastFd6, L7_SOL_SOCKET, L7_SO_REUSEADDR,
            (L7_uchar8 *)&opt, sizeof(opt)) != L7_SUCCESS)
  {
    SNTP_PRINTF("\r\n sntpLocalBcastSocketOpen(): osapiSetsockopt SO_REUSEADDR failed for IPv6 socket errorno %d\r\n", osapiErrnoGet()); 
    L7_LOGF( L7_LOG_SEVERITY_WARNING,L7_SNTP_COMPONENT_ID, \
             "sntpLocalBcastSocketOpen(): osapiSetsockopt SO_REUSEADDR failed errorno %d\n",
                  osapiErrnoGet());
     sntpLocalSocketClose();
     return L7_ERROR;
  }
   
  /* bind IN6ADDR_ANY */
  memset(&baddr6.u.sa6, 0, sizeof(baddr6.u.sa6));
  baddr6.u.sa6.sin6_family = L7_AF_INET6;
  
  /** Broadcast mode, bind to local broadcast address */
  baddr6.u.sa6.sin6_port = osapiHtons(sntpCfgData->sntpClientPort);
  if (osapiSocketBind(mcastFd6, (L7_sockaddr_t *)&(baddr6.u.sa6), sizeof(baddr6.u.sa6)) != L7_SUCCESS)
  {
     SNTP_PRINTF("Can't bind IPv6 socket to IPv6 address %d port %d - errno %d\n",
         baddr6.u.sa6.sin6_addr.in6.addr8, sntpCfgData->sntpClientPort, osapiErrnoGet());
     sntpLocalSocketClose();
     return L7_ERROR;
  }
  SNTP_PRINTF("socket open for IPv6 - socket %d\n", mcastFd6);

  sntpIPv6GroupMembershipMgmtPortAdd();
#endif
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  This routine opens (or reopens) the local socket and binds to it.
*
* @param    domain  @b{(input)} address type (unknown, IPv4, DNS,IPv6)
* @returns  Status - L7_ERROR if socket could not be opened.
*                   L7_SUCCESS if socket was successfully opened.
*
* @notes    If the socket is already open, this routine will close
*           it first.
*
*
* @end
*********************************************************************/
static L7_RC_t sntpLocalSocketOpen()
{
  L7_RC_t rc;

  sntpLocalSocketClose();

  /** Don't open socket until we're enabled. */
  if (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_DISABLED)
    return L7_SUCCESS;

  if (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_UNICAST)
  {
     rc = sntpLocalUcastSocketOpen();
  }
  else if (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_MULTICAST)
  {
     rc = sntpLocalMcastSocketOpen();
  }
  else if (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_BROADCAST)
  {
     rc = sntpLocalBcastSocketOpen();
  } 
  else
  {
    SNTP_PRINTF("SNTP: Inconsistent mode\n");
    sntpLocalSocketClose();
    return L7_ERROR;
  }
  return rc;
}

/**********************************************************************
* @purpose  Drain the receive socket as efficiently as possible
*
* @returns  None
*
* @notes    Checks the file descriptor and returns if invalid.
*           This routine is intended to be non-blocking,
*           although it make take a while if we're being plastered
*           with packets.
*           This routine depends on the behavior of a UDP socket
*           that only reading the first byte discards the rest
*           of the buffer.
*
*           Caller must hold sntp lock.
*
*
* @end
*********************************************************************/
static void sntpFlushReceiveBuffer()
{
  L7_uint32 numReads = 0, maxFd;
  L7_int32 rc, rc1, rc2;
  L7_char8 buf;
  fd_set readFds;
  L7_int32 rcvLen;
  L7_sockaddr_union_t raddr;
  L7_uint32 from_len = sizeof(raddr);

    /** File descriptor is valid? */
  while (numReads++ < 64)
  {
    FD_ZERO(&readFds);
    if (ucastFd < 0)
      return;
    FD_SET(ucastFd, &readFds);
    maxFd = ucastFd + 1;
    if ((sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_BROADCAST) && (mcastFd6 > 0))
    {
      FD_SET(mcastFd6, &readFds);
      maxFd = ((ucastFd > mcastFd6) ? (ucastFd + 1) : (mcastFd6 + 1));
    }
    /* 0 timeout is infinite wait */
    rc = osapiSelect(maxFd, &readFds, L7_NULL, L7_NULL, 0, 1);
    if (rc <= 0)
      return;
    if (ucastFd < 0)
      return;
    /** We just grab the first byte - the socket will discard the
    * rest of the datagram.
    */
    /* Reset the raddr union */
    memset(&raddr,0x00,sizeof(raddr));
    rc1 = L7_FAILURE;
    if (FD_ISSET(ucastFd,&readFds))
    {
      rc1 = osapiSocketRecvfrom(ucastFd, &buf, 1, 0, (L7_sockaddr_t *)&raddr, &from_len, &rcvLen);
    }

    rc2 = L7_FAILURE;
    memset(&raddr,0x00,sizeof(raddr));
    if((sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_BROADCAST) && (mcastFd6 > 0))
    {
      if(FD_ISSET(mcastFd6, &readFds))
      {
        rc2 = osapiSocketRecvfrom(mcastFd6, &buf, 1, 0, (L7_sockaddr_t *)&raddr, &from_len, &rcvLen);
      }
    }
    if ((rc1 != L7_SUCCESS) && (rc2 != L7_SUCCESS))
    {
      return;
    }
    SNTP_PRINTF("sntpFlushReceiveBuffer: flush %d rc(%d)\n", numReads, rc);
  }
  L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_SNTP_COMPONENT_ID,\
          "sntpFlushReceiveBuffer: flushed %d messages. Possible attack in progress!\n", \
          numReads);

}

/**********************************************************************
* @purpose  Choose the next available unicast server based on priority.
*
* @returns  none
*
* @notes    Always clears sntpServerAddress. Sets the active server 
*           if succesful.
*
*
* @end
*********************************************************************/
static void unicastServerSelect()
{
  register L7_uint32 cnt = 0;
  register L7_uint32 sNdx;
  L7_uint32 priority;
  L7_char8 address_dummy[L7_SNTP_MAX_ADDRESS_LEN];

  memset(&sntpServerAddress,0x00,sizeof(sntpServerAddress));
  
  SNTP_PRINTF("unicastServerSelect: server count - %d\n", sntpCfgData->sntpUnicastServerCount);

  if (sntpCfgData->sntpUnicastServerCount < 1)
    return;

  memset((void*)address_dummy, L7_NULL, sizeof(address_dummy));

  if (sntpActiveServer != NO_SERVER_SELECTED)
  {
    /** Move down the list from where we are. */
    priority = sntpCfgData->server[sntpActiveServer].priority;
    sNdx = (L7_uint32)sntpActiveServer;
  }
  else
  {
    /** Start at front of list. */
    priority = SNTP_MIN_SERVER_PRIORITY;
    sNdx= L7_NULL;
  }

  while (priority <= SNTP_MAX_SERVER_PRIORITY)
  {
    SNTP_PRINTF("unicastServerSelect: priority - %d\n", priority);
    /** Iterate up to L7_SNTP_MAX_SERVERS number of times */
    for (cnt = 0; cnt < L7_SNTP_MAX_SERVERS; cnt++)
    {
      /** Choose the next server in the list */
      if (++sNdx >= L7_SNTP_MAX_SERVERS)
        sNdx = 0;
      /** Valid server entry? We are looking for
      * a different server than the active one.
      */
      if ((sntpCfgData->server[sNdx].status == L7_SNTP_SERVER_STATUS_ACTIVE) &&
          (serverStats[sNdx].lastAttemptStatus != L7_SNTP_STATUS_KISS_OF_DEATH) &&
          (sNdx != sntpActiveServer) &&
          (sntpCfgData->server[sNdx].priority == priority) &&
          (sntpCfgData->server[sNdx].poll == L7_ENABLE) &&
          (sntpCfgData->server[sNdx].isRequestSent != L7_TRUE) &&
          (memcmp((void *)sntpCfgData->server[sNdx].address, (void *)address_dummy, L7_SNTP_MAX_ADDRESS_LEN) != 0) &&
          (sntpCfgData->server[sNdx].addressType != L7_SNTP_ADDRESS_UNKNOWN) &&
          (sntpCfgData->server[sNdx].port))
      {
        /** specify address and port */
        sntpActiveServer = sNdx;
        sntpCfgData->server[sNdx].isRequestSent = L7_TRUE;
        SNTP_PRINTF("sntpServerSelect: selected server %s with priority %d at index %d\n",
        sntpCfgData->server[sNdx].address,
        sntpCfgData->server[sNdx].priority, sNdx);
        retryCount = 0;
        return;
      }
    }

    /* reset the 'isRequestSent' for servers with this priority */
    for (cnt = 0; cnt < L7_SNTP_MAX_SERVERS; cnt++)
    {
      if (sntpCfgData->server[cnt].priority == priority)
      {
        sntpCfgData->server[cnt].isRequestSent = L7_FALSE; 
      }
    }    
    priority++;
  }
  /** If we get here, we didn't find one. (And we started at MIN_PRIORITY.) */
  if (sntpActiveServer == NO_SERVER_SELECTED)
    return; /** Not going to find one either. */

  /** Loop through again using MIN_PRIORITY */
  priority = SNTP_MIN_SERVER_PRIORITY;
/* Start iteration from server with index 0 */
  sNdx = NO_SERVER_SELECTED;
  while (priority <= sntpCfgData->server[sntpActiveServer].priority)
  {
    /** Iterate up to L7_SNTP_MAX_SERVERS number of times */
    for (cnt = 0; cnt < L7_SNTP_MAX_SERVERS; cnt++)
    {
      /** Choose the next server in the list */
      if (++sNdx >= L7_SNTP_MAX_SERVERS)
        sNdx = 0;
      /** Valid server entry? We will take any valid server,
      * including the active one.
      */
      if ((sntpCfgData->server[sNdx].status == L7_SNTP_SERVER_STATUS_ACTIVE) &&
          (serverStats[sNdx].lastAttemptStatus != L7_SNTP_STATUS_KISS_OF_DEATH) &&
          (sntpCfgData->server[sNdx].priority == priority) &&
          (sntpCfgData->server[sNdx].poll == L7_ENABLE) &&
          (sntpCfgData->server[sNdx].isRequestSent != L7_TRUE) &&
          (memcmp((void *)sntpCfgData->server[sNdx].address, (void *)address_dummy, L7_SNTP_MAX_ADDRESS_LEN) != 0) &&
          (sntpCfgData->server[sNdx].addressType != L7_SNTP_ADDRESS_UNKNOWN) &&
          (sntpCfgData->server[sNdx].port))
      {
        /** specify address and port */
        sntpActiveServer = sNdx;
        sntpCfgData->server[sNdx].isRequestSent = L7_TRUE;
        SNTP_PRINTF("sntpServerSelect: selected server %s "
                    "with priority %d at index %d\n",
        sntpCfgData->server[sNdx].address,
        sntpCfgData->server[sNdx].priority, sNdx);
        retryCount = 0;
        return;
      }
    }

    /* reset the 'isRequestSent' for servers with this priority */
    for (cnt = 0; cnt < L7_SNTP_MAX_SERVERS; cnt++)
    {
      if (sntpCfgData->server[cnt].priority == priority)
      {
        sntpCfgData->server[cnt].isRequestSent = L7_FALSE; 
      }
    }    
    priority++;
  }
  /** I really don't need to do this,
  * but it makes me feel a little more secure.
  */
  sntpActiveServer = NO_SERVER_SELECTED;
  SNTP_PRINTF("sntpServerSelect: no server selected\n");
}

/* routines for MD5 authentication fields */

/**********************************************************************
* @purpose  Create authenticator digest for SNTP packet
*
* @param    L7_uchar8  *key       @((input))  key value
* @param    L7_uchar8  *packet    @((input))  packet to authenticate
* @param    L7_uint32   length    @((input))  packet length - authenticate fields
* @param    L7_uchar8  *digest    @((output)) MD5 authentication digest
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
static void sntpAuthDigest(L7_char8  *key,
                           L7_uchar8 *packet,
                           L7_uint32  length,
                           L7_uchar8 *digest)
{
  L7_MD5_CTX_t   md5;

  /* MD5 of key concatenated with packet */
  l7_md5_init(&md5);
  l7_md5_update(&md5, key, strlen(key));
  l7_md5_update(&md5, packet, length);
  l7_md5_final(digest, &md5);

  return;
}

/**********************************************************************
* @purpose  Verify authenticator digest for SNTP packet
*
* @param    L7_uint32   keyid     @((input))  key identifier
* @param    L7_uchar8  *packet    @((input))  packet to authenticate
* @param    L7_uint32   length    @((input))  packet length - authenticate fields
* @param    L7_uchar8  *digest    @((input))  MD5 authentication digest
*
* @returns  L7_SUCCESS, packet authenticated
* @returns  L7_FAILURE, authentication failed
*
* @notes  task semaphore must be held for this call
*
* @end
*********************************************************************/
static L7_RC_t sntpAuthVerify(L7_uint32  keyid,
                              L7_uchar8 *packet,
                              L7_uint32  length,
                              L7_uchar8 *digest)
{
  L7_uchar8 buffer[SNTP_MD5_DIGEST_LENGTH];
  L7_uint32 i = 0;

  memset(buffer, 0, sizeof(buffer));

  /* find the configured key */
  for (i=0; i < L7_SNTP_MAX_AUTH_KEYS; i++)
  {
    if ((sntpCfgData->sntpKeys[i].status == L7_SNTP_SERVER_STATUS_ACTIVE) &&
        (sntpCfgData->sntpKeys[i].id == keyid))
    {
      break;
    }
  }
  if (i == L7_SNTP_MAX_AUTH_KEYS)
  {
    /* did not find a matching key */
    return L7_FAILURE;
  }

  /* make sure key is trusted or configured for this unicast server */
  if ((sntpCfgData->sntpKeys[i].trusted == L7_TRUE) ||
      ((sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_UNICAST) &&
       (sntpCfgData->server[sntpActiveServer].keyIndex == (i+1))))
  {
    sntpAuthDigest(sntpCfgData->sntpKeys[i].value, packet, length, buffer);

    if (memcmp(buffer, digest, SNTP_MD5_DIGEST_LENGTH) == 0)
    {
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}



/**
* SECTION: Packet handlers - routines for sending and receiving SNTP packets
*
*/

/**********************************************************************
* @purpose  Send a poll packet to the server as specified by the
*           sntpActiveServer index using the current selected server
*           (one must be selected) and the unicast poll interval.
*           Updates last attempt time. Simply returns without sending
*           anything if the sntpActiveServer index is invalid.
*           Fails if socket specified by ucastFd is not open.
*
* @param    error number
*
* @returns  none
*
* @notes    Must be called with client lock held. Accesses sntpActiveServer.
*
* @end
*********************************************************************/

static L7_RC_t  sntpPollSend()
{
  L7_uint32 bytesSent;
  L7_char8  outgoingPacket[SNTP_POLL_PACKET_LEN];
  L7_sockaddr_union_t saddr;
  L7_uint32 length = SNTP_PKT_HEADER_LEN;

  /** Don't bother with bad addresses.
  * If ucastFd == -1, we will just fail - that's ok
  */
  if (sntpActiveServer == NO_SERVER_SELECTED)
    return L7_SUCCESS;

   /* Resolve the new IP Address(IPv4 or IPv6) */ 
  if(sntpResolveAddress(sntpCfgData->server[sntpActiveServer].addressType,
                       sntpCfgData->server[sntpActiveServer].address) != L7_SUCCESS)
  {
      SNTP_PRINTF("Address Resolution Failed for ActiveServer %d\n", sntpActiveServer);
      return L7_FAILURE;
  }  
  /* Reset the saddr union */
  memset(&saddr,0x00,sizeof(saddr)); 
  /** Set the transmit timestamp so that we can estimate RTT. (Recommended) */
  sntpQuery.current = sntpQuery.transmit = (L7_uint32)osapiNTPTimeGet();
  sntpQuery.version = sntpCfgData->server[sntpActiveServer].version;
  sntpQuery.poll = sntpCfgData->sntpUnicastPollInterval;
  sntpPacketFormat(outgoingPacket, SNTP_POLL_PACKET_LEN, &sntpQuery);

  if ((sntpCfgData->sntpAuthMode == L7_TRUE) &&
      (sntpCfgData->server[sntpActiveServer].keyIndex != SNTP_AUTH_KEY_NOT_CONFIGURED))
  {
    /* authentication enabled, add key identifier and digest to packet */
    L7_uchar8 *p = outgoingPacket + SNTP_PKT_HEADER_LEN;
    sntpKeyCfgData_t *keyData = &sntpCfgData->sntpKeys[sntpCfgData->server[sntpActiveServer].keyIndex-1];

    *(L7_uint32 *)p = osapiHtonl(keyData->id);
    p += sizeof(L7_uint32);
    sntpAuthDigest(keyData->value, outgoingPacket, SNTP_PKT_HEADER_LEN, p); 
    length = SNTP_POLL_PACKET_LEN;
  }
  /** Send message */

  if(sntpServerAddress.family == L7_AF_INET)
  { 
    saddr.u.sa4.sin_family = L7_AF_INET;
    saddr.u.sa4.sin_port = osapiHtons( sntpCfgData->server[sntpActiveServer].port);
    saddr.u.sa4.sin_addr.s_addr = osapiHtonl(sntpServerAddress.addr.ipv4.s_addr);

    if (osapiSocketSendto(ucastFd, outgoingPacket, length, 0,
                          (L7_sockaddr_t *)&saddr.u.sa4,sizeof(L7_sockaddr_t),
                          &bytesSent) != L7_SUCCESS)
    {
      SNTP_PRINTF("Send failed - errno %d\n", osapiErrnoGet());
      L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_SNTP_COMPONENT_ID,"sntpPollSend: send failed - errno %d\n", osapiErrnoGet());
      return L7_FAILURE; 
    }
  }
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  else if(sntpServerAddress.family == L7_AF_INET6)
  {
    saddr.u.sa6.sin6_family = L7_AF_INET6;
    saddr.u.sa6.sin6_port = osapiHtons( sntpCfgData->server[sntpActiveServer].port);
    memcpy (&(saddr.u.sa6.sin6_addr), &(sntpServerAddress.addr.ipv6), sizeof(sntpServerAddress.addr.ipv6));   

    if (osapiSocketSendto(ucastFd, outgoingPacket, length, 0,
                          (L7_sockaddr_t *)&saddr.u.sa6,sizeof(saddr),
                          &bytesSent) != L7_SUCCESS)
    {
      SNTP_PRINTF("Send failed - errno %d\n", osapiErrnoGet());
      L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_SNTP_COMPONENT_ID,"sntpPollSend: send failed - errno %d\n", osapiErrnoGet());
      return L7_FAILURE;
    }
  }
#endif 
   else
  {
    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_SNTP_COMPONENT_ID,"\r\nInvalid Family");
  }
 /** Update attempt on transmit - implies unicast mode. */
  serverStats[sntpActiveServer].unicastServerNumRequests++;
  serverStats[sntpActiveServer].lastAttemptTime = sntpStatusData->sntpLastAttemptTime = osapiUTCTimeNow();
  return L7_SUCCESS;
}
#if defined(L7_STACKING_PACKAGE)
/**********************************************************************
* @purpose  Update the local clock from broadcast packet sent by
*           the management unit
*
* @param    src_key {(input)}  Key (mac-address) of the unit that sent the msg
* @param    msg     {(input)}  The buffer holding the message
* @param    msg_len {(input)}  The buffer length of the message
*
* @returns  L7_SUCCESS
*
* @notes    This routine takes a SNTP packet broadcast over the stack
*           and handles it.
*
* @end
*********************************************************************/
void sntpHandleStackBroadcast(L7_enetMacAddr_t src_key,
                              L7_uchar8 * msg, L7_uint32 len)
{
  L7_uint32 timestamp;
  PARSE_TIMESTAMP(msg, timestamp);

  /* Simply sanity check */
  if (timestamp >= L7_SNTP_DISCARD_TIME)
  {
    osapiNTPTimeSet (timestamp - (L7_uint32)osapiNTPTimeGet());
  }
}

/**********************************************************************
* @purpose  Send a broadcast packet.
*
* @returns  L7_SUCCESS
*
* @notes    This routine broadcast a timestamp packet over the stack.
*
* @end
*********************************************************************/
static void sntpBroadcastTimestamp()
{
  if (sysapiHpcTopOfStackGet() == SYSAPI_STACK_ROLE_MANAGEMENT_UNIT)
  {
    L7_char8  buf[L7_SNTP_STACK_PACKET_LEN];
    L7_uint32 now = (L7_uint32)osapiNTPTimeGet();
    FORMAT_TIMESTAMP(buf, now);
    /* Hope this gets to everybody. */
    if (sysapiHpcBroadcastMessageSend(L7_SNTP_COMPONENT_ID,
                  L7_SNTP_STACK_PACKET_LEN, buf) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING,L7_SNTP_COMPONENT_ID,"sntpBroadcastTimestamp: error on broadcat send to stack.");
      SNTP_PRINTF("sntpBroadcastTimestamp: error on broadcat send to stack.");
    }
  }
}
#endif

/**********************************************************************
* @purpose  Process a response packet
*
* @param    the address of the server (we cache this in a parameter)
* @param    the response bytes
* @param    the number of response bytes
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes    This routine will invoke the parsing of the packet and
*           time processing if the packet parses nicely.
*           Somes stats on the packet are collected here.
*           On an error in unicast mode, new server selection
*           may be invoked.
*
* @end
*********************************************************************/
static L7_RC_t sntpResponseProcess(L7_char8 * response, L7_int32 responseLen)
{
  L7_int32 delay;
  L7_int32 offset;
  sntpPacket_t ntpData;
  L7_RC_t updateStatus = L7_FAILURE;
  
  L7_uchar8 recvAddressDisplay[L7_SNTP_MAX_ADDRESS_LEN];

  memset(&ntpData, 0, sizeof(ntpData));
  memset(&recvAddressDisplay,0x00,sizeof(recvAddressDisplay));

  /** Check that we weren't disabled while waiting */

  ntpData.referenceId = 0; /* compiler happy */

  sntpStatusData->sntpLastAttemptStatus = sntpPacketParse (&ntpData, response, responseLen);

  sntpTaskLock(); /** Need to lock here because we access sntpActiveServer */

  if (sntpStatusData->sntpLastAttemptStatus == L7_SNTP_STATUS_SUCCESS)
  {
    delay = 0;
    offset = 0;
    /* Extract the IPv4 or IPv6 address of receiver */
    osapiInetNtop(recvSntpServerAddrs.family,(L7_uchar8 *)(&recvSntpServerAddrs.addr),\
                  recvAddressDisplay,sizeof(recvAddressDisplay));
    /* check authentication if enabled */
    if (sntpCfgData->sntpAuthMode == L7_TRUE)
    {
      L7_uint32 keyid = 0;
      if (ntpData.authenticator == L7_NULLPTR)
      {
        L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_SNTP_COMPONENT_ID,"SNTP: discarding unauthenticated packet from %s", recvAddressDisplay);
        SNTP_PRINTF("SNTP: discarding unauthenticated packet from %s", recvAddressDisplay);
        sntpTaskUnlock();
        return updateStatus;
      }
      keyid = osapiNtohl(*(L7_uint32 *)(ntpData.authenticator));
      if (keyid == 0)
      {
        L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_SNTP_COMPONENT_ID,"SNTP: discarding unauthenticated packet from %s", recvAddressDisplay);
        SNTP_PRINTF("SNTP: discarding unauthenticated packet from %s", recvAddressDisplay);
        sntpTaskUnlock();
        return updateStatus;
      }
      if (sntpAuthVerify(keyid, 
                         response, 
                         ntpData.authenticator - (L7_uchar8 *)response,
                         ntpData.authenticator + sizeof(L7_uint32)) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_SNTP_COMPONENT_ID,"SNTP: authentication failed for packet from %s, keyid = %i", 
                recvAddressDisplay, keyid);
        SNTP_PRINTF("SNTP: authentication failed for packet from %s, keyid = %i",recvAddressDisplay, keyid);
        sntpTaskUnlock();
        return updateStatus;
      }
    }

    /** Atomic copy operation for last received 'valid' packet */
    /** Note that we do not play with the byte order. */
    
    if(recvSntpServerAddrs.family == L7_AF_INET)
    {
     sntpStatusData->sntpServerAddressType = L7_SNTP_ADDRESS_IPV4;
     sntpStatusData->sntpServerIpAddress.family = L7_AF_INET;
     sntpStatusData->sntpServerIpAddress.addr.ipv4.s_addr = recvSntpServerAddrs.addr.ipv4.s_addr; 
    }
    else if(recvSntpServerAddrs.family == L7_AF_INET6)
    {
     sntpStatusData->sntpServerAddressType = L7_SNTP_ADDRESS_IPV6;
     sntpStatusData->sntpServerIpAddress.family = L7_AF_INET6;
     memcpy(&(sntpStatusData->sntpServerIpAddress.addr.ipv6),&(recvSntpServerAddrs.addr.ipv6), \
           sizeof(recvSntpServerAddrs.addr.ipv6)); 
    }
    else
    {
     sntpStatusData->sntpServerAddressType = L7_SNTP_ADDRESS_UNKNOWN;
    }

    /** Handle errors here */
    sntpRTTCalculate(&ntpData, &delay, &offset);
    /** Check that RTT is not greater than max delay??? */
    sntpStatusData->peerUpdate = ntpData.current;
    sntpStatusData->peerOffset = offset;
    if (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_UNICAST)
    {
      /* Modified to accept all Unicast packets recieved without filtering. */
      if (sntpActiveServer != NO_SERVER_SELECTED)
      {
        serverStats[sntpActiveServer].lastUpdateTime = sntpStatusData->sntpLastUpdateTime;
        serverStats[sntpActiveServer].lastAttemptStatus = sntpStatusData->sntpLastAttemptStatus;
      }
    }

    SNTP_PRINTF("SNTP: Adjusting clock: peerUpdate=%u, offset=%d\n", sntpStatusData->peerUpdate, offset);

    updateStatus = L7_SUCCESS;
    if (offset)
    {
      (void) osapiNTPTimeSet(offset);
      sntpStatusData->sntpLastUpdateTime = osapiUTCTimeNow();

      /* Need to add/update conversion routines in osapi */
      L7_char8 * buf = ctime((void *)&(sntpStatusData->sntpLastUpdateTime));
      buf[24] = '\0';
      SNTP_PRINTF("SNTP: system clock synchronized on %s UTC. Indicates that SNTP has"
                  " successfully synchronized the time of the box with the server.", buf);
      SNTP_PRINTF("SNTP: osapiUTCTimeNow = %u -> %s\n", osapiUTCTimeNow(), buf);
      L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_SNTP_COMPONENT_ID, "SNTP: system clock synchronized "
              "on %s UTC. Indicates that SNTP has successfully synchronized the time of"
              " the box with the server.", buf);
    }
   
    #if defined(L7_STACKING_PACKAGE)
      sntpBroadcastTimestamp();
    #endif
    
    /** We update broadcast on receive, unicast on send */
    if (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_BROADCAST)
    {
      sntpStatusData->sntpBroadcastCount++;
    }
    else if (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_MULTICAST)
    {
      sntpStatusData->sntpMulticastCount++;
    }
  }
  sntpTaskUnlock();
  return updateStatus;
}


/**
* SECTION: Unicast/Broadcast/Multicast Server Mode Handlers
*
*
* These routines set the mode of the SNTP client to the selected value.
* They all may cause the main client loop to fall through and restart.
*
*/


/**********************************************************************
* @purpose  verify auth mode or client mode configuration change.
*
* @param    L7_SNTP_CLIENT_MODE_t clientMode @b{(input)}  client mode config
* @param    L7_BOOL               authEnable @b((input))  auth enabled/disabled
*
* @returns  L7_SUCCESS mode update is valid
* @returns  L7_FAILURE mode update is invalid
*
* @notes    This is a utility routine to verify either a client or authenticate
*           mode change based on the current key configuration, do not
*           allow auth to be enabled or mode to be changed if the right
*           key configuration does not exist.
*           For now, just verify that at least one trusted key exists for
*           all modes, we may want to make this more specific in the future,
*           i.e. require each unicast server to have a key in unicast mode.
*
* @end
*********************************************************************/
L7_RC_t sntpClientAuthModeVerify(L7_SNTP_CLIENT_MODE_t clientMode,
                                 L7_BOOL               authEnable)
{
  L7_uint32 i = 0;

  if (authEnable == L7_TRUE)
  {
    /* for all modes, check if we have one trusted key */
    for (i=0; i < L7_SNTP_MAX_AUTH_KEYS; i++)
    {
      if ((sntpCfgData->sntpKeys[i].status == L7_SNTP_SERVER_STATUS_ACTIVE) &&
          (sntpCfgData->sntpKeys[i].trusted == L7_TRUE))
      {
        /* found a trusted key */
        return L7_SUCCESS;
      }
    }
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  select client mode
*
* @returns  none
*
* @notes    may cause the loop in sntpTask to fall thru if
*           it's blocked on a socket.
*
* @end
*********************************************************************/
L7_RC_t sntpClientModeSelect(L7_SNTP_CLIENT_MODE_t mode)
{
  sntpTaskLock();
  if (sntpCfgData->sntpClientMode != mode)
  {
    /* if authentication is enabled, need to ensure we have
       the right key configuration based on client mode */
    if (sntpClientAuthModeVerify(mode, sntpCfgData->sntpAuthMode) != L7_SUCCESS)
    {
      sntpTaskUnlock();
      return L7_FAILURE;
    }
    sntpCfgData->sntpClientMode = mode;
    sntpActiveServer = NO_SERVER_SELECTED;
    memset(&sntpServerAddress,0x00,sizeof(sntpServerAddress));
    sntpLocalSocketClose();
    sntpCfgMarkDataChanged();
  }
  sntpTaskUnlock();
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  select disabled mode
*
* @returns  none
*
* @notes    may cause the loop in sntpTask to fall thru if it's
*           blocked on a socket.
*
* @end
*********************************************************************/

void sntpDisabledModeSelect()
{
  sntpTaskLock();
  sntpCfgData->sntpClientMode = L7_SNTP_CLIENT_DISABLED;
  sntpActiveServer = NO_SERVER_SELECTED;
  memset(&sntpServerAddress,0x00,sizeof(sntpServerAddress));
  sntpLocalSocketClose();
  sntpTaskUnlock();
  sntpCfgMarkDataChanged();
}

/**********************************************************************
* @purpose  Get the IP address of the server for the last received valid
*           packet.
*
* @param    pointer to where result is stored (must be at least 64 bytes)
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes     - This is a null terminated string.
*
* @end
*********************************************************************/
L7_RC_t sntpClientServerIpAddressGet(L7_char8 * address)
{

  if (address == L7_NULL)
 {
   return L7_ERROR;
 }

 /** Grab this in one shot - no thread issues please. */
 /* Extract the IPv4 or IPv6 address of receiver */
 
 if(&(sntpStatusData->sntpServerIpAddress )!= L7_NULL)
 {
   if(osapiInetNtop((sntpStatusData->sntpServerIpAddress.family),\
                    (L7_uchar8 *)&(sntpStatusData->sntpServerIpAddress.addr),\
                    address,L7_SNTP_MAX_ADDRESS_LEN) == L7_NULL)
   {
     SNTP_PRINTF("\r\n sntpClientServerIpAddressGet:Invalid Family");
     return L7_FAILURE; 
   }
 } 
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Get the IP address type of the address of the
*           server for the last received valid packet.
*
* @param    type @b{(input)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sntpClientServerAddressTypeGet(L7_SNTP_ADDRESS_TYPE_t * type)
{
  if (type == L7_NULL)
    return L7_ERROR;
  *type = sntpStatusData->sntpServerAddressType;
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Get the claimed stratum of the server for the last received
*           valid packet.
*
* @param    pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sntpClientServerStratumGet(L7_uint32 * stratum)
{
  if (stratum == L7_NULL)
    return L7_ERROR;
  *stratum = sntpStatusData->sntpServerStratum;
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Get the reference clock identifier of the server for the
*           last received valid packet.
*
*   Reference Identifier: This is a 32-bit bitstring identifying the
*   particular reference source. In the case of NTP Version 3 or Version
*   4 stratum-0 (unspecified) or stratum-1 (primary) servers, this is a
*   four-character ASCII string, left justified and zero padded to 32
*   bits. In NTP Version 3 secondary servers, this is the 32-bit IPv4
*   address of the reference source. In NTP Version 4 secondary servers,
*   this is the low order 32 bits of the latest transmit timestamp of the
*   reference source.
*
* @param    refClockId @b{(input)} pointer to where result is
*           stored (storage MUST be at least 26 bytes).
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes     - Writes a null terminated ASCII string
*           not exceeding 26 bytes including the null terminator.
*
* @end
*********************************************************************/


L7_RC_t sntpClientServerRefIdGet(L7_char8 * refClockId)
{
  if (refClockId == L7_NULL)
    return L7_ERROR;
  if (sntpStatusData->sntpServerReferenceClockId == 0)
  {
    *refClockId = '\0';
    return L7_SUCCESS;
  }
  if ((sntpStatusData->sntpServerVersion == 3) || (sntpStatusData->sntpServerVersion == 4))
  {
    if (sntpStatusData->sntpServerStratum == 0)
    {
      sprintf(refClockId, "SNTP Bits: 0x%.08x", sntpStatusData->sntpServerReferenceClockId);
    }
    else if (sntpStatusData->sntpServerStratum < 2)
    {
        sprintf(refClockId, "SNTP Ref: %c%c%c%c",
                (sntpStatusData->sntpServerReferenceClockId & 0xff000000) >> 24,
                (sntpStatusData->sntpServerReferenceClockId & 0x00ff0000) >> 16,
                (sntpStatusData->sntpServerReferenceClockId & 0x0000ff00) >> 8,
                (sntpStatusData->sntpServerReferenceClockId & 0x000000ff));
    }
    else if (sntpStatusData->sntpServerStratum >= 2)
    {
      if(sntpStatusData->sntpServerVersion == 3)
      {
        sprintf(refClockId, "NTP Srv: %d.%d.%d.%d",
                (sntpStatusData->sntpServerReferenceClockId & 0xff000000) >> 24,
                (sntpStatusData->sntpServerReferenceClockId & 0x00ff0000) >> 16,
                (sntpStatusData->sntpServerReferenceClockId & 0x0000ff00) >> 8,
                (sntpStatusData->sntpServerReferenceClockId & 0x000000ff));
      }
      else if(sntpStatusData->sntpServerVersion == 4)
      {
        sprintf(refClockId, "NTP Bits: 0x%.08x", sntpStatusData->sntpServerReferenceClockId);
      }
    }
  }
  else
  {
    sprintf(refClockId, "NTP Bits: 0x%.08x", sntpStatusData->sntpServerReferenceClockId);
  }

  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Get the mode of the server for the
*           last received valid packet.
*
* @param    serverMode @b{(output)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sntpClientServerModeGet(L7_SNTP_SERVER_MODE_t * serverMode)
{
  if (serverMode == L7_NULL)
    return L7_ERROR;
  *serverMode = sntpStatusData->sntpServerMode;
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Get the current number of unsolicited broadcast SNTP messages
*           messages that have been received and processed by the SNTP
*           client since last reboot.
*
* @param    broadcastCount @b{(output)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sntpClientBroadcastCountGet(L7_uint32 * broadcastCount)
{
  if (broadcastCount == L7_NULL)
    return L7_ERROR;
  *broadcastCount = sntpStatusData->sntpBroadcastCount;
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Get the current number of unsolicited multicast SNTP messages
*           messages that have been received and processed by the SNTP
*           client since last reboot.
*
* @param    multicastCount @b{(output)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sntpClientMulticastCountGet(L7_uint32 * multicastCount)
{
  if (multicastCount == L7_NULL)
    return L7_ERROR;
  *multicastCount = sntpStatusData->sntpMulticastCount;
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Get the last attempt status of the client in unicast mode.
*
* @param    status @b {(output)} pointer to the status
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sntpClientLastAttemptStatusGet(L7_SNTP_PACKET_STATUS_t * status)
{
  if (status == L7_NULL)
    return L7_ERROR;
  *status = sntpStatusData->sntpLastAttemptStatus;
  return L7_SUCCESS;
}
/**********************************************************************
* @purpose  Get the last attempt time of the client in unicast mode.
*
* @param    attemptTime @b {(output)} pointer to the time
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sntpClientLastAttemptTimeGet(L7_uint32 * attemptTime)
{
  if (attemptTime == L7_NULL)
    return L7_ERROR;
  *attemptTime = sntpStatusData->sntpLastAttemptTime;
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Get the last update time of the client in any mode.
*
* @param    updateTime @b {(output)} pointer to the time
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sntpClientLastUpdateTimeGet(L7_uint32 * updateTime)
{
  if (updateTime == L7_NULL)
    return L7_ERROR;
  *updateTime = sntpStatusData->sntpLastUpdateTime;
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Clear the statistics data.
*
* @notes
*
* @end
*********************************************************************/

void sntpStatisticsClear()
{
  register L7_uint32 i;
  /** Clear server statistics structures */
  for (i = 0; i < L7_SNTP_MAX_SERVERS; i++)
  {
    serverStats[i].lastUpdateTime = serverStats[i].lastAttemptTime = 0;
    serverStats[i].lastAttemptStatus = L7_SNTP_STATUS_OTHER;
    serverStats[i].unicastServerNumRequests = 0;
    serverStats[i].unicastServerNumFailedRequests = 0;
  }

  sntpStatusData->sntpLastUpdateTime = sntpStatusData->sntpLastAttemptTime = 0;
  sntpStatusData->sntpLastAttemptStatus = L7_SNTP_STATUS_OTHER;
}

/**********************************************************************
* @purpose  Initialize the Status data.
*
* @notes
*
* @end
*********************************************************************/
void sntpStatusInit()
{
  if(sntpStatusData == L7_NULL)
    return;

  sntpStatusData->sntpBroadcastCount = 0;
  sntpStatusData->sntpLastUpdateTime = 0;
  sntpStatusData->sntpLastAttemptTime = 0;
  sntpStatusData->sntpLastAttemptStatus = L7_SNTP_STATUS_OTHER;
  memset(&(sntpStatusData->sntpServerIpAddress),0x00,sizeof(sntpStatusData->sntpServerIpAddress));
  sntpStatusData->sntpServerAddressType = L7_SNTP_ADDRESS_UNKNOWN;
  sntpStatusData->sntpServerStratum = 0;
  sntpStatusData->sntpServerVersion = 0;
  sntpStatusData->sntpServerReferenceClockId = 0;
  sntpStatusData->sntpServerMode = L7_SNTP_SERVER_MODE_RESERVED;
  sntpStatusData->sntpBroadcastCount = 0;
  sntpStatusData->sntpMulticastCount = 0;
  sntpStatusData->peerOffset = SNTP_MAX_DISTANCE;
  sntpStatusData->peerUpdate = 0;

}

/**********************************************************************
* @purpose  Clear the statistics -
*           Set everything to default values
*
* @param    version of configuration data, currently not used.
*
* @notes
*
* @end
*********************************************************************/
void sntpClientDefaultsSet(L7_uint32 ver)
{
  register L7_uint32 i;

  if (sntpCfgData == L7_NULL)
    return;

  sntpTaskLock();
  /** Default the global configuration data */
  sntpCfgData->sntpClientMode = L7_SNTP_DEFAULT_CLIENT_MODE;
  sntpLocalSocketClose();
  sntpCfgData->sntpClientPort = L7_SNTP_DEFAULT_CLIENT_PORT;
  sntpCfgData->sntpUnicastPollInterval = L7_SNTP_DEFAULT_POLL_INTERVAL;
  sntpCfgData->sntpUnicastPollTimeout = L7_SNTP_DEFAULT_POLL_TIMEOUT;
  sntpCfgData->sntpUnicastPollRetry = L7_SNTP_DEFAULT_POLL_RETRY;
  sntpCfgData->sntpBroadcastPollInterval = L7_SNTP_DEFAULT_POLL_INTERVAL;
  sntpCfgData->sntpMulticastPollInterval = L7_SNTP_DEFAULT_POLL_INTERVAL;
  sntpCfgData->sntpUnicastServerCount = 0;

  for (i = 0; i < L7_SNTP_MAX_SERVERS; i++)
  {
    /** Default the server configuration data */
    sntpCfgData->server[i].status = L7_SNTP_SERVER_STATUS_DESTROY;
    sntpCfgData->server[i].addressType = L7_SNTP_ADDRESS_UNKNOWN;
    sntpCfgData->server[i].address[0] = '\0';
    sntpCfgData->server[i].port = L7_SNTP_DEFAULT_SERVER_PORT;
    sntpCfgData->server[i].version = L7_SNTP_DEFAULT_VERSION;
    sntpCfgData->server[i].priority = L7_SNTP_DEFAULT_SERVER_PRIORITY;

  }
  sntpCfgData->sntpUnicastServerCount = 0;
  sntpCfgData->sntpAuthMode = L7_FALSE;
  memset(&sntpCfgData->sntpKeys[0], 0, sizeof(sntpKeyCfgData_t) * L7_SNTP_MAX_AUTH_KEYS);

  memset(&(sntpStatusData->sntpServerIpAddress),0x00,sizeof(sntpStatusData->sntpServerIpAddress));
  sntpStatusData->sntpServerAddressType = L7_SNTP_ADDRESS_UNKNOWN;
  sntpStatusData->sntpServerStratum = 0;
  sntpStatusData->sntpServerVersion = 0;
  sntpStatusData->sntpServerReferenceClockId = 0;
  sntpStatusData->sntpServerMode = L7_SNTP_SERVER_MODE_RESERVED;
  /** Start off with some instance of time other than 0. */
  sntpStatusData->peerUpdate = osapiNTPTimeGet();
  sntpTaskUnlock();
}

/**********************************************************************
* @purpose  sntpHandleError - cnetralized packet error handler
*
*
* @notes    Update stats and potentially choose a new server on an error.
*           Keys off of sntpLastAttemptStatus.
*
*           Last attempt status is updated.
*           Retry count is incremented.
*           If kiss of death packet, failed requests is incremented,
*           current server is marked dead and new server else selected,
*           else if retry count exceeded, failed requests is incremented
*           and a new server is selected.
*
*
* @end
*********************************************************************/
static void sntpHandleError()
{
  if (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_UNICAST)
  {
    retryCount++;
    sntpTaskLock();
    if (sntpActiveServer != NO_SERVER_SELECTED)
    {
      serverStats[sntpActiveServer].lastAttemptStatus = sntpStatusData->sntpLastAttemptStatus;
      if (sntpStatusData->sntpLastAttemptStatus == L7_SNTP_STATUS_KISS_OF_DEATH)
      {
        serverStats[sntpActiveServer].unicastServerNumFailedRequests++;
        /* Status was getting saved in the config file, which was causing the 
		  client not to poll the server after the reboot */ 
        /* We never send to this server again. */
        sntpCfgData->server[sntpActiveServer].status =
                      L7_SNTP_SERVER_STATUS_NOT_IN_SERVICE;

        /** Save the new server state */
        sntpCfgMarkDataChanged();
        unicastServerSelect();
         /* ReOpen the socket again */
        if (sntpLocalSocketOpen() != L7_SUCCESS)
        {
          SNTP_PRINTF("sntpTask: Could not open socket\n");
        }
        else
        {
          SNTP_PRINTF("sntpTask: open socket\n");
        }  
      }
      else if (retryCount > sntpCfgData->sntpUnicastPollRetry)
      {
        serverStats[sntpActiveServer].unicastServerNumFailedRequests++;
        unicastServerSelect();
         /* ReOpen the socket again */
        if (sntpLocalSocketOpen() != L7_SUCCESS)
        {
          SNTP_PRINTF("sntpTask: Could not open socket\n");
        }
        else
        {
          SNTP_PRINTF("sntpTask: open socket\n");
        }  
      }
    }
    sntpTaskUnlock();
  }
}

/**
* SECTION: Dispatch
*
* This section contains the main dispatch loop for the SNTP client.
*
*/

/**********************************************************************
* @purpose  sntpTask
*
*
* @notes    This is the main SNTP task.
*           The basic idea of this loop is that it will figure out what to
*           to do based on:*
*             sntpCfgData->sntpClientMode
*             sntpCfgData->sntpUnicastServerCount
*             server table
*
*           If client mode is disabled and the socket is then closed,
*           the loop will hold at the start and wait for a sane
*           configuration.
*
*           Asynchronously closing the socket (ucastFd) from anywhere causes
*           the entire loop to reset and start over from the beginning.
*
* @end
*********************************************************************/

static void sntpTask(void)
{
  L7_int32 rcvLen;
  L7_uchar8 response[1024];
  fd_set readFds;
  L7_sockaddr_union_t raddr;  
  L7_uint32 from_len = sizeof(raddr), maxFd;
  L7_uchar8 recvAddressDisplay[L7_SNTP_MAX_ADDRESS_LEN];
  L7_RC_t rcData = L7_FAILURE;

  osapiTaskInitDone(L7_SNTP_TASK_SYNC);

  while (1)
  {
    L7_RC_t rc = L7_FAILURE;
    L7_uint32 pollInterval;
    L7_uint32 pollIntervalTmp;

    SNTP_PRINTF("sntpTask: wait for valid configuration\n");
  
    /** Wait until we're enabled and have some semblence of sanity */
    while ((sntpCfgData == L7_NULL) ||
           (serverStats == L7_NULL) ||
           (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_DISABLED) ||
           ((sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_UNICAST) &&
            (sntpCfgData->sntpUnicastServerCount == 0)))
    {
      osapiSleep(3); /** Yawn */
    }
    SNTP_PRINTF("sntpTask: valid configuration - mode %d\n", sntpCfgData->sntpClientMode);

    /** Unicast mode - find a server for the retry loop below */
    if (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_UNICAST)
    {
      /* No server or retries exceeded */
      if ((sntpStatusData->sntpLastAttemptStatus != L7_SNTP_STATUS_SUCCESS) ||
          (sntpActiveServer == NO_SERVER_SELECTED))
      {
        sntpTaskLock();
        SNTP_PRINTF("\r\nIn the process of Active Server selection\r\n");
        unicastServerSelect();
        sntpTaskUnlock();
      }


      /** No server, start again at top */
      if (sntpActiveServer == NO_SERVER_SELECTED)
      {
        /* It is possible that we have a unicast server defined (sntpUnitcastServerCount > 0)
        * and the client is in unicast mode and we still do not have a server selected.
        * This situation has been observed to occur when the server sends back a kiss of death
        * packet and there is only one server defined. So we sleep before trying again.
        */
        osapiSleep(3);
        continue;
      }
      SNTP_PRINTF("sntpTask: select server %d\n", sntpActiveServer);
    }
    
    /** No socket? Go get one */
    if ((ucastFd < 0) 
         #if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
         ||
          ((sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_BROADCAST) && (mcastFd6 < 0))
         #endif
        )
    {
      sntpTaskLock();
      if (sntpLocalSocketOpen() != L7_SUCCESS)
      {
        SNTP_PRINTF("sntpTask: Could not open socket\n");
      }
      else
      {
        SNTP_PRINTF("sntpTask: open socket\n");
      }
      sntpTaskUnlock();
      if ((ucastFd < 0) 
           #if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
           ||
            ((sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_BROADCAST) && (mcastFd6 < 0))
           #endif
         )
      {
        continue;
      }
    }

   /** Implement the wait interval - We use a socket
    * so we can break out on management changes.
    * Anything that comes in is discarded during the wait interval.
    */
    retryCount = 0;
    if (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_UNICAST)
    {
      if((sntpCfgData->sntpUnicastPollInterval >= SNTP_MIN_POLL_INTERVAL) &&
         (sntpCfgData->sntpUnicastPollInterval <= SNTP_MAX_POLL_INTERVAL))
        pollInterval = 1 << sntpCfgData->sntpUnicastPollInterval;
      else
        pollInterval =  sntpCfgData->sntpUnicastPollInterval;
    }
    else if (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_BROADCAST)
    {
      if((sntpCfgData->sntpBroadcastPollInterval >= SNTP_MIN_POLL_INTERVAL) &&
         (sntpCfgData->sntpBroadcastPollInterval <= SNTP_MAX_POLL_INTERVAL))
        pollInterval = 1 << sntpCfgData->sntpBroadcastPollInterval;
      else
        pollInterval =  sntpCfgData->sntpBroadcastPollInterval;
    }
    else if (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_MULTICAST)
    {
      if((sntpCfgData->sntpMulticastPollInterval >= SNTP_MIN_POLL_INTERVAL) &&
         (sntpCfgData->sntpMulticastPollInterval <= SNTP_MAX_POLL_INTERVAL))
        pollInterval = 1 << sntpCfgData->sntpMulticastPollInterval;
      else
        pollInterval =  sntpCfgData->sntpMulticastPollInterval;
    }
    else
    {
      continue;
    }
    SNTP_PRINTF("sntpTask: enter poll loop - mode %d\n", sntpCfgData->sntpClientMode);
    /** Perform the poll loop  - retries for a unicast server are performed within this loop */
    pollIntervalTmp = pollInterval;

    memset(recvAddressDisplay,0x00,sizeof(recvAddressDisplay));
     
    /* Don't start sending Poll if switch has not been given IP Address */
    do
    {
      L7_uint32 pollStart = osapiUpTimeRaw();
      L7_uint32 pollTimeout = 0;

      SNTP_PRINTF("sntpTask: starting wait interval %d\n", pollIntervalTmp);

      sntpTaskLock();
      if (ucastFd < 0)
      {
        sntpTaskUnlock();
        break;
      }
      FD_ZERO(&readFds);
      FD_SET(ucastFd, &readFds);
      maxFd = ucastFd + 1;
      if ((sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_BROADCAST) && (mcastFd6 > 0))
      {
        FD_SET(mcastFd6, &readFds);
        maxFd = ((ucastFd > mcastFd6) ? (ucastFd + 1) : (mcastFd6 + 1));
      }
      sntpTaskUnlock();
      rc = osapiSelect(maxFd, &readFds, L7_NULL, L7_NULL, pollIntervalTmp, 1);
      if (ucastFd < 0) /* Linux bug 546 */
        break;
      if (rc < 0)
      {
        SNTP_PRINTF("sntpTask: select returned %d\n", rc);
        /** This is really ok - we might have just been told to switch modes. */
        SNTP_PRINTF("Select failed during interval - errno %d\n",
                                          osapiErrnoGet());
        break;
      }
      else if (rc == 0) /** Timed out */
      {
        SNTP_PRINTF("sntpTask: select returned %d\n", rc);
        /** Do nothing since this is what we really wanted. */ ;
      }
      else if (rc > 0)
      {
        SNTP_PRINTF("sntpTask: select returned %d - flushing receive buffers\n", rc);
        /** Flush the receive buffer here.
        * No stale replies or broadcasts needed.
        */
        sntpTaskLock();
        sntpFlushReceiveBuffer();
        sntpTaskUnlock();
        SNTP_PRINTF("sntpTask: flushed receive buffer - elapsed time %d\n", osapiUpTimeRaw() - pollStart);
        /* Wait the full poll interval */
        if ((osapiUpTimeRaw() - pollStart) < pollIntervalTmp)
        {
          pollIntervalTmp -= (osapiUpTimeRaw() - pollStart);
          SNTP_PRINTF("sntpTask: poll interval not complete - %d\n", pollIntervalTmp);
          continue;
        }
      }

      pollIntervalTmp = pollInterval;
      /** If in unicast mode, send a poll. If no valid address is specified,
      *   the poll goes into the bit bucket.
      *   Must be in unicast mode and have a valid server selected.
      */
      sntpTaskLock();
      if ((sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_UNICAST) &&
          (sntpActiveServer >= 0)
          && (sntpCfgData->server[sntpActiveServer].poll == L7_ENABLE))

      {
        if( sntpPollSend() != L7_SUCCESS)
        {
           SNTP_PRINTF("sntpTask: Failed to send Poll Messsage\n");
        }
        else
        {
           SNTP_PRINTF("sntpTask: unicast poll sent\n");
        }
      }
      sntpTaskUnlock();

      /** We've sent a poll, now try to get a response.
      *  Errors drop out of this loop.  This loop exists because of linux bug 546.
      */
      do
      {
          /** We wait forever for a broadcast/multicast packet. */
        pollTimeout = (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_UNICAST) ?
                        sntpCfgData->sntpUnicastPollTimeout : 10; /* Linux bug 546 */
        /** Wait for something to arrive on our port */
        sntpTaskLock();
        if (ucastFd < 0)
        {
          sntpTaskUnlock();
          break;
        }
        FD_ZERO(&readFds);
        FD_SET(ucastFd, &readFds);
        maxFd = ucastFd + 1;
        if((sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_BROADCAST) && (mcastFd6 > 0))
        {
          FD_SET(mcastFd6, &readFds);
          maxFd = ((ucastFd > mcastFd6) ? (ucastFd + 1) : (mcastFd6 + 1));
        }

        sntpTaskUnlock();
        SNTP_PRINTF("sntpTask: waiting for SNTP packet - timeout value %d - retry count %d - ucastFd %d\n",
                                                                    pollTimeout, retryCount, ucastFd);
        rc = osapiSelect(maxFd, &readFds, L7_NULL, L7_NULL, pollTimeout, 0);
        if ((ucastFd < 0) ||
            ((sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_BROADCAST) && (mcastFd6 < 0)))
        {
          break;
        }
      } while (((sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_MULTICAST) ||
                (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_BROADCAST)) && (rc == 0));

      /** This is really ok - we might have just been told to change modes. */
      if (ucastFd < 0)
        break;

      if (rc < 0) /** Error */
      {
        sntpStatusData->sntpLastAttemptStatus = L7_SNTP_STATUS_OTHER;
        sntpHandleError();
        SNTP_PRINTF("sntpTask: Select failed for poll - errno %d last attempt status %d - retry count %d\n",
                                              osapiErrnoGet(), sntpStatusData->sntpLastAttemptStatus, retryCount);
        break;
      }
      else if (rc == 0)
      {
        sntpStatusData->sntpLastAttemptStatus = L7_SNTP_STATUS_REQUEST_TIMED_OUT;
        sntpHandleError();
        SNTP_PRINTF("sntpTask:Select timed out - errno %d - last attempt status %d - retry count %d\n",
                                            osapiErrnoGet(), sntpStatusData->sntpLastAttemptStatus, retryCount);
      }
      else if (rc > 0)
      {
        /** Read the response and process the packet */
        SNTP_PRINTF("sntpTask: Select indicates data available\n");
       
        /* Reset the raddr union */
        memset(&raddr,0x00,sizeof(raddr));
        rcvLen = 0; 

        /* At this point the data could be ready on 
         * (a) v4 and v6 sockets as well if in broadcast mode (or)
         * (b) one of v4 or v6 socket if in unicast mode
         * If either mode, read the data from the first data ready socket in the
         * order v4 followed by v6. */
        rcData = L7_FAILURE;
        if (FD_ISSET(ucastFd,&readFds))
        {
           rcData = osapiSocketRecvfrom(ucastFd, (void *)response,
                                    sizeof(response), 0, (L7_sockaddr_t *)&raddr,&from_len,
                                    &rcvLen);
        }
        if (rcData != L7_SUCCESS)
        {
            memset(&raddr,0x00,sizeof(raddr));
            if((sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_BROADCAST) && (mcastFd6 > 0))
            {
              if(FD_ISSET(mcastFd6, &readFds))
              {
                rcData = osapiSocketRecvfrom(mcastFd6, (void *)response,
                                         sizeof(response), 0, (L7_sockaddr_t *)&raddr,&from_len,
                                         &rcvLen);
              }
           }
        }
        if (rcData != L7_SUCCESS)       /** Error? */
        {
           sntpStatusData->sntpLastAttemptStatus = L7_SNTP_STATUS_OTHER;
           sntpHandleError();
           SNTP_PRINTF("sntpTask:recv6 failed - errno %d - last attempt status %d - retry count %d\n",
                       osapiErrnoGet(), sntpStatusData->sntpLastAttemptStatus, retryCount);
        }
        else if (rcvLen == 0)
        {
          /** No data? */
          sntpStatusData->sntpLastAttemptStatus = L7_SNTP_STATUS_REQUEST_TIMED_OUT;
          sntpHandleError();
          SNTP_PRINTF("sntpTask: osapiRecvFrom returns no data - errno %d last attempt status %d - retry count %d\n",
                                              osapiErrnoGet(), sntpStatusData->sntpLastAttemptStatus, retryCount);
        }
        else if (rcvLen > 0)  /** A packet awaits */
        {
          SNTP_PRINTF("sntpTask: osapiRecvFrom returns data\n");
          #if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
          if(raddr.u.sa.sa_family == L7_AF_INET6)
          {
            if(L7_IP6_IS_ADDR_V4MAPPED(&raddr.u.sa6.sin6_addr))
            {
             recvSntpServerAddrs.family = L7_AF_INET;
             recvSntpServerAddrs.addr.ipv4.s_addr  = L7_IP6_ADDR_V4MAPPED(&raddr.u.sa6.sin6_addr);
             raddr.u.sa4.sin_addr.s_addr = recvSntpServerAddrs.addr.ipv4.s_addr;
            }
            else
            {
             recvSntpServerAddrs.family = L7_AF_INET6;
             recvSntpServerAddrs.addr.ipv6 = *(L7_in6_addr_t *)&raddr.u.sa6.sin6_addr;
            }
          }
          else
          #endif
          {
             recvSntpServerAddrs.family = L7_AF_INET;
             recvSntpServerAddrs.addr.ipv4.s_addr = raddr.u.sa4.sin_addr.s_addr;
          } 
           /* Extract the IPv4 or IPv6 address of receiver */
           osapiInetNtop(recvSntpServerAddrs.family,(L7_uchar8 *)(&recvSntpServerAddrs.addr),\
                 recvAddressDisplay,sizeof(recvAddressDisplay));

          if (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_UNICAST)
          {
            /* Ignore non-selected servers when in unicast mode 
               Check for family and IPv4 and IPv6 address also
             */
             if (((recvSntpServerAddrs.family == L7_AF_INET ) && 
                  ( recvSntpServerAddrs.addr.ipv4.s_addr != sntpServerAddress.addr.ipv4.s_addr))
                #if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
                 || ((recvSntpServerAddrs.family == L7_AF_INET6) &&
                     (memcmp(&(recvSntpServerAddrs.addr.ipv6),&(sntpServerAddress.addr.ipv6), \
                  sizeof(recvSntpServerAddrs.addr.ipv6)) != L7_NULL))
               #endif
                )
            {
              sntpStatusData->sntpLastAttemptStatus = L7_SNTP_STATUS_OTHER;
              sntpHandleError();
               SNTP_PRINTF("sntpTask: invalid sender %s - last attempt status %d - retry count %d\n",
                                                recvAddressDisplay,sntpStatusData->sntpLastAttemptStatus, retryCount);
            }
            else
            {
              SNTP_PRINTF("Process response from %s len %d\n", recvAddressDisplay,rcvLen);
              if( sntpResponseProcess(response, rcvLen) != L7_SUCCESS)
                SNTP_PRINTF("sntpTask: sntpResponseProcess failed with LastAttemptStatus as:%d\n",
                              sntpStatusData->sntpLastAttemptStatus);
              if (sntpStatusData->sntpLastAttemptStatus == L7_SNTP_STATUS_SUCCESS)
              {
                /* Got a good packet - retry forever */
                retryCount = sntpCfgData->sntpUnicastPollRetry + 1;
                SNTP_PRINTF("sntpTask: packet processed - last attempt status %d - retry count %d\n",
                                                sntpStatusData->sntpLastAttemptStatus, retryCount);
              }
              else
              {
                sntpHandleError();
                SNTP_PRINTF("sntpTask: packet process failed - last attempt status %d - retry count %d\n",
                                                sntpStatusData->sntpLastAttemptStatus, retryCount);
              }
            }
          }
          else
          {
            SNTP_PRINTF("Process response from %s len %d\n", recvAddressDisplay, rcvLen);
            if( sntpResponseProcess(response, rcvLen) != L7_SUCCESS)
                SNTP_PRINTF("sntpTask: sntpResponseProcess failed with LastAttemptStatus as:%d\n",
                              sntpStatusData->sntpLastAttemptStatus);

            /** We got a response - no more retry */
            retryCount = sntpCfgData->sntpUnicastPollRetry + 1;
            if (sntpStatusData->sntpLastAttemptStatus == L7_SNTP_STATUS_SUCCESS)
            {
              SNTP_PRINTF("sntpTask: packet processed - last attempt status %d - retry count %d\n",
                                              sntpStatusData->sntpLastAttemptStatus, retryCount);
            }
            /* No need to handle errors on multicast/broadcast mode */
          }
        }
      }
    } while (retryCount <= sntpCfgData->sntpUnicastPollRetry);  /* end do */
  } /* end while */
}

#if defined(L7_IPV6_PACKAGE)
/**********************************************************************
*
* @purpose Group Membership Registration for IPv6 broadcast mode for SNTP client
*
* @param intIfNum @b{(input)} internal interface number
* @param joinFlag @b{(input)} join or leave the multicast group
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t sntpGroupMembershipAdd(L7_uint32 intIfNum, L7_BOOL joinFlag)
{
  
  struct L7_ip_mreq6_s mreq6;
  L7_int32 sockopt;
  L7_in6_addr_t mcastAddress;
  
  osapiInetPton(L7_AF_INET6, SNTP_IPV6_LINK_LOCAL_SCOPED_ADDRESS, (L7_uchar8 *)&(mcastAddress)); 
  memcpy(&mreq6.imr6_multiaddr, &mcastAddress, L7_IP6_ADDR_LEN);
  mreq6.imr6_intIfNum = intIfNum;

  if (joinFlag)
  {
    sockopt = L7_IPV6_ADD_MEMBERSHIP;
  }
  else
  {
    sockopt = L7_IPV6_DROP_MEMBERSHIP;
  }

  return osapiSetsockopt(mcastFd6, IPPROTO_IPV6, sockopt,
                         (L7_uchar8 *) &mreq6, sizeof(struct L7_ip_mreq6_s));
}

/*********************************************************************
* @purpose  This is the callback function SNTP registers with IP6 MAP
*           for notification of routing events.   
*
* @param    intIfnum @b{(input)}  internal interface number
* @param    event    @b{(input)}  an event listed in l7_ip6_api.h
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
L7_RC_t sntpIPv6RoutingEventChangeCallBack (L7_uint32 intIfNum, 
                                            L7_uint32 event,
                                            void *pData, 
                                           ASYNC_EVENT_NOTIFY_INFO_t *response)
{
    ASYNC_EVENT_COMPLETE_INFO_t event_completion;

    switch (event)
    {
    case RTR6_EVENT_INTF_ENABLE: 
      /* Join the reserved link-local multicast group for
       * SNTPv6 group of routers */
        if (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_BROADCAST)
        {
          sntpGroupMembershipAdd(intIfNum, L7_TRUE);
        }
      break;

    case RTR6_EVENT_INTF_DISABLE:
      /* Leave the reserved link-local multicast group for
       * SNTPv6 group of routers */
        if (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_BROADCAST)
        {
          sntpGroupMembershipAdd(intIfNum, L7_FALSE);
        }
      break;

    default:
    /* No action necessary for events SNTP doesn't care about. */
      break;
    }
    if ((response != L7_NULLPTR) && 
        (response->handlerId != 0) && 
        (response->correlator != 0))
    {
      /* acknowledge SNTP's processing of the event */
      /* Purposely overloading componentID with L7_IPMAP_REGISTRANTS_t */
      event_completion.componentId = L7_IPRT_SNTP;
      event_completion.handlerId = response->handlerId;
      event_completion.correlator = response->correlator;
      event_completion.async_rc.rc = L7_SUCCESS;
      event_completion.async_rc.reason = ASYNC_EVENT_REASON_RC_SUCCESS;    
      asyncEventCompleteTally(&event_completion);
    }

    return L7_SUCCESS;
}
#endif



/**********************************************************************
*
* @purpose System Initialization for SNTP client
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t sntpClientInit(struct sntpCfgData_s * cfg)
{
  extern void sntpApiInit(struct sntpCfgData_s * cfg,
                    struct sntpServerStats_s * srv);
  L7_RC_t sntpRC = L7_SUCCESS;

  /* Get a pointer to our cfg data. */
  if (cfg == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNTP_COMPONENT_ID,"SNTP: Unable to obtain configuration data\n");
    sntpRC = L7_FAILURE;
    return sntpRC;
  }
  sntpCfgData = cfg;

  sntpStatusData = osapiMalloc(L7_SNTP_COMPONENT_ID, sizeof(struct sntpStatusData_s));
  if (sntpStatusData == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNTP_COMPONENT_ID,"SNTP: Unable to carve out memory for Status information\n");
    sntpRC = L7_FAILURE;
    return sntpRC;
  }
  sntpStatusInit();

  serverStats = osapiMalloc(L7_SNTP_COMPONENT_ID, L7_SNTP_MAX_SERVERS * sizeof(struct sntpServerStats_s));
  if (serverStats == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNTP_COMPONENT_ID,"SNTP: Unable to create the task semaphore\n");
    sntpRC = L7_FAILURE;
    return sntpRC;
  }
  sntpStatisticsClear();

  /* Now that we have all dynamic data allocated, initialize the api. */
  sntpApiInit(cfg, serverStats);

  /** semaphore creation for task protection over the common data*/
  sntpTaskSyncSemaphore = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (sntpTaskSyncSemaphore == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNTP_COMPONENT_ID,"SNTP: Unable to create the task semaphore\n");
    sntpRC = L7_FAILURE;
    return sntpRC;
  }

  /** create SNTP client task */
  sntpClientTaskId = (L7_uint32)osapiTaskCreate(L7_SNTP_TASK,
                                          (void *)sntpTask,
                                          L7_NULL,
                                          L7_NULL,
                                          L7_SNTP_DEFAULT_STACK_SIZE,
                                          L7_SNTP_DEFAULT_TASK_PRI,
                                          L7_SNTP_DEFAULT_TASK_SLICE);

  if (osapiWaitForTaskInit(L7_SNTP_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNTP_COMPONENT_ID,"SNTP: Unable to initialize sntp task\n");
    sntpRC   = L7_FAILURE;
  }

  return sntpRC;
}

/**********************************************************************
*
* @purpose System Deinitialization for SNTP client
*
* @notes Stops thread and deallocates resources
*
* @end
*
*********************************************************************/
void sntpClientTerminate()
{
  sntpClientModeSet(L7_SNTP_CLIENT_DISABLED);
  sntpLocalSocketClose();

  if (sntpClientTaskId != 0)
  {
    osapiTaskDelete(sntpClientTaskId);
    sntpClientTaskId = 0;
  }
  if (sntpTaskSyncSemaphore != L7_NULL)
  {
    osapiSemaDelete(sntpTaskSyncSemaphore);
    sntpTaskSyncSemaphore = L7_NULL;
  }
}

#ifdef SNTP_CLIENT_DEBUG

/**********************************************************************
* @purpose  sntpStatusShow
*
*
**********************************************************************/
static L7_char8 * sntpModeString(L7_SNTP_CLIENT_MODE_t m)
{
  switch(m)
  {
  case  L7_SNTP_CLIENT_DISABLED:
    return "Disabled";
  case  L7_SNTP_CLIENT_UNICAST:
    return "Unicast";
  case  L7_SNTP_CLIENT_BROADCAST:
    return "Broadcast";
  case  L7_SNTP_CLIENT_MULTICAST:
    return "Multicast";
  default:
    return "Unknown";
  }
}

static L7_char8 * sntpServerModeString(L7_SNTP_SERVER_MODE_t m)
{
  switch (m)
  {
  case L7_SNTP_SERVER_MODE_RESERVED:
    return "Reserved";
  case L7_SNTP_SERVER_MODE_SYMMETRIC_ACTIVE:
    return "Symmetric Active";
  case L7_SNTP_SERVER_MODE_SYMMETRIC_PASSIVE:
    return "Symmetric Passive";
  case L7_SNTP_SERVER_MODE_CLIENT:
    return "Client";
  case L7_SNTP_SERVER_MODE_SERVER:
    return "Server";
  case L7_SNTP_SERVER_MODE_BROADCAST:
    return "Broadcast";
  case L7_SNTP_SERVER_MODE_RESERVED_CONTROL:
    return "Control";
  case L7_SNTP_SERVER_MODE_RESERVED_PRIVATE:
    return "Private";
  default:
    return "Unknown";
  }
}

void sntpStatusShow()
{
  register L7_int32 i;
  L7_char8  refBuf[24];
  sntpClientServerRefIdGet(refBuf);

  printf("\n- SNTP Status -\n");
  printf("Client Mode: %d %s\n", sntpCfgData->sntpClientMode,
                      sntpModeString(sntpCfgData->sntpClientMode));
  printf("Unicast Port: %d\n",
                      sntpCfgData->sntpClientPort);
  printf("Unicast Poll Interval: %d\n",
                      sntpCfgData->sntpUnicastPollInterval);
  printf("Unicast Poll Retry: %d\n",
                      sntpCfgData->sntpUnicastPollRetry);
  printf("Unicast Poll Timeout: %d\n",
                      sntpCfgData->sntpUnicastPollTimeout);
  printf("Broadcast Poll Interval: %d\n",
                      sntpCfgData->sntpBroadcastPollInterval);
  printf("Multicast Poll Interval: %d\n",
                      sntpCfgData->sntpMulticastPollInterval);
  printf("UnicastServerCount: %d\n",
                      sntpCfgData->sntpUnicastServerCount);
  printf("Active Server Index: %d\n", sntpActiveServer);
 
  for (i = 0; i <L7_SNTP_MAX_SERVERS; i++)
  {
    printf("\tserver[%d] status: %d\n", i,
                      sntpCfgData->server[i].status);
    if ((sntpCfgData->server[i].status == L7_SNTP_SERVER_STATUS_ACTIVE) ||
        (sntpCfgData->server[i].status == L7_SNTP_SERVER_STATUS_NOT_IN_SERVICE))
    {
     printf("\tserver[%d] address: %s\n", i,
                      sntpCfgData->server[i].address);
     printf("\tserver[%d] address type: %d\n", i,
                      sntpCfgData->server[i].addressType);
     printf("\tserver[%d] port: %d\n", i,
                      sntpCfgData->server[i].port);
     printf("\tserver[%d] priority: %d\n", i,
                      sntpCfgData->server[i].priority);
     printf("\tserver[%d] poll: %d\n", i,
                      sntpCfgData->server[i].poll);     
     printf("\tserver[%d] version: %d\n", i,
                      sntpCfgData->server[i].version);

     printf("\tserver[%d] last attempt status: %d\n", i,
                      serverStats[i].lastAttemptStatus);
     printf("\tserver[%d] last attempt time: %u\n", i,
                      serverStats[i].lastAttemptTime);
     printf("\tserver[%d] last update time: %u\n", i,
                      serverStats[i].lastUpdateTime);
     printf("\tserver[%d] num requests: %d\n", i,
                      serverStats[i].unicastServerNumRequests);
     printf("\tserver[%d] num failed requests: %d\n", i,
                      serverStats[i].unicastServerNumFailedRequests);
    }
  } /* end for */

  printf("Last update time: %u\n", sntpStatusData->sntpLastUpdateTime);
  printf("Last attempt time: %u\n", sntpStatusData->sntpLastAttemptTime);
  printf("Last attempt status: %d\n", sntpStatusData->sntpLastAttemptStatus);
  if (sntpStatusData->sntpServerIpAddress.family == L7_AF_INET)
    printf("Server IP address: %s\n",
                      osapiInet_ntoa(sntpStatusData->sntpServerIpAddress.addr.ipv4.s_addr));
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  else if (sntpStatusData->sntpServerIpAddress.family == L7_AF_INET6)
    printf("Server IP address: %s\n",sntpStatusData->sntpServerIpAddress.addr.ipv6.in6.addr8);
#endif
  else
  printf("Server IP address: not set\n");
  printf("Server IP address type: %d\n", sntpStatusData->sntpServerAddressType);
  printf("Server stratum: %d\n", sntpStatusData->sntpServerStratum);
  printf("Server version: %d\n", sntpStatusData->sntpServerVersion);
  printf("Server ref clock id: 0x%x %s\n",
                      sntpStatusData->sntpServerReferenceClockId, refBuf);
  printf("Server mode: %d - %s\n", sntpStatusData->sntpServerMode,
                      sntpServerModeString(sntpStatusData->sntpServerMode));
  printf("Broadcast count: %d\n", sntpStatusData->sntpBroadcastCount);
  printf("Multicast count: %d\n", sntpStatusData->sntpMulticastCount);
  
  printf("\nAuthenticate Mode: %s\n", 
         sntpCfgData->sntpAuthMode ? "Enabled" : "Disabled");
  for (i=0; i < L7_SNTP_MAX_AUTH_KEYS; i++)
  {
    printf("\tkey[%d] status: %d\n", i,
                      sntpCfgData->sntpKeys[i].status);
    if ((sntpCfgData->sntpKeys[i].status == L7_SNTP_SERVER_STATUS_ACTIVE) ||
        (sntpCfgData->sntpKeys[i].status == L7_SNTP_SERVER_STATUS_NOT_IN_SERVICE))
    {
      printf("\tkey[%d] id: %d\n", i,
                        sntpCfgData->sntpKeys[i].id);
      printf("\tkey[%d] value: %s\n", i,
                        sntpCfgData->sntpKeys[i].value);

      printf("\tkey[%d] trusted: %s\n", i,
                        sntpCfgData->sntpKeys[i].trusted ? "Yes" : "No");
    }
  }
  
}

void
sntpDebugMode(int i)
{
  sntpDebug = i;
}

L7_uint32
sntpDebugCurrentTime()
{
  L7_uint32 current = osapiUTCTimeNow();
  L7_char8 buf[128];

  memset(buf, 0, sizeof(buf));

  osapiStrncpySafe(buf, ctime((void *)&current)+4, sizeof(buf));
  printf("Current time: %s", buf);

  return current;
}
#endif /* SNTP_CLIENT_DEBUG */

#ifdef SNTP_CLIENT_UNIT_TEST
/**
*
* SECTION: Unit tests
*
*
*
*/

void sntpUnitTest1()
{
  L7_uint32 ndx;
  /** Check that the server selection can find a
  * valid server with a middle priority.
  */
  sntpClientDefaultsSet(L7_SNTP_CFG_VER_CURRENT);

  sntpServerAdd("192.168.21.51", L7_SNTP_ADDRESS_IPV4, &ndx);
  sntpServerTablePrioritySet(1,1);
  sntpServerAdd("192.168.21.50", L7_SNTP_ADDRESS_IPV4, &ndx);
  sntpServerTablePrioritySet(2,3);

  sntpServerAdd("198.82.162.213", L7_SNTP_ADDRESS_IPV4, &ndx);
  sntpServerTablePrioritySet(3,2);
  sntpServerTableVersionSet(3,3);
  sntpClientModeSet(L7_SNTP_CLIENT_UNICAST);
  sntpStatusShow();
  /** After the first server poll fails, the system should converge
  * on the last server table entry since it has the middle priority.
  * It should not go to the middle entry (with the lowest
  * priority).
  */
}

void sntpUnitTest2()
{
  L7_uint32 ndx;
  /** Test that the server API works for sets. */
  sntpClientDefaultsSet(L7_SNTP_CFG_VER_CURRENT);
  if (sntpServerAdd("198.82.162.213", L7_SNTP_ADDRESS_IPV4, &ndx) != L7_SUCCESS)
    printf("sntp unit test 2-a failed on server add\n");

  if (sntpCfgData->server[0].status != L7_SNTP_SERVER_STATUS_ACTIVE)
    printf("sntp unit test 2-b failed on server status\n");

  if (sntpServerTablePrioritySet(1,2) != L7_SUCCESS)
    printf("sntp unit test 2-c failed on priority set\n");

  if (sntpCfgData->server[0].priority != 2)
    printf("sntp unit test 2-d failed on priority retrieve\n");

  if (sntpServerTableVersionSet(1,3) != L7_SUCCESS)
    printf("sntp unit test 2-e failed on version set\n");

  if (sntpCfgData->server[0].version != 3)
    printf("sntp unit test 2-f failed on priority retrieve\n");

  if (sntpServerAdd("192.168.21.220", L7_SNTP_ADDRESS_IPV4, &ndx) != L7_SUCCESS)
    printf("sntp unit test 2-g failed on server add\n");

  if (sntpServerTablePrioritySet(2,1) != L7_SUCCESS)
    printf("sntp unit test 2-h failed on priority set\n");

  if (sntpServerAdd("192.168.21.221", L7_SNTP_ADDRESS_IPV4, &ndx) != L7_SUCCESS)
    printf("sntp unit test 2-i failed on server add\n");

  if (sntpCfgData->sntpUnicastServerCount != 3)
    printf("sntp unit test 2-j failed on server count\n");

  if (sntpServerAdd("10.0.0.22", L7_SNTP_ADDRESS_IPV4, &ndx) == L7_SUCCESS)
    printf("sntp unit test 2-k improperly succeeded on server add\n");

  if (sntpServerTablePrioritySet(3,2) != L7_SUCCESS)
    printf("sntp unit test 2-l failed on priority set\n");

  if (sntpCfgData->server[2].priority != 2)
    printf("sntp unit test 2-m failed on priority retrieve\n");

  if (sntpClientModeSet(L7_SNTP_CLIENT_UNICAST) != L7_SUCCESS)
    printf("sntp unit test 2-n failed on mode set\n");

  if (sntpCfgData->sntpClientMode != L7_SNTP_CLIENT_UNICAST)
    printf("sntp unit test 2-n failed on mode retrieve\n");

  sntpStatusShow();
}

void sntpUnitTest3()
{
  L7_uint32 ndx;
  /** Check that the select server routine eventually ends up on the last
  *  (and only valid) NTP server address.
  */
  register L7_int32 i;
  sntpClientDefaultsSet(L7_SNTP_CFG_VER_CURRENT);
  sntpServerAdd("10.0.0.5", L7_SNTP_ADDRESS_IPV4, &ndx);
  sntpClientModeSet(L7_SNTP_CLIENT_UNICAST);
  osapiSleep(3); /** Let the little booger start. */
  sntpServerAdd("10.0.0.7", L7_SNTP_ADDRESS_IPV4, &ndx);
  sntpServerAdd("198.82.162.213", L7_SNTP_ADDRESS_IPV4, &ndx);

  for (i = 0; i < 3; i++)
  {
    if (sntpServerDelete("10.0.0.5", L7_SNTP_ADDRESS_IPV4) != L7_SUCCESS)
      printf("sntp unit test 3-b failed on server delete\n");
    osapiSleep(5);
    if (sntpServerAdd("10.0.0.5", L7_SNTP_ADDRESS_IPV4, &ndx) != L7_SUCCESS)
      printf("sntp unit test 3-c failed on server delete\n");
    osapiSleep(5);
  }
}


void sntpUnitTest4()
{
  /** Check the default settings */
  register L7_int32 i;
  sntpClientDefaultsSet(L7_SNTP_CFG_VER_CURRENT);
  if (sntpCfgData->sntpUnicastServerCount != 0)
    printf("sntp unit test 4-a failed on server count default\n");
  if (sntpCfgData->sntpBroadcastPollInterval != L7_SNTP_DEFAULT_POLL_INTERVAL)
    printf("sntp unit test 4-b failed on broadcast poll interval\n");
  if (sntpCfgData->sntpClientMode != L7_SNTP_CLIENT_DISABLED)
    printf("sntp unit test 4-c failed on client mode\n");
  if (sntpCfgData->sntpClientPort != L7_SNTP_DEFAULT_CLIENT_PORT)
    printf("sntp unit test 4-d failed on client port\n");
  if (sntpCfgData->sntpUnicastPollInterval != L7_SNTP_DEFAULT_POLL_INTERVAL)
    printf("sntp unit test 4-e failed on unicast poll interval\n");
  if (sntpCfgData->sntpUnicastPollTimeout != L7_SNTP_DEFAULT_POLL_TIMEOUT)
    printf("sntp unit test 4-f failed on unicast poll timeout\n");
  if (sntpCfgData->sntpUnicastPollRetry != L7_SNTP_DEFAULT_POLL_RETRY)
    printf("sntp unit test 4-g failed on unicast poll retry\n");
  if (sntpCfgData->sntpBroadcastPollInterval != L7_SNTP_DEFAULT_POLL_INTERVAL)
    printf("sntp unit test 4-h failed on unicast poll retry\n");
  if (sntpCfgData->sntpMulticastPollInterval != L7_SNTP_DEFAULT_POLL_INTERVAL)
    printf("sntp unit test 4-i failed on unicast poll retry\n");

  /** Check the default server configuration data */
  for (i = 0; i < L7_SNTP_MAX_SERVERS; i++)
  {
    if (sntpCfgData->server[i].status != L7_SNTP_SERVER_STATUS_DESTROY)
      printf("sntp unit test 4-j failed on server status for server %d\n", i);
  }
}

void
sntpUnitTest5()
{
  sntpClientDefaultsSet(L7_SNTP_CFG_VER_CURRENT);
  sntpClientModeSet(L7_SNTP_CLIENT_MULTICAST);
}

void
sntpUnitTest6()
{
  L7_uint32 ndx;
  sntpClientDefaultsSet(L7_SNTP_CFG_VER_CURRENT);
  sntpServerAdd("192.168.21.50", L7_SNTP_ADDRESS_IPV4, &ndx);
  sntpClientModeSet(L7_SNTP_CLIENT_UNICAST);
}

void
sntpUnitTest7()
{
  sntpClientModeSet(L7_SNTP_CLIENT_BROADCAST);
  sntpClientPortSet(321);
}

void
sntpUnitTest8()
{
  sntpClientModeSet(L7_SNTP_CLIENT_BROADCAST);
  sntpClientPortSet(123);
}
#endif /* SNTP_CLIENT_UNIT_TEST */
