/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename traceroutedebug.c
*
* @purpose Provide debug functions for traceroute
*
* @component traceroute
*
* @comments 
*       This file contains functions used to debug traceroute
*
* @create 08/16/2005
*
* @author Praveen K V 
*
* @end
*
**********************************************************************/

#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "osapi.h"
#include "osapi_support.h"
#include "nimapi.h"
#include "log.h"
#include "string.h"

#include "traceroute_map_api.h"
#include "traceroute.h"

unsigned short traceRouteHandles[16];


#if 0
#define dbgTraceRoutePrintf L7_LOGFNC
#endif

/* GCC 3.0 gives lots of warning for L7_LOGFNC macros. This is replacement for
 * LOGFNC on linux host testing
 */
static void
dbgTraceRoutePrintf( int flag, char *fmt, ... )
{
    va_list ap;

    va_start( ap, fmt );
    vprintf( fmt, ap );
    va_end( ap );
    return;

}

/*********************************************************************
* @purpose  Callback function for traceroute
*
* @param    void *param          @b{(input)}  User handle
* @param    L7_ushort6 handle    @b{(input)}  handle for the session
* @param    L7_uint32  ttl       @b{(input)}  TTL being currently used
* @param    L7_uint32  ipDa      @b{(input)}  IP Address of the hop at ttl
* @param    L7_ushort16 hopCount @b{(input)}  Current hop count
* @param    L7_ushort16 probeCount @b{(input)}  probe count for the hop
* @param    L7_uint32   rtt      @b{(input)}  RTT for the probe
* @param    L7_char8    errSym   @b{(input)}  Error symbol
*
* @returns  None
*
* @notes    
*
* @end
*********************************************************************/
static L7_RC_t 
dbgTraceRouteCallback( void *param, L7_ushort16 handle, 
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

  if ( probeCount == probePerHop )
  {
    strcat( buff, "\r\n" );
  }

  dbgTraceRoutePrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, buff );
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Executes a traceroute query
*
* @param    L7_BOOL     mode    @b{(input)}  Mode of operation
* @param    L7_uint32   ipDa    @b{(input)}  IP Destination Address
* @param    L7_ushort16 probes  @b{(input)}  Probes per hop
* @param    L7_ushort16 interval @b{(input)} Time between probes in seconds
* @param    L7_ushort16 port     @b{(input)} Target port
* @param    L7_ushort16 maxTtl   @b{(input)} Max TTL
* @param    L7_ushort16 initTtl  @b{(input)} Initial TTL
* @param    L7_ushort16 *handle  @b{(output)} Handle for the session
*
* @returns  None
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t
dbgTraceRouteExec( L7_BOOL mode, L7_uint32 ipDa, L7_ushort16 probes,
        L7_ushort16 interval, L7_ushort16 port, L7_ushort16 maxTtl,
        L7_ushort16 initTtl, L7_ushort16 *handle )
{
    L7_RC_t rc;
    L7_uint32 probeCount = probes;
    L7_uchar8 name[10];
    static L7_uint32 sessionNum = 0;

    sprintf( name, "%d", sessionNum++ );

    rc = traceRouteAlloc( "dbgTraceRoute", name, mode, 
            (traceRouteCallback)dbgTraceRouteCallback, (void *)probeCount, 
            handle );

    if ( rc == L7_SUCCESS )
    {
        dbgTraceRoutePrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                "Handle for session is 0x%x\n", *handle );
    } 
    else
    {
        dbgTraceRoutePrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                "Error in allocating session\n" );
        return rc;
    }

    rc = traceRouteDestAddrSet( *handle, ipDa );
    if ( rc != L7_SUCCESS )
    {
        dbgTraceRoutePrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Error in setting dest addr\n" );
    }

    rc = traceRouteProbePerHopSet( *handle, probes );
    if ( rc != L7_SUCCESS )
    {
        dbgTraceRoutePrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Error in setting probePerHop\n" );
    }

    rc = traceRouteProbeIntervalSet( *handle, interval );
    if ( rc != L7_SUCCESS )
    {
        dbgTraceRoutePrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Error in setting porbeInterval\n" );
    }

    rc = traceRouteDestPortSet( *handle, port );
    if ( rc != L7_SUCCESS )
    {
        dbgTraceRoutePrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Error in setting destination port\n" );
    }

    rc = traceRouteMaxTtlSet( *handle, maxTtl );
    if ( rc != L7_SUCCESS )
    {
        dbgTraceRoutePrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Error in setting maxTtl\n" );
    }

    rc = traceRouteInitTtlSet( *handle, initTtl );
    if ( rc != L7_SUCCESS )
    {
        dbgTraceRoutePrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Error in setting initTtl\n" );
    }

    rc = traceRouteStart( *handle );
    if ( rc != L7_SUCCESS )
    {
        dbgTraceRoutePrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                "Error in starting session\n" );
    }

    return rc;
}

