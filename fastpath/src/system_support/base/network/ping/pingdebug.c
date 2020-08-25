#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "osapi.h"
#include "osapi_support.h"
#include "ping_api.h"
#include "traceroute_map_api.h"
#include "nimapi.h"
#include "log.h"
#include "string.h"

static L7_RC_t
osapiTimeUSecElapsed( L7_clocktime *end, L7_clocktime *start, 
        L7_uint32 *elapsedTime )
{
      *elapsedTime = (end->seconds - start->seconds) * 1000000;
      if ( start->nanoseconds > end->nanoseconds )
      {
          *elapsedTime -= ((start->nanoseconds - end->nanoseconds)/1000);
      }
      else
      {
          *elapsedTime += ((end->nanoseconds - start->nanoseconds)/1000);
      }

      return L7_SUCCESS;
}

L7_RC_t pingCallback1( void *ctxt, L7_ushort16 seqNo, L7_uint32 rtt )
{
  L7_char8 str[L7_CLI_MAX_STRING_LENGTH];

  sprintf(str,"\r\nReceived response for Seq Num %d Rtt  %d usec", seqNo, rtt);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_ALWAYS, str );
  return L7_SUCCESS;
}

unsigned short handles[16];

void *syncPingTest( int argc, void *argv[] )
{
    unsigned short handle;
    pingSessionResp_t resp;
    int ret;
    pingSessionReq_t *req = argv[0];
    L7_uint32 taskId;

    ret = pingSessionStart( req, pingCallback1, NULL, &handle, &resp );
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_ALWAYS,
           "Ping Sent %d queries Received %d responses. AvgRTT %d MaxRTT %d"
           " MinRTT %d \n", resp.probeSent, resp.probeSuccess, 
           resp.avgRtt, resp.maxRtt, resp.minRtt );
    if ( ret == L7_SUCCESS )
    {
       SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_ALWAYS,"Handle returned %x\n",
               handle );
       handles[handle&0xf] = handle;
    }
    else
    {
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "pingSessionStart failed\n" );
    }
    if ( osapiTaskIDSelfGet( &taskId ) == L7_SUCCESS )
    {
        osapiTaskDelete( taskId );
    }
    return NULL;
}

unsigned short traceRouteHandles[16];

