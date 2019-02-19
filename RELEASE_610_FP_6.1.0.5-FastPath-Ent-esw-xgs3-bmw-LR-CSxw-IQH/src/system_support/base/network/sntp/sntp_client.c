 /*********************************************************************
**********************************************************************
* @file     sntp_client.c
*
* @purpose   SNTP Client main implementation
*
* @component SNTP
*
* @comments
* Floating point arithmetic is used in this implementation.
* As it happens, there really don't seem to be many GP cores left
* that do NOT have FP hardware. Of course, if you're running this on a
* 6802, your mileage may vary - significantly.
*
* The algorithm used here is more than the minimum offset calculation
* specified in RFC 2030. The clock filter algorithm and a minimum
* adjustment filter from NTP are also implemented. Some of the filter
* algorithm ideas come from the very latest stuff (e.g. NTPv4).
*
* Please note that the NTP algorithm uses a number of 'global' variables.
* The original intent of the author of the algorithm was to increase
* efficiency by not passing values around on the stack. I believe I
* have documented the 'global' variables quite clearly by both
* describing them accurately and naming them appropriately. In
* addition, I have declared all of them as 'static' in order to
* not pollute the global namespace. Comments have been added in7 some
* places to indicate that the variable is 'global' when the value
* is modified in order to make the operation of the algorithm
* clear.
*
* Provisioning actions do not take effect immediately.
* Some of them, e.g. poll interval, will take effect on the
* 'next' invocation of the SNTP processing loop.
* See threading issues below. The following operations
* MAY take effect immediately:
*
* Set client status (enabled or disabled)
* Set client mode (unicast/broadcast/multicast)
* Set client port
* Add/Delete a server
* Set server priority
* Set server address
* Set server address type
* Set server status
*
* Taking effect immediately means that the current poll is aborted,
* the local socket is closed (and potentially reopened),
* All other operations will take effect at the 'next' unicast or
* broadcast interval. In some cases, that can be a "long time,"
* e.g. 2^14 seconds is about 4.5 hours. One may force a change to take
* effect "immediately" by disabling and re-enabling the SNTP client.
*
* Many of the comments are taken directly from the NTPv4 implementation,
* as is the the clock filter algorithm (not the code) and the minimum
* adjustment filter and NTP parameters.
*
* This code is divided into the following sections: (search for
* keyword "SECTION")
*
* Local Clock Handler - NTP clock algorithm manifest constants and routines
* to handle adjustment of the local clock.
* Also provides routines for obtaining local corrected time
* as well as NTP time. Implements the minimum adjustment filter.
*
* Packet manipulation - provides support for parsing and formating of SNTP packets.
*
* Sntp Time Calculation - implements the SNTP offset and delay calculations
* as well as the clock filter algorithm.
*
* Socket Handling - routines for opening, closing and binding the client socket,
* address resolution, server selection, and multicast group registration.
*
* Packet handlers - routines for sending and receiving NTP packets
*
* Unicast/Broadcast/Multicast/Disabled Mode Selection Handlers -
* routines for selecting unicast/broadcast/multicast/disabled mode.
*
* Dispatch  - the main dispatch loop. The client runs as a separate thread.
*
* Threading Issues: In general, there shouldn't be any. The management
* thread is free to call any of the routines published in sntp_api.h
* asynchronously. All API routines are reentrant and thread-safe.
* Most, if not all, of the API access routines simply return a value
* without the benefit of locking. In these cases, some data items
* may appear inconsistent with other data items if the data accessed
* was in the process of being collected during the packet processing
* operation.
*
* There are NO data items that ever inconsistent in and of themselves (unless
* your CPU happens to implement word size stores and loads in a non-atomic
* fashion).
*
* The reason for this choice of implementation is that should we lock for
* every data access by management, we have the very real possibility of
* introducing significant delays in the clock algorithm and making a
* mess out of some perfectly good timestamps. The actual possibility of
* retrieving inconsistent data is low  (10^-4 to 10^-6) as packet processing
* occurs during a few millseconds of each processing interval. Given
* nominal processing intervals of one minute to over one hour, this seems
* to be a relatively good tradeoff with only minimal potential for
* inconsistencies.
*
* The implementation uses two threads (config and the sntp client).
* There is one queue for configuration updates. Management changes
* (via sntp_api) occur on the data items directly. This can happen
* safely because all data load/stores occur atomically
* and all storage is allocated and initialized with reasonable defaults at
* startup. In addition, error checking throughout the code handles anomolous
* situations gracefully.
*
*
* create 1/8/2004
*
* author Mclendon
*
**********************************************************************/
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
#include "l7_socket.h"
#include "dns_client_api.h"
#include "usmdb_dns_client_api.h"
#include "md5_api.h"

static int sntpDebug=0;

/*
 * SIMPLE_TIME_SYNCH modifies SNTP's behavior to unconditionally
 * accept the latest response from an SNTP server and adjust our time
 * accordingly.
 */

#define SNTP_MD5_DIGEST_LENGTH 16
#define SIMPLE_TIME_SYNCH 1

#define NTP_PRINTF   if (sntpDebug) sysapiPrintf



/* Task level defines */
/* Our task name */
#define L7_SNTP_TASK                     "SNTP"

#define L7_SNTP_STACK_PACKET_LEN          16

/* Our clock stratum - This is fixed. We can never be anything else. */
#define L7_SNTP_DEFAULT_STRATUM              15


/** This is the scaling factor used to make the integer
*   part of a timestamp fractional and the fractional part of a
*   timestamp integer. We need this because we do everything in
*   floating point. The scale factor is 2^32, which is the maximum
*   integer that can fit in a four byte value (e.g. the integer part
*   or the fractional part. Hint - The integer part of a timestamp
*   is four bytes.
*/

#define SNTP_TIMESTAMP_SCALE_FACTOR  4294967296.0

/** Please do not change the next two #defines.
* They are part of the NTP algorithm.
*/

/** Maximum acceptable delay - Discard packets with more than this RTT */
#define SNTP_MAX_DELAY         6

/** Maximum acceptable dispersion */
#define SNTP_MAX_DISPERSION  16.0

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
#define SNTP_MAX_DISTANCE    1.0

/** The ALLAN intercept used to discard "popcorn" noise. */
#define SNTP_ALLAN_INTERCEPT     1500.0

/** Internal scaling factor */
#define SNTP_FWEIGHT             0.5

/** Scale factor for determining discard limit for spikes */
#define SNTP_POPCORN_SCALE_FACTOR       3.0

/** The maximum number of historical timestamps kept
* for the clock filter algorithm. This MUST be a power of two!
 */
#define SNTP_MAX_HISTORY      8

/** The minimum number of historical records that must
* be present for the clock filter.
*/
#define SNTP_MIN_HISTORY      2


/*
*
* Private SNTP Types
*
*
*/

/** A place to hold the interesting data in a packet.
* These are the things that we parse out.
*/

typedef struct sntpPacket_s
{
  L7_uchar8     li;       /** Leap indicator */
  L7_uchar8     version;  /** NTP version */
  L7_uchar8     mode;     /** Server mode */
  L7_uchar8     stratum;  /** Claimed server stratum */
  L7_uchar8     poll;     /** Polling interval */
  L7_char8      precision; /** Precision of server clock - Signed value. */
  L7_int32      referenceId; /** Ref id. */
  L7_double64   reference;  /** Timestamp */
  L7_double64   originate;  /** Timestamp */
  L7_double64   receive;    /** Timestamp */
  L7_double64   transmit;   /** Timestamp */
  L7_double64   current;    /** Timestamp */
  L7_uchar8    *authenticator;
} sntpPacket_t;

/** This is the data saved from received packets and used to calculate UTC. */

struct sntpHistoricalTimestamp_s
{
  L7_double64 dispersion;
  L7_double64 offset;
  L7_double64 delay;
  L7_double64 epoch;    /** Local time this sample was received */
};

/*
* Task and config related data elements.
*
*/

/* A pointer to our copy of the config data.
* Initialized by sntpClientInit.
*/

static struct sntpCfgData_s  * sntpCfgData = L7_NULL;

/* Our mutex to protect access to shared data structures. */
static void * sntpTaskSyncSemaphore = L7_NULL;

/* Our task id */
static L7_int32  sntpClientTaskId = 0;



/** Global Status Parameters that are actual variables.
* (Some are manifest constants).
*/

/** The local date and time (UTC) the SNTP client
* last updated the system clock.
*/
static L7_uint32 sntpLastUpdateTime = 0;

/** The local date and time (UTC) of the last SNTP
* request or receipt of an unsolicited message.
*/
static L7_uint32 sntpLastAttemptTime = 0;