/*********************************************************************
* @purpose  Queries a traceroute session
*
* @param    L7_ushort16 handle  @b{(input)}  Handle for the session
*
* @returns  None
*
* @notes    
*
* @end
*********************************************************************/
void
dbgTraceRouteQuery( L7_ushort16 handle )
{
    L7_RC_t rc;
    L7_BOOL operStatus;
    L7_ushort16 currTtl, currHopCount, currProbeCount, testAttempt, testSuccess;

    rc = traceRouteQuery( handle, &operStatus, &currTtl, &currHopCount,
            &currProbeCount, &testAttempt, &testSuccess );

    if ( rc != L7_SUCCESS )
    {
        dbgTraceRoutePrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Error in quering session\n" );
    }
    else
    {
        dbgTraceRoutePrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "OperStatus = %d currTtl = %d currHopCount = %d "
                " currProbeCount = %d testAttempt = %d testSuccess = %d\n", 
                operStatus, currTtl, currHopCount, currProbeCount, 
                testAttempt, testSuccess );

    }

    return;
}

/*********************************************************************
* @purpose  Gets handle from index and name
*
* @param    L7_uchar8 *index  @b{(input)}  Index for session
*
* @returns  None
*
* @notes    
*
* @end
*********************************************************************/
void
dbgTraceRouteHandleGet( int num )
{
    L7_RC_t rc;
    L7_BOOL operStatus;
    L7_uchar8 *index = "dbgTraceRoute";
    L7_ushort16 handle;
    L7_ushort16 vrfId;
    L7_uint32 ipDa;
    L7_BOOL dontFrag;
    L7_ushort16 probeSize, probePerHop, probeInterval;
    L7_ushort16 port, maxTtl, initTtl, maxFail;
    L7_uchar8 tmpBuf[100];

    sprintf( tmpBuf, "%d", num );
    rc = traceRouteResultGet( index, tmpBuf, &handle, &operStatus, &vrfId,
            &ipDa, &probeSize, &probePerHop, &probeInterval, &dontFrag, 
            &port, &maxTtl, &initTtl, &maxFail );

    if ( rc != L7_SUCCESS )
    {
        dbgTraceRoutePrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Error in quering session\n" );
    }
    else
    {
        osapiInetNtoa( ipDa, tmpBuf );
        dbgTraceRoutePrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                "Index=%s name=%s handle=0x%x operStatus=%2d vrfId=%04x"
                " ipDa=%s probePerHop=%02d probeInterval=%02d"
                " port=%d maxTtl=%02d minTtl=%02d\n", 
                index, tmpBuf, handle, operStatus, vrfId, tmpBuf, probePerHop, 
                probeInterval, port, maxTtl, initTtl );
    }


    return;
}