static L7_RC_t traceRouteCallbackFn1( void *param, L7_ushort16 handle, 
        L7_uint32 ttl, L7_uint32 ipDa, L7_ushort16 hopCount, 
        L7_ushort16 probeCount, L7_uint32 rtt, L7_char8 errSym )
{
  static L7_uint32 lastIpAddr = 0;
  L7_uchar8  buff[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8  ipBuff[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8  tmpBuff[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 probePerHop = (L7_uint32)param;

  buff[0] = '\0';
  if ( probeCount == 1 )
  {
    lastIpAddr = ipDa;
    osapiInetNtoa( ipDa, tmpBuff );
    sprintf(buff, "%d %s   ", ttl, tmpBuff );
  }

  if ( ipDa != lastIpAddr )
  {
    osapiInetNtoa( ipDa, tmpBuff );
    sprintf( ipBuff, "(%s) ", tmpBuff );
    strcat( buff, ipBuff );
  }

  sprintf( tmpBuff, "%d usec %c   ", rtt, errSym );
  strcat( buff, tmpBuff );
  if ( probePerHop == 0 )
  {
    probePerHop = L7_TRACEROUTE_NUM_PROBE_PACKETS;
  }
  if ( probeCount == probePerHop )
  {
    strcat( buff, "\r\n" );
  }

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_ALWAYS, buff );
  return L7_SUCCESS;
}

void *syncTraceRouteTest( int argc, void *argv[] )
{
    unsigned short handle;
    traceRouteResp_t resp;
    int ret;
    traceRouteReq_t *req = argv[0];
    L7_uint32 taskId;

    ret = traceRouteStart( req, traceRouteCallbackFn1, NULL, &handle, &resp );
    if ( ret == L7_SUCCESS )
    {
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Handle returned %x\n", handle );
        traceRouteHandles[handle&0xf] = handle;
    }
    else
    {
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "traceRouteStart failed\n" );
    }
    if ( osapiTaskIDSelfGet( &taskId ) == L7_SUCCESS )
    {
        osapiTaskDelete( taskId );
    }
    return NULL;
}

L7_ushort16
traceRouteRandomTest()
{
    traceRouteReq_t req;
    traceRouteResp_t resp;
    unsigned short handle;
    unsigned char *ipAddr[] = {   "127.0.0.1",
                                  "10.254.2.102",
                                  "1.1.1.1",
                                  "192.168.77.133"
    };


    int num;
    int addr;
    int size;
    int count;
    unsigned int interval;
    int ret;

    num = rand() & 0x3;
    addr = ntohl(inet_addr( ipAddr[rand() & 0x3] ));
    count = rand() & 0x1F;
    interval = rand() & 0xFFFFF;
    if ( interval < 0x10000 )
    {
        interval = 0x10000;
    }
    if ( interval > 500000 )
    {
        interval = 100000;
    }

    handle = rand() & 0xF;
    size = rand() & 0xFF;

    bzero( &req, sizeof( req ) );
    switch ( num ) 
    {
    case 0:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Calling TRACEROUTE SYNC\n" );
    case 1:
        if ( num == 0 )
        {
            req.mode = TRACEROUTE_MODE_SYNC;
        }
        else
        {
            SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    "Calling TRACEROUTE ASYNC\n" );
            req.mode = TRACEROUTE_MODE_ASYNC;
        }
        req.vrfId = 0;
        req.ipDa = addr;
        req.addrType = TRACEROUTE_ADDR_IPV4;
        req.probeSize = size;
        req.probePerHop = count;
        req.probeInterval = interval;
        if ( num == 0 )
        {
            void *rxArgs[1];
            L7_uint32 taskHandle;
            traceRouteReq_t *req1;

            req1 = malloc( sizeof( traceRouteReq_t ) );
            memcpy( req1, &req, sizeof( req ) );
            rxArgs[0] = req1;

            if ((taskHandle =  osapiTaskCreate( "cliTraceRouteAsync", 
                        syncTraceRouteTest, 1, &rxArgs, L7_DEFAULT_STACK_SIZE,
                        L7_DEFAULT_TASK_PRIORITY,
                        L7_DEFAULT_TASK_SLICE)) == L7_FAILURE)
            {
                SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_ALWAYS,
                        "Could not create task\n" );
                return L7_FAILURE;
            }
            usleep( 200000 );
            ret = L7_FAILURE;
        }
        else
        {
            ret = traceRouteStart( &req, traceRouteCallbackFn1, NULL, &handle, 
                    &resp );
            if ( ret == L7_SUCCESS && req.mode == TRACEROUTE_MODE_ASYNC )
            {
                SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_ALWAYS,
                        "Handle returned %x\n", handle );
                traceRouteHandles[handle&0xf] = handle;
            }
        }
        break;

    case 2:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Calling traceRouteEnd for %d\n", handle );
        traceRouteEnd( traceRouteHandles[handle] );
        break;
    case 3:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Calling traceRouteQuery for %d\n", handle );
        if ( traceRouteQuery( handle, &resp ) == L7_SUCCESS )
        {
             SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_ALWAYS,
                     "Traceroute request success\n" );
        }
        else
        {
            SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_ALWAYS,"Error\n" );
        }
        break;
    }

    usleep( 200000 );
    return 0;
}
L7_RC_t
randomPingTest()
{
    pingSessionReq_t req;
    pingSessionResp_t resp;
    unsigned short handle;
    unsigned char *ipAddr[] = {   "127.0.0.1",
                                  "10.254.2.102",
                                  "1.1.1.1",
                                  "192.168.77.133"
    };


    int num;
    int addr;
    int size;
    int count;
    unsigned int interval;
    int ret;
    int i;
    int rtt;

    num = rand() & 0x3;
    addr = osapiNtohl(inet_addr( ipAddr[rand() & 0x3] ));
    count = rand() & 0x7F;
    interval = rand() & 0xFFFFF;
    if ( interval < 0x10000 )
        interval = 0x10000;
    if ( interval > 500000 )
        interval = 100000;

    handle = rand() & 0xF;
    size = rand() & 0xFF;
    switch ( num ) 
    {
    case 0:
       SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_ALWAYS,
               "Calling PING SYNC\n" );
    case 1:
       if ( num == 0 )
       {
           req.mode = PING_PROBE_MODE_SYNC;
       }
       else
       {
           SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_ALWAYS,
                   "Calling PING ASYNC\n" );
           req.mode = PING_PROBE_MODE_ASYNC;
       }
       req.vrfId = 0;
       req.ipDa = addr;
       req.addrType = PING_PROBE_ADDR_IPV4;
       req.proto = PING_PROBE_PROTO_ICMP;
       req.probeCount = count;
       req.probeSize = size;
       req.probeInterval = interval;
       if ( num == 0 )
       {
            void *rxArgs[1];
            L7_uint32 taskHandle;
            pingSessionReq_t *req1;

            req1 = malloc( sizeof( pingSessionReq_t ) );
            memcpy( req1, &req, sizeof( req ) );
            rxArgs[0] = req1;

            if ((taskHandle =  osapiTaskCreate( "cliPingSessionAsync", 
                        syncPingTest, 1, &rxArgs, L7_DEFAULT_STACK_SIZE,
                        L7_DEFAULT_TASK_PRIORITY,
                        L7_DEFAULT_TASK_SLICE)) == L7_FAILURE)
            {
                SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_ALWAYS,
                        "Could not create task\n" );
                return L7_FAILURE;
            }
            usleep( 200000 );
            ret = L7_FAILURE;
       }
       else
       {
           ret = pingSessionStart( &req, pingCallback1, NULL, &handle, &resp );
           if ( ret == L7_SUCCESS && req.mode == PING_PROBE_MODE_ASYNC ) 
           {
                SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_ALWAYS,
                   "Handle returned %x\n", handle );
                handles[handle&0xf] = handle;
                SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_ALWAYS,
                   "Ping Sent %d queries Received %d responses. AvgRTT %d "
                   "MaxRTT %d MinRTT %d \n", resp.probeSent, resp.probeSuccess,
                   resp.avgRtt, resp.maxRtt, resp.minRtt );
           }
       }
       break;

    case 2:
       SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_ALWAYS,
               "Calling pingSessionEnd for %d\n", handle );
       pingSessionEnd( handles[handle] );
       break;
    case 3:
       SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_ALWAYS,
               "Calling pingSessionQuery for %d\n", handle );
       if ( pingSessionQuery( handle, &resp ) == L7_SUCCESS )
       {
           for ( i = 0; i < 16; i++ )
           {
                osapiTimeUSecElapsed(&(resp.info[i].rxTime),
                        &(resp.info[i].txTime), &rtt);
                SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_ALWAYS,
                        "Probe %d : txTime %u:%u rxTime %u:%u Rtt = %d \n", i, 
                resp.info[i].txTime.seconds,
                resp.info[i].txTime.nanoseconds,
                resp.info[i].rxTime.seconds,
                resp.info[i].rxTime.nanoseconds,
                rtt
                );
            }
            SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    "Ping Sent %d queries Received %d responses. AvgRTT %d "
                    "MaxRTT %d MinRTT %d \n", resp.probeSent, resp.probeSuccess,
                    resp.avgRtt, resp.maxRtt, resp.minRtt );

        }
       else
       {
           SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_ALWAYS,"Error\n" );
       }
       break;
    }

    usleep( 200000 );
    return 0;
}