/** The status of the last SNTP request or unsolicited message. */
static L7_SNTP_PACKET_STATUS_t sntpLastAttemptStatus =
                                          L7_SNTP_STATUS_OTHER;

/** The IP Address of the server for the last received valid packet.
* We store this in the received format. The retrieve routine converts it
* into a string.
*/
static L7_ulong32 sntpServerIpAddress = L7_NULL_IP_ADDR;

/** The address type of the SNTP server address for
* the last received valid packet.
*/
static L7_SNTP_ADDRESS_TYPE_t sntpServerAddressType =
                          L7_SNTP_ADDRESS_UNKNOWN;

/** The claimed stratum of the server for the last received valid packet */
static L7_uchar8 sntpServerStratum = 0;

/** The claimed version of the server for the last received valid packet */
static L7_uchar8 sntpServerVersion = 0;

/** The reference clock identifier of the server for the last
* received valid packet */
static L7_uint32 sntpServerReferenceClockId = 0;

/** The mode of the server for the last received valid packet. */
static L7_SNTP_SERVER_MODE_t  sntpServerMode =
                          L7_SNTP_SERVER_MODE_RESERVED;

/**
*  The number of unsolicited broadcast SNTP messages that have been
*  received and processed by the SNTP client since last reboot while the
*  client is operating in broadcast mode.
*/
static L7_uint32 sntpBroadcastCount = 0;

/**
*  The number of unsolicited multicast SNTP messages that have been
*  received and processed by the SNTP client since last reboot while the
*  client is operating in broadcast mode.
*/

static L7_uint32 sntpMulticastCount = 0;


/** This is the array of servers. The unicast server indices range
   from [0..L7_SNTP_MAX_SERVERS-1].
 */

static struct sntpServerStats_s * serverStats = L7_NULL;

/** This is the index of the active unicast server. It will
* be NO_SERVER_SELECTED if unitialized or
* when we are in broadcast mode. Otherwise, it will
* range from [0..L7_SNTP_MAX_SERVERS - 1].
*/

static L7_uint32 sntpActiveServer = NO_SERVER_SELECTED;



/** Calculated or recorded information about the server.
 * These next two items are simply recorded from valid received packets.
 */

/** Total roundtrip delay to the primary reference source.
* Range is negative values of a few milliseconds
* to positive values of several hundred millseconds.
* Updated on each received valid packet.
*/
static L7_double64 peerRootDelay = SNTP_MAX_DELAY;

/** Nominal error relative to primary reference.
* Range from 0 to several hundred milliseconds.
* Updated on each received valid packet
*/
static L7_double64 peerRootDispersion = SNTP_MAX_DISPERSION;

/** These items are calculated from the received packet.
 * They are updated by the clock filter algorithm.
 */

#if ! defined(SIMPLE_TIME_SYNCH)
/** Propogation delay between server and client. */
static L7_double64 peerDelay = SNTP_MAX_DELAY;
#endif

/** System clock offset relative to the server. */
static L7_double64 peerOffset = SNTP_MAX_DISTANCE;

#if ! defined(SIMPLE_TIME_SYNCH)

/** Jitter of the server */
static L7_double64 peerJitter = 0.0;

/** Dispersion of the server */
static L7_double64 peerDispersion = SNTP_MAX_DISPERSION;

#endif
/** Last time we received a packet from the server */
static L7_double64 peerUpdate = 0.0;

/** The epoch of the last "good" timestamp received from any server.
   Note that received timestamps "before" the initialized time will
   never be "good" enough. TODO - Change this to the MIN TIME on a run to
   make certain it works. */

static L7_double64 peerEpoch = 0.0;  /** Updated by the clock filter */


/** The history array stores the last 8 valid historical timestamps.
 *  The history array simulates a fifo queue. hhead indexes the next "empty"
 * slot in the queue.
 */

static struct sntpHistoricalTimestamp_s history[SNTP_MAX_HISTORY];

/** hhead ranges from [0..SNTP_MAX_HISTORY-1] and is used to
* index the historical timestamp array. We choose SNTP_MAX_HISTORY to
* be a power of two in order to avoid using the % operator.
* It's way too expensive (5 clocks on a PPC vs. 1 clock to use the & operator).
*/
#if ! defined(SIMPLE_TIME_SYNCH)


static L7_uint32 hhead;

/** How many times history has been recorded - This item could
* conceptually overflow, but it's very unlikely to happen in my lifetime -
* or yours.
*/
static L7_ulong32 sntpRecordedHistoryCount = 0;

#endif


/** Local variables used for communication - These
* are shared among the main loop and the SOCKET handlers.
*/

/** Our socket. We flag a closed socket with a -1. */
static L7_int32 fd = -1;
/** Client local address. */
static L7_ulong32 localAddr;
/** Current server address. */
static L7_ulong32 remoteAddr;
/** Server address from last received packet. */
static L7_ulong32 recvAddr;

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
  (int)0x4c564c37,    /** LVL7 in hex */
  0.0,
  0.0,
  0.0,
  0.0,
  0.0
};

#if defined(L7_STACKING_PACKAGE)
static void sntpBroadcastTimestamp(void);
#endif

#if ! defined(SIMPLE_TIME_SYNCH)


/*
*
* Math helper routines so we do not need  math.h or libm.
*
*
*/
/**********************************************************************
* @purpose  Find absolute value of x
*
* @param x @b{(input)} argument
*
* @returns absolute value of arg.
* @end
*********************************************************************/

static L7_double64 fabs(L7_double64 x)
{
  return (x > 0.0) ? x : -x;
}
/**********************************************************************
* @purpose  Find max of x or y
*
* @param x @b{(input)} argument
* @param y @b{(input)} argument
*
* @returns maximum of x or y
* @end
*********************************************************************/

static L7_double64 fmax(L7_double64 x, L7_double64 y)
{
  return (x > y) ? x : y;
}

/**********************************************************************
* @purpose  Find sqrt using old newton's method.
*
* @param r @b{(input)} argument
*
* @returns sqrt of arg or 0.0 if arg <= 0.0
* @end
*********************************************************************/

#define EPS   1.0E-8

/* Sqrt using Newton's method. */
static L7_double64 sqrt(L7_double64 r)
{
  L7_double64 delta = 1.0;
  L7_double64 x = r;
  if (r <= 0.0)
     return 0.0;         /* the square root of 0.0 is a special case */
  while ((delta > EPS) ||
         (delta < -EPS))
  {
    delta = (x * x - r) / (2.0 * x);
    x -= delta;
  }
  return x;
}
#endif

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
#if 0
  NTP_PRINTF("sntpTaskLock - semid(%p)\n", sntpTaskSyncSemaphore);
#endif
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
#if 0
  NTP_PRINTF("sntpTaskUnlock - semid(%p)\n", sntpTaskSyncSemaphore);
#endif
  return osapiSemaGive(sntpTaskSyncSemaphore);
}

/**
*  SECTION: Packet manipulation
*
*  These routines provide support for parsing and formating of SNTP packets.
*  Syntactic error checking and some basic semantic error checking
*  is also performed in the parse routine.
*
*/


/** A comment from Dr. Mills (ntp_fp.h):
 *
 * NTP uses two fixed point formats.  The first (l_fp) is the "long"
 * format and is 64 bits long with the decimal between bits 31 and 32.
 * This is used for time stamps in the NTP packet header (in network
 * byte order) ...
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |             Integral Part           |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |             Fractional Part           |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 */


/** Format timestamp data into an SNTP packet.
* Expects data to be char * and timestamp to be double.
* Make the whole timestamp a fraction and then multiply by 256,
* getting the integer part that fits into an 8-bit byte.
* We then subtract off the integer part and repeat the whole
* process again, eight times.
*/

#define FORMAT_TIMESTAMP(data, timestamp)               \
{                                                       \
  L7_int32 offset;                                           \
  L7_double64 t = (timestamp) / SNTP_TIMESTAMP_SCALE_FACTOR; \
  for (offset = 0; offset < 8; offset++)                \
  {                                                     \
    L7_int32 partial;                                   \
    t *= 256.0;                                         \
    partial = (int)t;                                   \
    (data)[offset] = (partial >= 256) ? 255 : partial;  \
    t -= partial;                                       \
  }                                                     \
}

/** Tear apart a timestamp and place it into a double.
* Expects data to be char * and timestamp to be double.
* Places the whole timestamp into a double as an interger,
* then rescales back to the integer and fractional parts.
*/