/*********************************************************************
* @purpose  Walks through Traceroute hop table
*
* @param    None
*
* @returns  None
*
* @notes    
*
* @end
*********************************************************************/
void
dbgTraceRouteWalkHopTable( L7_ushort16 handle )
{
    L7_RC_t rc;
    L7_ushort16 hopIndex, ttl, probeSent, probeRecvd;
    L7_uint32 ipDa, minRtt, maxRtt, avgRtt;
    L7_uchar8 tmpBuff[100];

    rc = traceRouteHopGetFirst( handle, &hopIndex, &ttl, &ipDa, &minRtt,
            &maxRtt, &avgRtt, &probeSent, &probeRecvd );

    if ( rc == L7_SUCCESS )
    {
        osapiInetNtoa( ipDa, tmpBuff );
        dbgTraceRoutePrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                "Handle = %04d HopIndex = %02d TTL = %02x IP-DA = %s "
                " Min/Max/Avg RTT = %d/%d/%d usec ProbeSent = %02d "
                "ProbeRecvd = %02d\n", handle, hopIndex, ttl, tmpBuff, minRtt, 
                maxRtt, avgRtt, probeSent, probeRecvd );
    }

    while ( rc == L7_SUCCESS )
    {
        rc = traceRouteHopGetNext( handle, &hopIndex, &ttl, &ipDa, &minRtt,
                &maxRtt, &avgRtt, &probeSent, &probeRecvd );
        if ( rc == L7_SUCCESS )
        {
            osapiInetNtoa( ipDa, tmpBuff );
            dbgTraceRoutePrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                "Handle = %04d HopIndex = %02d TTL = %02x IP-DA = %s "
                " Min/Max/Avg RTT = %d/%d/%d usec ProbeSent = %02d "
                "ProbeRecvd = %02d\n", handle, hopIndex, ttl, tmpBuff, minRtt, 
                maxRtt, avgRtt, probeSent, probeRecvd );
        }
    }
}

/*********************************************************************
* @purpose  Walks through Traceroute request table
*
* @param    None
*
* @returns  None
*
* @notes    
*
* @end
*********************************************************************/
void
dbgTraceRouteWalkReqTable( void )
{
    L7_RC_t rc;
    L7_BOOL operStatus;
    char index[100];
    char name[100];
    char tmpBuff[100];
    L7_ushort16 handle;
    L7_ushort16 vrfId;
    L7_uint32 ipDa;
    L7_BOOL dontFrag;
    L7_ushort16 probeSize, probePerHop, probeInterval;
    L7_ushort16 port, maxTtl, initTtl, maxFail;

    rc = traceRouteGetFirst( index, name, &handle, &operStatus, &vrfId,
            &ipDa, &probeSize, &probePerHop, &probeInterval, &dontFrag, 
            &port, &maxTtl, &initTtl, &maxFail );

    if ( rc == L7_SUCCESS )
    {
        osapiInetNtoa( ipDa, tmpBuff );
        dbgTraceRoutePrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                "Index=%s name=%s handle=0x%x operStatus=%2d vrfId=%04x"
                " ipDa=%s probePerHop=%02d probeInterval=%02d"
                " port=%d maxTtl=%02d minTtl=%02d\n", 
                index, name, handle, operStatus, vrfId, tmpBuff, probePerHop, 
                probeInterval, port, maxTtl, initTtl );
    }

    while ( rc == L7_SUCCESS )
    {
        rc = traceRouteGetNext( index, name, &handle, &operStatus, 
                &vrfId, &ipDa, &probeSize, &probePerHop, &probeInterval, 
                &dontFrag, &port, &maxTtl, &initTtl, &maxFail );

        if ( rc == L7_SUCCESS )
        {
            dbgTraceRoutePrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                "Index=%s name=%s handle=0x%x operStatus=%2d vrfId=%04x"
                " ipDa=%s probePerHop=%02d probeInterval=%02d"
                " port=%d maxTtl=%02d minTtl=%02d\n", 
                index, name, handle, operStatus, vrfId, tmpBuff, probePerHop, 
                probeInterval, port, maxTtl, initTtl );
        }
    }
}