L7_RC_t
pingTraceRouteRandomTest()
{
    while ( 1 ) 
    {
        traceRouteRandomTest();
//        randomPingTest();
    }
    return L7_SUCCESS;
}

L7_RC_t
pingStartSync( pingSessionReq_t *req )
{
    void *rxArgs[1];
    L7_uint32 taskHandle;
    traceRouteReq_t *req1;

    req1 = malloc( sizeof( traceRouteReq_t ) );
    memcpy( req1, &req, sizeof( req ) );
    rxArgs[0] = req1;

    if ((taskHandle =  osapiTaskCreate( "cliTraceRouteAsync", 
            syncTraceRouteTest, 1, &rxArgs, L7_DEFAULT_STACK_SIZE,
            L7_DEFAULT_TASK_PRIORITY, L7_DEFAULT_TASK_SLICE)) == L7_FAILURE)
    {
       SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_ALWAYS,
               "Could not create task\n" );
       return L7_FAILURE;
    }
    return L7_SUCCESS;
}

L7_RC_t
pingStart( L7_char8 *addr, L7_uint32 mode, L7_uint32 count, L7_uint32 interval )
{
    pingSessionReq_t req;
    pingSessionResp_t resp;
    L7_ushort16 asyncHandle;

    req.vrfId = 0;
    req.ipDa = ntohl(inet_addr( addr ) );
    req.addrType = PING_PROBE_ADDR_IPV4;
    req.proto = PING_PROBE_PROTO_ICMP;
    req.probeCount = count;
    req.probeSize = 0;
    req.probeInterval = interval;
    if ( mode )
    {
        req.mode = PING_PROBE_MODE_SYNC;
        return pingStartSync( &req );
    }
    else
    {
        req.mode = PING_PROBE_MODE_ASYNC;
        return pingSessionStart(&req, pingCallback1, NULL, &asyncHandle, &resp);
    }

}

L7_RC_t
pingStop( L7_uint32 handle )
{
    return pingSessionEnd( handle );
}

L7_RC_t
pingGet( L7_uint32 handle )
{
    pingSessionResp_t resp;
    pingSessionQuery( handle, &resp );
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_ALWAYS,
            "Ping Sent %d queries Received %d responses. AvgRTT %d MaxRTT %d "
            "MinRTT %d \n", resp.probeSent, resp.probeSuccess, resp.avgRtt, 
            resp.maxRtt, resp.minRtt );
    return L7_SUCCESS;
}