#define PARSE_TIMESTAMP(data, timestamp)                \
{                                                       \
  L7_int32 offset;                                      \
  L7_double64 t = 0.0;                                  \
  for (offset = 0; offset < 8; offset++)                \
    t = (t * 256.0) + (data)[offset];                   \
  (timestamp) = t / SNTP_TIMESTAMP_SCALE_FACTOR;        \
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
*           for SNTP polls. This is NOT a general purpose NTP packet
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
*         Updates peerRootDelay and peerRootDispersion.
*
* @end
*********************************************************************/

static L7_SNTP_PACKET_STATUS_t sntpPacketParse(sntpPacket_t * data,
                          L7_uchar8 * packet, L7_int32 length)
{

  L7_int32 i;
  L7_double64 rootDelay = 0.0;
  L7_double64 rootDispersion = 0.0;

  /** It's best if this comes first -
  *  It makes our local processing time
  *  part of the RTT calculation.
  */
  data->current = osapiNTPTimeGet();
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
  rootDelay = (L7_double64)(packet[SNTP_DELAY_OFFSET] & 0x7f);
  rootDelay = rootDelay * 256.0 + packet[SNTP_DELAY_OFFSET + 1];
  rootDelay = rootDelay * 256.0 + packet[SNTP_DELAY_OFFSET + 2];
  rootDelay = rootDelay * 256.0 + packet[SNTP_DELAY_OFFSET + 3];
  rootDelay = rootDelay / 65536.0;
  if (packet[SNTP_DELAY_OFFSET] && 0x80)  /** Handle negative values */
    rootDelay = -rootDelay;

  /** Get the dispersion - note that this field is only 4 bytes and is unsigned */
  rootDispersion = 0.0;
  for (i = 0; i < 4; i++)
  {
    rootDispersion = rootDispersion * 256.0 +
        packet[SNTP_DISPERSION_OFFSET + i];
  }
  rootDispersion = rootDispersion / 65536.0;

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
    return L7_SNTP_STATUS_KISS_OF_DEATH;

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
  if ((rootDelay < 0.0) ||
      (rootDelay > SNTP_MAX_DELAY) ||
      (rootDispersion < 0.0) ||
      (((rootDelay / 2.0) + rootDispersion) >= SNTP_MAX_DISPERSION))
    return L7_SNTP_STATUS_OTHER;

  /** Update the system status */
  peerRootDelay = rootDelay;
  peerRootDispersion = rootDispersion;
  sntpServerMode = data->mode;
  sntpServerReferenceClockId = data->referenceId;
  sntpServerStratum = data->stratum;
  sntpServerVersion = data->version;

  NTP_PRINTF("SNTP Packet:\n");
  NTP_PRINTF(" delay=%f\n", rootDelay);
  NTP_PRINTF(" dispersion=%f\n", rootDispersion);
  NTP_PRINTF(" li=%x\n", data->li);
  NTP_PRINTF(" version=%x\n", data->version);
  NTP_PRINTF(" mode=%x\n", data->mode);
  NTP_PRINTF(" stratum=%d\n", data->stratum);
  NTP_PRINTF(" poll=%d\n", data->poll);
  NTP_PRINTF(" precision=%x\n", data->precision);
  NTP_PRINTF(" referenceId=%d\n", data->referenceId);
  NTP_PRINTF(" reference=%f\n", data->reference);
  NTP_PRINTF(" originate=%f\n", data->originate);
  NTP_PRINTF(" receive=%f\n", data->receive);
  NTP_PRINTF(" transmit=%f\n", data->transmit);
  NTP_PRINTF(" current=%f\n", data->current);

  return L7_SNTP_STATUS_SUCCESS;
}


/**
* SECTION: Sntp Time Calculation - implements the SNTP
*                 offset and delay calculations
*                 as well as the clock filter algorithm.
*
*/


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
*   its clock in NTP timestamp format. The following table summarizes the
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

static void sntpRTTCalculate(sntpPacket_t * packet, L7_double64 * delay,
                                        L7_double64 * offset)
{
  L7_double64 t1 = packet->originate;
  L7_double64 t2 = packet->receive;
  L7_double64 t3 = packet->transmit;
  L7_double64 t4 = packet->current;

  if ((sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_BROADCAST) ||
      (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_MULTICAST))
  {
    *delay = 0.0;
    *offset = t3 - t4; /** Best we can do. */
    NTP_PRINTF("sntpRTTCalculate: broadcast/multicast mode - delay %g offset %g\n",
                    *delay, *offset);
  }
  else if (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_UNICAST)
  {
    *delay = (t4 - t1) - (t2 - t3);
    *offset = ((t2 - t1) + (t3 - t4)) * 0.5;

    NTP_PRINTF("sntpRTTCalculate: unicast mode - delay %g offset %g\n",
                    *delay, *offset);
  }
}
#if ! defined(SIMPLE_TIME_SYNCH)

/**********************************************************************
* @purpose  Record a validated timestamp and return index of
*           record just written.
*
* @param    pkt @b{(input)} Pointer to sntpPacket data structure
*                           (contains anything of interest)
* @param    dispersion @b{(input)} the calculated sample dispersion
* @param    offset @b{(input)} the calculated difference between
*                             UTC and the local clock.
* @param    delay @b{(input)} the round trip time for the poll of the server
*
* @returns  Returns index of record just recorded.
*
* @notes    Overwrites oldest timestamp if history array is full.
*           Updates hhead to point ot the next sample to write.
* @end
*********************************************************************/
static L7_uint32 sntpTimestampRecord(sntpPacket_t * pkt,
             L7_double64 dispersion, L7_double64 offset, L7_double64 delay)
{
  L7_uint32 rval = hhead;
  sntpRecordedHistoryCount++;
  history[hhead].dispersion = dispersion;
  history[hhead].offset = offset;
  history[hhead].delay = delay;
  history[hhead].epoch = pkt->current;
  NTP_PRINTF("sntpTimestampRecord: slot %d epoch %g dispersion %g delay %g offset %g\n",
                    hhead, pkt->current, dispersion, delay, offset);
  hhead = (hhead + 1) & (SNTP_MAX_HISTORY - 1);
  return rval;
}

/**********************************************************************
* @purpose  Filter (and record) the timestamp based on the NTPv4
*           clock filter algorithm.
*
* @param    pkt @b{(input)} Pointer to sntpPacket data structure
*                           (contains anything of interest)
* @param    rcvDelay @b{(input)} the calculated receive delay
* @param    rcvOffset @b{(input)} the calculated receive offset
*
* @returns  Returns 1 if values may be used to update the local
*           clock, 0 otherwise.
*
* @notes    Algorithm adapted from the program listing
*           in Appendix I of RFC 1305 and the code in the NTPv4 distribution.
*           Computes peerOffset, peerDelay, and possibly peerJitter and peerEpoch.
*           Commentary copied directly from Dr. Mills.
*
* @end
*********************************************************************/

L7_int32 sntpTimestampFilter (sntpPacket_t * pkt, L7_double64 rcvDelay,
                              L7_double64 rcvOffset)
{

  /** Loop variables */
  register L7_uint32 i;
  register L7_uint32 j;
  register L7_uint32 tmp;
  /** Locally cached values */
  L7_double64 currentTime = pkt->current;
  /** Index of the recorded timestamp. */
  L7_uint32 currentIndex;
  /** Index of the first sample failing the dispersion test. */
  L7_uint32 maxAcceptableSamples = 0;
  /** Index of the history sample with the lowest dispersion,
  * i.e. ndx[0] after sorting.
  */
  L7_uint32 lowestDelayIndex = 0;
  L7_double64  dst[SNTP_MAX_HISTORY]; /** calculated distance vector */
  L7_uint32 ndx[SNTP_MAX_HISTORY];    /** index vector */
  /** Index of the first invalid sample in the history array.
  *   Really only useful if we've collected fewer than MAX_HISTORY samples.
  */
  L7_uint32 maxHistory;

  L7_double64  dispersionBiasAdjustment;

  L7_double64 jitter;


  L7_double64 sampleDispersion = SNTP_CLOCK_PHI *
      fmax((pkt->current - pkt->originate), 1.0 / (1 << -(L7_SNTP_LOCAL_CLOCK_PRECISION)));
  /** Calculate the dispersion of the received packet.
  * Ok if this is > MAX_DISPERSION (for now).
  */
  L7_double64 dispersion = sampleDispersion +
                1.0 / (1 << -pkt->precision) +
                1.0 / (1 << -L7_SNTP_LOCAL_CLOCK_PRECISION);

  if (dispersion > SNTP_MAX_DISPERSION)
    dispersion = SNTP_MAX_DISPERSION;

  /** Record the received data with the calculated dispersion. */
  currentIndex = sntpTimestampRecord(pkt, dispersion, rcvOffset,
                            (rcvDelay < 0.0) ? -rcvDelay : rcvDelay);

  /** Find the index of the first invalid sample. */
  maxHistory = (sntpRecordedHistoryCount < SNTP_MAX_HISTORY) ?
                            hhead : SNTP_MAX_HISTORY;


  /** Comment from Dr. Mills:
   * Update dispersions received since the last update and at the same
   * time initialize the distance and index lists. The distance
   * list uses a compound metric. If the sample is valid and
   * younger than the minimum Allan intercept, use delay;
   * otherwise, use biased dispersion.
   */
  dispersionBiasAdjustment = min (SNTP_CLOCK_PHI * (currentTime - peerUpdate),
                                          SNTP_MAX_DISPERSION);
  peerUpdate = pkt->current;

  NTP_PRINTF("sntpTimestampFilter: currentIndex %d maxHistory %d sample "
  "dispersion %g dispersion %g dispersion factor %.12g precision %d root dispersion %g\n",
            currentIndex, maxHistory, sampleDispersion, dispersion,
            dispersionBiasAdjustment, (int)pkt->precision, peerRootDispersion);

  /** Initialize the dst and ndx arrays. Age the dispersions in the history. */
  for (i = 0; i < maxHistory; i++)
  {
    /** Save the index in an array so we can sort an array of integer indices */
    ndx[i] = i;

    /**  Weight older timestamps as less accurate. */
    if (i != currentIndex)
      history[i].dispersion += dispersionBiasAdjustment;

    /** Take care of records with dispersion > MAX_DISPERSION. */
    if (history[i].dispersion >= SNTP_MAX_DISPERSION)
    {
      history[i].dispersion = SNTP_MAX_DISPERSION;
      dst[i] = SNTP_MAX_DISPERSION;
    }
    else if ((peerUpdate - history[i].epoch) > SNTP_ALLAN_INTERCEPT)
    {
      /** This timestamp is between the ALLAN intercept and MAX_DISPERSION.
         Use biased dispersion. */
      dst[i] = SNTP_MAX_DISPERSION + history[i].dispersion;
    }
    else
    {
      /** Less than Allan intercept, use delay. */
      dst[i] = history[i].delay;
    }
    NTP_PRINTF("sntpTimeStampFilter: slot %d delay %.8g "
          "epoch %.8g dispersion %.8g offset %.8g\n", i, dst[i],
           history[i].epoch, history[i].dispersion, history[i].offset);
  }

/** Sort the list in increasing magnitude of the dispersion/delay.
*     Note we are simply moving around the indices - much
*     faster than the original algorithm. (Bubble sort over 8 elements is ok)
*/
  for (i = 0; i < maxHistory; i++)
  {
    for (j = i + 1; j < maxHistory; j++)
    {
      if (dst[ndx[i]] > dst[ndx[j]])
      {
        tmp = ndx[i];
        ndx[i] = ndx[j];
        ndx[j] = tmp;
      }
    }
  }

  lowestDelayIndex = ndx[0];

  /** Comment from Dr. Mills:
   * Prune the distance list to samples less than MAXDISTANCE, but
   * keep at least two valid samples for peerJitter calculation.
   *
   * At this point, dst[ndx[i]] is sorted in increasing order of dispersion.
   * We walk the sorted list from the beginning looking for samples meeting
   * our criteria. We want samples to always be less than MAX_DISPERSION.
   * We would like the samples to be less than MAX_DISTANCE. However, we will
   * accept two samples greater than MAX_DISTANCE if they are less than
   * MAX_DISPERSION.
   */
  for (i = 0; i < maxHistory; i++)
  {
    /** If the sample is beyond MAX_DISPERSION, we're done */
    if (dst[ndx[i]] >= SNTP_MAX_DISPERSION)
      break;
    /** If we have at least two samples and we've found a record > MAX_DISTANCE,
    * stop collecting samples.
    */
    if ((dst[ndx[i]] >= SNTP_MAX_DISTANCE) && (maxAcceptableSamples > 2))
      break;

    maxAcceptableSamples++;
  }


 /** At this point, the values in dst[ndx[0..maxAcceptableSamples]]
  * contain valid values.
  */

  /** Comment from Dr. Mills:
   * Compute the peer dispersion and peer jitter squares. The dispersion
   * is weighted exponentially by SNTP_FWEIGHT (0.5) so it is
   * normalized close to 1.0. The peer jitter is the mean of the square
   * differences relative to the lowest delay sample. If no
   * acceptable samples remain in the shift register, quietly
   * tiptoe home leaving only the dispersion.
   */
  /** Note that we compute the dispersion over all received history.
   *  We compute the peer jitter only over the range [0..maxAcceptableSamples].
   */
  jitter = 0.0;
  /** We are reusing this variable to calculate the root dispersion squared. */
  dispersion = 0.0;
  for (i = 0; i < maxHistory; i++)
  {
    j = ndx[i];
    dispersion = SNTP_FWEIGHT * (dispersion + history[j].dispersion);
    if (i <= maxAcceptableSamples)
      jitter += history[j].offset - history[lowestDelayIndex].offset;
  }

  NTP_PRINTF("sntpTimestampFilter: maxAcceptableSamples %d"
              " lowestDelayIndex %d maxHistory %d\n",
                    maxAcceptableSamples, lowestDelayIndex, maxHistory);

  /** Comment from Dr. Mills:
   * If no acceptable samples exist, quietly tiptoe home leaving only
   * the summed dispersion. Otherwise, save the offset, delay and
   * peer jitter average. Note the peer jitter
   * must not be less than the system precision.
   */
  peerDispersion = dispersion;
  if (maxAcceptableSamples == 0)
  {
    return 0; /** Cannot use this sample */
  }
  /** Update the status of the peer */
  peerOffset = history[lowestDelayIndex].offset;
  peerDelay = history[lowestDelayIndex].delay;


  /** Average jitter */
  if (maxAcceptableSamples >= SNTP_MIN_HISTORY)
    jitter /= (maxAcceptableSamples - 1);

  /** Note that jitter is stored as a square - use sqrt to
  * get the real jitter on presentation.
  */
  peerJitter = fmax(jitter, L7_SNTP_LOCAL_CLOCK_PRECISION
                  * L7_SNTP_LOCAL_CLOCK_PRECISION);

  NTP_PRINTF("sntpTimestampFilter: peer jitter %g peer offset "
                  "%g peer delay %g peerDispersion %g\n",
                    sqrt(peerJitter), peerOffset, peerDelay,
                    peerDispersion);


  /**
   * If the difference between the last offset and the current one
   * exceeds the peer jitter by POPCORN_LIMIT and the interval since the
   * last update is less than twice the system poll interval,
   * consider the update a popcorn spike and ignore it.
   */
  if ((maxAcceptableSamples >= SNTP_MIN_HISTORY) &&
     (fabs((peerOffset-history[lowestDelayIndex].offset)) >
            (SNTP_POPCORN_SCALE_FACTOR * sqrt(peerJitter))) &&
     (long)(history[lowestDelayIndex].epoch - peerEpoch) <
            (1 << sntpCfgData->sntpUnicastPollInterval))
  {
    NTP_PRINTF("sntpTimestampFilter: SPIKE! offset delta %12g epoch %12g peerEpoch %12g\n",
        fabs((peerOffset - history[lowestDelayIndex].offset)),
        history[lowestDelayIndex].epoch, peerEpoch);
    /** Spike occurred */
    return 0;
  }

  /**
   * Record the new peer epoch.
   */
  peerEpoch = history[lowestDelayIndex].epoch;

  return 1; /** Got a valid sample. */
}
#endif
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

  if (fd >= 0)
  {
    /* Close out all multicast/broadcast associations */
    /** fill in the argument structure to leave the multicast group */
    /** initialize the multicast address to leave */
    ipMreq.imr_multiaddr.s_addr = osapiHtonl(osapiInet_addr(SNTP_MULTICAST_GROUP_ADDRESS));
    /** unicast interface addr from which to receive the multicast packets */
    ipMreq.imr_interface.s_addr = osapiHtonl(INADDR_ANY);
    /** set the socket option to join the MULTICAST group */
    (void)osapiSetsockopt (fd, IPPROTO_IP, L7_IP_DROP_MEMBERSHIP, (char *)&ipMreq, sizeof(ipMreq));
    /* TODO - add this to osapi. */
    shutdown(fd, 2); /** Mark it for complete closure */
    osapiSocketClose(fd);
    fd = -1;
  }
  NTP_PRINTF("socket close - socket %d\n", fd);
  remoteAddr = L7_NULL_IP_ADDR;
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  This routine opens (or reopens) the local socket and binds to it.
*
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
  L7_uint32 on = 1;
  L7_uint32 off = 0;
  L7_sockaddr_in_t baddr;

  sntpLocalSocketClose();

  /** Don't open socket until we're enabled. */
  if (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_DISABLED)
    return L7_SUCCESS;

  /** Open a socket and begin querying the server. */
  if (osapiSocketCreate(L7_AF_INET, L7_SOCK_DGRAM, 0, &fd) != L7_SUCCESS)
  {
    fd = -1;
    NTP_PRINTF("socket open failed - errno %d\n", osapiErrnoGet());
    return L7_ERROR;
  }
  NTP_PRINTF("socket open - socket %d\n", fd);

  /** Bind to local port */
  localAddr = 0;

  if ((sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_UNICAST) ||
      (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_MULTICAST))
  {
    /** Unicast mode, bind to local unicast address */
    localAddr = L7_INADDR_ANY;
    baddr.sin_port = L7_SNTP_DEFAULT_UNICAST_CLIENT_PORT;
  }
  else if (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_BROADCAST)
  {
    /** Broadcast mode, bind to local broadcast address */
    localAddr = L7_INADDR_ANY;
    baddr.sin_port = osapiHtons(sntpCfgData->sntpClientPort);
  }
  else
  {
    NTP_PRINTF("SNTP: Inconsistent mode\n");
    osapiSocketClose(fd);
    fd = -1;
    return L7_ERROR;
  }

  baddr.sin_family = L7_AF_INET;
  baddr.sin_addr.s_addr = osapiHtonl(localAddr);

  if (osapiSocketBind(fd, (L7_sockaddr_t *)&baddr,sizeof(baddr)) != L7_SUCCESS)
  {
    NTP_PRINTF("Can't bind socket to local address 0x%lx port %d - errno %d\n",
        localAddr, sntpCfgData->sntpClientPort, osapiErrnoGet());
    return L7_ERROR;
  }
  if (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_UNICAST)
  {
    if (osapiSetsockopt(fd, L7_SOL_SOCKET, L7_SO_BROADCAST,(L7_char8 *) &off, sizeof(off))== L7_FAILURE)
    {
      LOG_MSG("sntpLocalSocketOpen: osapiSetsockopt SO_BROADCAST off failed");
      return L7_ERROR;
    }
  }

  else if (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_MULTICAST)
  {
    struct L7_ip_mreq_s  ipMreq;

    if (osapiSetsockopt(fd, L7_SOL_SOCKET, L7_SO_BROADCAST,(L7_char8 *) &on, sizeof(on))== L7_FAILURE)
    {
      LOG_MSG("sntpLocalSocketOpen: osapiSetsockopt SO_BROADCAST on failed");
      return L7_ERROR;
    }
    /** fill in the argument structure to join the multicast group */
    /** initialize the multicast address to join */
    ipMreq.imr_multiaddr.s_addr = osapiHtonl(osapiInet_addr(SNTP_MULTICAST_GROUP_ADDRESS));
    /** unicast interface addr from which to receive the multicast packets */
    ipMreq.imr_interface.s_addr = osapiHtonl(INADDR_ANY);
    /** set the socket option to join the MULTICAST group */
    if (osapiSetsockopt (fd, IPPROTO_IP, L7_IP_ADD_MEMBERSHIP,
                          (char *)&ipMreq, sizeof(ipMreq)) != L7_SUCCESS)
    {
      LOG_MSG("SNTP: Can't join multicast group - errno %d\n", osapiErrnoGet());
      return L7_ERROR;
    }
  }
  else if (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_BROADCAST)
  {
    if (osapiSetsockopt(fd, L7_SOL_SOCKET, L7_SO_BROADCAST,(L7_char8 *) &on, sizeof(on))== L7_FAILURE)
    {
      LOG_MSG("sntpLocalSocketOpen: osapiSetsockopt SO_BROADCAST on failed");
      return L7_ERROR;
    }
  }
  return L7_SUCCESS;
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
  L7_uint32 numReads = 0;
  L7_int32 rc;
  L7_char8 buf;
  fd_set readFds;
  L7_int32 rcvLen;
  L7_sockaddr_in_t raddr;
  L7_uint32 from_len = sizeof(raddr);

    /** File descriptor is valid? */
  while (numReads++ < 64)
  {

    FD_ZERO(&readFds);
    if (fd < 0)
      return;
    FD_SET(fd, &readFds);
    /* 0 timeout is infinite wait */
    rc = osapiSelect(fd+1, &readFds, L7_NULL, L7_NULL, 0, 1);
    if (rc <= 0)
      return;
    if (fd < 0)
      return;
    /** We just grab the first byte - the socket will discard the
    * rest of the datagram.
    */
    rc = osapiSocketRecvfrom(fd, &buf, 1, 0, (L7_sockaddr_t *)&raddr,&from_len, &rcvLen);
    if (rc != L7_SUCCESS)
      return;
    NTP_PRINTF("sntpFlushReceiveBuffer: flush %d rc(%d)\n", numReads, rc);
  }
  LOG_MSG("sntpFlushReceiveBuffer: flushed %d messages. Possible attack in progress!\n", numReads);

}