/*********************************************************************
* @purpose  Walks through Traceroute response table
*
* @param    L7_ushort16 handle  @b{(input)}  Handle for the session
*
* @returns  None
*
* @notes    
*
* @end
*********************************************************************/
void
dbgTraceRouteWalkRespTable( void )
{
    L7_RC_t rc;
    L7_BOOL operStatus, dontFrag;
    char index[100];
    char name[100];
    L7_ushort16 handle;
    L7_ushort16 vrfId;
    L7_uint32 ipDa;
    L7_ushort16 probeSize, probePerHop, probeInterval;
    L7_ushort16 port, maxTtl, initTtl, maxFail;

    rc = traceRouteGetFirst( index, name, &handle, &operStatus, &vrfId,
            &ipDa, &probeSize, &probePerHop, &probeInterval, &dontFrag, 
            &port, &maxTtl, &initTtl, &maxFail );

    if ( rc == L7_SUCCESS )
    {
        dbgTraceRouteQuery( handle );
    }

    while ( rc == L7_SUCCESS )
    {
        rc = traceRouteGetNext( index, name, &handle, &operStatus, 
                &vrfId, &ipDa, &probeSize, &probePerHop, &probeInterval, 
                &dontFrag, &port, &maxTtl, &initTtl, &maxFail );

        if ( rc == L7_SUCCESS )
        {
            dbgTraceRouteQuery( handle );
        }
    }
}


/*********************************************************************
* @purpose  Executes a random traceroute session 
*
* @param    L7_uint32 argc    @b{(input)} Not used
* @param    L7_uchar8 *argv[] @b{(input)} Not used
*
* @returns  None
*
* @notes    
*       This function is invoked in a new thread
*
* @end
*********************************************************************/
void
dbgTraceRouteSync( L7_uint32 argc, L7_uchar8 *argv[] )
{
    L7_RC_t rc;
    L7_uint32 addr;
    L7_ushort16 probes, interval, port, maxTtl, initTtl, handle;
    L7_uint32 taskId;
    unsigned char *ipAddr[] = {   "127.0.0.1",
                            "172.22.75.80",
                            "172.22.64.8",
                            "172.22.192.8"
    };

    addr = ntohl(inet_addr( ipAddr[rand() & 0x3] ));
    probes = rand() & 0x7;
    interval = rand() & 0x7;
    port = rand() & 0xFFFF;
    maxTtl = rand() & 0x31;
    initTtl = 0;

    dbgTraceRoutePrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
            "Calling SYNC dbgTraceRouteSync with addr = %x "
            " count = %d interval = %d\n", addr, probes, interval );

    rc = dbgTraceRouteExec( L7_TRUE, addr, probes, interval, port, maxTtl,
            initTtl, &handle );
    if ( rc == L7_SUCCESS )
        traceRouteHandles[handle & 0xF] = handle;
    if ( osapiTaskIDSelfGet( &taskId ) == L7_SUCCESS )
    {
        osapiTaskDelete( taskId );
    }

}

/*********************************************************************
* @purpose  Executes a random traceroute ASYNC session
*
* @param    None
*
* @returns  None
*
* @notes    
*
* @end
*********************************************************************/
void
dbgTraceRouteASync( )
{
    L7_RC_t rc;
    L7_uint32 addr;
    L7_ushort16 probes, interval, port, maxTtl, initTtl, handle;
    unsigned char *ipAddr[] = {   "127.0.0.1",
                            "172.22.75.80",
                            "172.22.64.8",
                            "172.22.192.8"
    };

    addr = ntohl(inet_addr( ipAddr[rand() & 0x3] ));
    probes = rand() & 0x7;
    interval = rand() & 0x7;
    port = rand() & 0xFFFF;
    maxTtl = rand() & 0x31;
    initTtl = 0;

    dbgTraceRoutePrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
            "Calling ASYNC dbgTraceRouteSync with addr = %x "
            " count = %d interval = %d\n", addr, probes, interval );

    rc = dbgTraceRouteExec( L7_FALSE, addr, probes, interval, port, maxTtl,
            initTtl, &handle );
    if ( rc == L7_SUCCESS )
        traceRouteHandles[handle & 0xF] = handle;
}

/*********************************************************************
* @purpose  Shows Traceroute Session Table
*
* @param    None
*
* @returns  None
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t
dbgTraceRouteShowTable( void )
{
    L7_uint32 i;
    traceRouteTable_t *entry;

    dbgTraceRoutePrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
            "  Id   HNDL    STATE    IP-DA\n" );
    dbgTraceRoutePrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
            "------------------------------------\n" );
    for ( i = 0; i < TRACEROUTE_MAX_SESSIONS; i++ )
    {
        traceRouteGetEntry( i, &entry );
        if ( entry->state == TRACEROUTE_STATE_FREE )
        {
            dbgTraceRoutePrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                    "%4d  %4d   FREE    \n", i, entry->handle );
        }
        else
        {
            dbgTraceRoutePrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                    "%4d  %4d   %4d    %8x\n", i, entry->handle, 
                    entry->state, entry->ipDa );
        }
    }
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Randomly executes a traceroute command
*
* @param    L7_ushort16 count  @b{(input)}  Number of sessions to execute
*
* @returns  None
*
* @notes    
*
* @end
*********************************************************************/
void
dbgTraceRouteRandom( int count )
{
    L7_ushort16 handle;
    L7_uint32 iterations = 0;
    void *rxArgs[1];
    L7_uint32 taskHandle;
    L7_uint32 oper;

    while ( count != 0 )
    {
        oper = rand() & 0x3;

        switch ( oper )
        {
        case 0:
            if ((taskHandle =  osapiTaskCreate( "dbgTraceRouteRandomSync",
                        dbgTraceRouteSync, 1, &rxArgs, L7_DEFAULT_STACK_SIZE,
                        L7_DEFAULT_TASK_PRIORITY,
                        L7_DEFAULT_TASK_SLICE)) == L7_FAILURE)
            {
                dbgTraceRoutePrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS,
                        "Could not create task\n" );
            }
            break;

        case 1:
            dbgTraceRouteASync( );
            break;

        case 2:
            handle = traceRouteHandles[ rand() & 0xF ];
            dbgTraceRoutePrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                    "Calling traceRouteFree with handle = %x\n", handle);
            traceRouteFree( handle );
            break;

        default:
            break;
        }
        if ( iterations++ % 16 == 0 ) {
            dbgTraceRouteShowTable( );
        }
        count--;
        osapiSleepMSec(100);
    }
}

/*********************************************************************
* @purpose  Shows all Traceroute commands
*
* @param    None
*
* @returns  None
*
* @notes    
*
* @end
*********************************************************************/
void
dbgTraceRouteShowCommands( )
{

    dbgTraceRoutePrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
        "dbgTraceRouteExec( L7_BOOL mode, L7_uint32 ipDa, L7_ushort16 probes," 
        "L7_ushort16 interval, L7_ushort16 port, L7_ushort16 maxTtl,"
       " L7_ushort16 initTtl, L7_ushort16 *handle )\n" );

    dbgTraceRoutePrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
            "dbgTraceRouteQuery( L7_ushort16 handle )\n" );

    dbgTraceRoutePrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
            "dbgTraceRouteWalkHopTable( L7_ushort16 handle )\n" );

    dbgTraceRoutePrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
            "dbgTraceRouteWalkReqTable( void )\n" );

    dbgTraceRoutePrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
            "dbgTraceRouteWalkRespTable( void )\n" );

    dbgTraceRoutePrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
            "dbgTraceRouteShowTable( void )\n" );

    dbgTraceRoutePrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
            "dbgTraceRouteShowTable( void )\n" );

    dbgTraceRoutePrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
            "dbgTraceRouteRandom()\n" );
}