/**********************************************************************
* @purpose  Resolve an address into it's internal representation.
*
* @param    atype @b{(input)} address type (unknown, IPv4, DNS)
* @param    address @b{(input)} address string to resolve
*
* @returns  IP address (L7_int32)
*
* @notes    Returns 0 if unable to resolve address.
*           Potentially invokes DNS to resolve address
*
* @end
*********************************************************************/

static L7_int32 sntpResolveAddress(L7_SNTP_ADDRESS_TYPE_t atype,
                                  L7_char8 * address)
{
  L7_char8 hostname[DNS_DOMAIN_NAME_SIZE_MAX];
  L7_uint32 ipAddr = 0;
  dnsClientLookupStatus_t  status;

  if (atype == L7_SNTP_ADDRESS_DNS)
  {
    if(usmDbDNSClientNameLookup(address, &status, hostname, &ipAddr) == L7_SUCCESS)
    {
      return ipAddr;
    }
  }
  else if (atype == L7_SNTP_ADDRESS_IPV4)
  {
      return osapiInet_addr(address);
  }
  return 0;
}


/**********************************************************************
* @purpose  Choose the next available unicast server based on priority.
*
* @returns  none
*
* @notes    Always clears remoteAddr.
*           This routine MUST only be called under lock.
*           Sets remoteAddr if successful.
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

  remoteAddr = L7_NULL_IP_ADDR;
  NTP_PRINTF("unicastServerSelect: server count - %d\n", sntpCfgData->sntpUnicastServerCount);

  if (sntpCfgData->sntpUnicastServerCount < 1)
    return;

  memset((void*)address_dummy, L7_NULL, sizeof(address_dummy));

  if (sntpActiveServer == NO_SERVER_SELECTED)
  {
    /** Start at front of list. */
    priority = SNTP_MIN_SERVER_PRIORITY;
  }
  else
  {
    /** Move down the list from where we are. */
    priority = sntpCfgData->server[sntpActiveServer].priority;
  }

  sNdx = sntpActiveServer;
  while (priority <= SNTP_MAX_SERVER_PRIORITY)
  {
    NTP_PRINTF("unicastServerSelect: priority - %d\n", priority);
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
        peerEpoch = 0.0;
        NTP_PRINTF("sntpServerSelect: selected server %s with priority %d at index %d\n",
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
        peerEpoch = 0.0;
        NTP_PRINTF("sntpServerSelect: selected server %s "
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
  NTP_PRINTF("sntpServerSelect: no server selected\n");
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
* SECTION: Packet handlers - routines for sending and receiving NTP packets
*
*/

/**********************************************************************
* @purpose  Send a poll packet to the server as specified by the
*           sntpActiveServer index using the current selected server
*           (one must be selected) and the unicast poll interval.
*           Updates last attempt time. Simply returns without sending
*           anything if the sntpActiveServer index is invalid.
*           Fails if socket specified by fd is not open.
*
* @returns  none
*
* @notes    Must be called with client lock held. Accesses sntpActiveServer.
*
* @end
*********************************************************************/

static void sntpPollSend()
{
  L7_uint32 bytesSent;
  L7_char8  outgoingPacket[SNTP_POLL_PACKET_LEN];
  L7_sockaddr_in_t saddr;
  L7_uint32 length = SNTP_PKT_HEADER_LEN;

  /** Don't bother with bad addresses.
  * If fd == -1, we will just fail - that's ok
  */
  if (sntpActiveServer == NO_SERVER_SELECTED)
    return;

  remoteAddr = sntpResolveAddress(sntpCfgData->server[sntpActiveServer].addressType,
                                      sntpCfgData->server[sntpActiveServer].address);
  if (remoteAddr <= L7_NULL_IP_ADDR)
    return;
  /** Set the transmit timestamp so that we can estimate RTT. (Recommended) */
  sntpQuery.current = sntpQuery.transmit = osapiNTPTimeGet();
  sntpQuery.version = sntpCfgData->server[sntpActiveServer].version;
  sntpQuery.poll = sntpCfgData->sntpUnicastPollInterval;
  sntpPacketFormat(outgoingPacket, SNTP_POLL_PACKET_LEN, &sntpQuery);
  /** Send message. */
  if (fd < 0)
    return;

  saddr.sin_family = L7_AF_INET;
  saddr.sin_port = osapiHtons( sntpCfgData->server[sntpActiveServer].port);
  saddr.sin_addr.s_addr = osapiHtonl(remoteAddr);
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

  if (osapiSocketSendto(fd, outgoingPacket, length, 0,
                        (L7_sockaddr_t *)&saddr,sizeof(saddr),
                        &bytesSent) != L7_SUCCESS)
  {
    NTP_PRINTF("Send failed - errno %d\n", osapiErrnoGet());
    LOG_MSG("sntpPollSend: send failed - errno %d\n", osapiErrnoGet());
  }
  /** Update attempt on transmit - implies unicast mode. */
  serverStats[sntpActiveServer].unicastServerNumRequests++;
  serverStats[sntpActiveServer].lastAttemptTime = sntpLastAttemptTime = osapiUTCTimeNow();
}

/**********************************************************************
* @purpose  Process a response packet
*
* @param    the address of the server (we cache this in a parameter)
* @param    the response bytes
* @param    the number of response bytes
*
* @returns  none
*
* @notes    This routine will invoke the parsing of the packet and
*           time processing if the packet parses nicely.
*           Somes stats on the packet are collected here.
*           On an error in unicast mode, new server selection
*           may be invoked.
*
* @end
*********************************************************************/
static void sntpResponseProcess(L7_ulong32 recvAddr, L7_short16 family,
                            L7_char8 * response, L7_int32 responseLen)
{
  L7_double64 delay;
  L7_double64 offset;
  sntpPacket_t ntpData;
  #if ! defined(SIMPLE_TIME_SYNCH)
  static L7_BOOL firstTime = L7_TRUE;
  #endif

  memset(&ntpData, 0, sizeof(ntpData));

  /** Check that we weren't disabled while waiting */

  ntpData.referenceId = 0; /* compiler happy */

  sntpLastAttemptStatus = sntpPacketParse (&ntpData, response, responseLen);

  sntpTaskLock(); /** Need to lock here because we access sntpActiveServer */

  if (sntpLastAttemptStatus == L7_SNTP_STATUS_SUCCESS)
  {
    L7_RC_t updateStatus;
    delay = 0.0;
    offset = 0.0;

    /* check authentication if enabled */
    if (sntpCfgData->sntpAuthMode == L7_TRUE)
    {
      L7_uint32 keyid = 0;
      if (ntpData.authenticator == L7_NULLPTR)
      {
        LOG_MSG("SNTP: discarding unauthenticated packet from %s", osapiInet_ntoa(recvAddr));
        sntpTaskUnlock();
        return;
      }
      keyid = osapiNtohl(*(L7_uint32 *)(ntpData.authenticator));
      if (keyid == 0)
      {
        LOG_MSG("SNTP: discarding unauthenticated packet from %s", osapiInet_ntoa(recvAddr));
        sntpTaskUnlock();
        return;
      }
      if (sntpAuthVerify(keyid, 
                         response, 
                         ntpData.authenticator - (L7_uchar8 *)response,
                         ntpData.authenticator + sizeof(L7_uint32)) != L7_SUCCESS)
      {
        LOG_MSG("SNTP: authentication failed for packet from %s, keyid = %i", 
                osapiInet_ntoa(recvAddr), keyid);
        sntpTaskUnlock();
        return;
      }
    }

    /** Atomic copy operation for last received 'valid' packet */
    /** Note that we do not play with the byte order. */
    sntpServerIpAddress = recvAddr;
    sntpServerAddressType = (family == L7_AF_INET) ?
            L7_SNTP_ADDRESS_IPV4 : L7_SNTP_ADDRESS_UNKNOWN;

    /** Handle errors here */
    sntpRTTCalculate(&ntpData, &delay, &offset);
    /** Check that RTT is not greater than max delay??? */
    if (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_UNICAST)
    {
      /* Modified to accept the first Unicast packet recieved without filtering.
         This allows the current time to be adjusted in the case that the difference
         between the times is so great, that it would ordinarily be thrown out. */
   #if ! defined(SIMPLE_TIME_SYNCH)
      if (firstTime == L7_TRUE)
      {
   #endif

        peerUpdate = ntpData.current;
        peerOffset = offset;

   #if ! defined(SIMPLE_TIME_SYNCH)

        firstTime = L7_FALSE;
      }
      else
      {
      sntpTimestampFilter(&ntpData, delay, offset);
      }
   #endif
      updateStatus = osapiNTPTimeSet(peerUpdate, peerOffset);
      sntpLastUpdateTime = osapiUTCTimeNow();
      if (sntpActiveServer != NO_SERVER_SELECTED)
      {
        serverStats[sntpActiveServer].lastUpdateTime = sntpLastUpdateTime;
        serverStats[sntpActiveServer].lastAttemptStatus = sntpLastAttemptStatus;
      }
      if (updateStatus == L7_SUCCESS)
      {
        /* Need to add/update conversion routines in osapi */
        L7_char8 * buf = ctime((void *)&sntpLastUpdateTime);
        buf[24] = '\0';
		L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_SNTP_COMPONENT_ID, "SNTP: system clock synchronized "
			           "on %s UTC. Indicates that SNTP has successfully synchronized the time of"
					   " the box with the server.", buf); 
      }
#if defined(L7_STACKING_PACKAGE)
      sntpBroadcastTimestamp();
#endif

    }
    else
    {
      updateStatus = osapiNTPTimeSet(ntpData.current, offset);
      sntpLastUpdateTime = osapiUTCTimeNow();

      NTP_PRINTF("SNTP: Adjusting clock: peerUpdate=%f, offset=%f\n", peerUpdate, offset);

      if (updateStatus == L7_SUCCESS)
      {
        /* Need to add/update conversion routines in osapi */
        L7_char8 * buf = ctime((void *)&sntpLastUpdateTime);
        buf[24] = '\0';
		L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_SNTP_COMPONENT_ID, "SNTP: system clock synchronized "
			           "on %s UTC. Indicates that SNTP has successfully synchronized the time of"
					   " the box with the server.", buf);
        NTP_PRINTF("SNTP: osapiUTCTimeNow = %d -> %s\n", osapiUTCTimeNow(), buf);
      }
      else
      {
        NTP_PRINTF("SNTP: Could not update system time\n");
      }
#if defined(L7_STACKING_PACKAGE)
      sntpBroadcastTimestamp();
#endif
    }
    /** We update broadcast on receive, unicast on send */
    if (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_BROADCAST)
    {
      sntpBroadcastCount++;
    }
    else if (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_MULTICAST)
    {
      sntpMulticastCount++;
    }
  }
  sntpTaskUnlock();
}


/**********************************************************************
* @purpose  Update the local clock from broadcast packet sent by
*           the management unit
*
* @param    msg @b{(input)} a timestamp packet received from management unit.
* @param    len @b{(input)} the length of the packet (16).
*
* @returns  L7_SUCCESS
*
* @notes    This routine takes a SNTP packet broadcast over the stack
*           and handles it.
*
* @end
*********************************************************************/

#if defined(L7_STACKING_PACKAGE)
void sntpHandleStackBroadcast(L7_uchar8 * msg, L7_uint32 len)
{
  L7_double64 timestamp;
#if 0
/* At some point, stacking will send the correct length */
  if (len == L7_SNTP_STACK_PACKET_LEN)
  {
#endif
    L7_double64 now = osapiNTPTimeGet();
    PARSE_TIMESTAMP(msg, timestamp);
    /* Simply sanity check */
    if (timestamp >= L7_SNTP_DISCARD_TIME)
      osapiNTPTimeSet(now, timestamp - osapiNTPTimeGet());
#if 0
  }
#endif
}
#endif

/**********************************************************************
* @purpose  Send a broadcast packet.
*
* @returns  L7_SUCCESS
*
* @notes    This routine broadcast a timestamp packet over the stack.
*
* @end
*********************************************************************/


#if defined(L7_STACKING_PACKAGE)
static void sntpBroadcastTimestamp()
{
  if (sysapiHpcTopOfStackGet() == SYSAPI_STACK_ROLE_MANAGEMENT_UNIT)
  {
    L7_char8  buf[L7_SNTP_STACK_PACKET_LEN];
    L7_double64 now = osapiNTPTimeGet();
    FORMAT_TIMESTAMP(buf, now);
    /* Hope this gets to everybody. */
    if (sysapiHpcBroadcastMessageSend(L7_SNTP_COMPONENT_ID,
                  L7_SNTP_STACK_PACKET_LEN, buf) != L7_SUCCESS)
      LOG_MSG("sntpBroadcastTimestamp: error on broadcat send to stack.");
  }
}
#endif

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
    remoteAddr = L7_NULL_IP_ADDR;
    sntpLocalSocketClose();
    peerEpoch = 0.0;
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
  remoteAddr = L7_NULL_IP_ADDR;
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
  /** Grab this in one shot - no thread issues please. */
  L7_ulong32 ipAddress = sntpServerIpAddress;
  if (address == L7_NULL)
    return L7_ERROR;
  /** Format the string into the passed in pointer. */
  strncpy(address, osapiInet_ntoa(ipAddress), L7_SNTP_MAX_ADDRESS_LEN);
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
  *type = sntpServerAddressType;
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
  *stratum = sntpServerStratum;
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Get the reference clock identifier of the server for the
*           last received valid packet.
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
  if (sntpServerReferenceClockId == 0)
  {
    *refClockId = '\0';
    return L7_SUCCESS;
  }
  if ((sntpServerVersion == 3) || (sntpServerVersion == 4))
  {
    if (sntpServerStratum == 0)
    {
      sprintf(refClockId, "NTP Bits: 0x%.08x", sntpServerReferenceClockId);
    }
    else if (sntpServerStratum < 2)
    {
      sprintf(refClockId, "NTP Ref: %c%c%c%c",
              (sntpServerReferenceClockId & 0xff000000) >> 24,
              (sntpServerReferenceClockId & 0x00ff0000) >> 16,
              (sntpServerReferenceClockId & 0x0000ff00) >> 8,
              (sntpServerReferenceClockId & 0x000000ff));

    }
    else if (sntpServerStratum >= 2)
    {
      sprintf(refClockId, "NTP Srv: %d.%d.%d.%d",
              (sntpServerReferenceClockId & 0xff000000) >> 24,
              (sntpServerReferenceClockId & 0x00ff0000) >> 16,
              (sntpServerReferenceClockId & 0x0000ff00) >> 8,
              (sntpServerReferenceClockId & 0x000000ff));
    }
  }
  else
  {
    sprintf(refClockId, "NTP Bits: 0x%.08x", sntpServerReferenceClockId);
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
L7_RC_t sntpClientServerModeGet( L7_SNTP_SERVER_MODE_t * serverMode)
{
  if (serverMode == L7_NULL)
    return L7_ERROR;
  *serverMode = sntpServerMode;
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
  *broadcastCount = sntpBroadcastCount;
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
  *multicastCount = sntpMulticastCount;
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
  *status = sntpLastAttemptStatus;
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
  *attemptTime = sntpLastAttemptTime;
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
  *updateTime = sntpLastUpdateTime;
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
  /** Initialize time history */
  for (i = 0; i < SNTP_MAX_HISTORY; i++)
  {
    history[i].delay = SNTP_MAX_DELAY;
    history[i].dispersion = SNTP_MAX_DISPERSION;
    history[i].epoch = history[i].offset = 0.0;
  }
  /** Clear server statistics structures */
  for (i = 0; i < L7_SNTP_MAX_SERVERS; i++)
  {
    serverStats[i].lastUpdateTime = serverStats[i].lastAttemptTime = 0;
    serverStats[i].lastAttemptStatus = L7_SNTP_STATUS_OTHER;
    serverStats[i].unicastServerNumRequests = 0;
    serverStats[i].unicastServerNumFailedRequests = 0;
  }

  sntpLastUpdateTime = sntpLastAttemptTime = 0;
  sntpLastAttemptStatus = L7_SNTP_STATUS_OTHER;
}

/**********************************************************************
* @purpose  Clear the history file - Clear the statistics -
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

  sntpServerIpAddress = L7_NULL_IP_ADDR;
  sntpServerAddressType = L7_SNTP_ADDRESS_UNKNOWN;
  sntpServerStratum = 0;
  sntpServerVersion = 0;
  sntpServerReferenceClockId = 0;
  sntpServerMode = L7_SNTP_SERVER_MODE_RESERVED;
  /** Start off with some instance of time other than 0. */
  peerUpdate = osapiNTPTimeGet();
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
      serverStats[sntpActiveServer].lastAttemptStatus = sntpLastAttemptStatus;
      if (sntpLastAttemptStatus == L7_SNTP_STATUS_KISS_OF_DEATH)
      {
        serverStats[sntpActiveServer].unicastServerNumFailedRequests++;
        /* We never send to this server again. */
        sntpCfgData->server[sntpActiveServer].status =
                      L7_SNTP_SERVER_STATUS_NOT_IN_SERVICE;
        /** Save the new server state */
        sntpCfgMarkDataChanged();
        unicastServerSelect();
      }
      else if (retryCount > sntpCfgData->sntpUnicastPollRetry)
      {
        serverStats[sntpActiveServer].unicastServerNumFailedRequests++;
        unicastServerSelect();
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
*           Asynchronously closing the socket (fd) from anywhere causes
*           the entire loop to reset and start over from the beginning.
*
* @end
*********************************************************************/

static void sntpTask(void)
{
  L7_int32 rcvLen;
  L7_ushort16 rcvPort;
  L7_ushort16 family = L7_AF_INET;
  L7_uchar8 response[1024];
  fd_set readFds;
  L7_sockaddr_in_t raddr;
  L7_uint32 from_len = sizeof(raddr);

  osapiTaskInitDone(L7_SNTP_TASK_SYNC);

  while (1)
  {
    L7_int32 rc;
    L7_uint32 pollInterval;
    L7_uint32 pollIntervalTmp;

    NTP_PRINTF("sntpTask: wait for valid configuration\n");

    /** Wait until we're enabled and have some semblence of sanity */
    while ((sntpCfgData == L7_NULL) ||
           (serverStats == L7_NULL) ||
           (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_DISABLED) ||
           ((sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_UNICAST) &&
            (sntpCfgData->sntpUnicastServerCount == 0)))
    {
      osapiSleep(3); /** Yawn */
    }
    NTP_PRINTF("sntpTask: valid configuration - mode %d\n", sntpCfgData->sntpClientMode);

    /** No socket? Go get one */
    if (fd < 0)
    {
      sntpTaskLock();
      if (sntpLocalSocketOpen() != L7_SUCCESS)
      {
        NTP_PRINTF("sntpTask: Could not open socket\n");
      }
      else
      {
      NTP_PRINTF("sntpTask: open socket\n");
      }
      sntpTaskUnlock();
      if (fd < 0)
        continue;
    }

    /** Unicast mode - find a server for the retry loop below */
    if (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_UNICAST)
    {
      /* No server or retries exceeded */
      if ((sntpLastAttemptStatus != L7_SNTP_STATUS_SUCCESS) ||
          (sntpActiveServer == NO_SERVER_SELECTED))
      {
        sntpTaskLock();
        unicastServerSelect();
        sntpTaskUnlock();
      }
      NTP_PRINTF("sntpTask: select server %d\n", sntpActiveServer);
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
    }
    /** Implement the wait interval - We use a socket
    * so we can break out on management changes.
    * Anything that comes in is discarded during the wait interval.
    */
    retryCount = 0;
    if (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_UNICAST)
    {
      pollInterval = 1 << sntpCfgData->sntpUnicastPollInterval;
    }
    else if (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_BROADCAST)
    {
      pollInterval = 1 << sntpCfgData->sntpBroadcastPollInterval;
    }
    else if (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_MULTICAST)
    {
      pollInterval = 1 << sntpCfgData->sntpMulticastPollInterval;
    }
    else
    {
      continue;
    }
    NTP_PRINTF("sntpTask: enter poll loop - mode %d\n", sntpCfgData->sntpClientMode);

    /** Perform the poll loop  - retries for a unicast server are performed within this loop */
    pollIntervalTmp = pollInterval;
    do
    {
      L7_uint32 pollStart = osapiUpTimeRaw();
      L7_uint32 pollTimeout = 0;

      NTP_PRINTF("sntpTask: starting wait interval %d\n", pollIntervalTmp);

      sntpTaskLock();
      if (fd < 0)
      {
        sntpTaskUnlock();
        break;
      }
      FD_ZERO(&readFds);
      FD_SET(fd, &readFds);
      sntpTaskUnlock();

      rc = osapiSelect(fd + 1, &readFds, L7_NULL, L7_NULL, pollIntervalTmp, 1);
      if (fd < 0) /* Linux bug 546 */
        break;
      if (rc < 0)
      {
        NTP_PRINTF("sntpTask: select returned %d\n", rc);
        /** This is really ok - we might have just been told to switch modes. */
        NTP_PRINTF("Select failed during interval - errno %d\n",
                                          osapiErrnoGet());
        break;
      }
      else if (rc == 0) /** Timed out */
      {
        NTP_PRINTF("sntpTask: select returned %d\n", rc);
        /** Do nothing since this is what we really wanted. */ ;
      }
      else if (rc > 0)
      {
        NTP_PRINTF("sntpTask: select returned %d - flushing receive buffers\n", rc);
        /** Flush the receive buffer here.
        * No stale replies or broadcasts needed.
        */
        sntpTaskLock();
        sntpFlushReceiveBuffer();
        sntpTaskUnlock();
        NTP_PRINTF("sntpTask: flushed receive buffer - elapsed time %d\n", osapiUpTimeRaw() - pollStart);
        /* Wait the full poll interval */
        if ((osapiUpTimeRaw() - pollStart) < pollIntervalTmp)
        {
          pollIntervalTmp -= (osapiUpTimeRaw() - pollStart);
          NTP_PRINTF("sntpTask: poll interval not complete - %d\n", pollIntervalTmp);
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
          && (sntpCfgData->server[sntpActiveServer].poll == L7_ENABLE)
)
      {
        sntpPollSend();
        NTP_PRINTF("sntpTask: unicast poll sent\n");
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
        if (fd < 0)
        {
          sntpTaskUnlock();
          break;
        }
        FD_ZERO(&readFds);
        FD_SET(fd, &readFds);
        sntpTaskUnlock();
        NTP_PRINTF("sntpTask: waiting for NTP packet - timeout value %d - retry count %d - fd %d\n",
                                                                    pollTimeout, retryCount, fd);
        rc = osapiSelect(fd + 1, &readFds, L7_NULL, L7_NULL, pollTimeout, 0);
        if (fd < 0)
          break;
      } while (((sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_MULTICAST) ||
                (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_BROADCAST)) && (rc == 0));

      /** This is really ok - we might have just been told to change modes. */
      if (fd < 0)
        break;

      if (rc < 0) /** Error */
      {
        sntpLastAttemptStatus = L7_SNTP_STATUS_OTHER;
        sntpHandleError();
        NTP_PRINTF("sntpTask: Select failed for poll - errno %d last attempt status %d - retry count %d\n",
                                              osapiErrnoGet(), sntpLastAttemptStatus, retryCount);
        break;
      }
      else if (rc == 0)
      {
        sntpLastAttemptStatus = L7_SNTP_STATUS_REQUEST_TIMED_OUT;
        sntpHandleError();
        NTP_PRINTF("sntpTask:Select timed out - errno %d - last attempt status %d - retry count %d\n",
                                            osapiErrnoGet(), sntpLastAttemptStatus, retryCount);
      }
      else if (rc > 0)
      {
        /** Read the response and process the packet */
        NTP_PRINTF("sntpTask: Select indicates data available\n");
        rc = osapiSocketRecvfrom(fd, (void *)response,
                                sizeof(response), 0, (L7_sockaddr_t *)&raddr,&from_len,
                                &rcvLen);
        rcvPort = osapiNtohs(raddr.sin_port);
        recvAddr = osapiNtohl(raddr.sin_addr.s_addr);
        if (rc != L7_SUCCESS)       /** Error? */
        {
          sntpLastAttemptStatus = L7_SNTP_STATUS_OTHER;
          sntpHandleError();
          NTP_PRINTF("sntpTask:recv failed - errno %d - last attempt status %d - retry count %d\n",
                                            osapiErrnoGet(), sntpLastAttemptStatus, retryCount);
        }
        else if (rcvLen == 0)
        {
          /** No data? */
          sntpLastAttemptStatus = L7_SNTP_STATUS_REQUEST_TIMED_OUT;
          sntpHandleError();
          NTP_PRINTF("sntpTask: osapiRecvFrom returns no data - errno %d last attempt status %d - retry count %d\n",
                                              osapiErrnoGet(), sntpLastAttemptStatus, retryCount);
        }
        else if (rcvLen > 0)  /** A packet awaits */
        {
          NTP_PRINTF("sntpTask: osapiRecvFrom returns data\n");
          if (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_UNICAST)
          {
            /* Ignore non-selected servers when in unicast mode */
            if (recvAddr != remoteAddr)
            {
              sntpLastAttemptStatus = L7_SNTP_STATUS_OTHER;
              sntpHandleError();
              NTP_PRINTF("sntpTask: invalid sender - last attempt status %d - retry count %d\n",
                                                sntpLastAttemptStatus, retryCount);
            }
            else
            {
              NTP_PRINTF("Process response from 0x%lx len %d\n", recvAddr, rcvLen);
              sntpResponseProcess(recvAddr, family, response, rcvLen);
              if (sntpLastAttemptStatus == L7_SNTP_STATUS_SUCCESS)
              {
                /* Got a good packet - retry forever */
                retryCount = sntpCfgData->sntpUnicastPollRetry + 1;
                NTP_PRINTF("sntpTask: packet processed - last attempt status %d - retry count %d\n",
                                                sntpLastAttemptStatus, retryCount);
              }
              else
              {
                sntpHandleError();
                NTP_PRINTF("sntpTask: packet process failed - last attempt status %d - retry count %d\n",
                                                sntpLastAttemptStatus, retryCount);
              }
            }
          }
          else
          {
            NTP_PRINTF("Process response from 0x%lx len %d\n", recvAddr, rcvLen);
            sntpResponseProcess(recvAddr, family, response, rcvLen);
            /** We got a response - no more retry */
            retryCount = sntpCfgData->sntpUnicastPollRetry + 1;
            if (sntpLastAttemptStatus == L7_SNTP_STATUS_SUCCESS)
            {
              NTP_PRINTF("sntpTask: packet processed - last attempt status %d - retry count %d\n",
                                              sntpLastAttemptStatus, retryCount);
            }
            /* No need to handle errors on multicast/broadcast mode */
          }
        }
      }
    } while (retryCount <= sntpCfgData->sntpUnicastPollRetry);  /* end do */
  } /* end while */
}

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
    LOG_MSG("SNTP: Unable to obtain configuration data\n");
    sntpRC = L7_FAILURE;
    return sntpRC;
  }
  sntpCfgData = cfg;

  serverStats = osapiMalloc(L7_SNTP_COMPONENT_ID, L7_SNTP_MAX_SERVERS * sizeof(struct sntpServerStats_s));
  if (serverStats == L7_NULL)
  {
    LOG_MSG("SNTP: Unable to create the task semaphore\n");
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
    LOG_MSG("SNTP: Unable to create the task semaphore\n");
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
    LOG_MSG("SNTP: Unable to initialize sntp task\n");
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
     printf("\tserver[%d] last attempt time: %d\n", i,
                      serverStats[i].lastAttemptTime);
     printf("\tserver[%d] last update time: %d\n", i,
                      serverStats[i].lastUpdateTime);
     printf("\tserver[%d] num requests: %d\n", i,
                      serverStats[i].unicastServerNumRequests);
     printf("\tserver[%d] num failed requests: %d\n", i,
                      serverStats[i].unicastServerNumFailedRequests);
    }
  } /* end for */

  printf("Last update time: %d\n", sntpLastUpdateTime);
  printf("Last attempt time: %d\n", sntpLastAttemptTime);
  printf("Last attempt status: %d\n", sntpLastAttemptStatus);
  if (sntpServerIpAddress)
    printf("Server IP address: %s\n",
                      osapiInet_ntoa(sntpServerIpAddress));
  else
    printf("Server IP address: not set\n");
  printf("Server IP address type: %d\n", sntpServerAddressType);
  printf("Server stratum: %d\n", sntpServerStratum);
  printf("Server version: %d\n", sntpServerVersion);
  printf("Server ref clock id: 0x%x %s\n",
                      sntpServerReferenceClockId, refBuf);
  printf("Server mode: %d - %s\n", sntpServerMode,
                      sntpServerModeString(sntpServerMode));
  printf("Broadcast count: %d\n", sntpBroadcastCount);
  printf("Multicast count: %d\n", sntpMulticastCount);
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
  sntpClientDefaultsSet(L7_SNTP_CFG_VER_1);

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
  sntpClientDefaultsSet(L7_SNTP_CFG_VER_1);
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
  sntpClientDefaultsSet(L7_SNTP_CFG_VER_1);
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
  sntpClientDefaultsSet(L7_SNTP_CFG_VER_1);
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
  sntpClientDefaultsSet(L7_SNTP_CFG_VER_1);
  sntpClientModeSet(L7_SNTP_CLIENT_MULTICAST);
}

void
sntpUnitTest6()
{
  L7_uint32 ndx;
  sntpClientDefaultsSet(L7_SNTP_CFG_VER_1);
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
  sysapiPrintf("Current time: %s", buf);

  return current;
}

