/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename ping.c
*
* @purpose Provide "ping" functionality as defined in RFC 2925
*
* @component ping
*
* @comments 
*       This file contains functions used to debug ping
*
* @create 08/16/2005
*
* @author Praveen K V 
*
* @end
*
**********************************************************************/
#define DEBUG

#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "osapi.h"
#include "osapi_support.h"
#include "ping_api.h"
#include "nimapi.h"
#include "log.h"
#include "string.h"
#include "ping_private.h"
#include "ping_api.h"

static unsigned short pingHandles[16];
static L7_BOOL pingDebugPathTraceFlag = L7_FALSE;
void pingDebugPathTraceFlagSet(L7_BOOL flag);
L7_BOOL pingDebugPathTraceFlagGet();

#if 0
#define dbgPingPrintf L7_LOGFNC
#endif
/* GCC 3.0 gives lots of warning for L7_LOGFNC macros. This is replacement for
 * LOGFNC on linux host testing
 */
static void
dbgPingPrintf( int flag, char *fmt, ... )
{
    va_list ap;

    va_start( ap, fmt );
    vprintf( fmt, ap );
    va_end( ap );
    return;

}

/*********************************************************************
* @purpose  Callback message for ping
*
* @param    void *handle @b{(input)} User handle
* @param    void *seqNo  @b{(input)} Sequence number of the probe
* @param    void *rtt    @b{(input)} RTT for the probe
*
* @returns  L7_SUCCESS 
*
* @notes    
*           Prints the response for a probe
* @end
*********************************************************************/
static L7_RC_t 
dbgPingCallback( void *handle, L7_ushort16 seqNo, L7_uint32 rtt )
{
  L7_char8 str[L7_CLI_MAX_STRING_LENGTH];

  sprintf(str,"\r\nReceived response for Seq Num %d Rtt  %d usec", seqNo, rtt);
  dbgPingPrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, str );
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Executes a ping command
*
* @param    L7_BOOL     mode     @b{(input)}  SYNC/ASYNC mode
* @param    L7_uint32   ipDa     @b{(input)}  IP Destination Address
* @param    L7_ushort16 count    @b{(input)}  Probe count
* @param    L7_ushort16 size     @b{(input)}  Probe size
* @param    L7_ushort16 interval @b{(input)}  Probe interval
* @param    L7_ushort16 *handle  @b{(input)}  Handle for the session
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE 
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t
dbgPingExec( L7_BOOL mode, L7_uint32 ipDa, L7_ushort16 count, L7_ushort16 size, 
        L7_ushort16 interval, L7_ushort16 *handle )
{
    L7_RC_t rc;
    L7_uchar8 name[10];
    static L7_uint32 sessionNum = 0;

    sprintf( name, "%d", sessionNum++ );

    rc = pingSessionAlloc( "dbgPing", name, mode, 
            (pingSessionCallback *)dbgPingCallback, NULL, 0, handle );

    if ( rc == L7_SUCCESS )
    {
        dbgPingPrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                "Handle for session is 0x%x\n", *handle );
    } 
    else
    {
        dbgPingPrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                "Error in allocating session\n" );
        return rc;
    }

    rc = pingDestAddrSet( *handle, ipDa );
    if ( rc != L7_SUCCESS )
    {
        dbgPingPrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Error in setting dest addr\n" );
    }
    rc = pingProbeSizeSet( *handle, size );
    if ( rc != L7_SUCCESS )
    {
        dbgPingPrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Error in setting size\n" );
    }

    rc = pingProbeIntervalSet( *handle, interval );
    if ( rc != L7_SUCCESS )
    {
        dbgPingPrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Error in setting interval\n" );
    }

    rc = pingProbeCountSet( *handle, count );
    if ( rc != L7_SUCCESS )
    {
        dbgPingPrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Error in setting count\n" );
    }

    rc = pingSessionStart( *handle );
    if ( rc != L7_SUCCESS )
    {
        dbgPingPrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                "Error in starting session\n" );
    }

    return rc;
}

/*********************************************************************
* @purpose  Executes a ping query
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
dbgPingQueryExec( L7_ushort16 handle )
{
    L7_RC_t rc;
    L7_BOOL operStatus;
    L7_ushort16 sent, succ, fail;
    L7_uint32 minRtt, maxRtt, avgRtt;

    rc = pingSessionQuery( handle, &operStatus, &sent, &succ, &fail, &minRtt, 
            &maxRtt, &avgRtt );

    if ( rc != L7_SUCCESS )
    {
        dbgPingPrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Error in quering session\n" );
    }
    else
    {
        dbgPingPrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "OperStatus = %d Sent = %d Received = %d min/max/avg RTT = "
                "%d/%d/%d usec\n", operStatus, sent, succ, minRtt,
                maxRtt, avgRtt );
    }

    return;
}

/*********************************************************************
* @purpose  Queries handle for a index/name strings
*
* @param    L7_uchar8 *index    @b{(input)}  Index of the session
*
* @returns  None
*
* @notes    
*
* @end
*********************************************************************/
void
dbgPingQueryHandleGet( int num )
{
    L7_RC_t rc;
    L7_BOOL operStatus;
    L7_ushort16 handle;
    L7_uint32 vrfId;
    L7_ushort16 count, size, interval;
    L7_uint32 ipDa;
    L7_uchar8 *index = "dbgPing";
    L7_uchar8 tmpBuf[10];

    sprintf( tmpBuf, "%d", num );
    rc = pingSessionHandleGet( index, tmpBuf, &handle, &operStatus, &vrfId,
            &ipDa, &count, &size, &interval );

    if ( rc != L7_SUCCESS )
    {
        dbgPingPrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Error in quering session\n" );
    }
    else
    {
        dbgPingPrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                "%16s %16s %04x %2d %04x %08x %04x %04x %04x\n", index, tmpBuf, 
                handle, operStatus, vrfId, ipDa, count, size, interval );
    }

    return;
}

/*********************************************************************
* @purpose  Walk and show ping request table
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
dbgPingWalkReqTable( void )
{
    L7_RC_t rc;
    L7_BOOL operStatus;
    char index[100];
    char name[100];
    L7_ushort16 handle;
    L7_uint32 vrfId;
    L7_ushort16 count, size, interval;
    L7_uint32 ipDa;

    rc = pingSessionGetFirst( index, name, &handle, &operStatus, &vrfId,
            &ipDa, &count, &size, &interval );

    if ( rc == L7_SUCCESS )
    {
        dbgPingPrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                "%16s %16s %04x %2d %04x %08x %04x %04x %04x\n", index, name, 
                handle, operStatus, vrfId, ipDa, count, size, interval );
    }

    while ( rc == L7_SUCCESS )
    {
        rc = pingSessionGetNext( index, name, &handle, &operStatus, &vrfId,
                &ipDa, &count, &size, &interval );

        if ( rc == L7_SUCCESS )
        {
            dbgPingPrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                    "%16s %16s %04x %2d %04x %08x %04x %04x %04x\n", index, 
                    name, handle, operStatus, vrfId, ipDa, count, size, 
                    interval );
        }
    }
}

/*********************************************************************
* @purpose  Show complete response table
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
dbgPingWalkRespTable( void )
{
    L7_RC_t rc;
    L7_BOOL operStatus;
    char index[100];
    char name[100];
    L7_ushort16 handle;
    L7_uint32 vrfId;
    L7_ushort16 count, size, interval;
    L7_uint32 ipDa;

    rc = pingSessionGetFirst( index, name, &handle, &operStatus, &vrfId,
            &ipDa, &count, &size, &interval );

    if ( rc == L7_SUCCESS )
    {
        dbgPingQueryExec( handle );
    }

    while ( rc == L7_SUCCESS )
    {
        rc = pingSessionGetNext( index, name, &handle, &operStatus, &vrfId,
                &ipDa, &count, &size, &interval );

        if ( rc == L7_SUCCESS )
        {
            dbgPingQueryExec( handle );
        }
    }

}

/*********************************************************************
* @purpose  Show ping session table
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
dbgPingShowTable( void )
{
    L7_uint32 i;
    pingSessionTable_t *pingEntry;

    dbgPingPrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
            "  Id   HNDL    STATE    IP-DA\n" );
    dbgPingPrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
            "------------------------------------\n" );
    for ( i = 0; i < PING_MAX_SESSIONS; i++ )
    {
        pingGetEntry( i, &pingEntry );
        if ( pingEntry->state == PING_SESSION_STATE_FREE )
        {
            dbgPingPrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                    "%4d  %4d   FREE    \n", i, pingEntry->handle );
        }
        else
        {
            dbgPingPrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                    "%4d  %4d   %4d    %8x\n", i, pingEntry->handle, 
                    pingEntry->state, pingEntry->ipDa );
        }
    }
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Start a random ping session in sync mode
*
* @param    int argc   @{(input)} Not used
* @param    char *argv @{(input)} Not used
*
* @returns  None
*
* @notes    
*
* @end
*********************************************************************/
void
dbgPingSync( )
{
    L7_RC_t rc;
    L7_uint32 addr;
    L7_ushort16 size;
    L7_ushort16 count;
    L7_ushort16 interval;
    L7_ushort16 handle;
    unsigned char *ipAddr[] = {   "127.0.0.1",
                            "172.22.75.80",
                            "172.22.64.8",
                            "172.22.192.8"
    };

    addr = ntohl(inet_addr( ipAddr[rand() & 0x3] ));
    count = rand() & 0x1F;
    interval = rand() & 0x7;
    size = rand() & 0xFF;
    dbgPingPrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
            "Calling SYNC dbgPingExec with addr = %x "
            " count = %d interval = %d\n", addr, count, interval );
    rc = dbgPingExec( L7_TRUE, addr, count, size, interval, &handle );
    if ( rc == L7_SUCCESS )
        pingHandles[handle & 0xF] = handle;

}

static void
dbgPingSyncTask( int argc, char *argv[] )
{
    L7_uint32 taskId;
    dbgPingSync();
    if ( osapiTaskIDSelfGet( &taskId ) == L7_SUCCESS )
    {
        osapiTaskDelete( taskId );
    }
}

/*
* @purpose  Start a random ping session in async mode
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
dbgPingASync( )
{
    L7_RC_t rc;
    L7_uint32 addr;
    L7_ushort16 size;
    L7_ushort16 count;
    L7_ushort16 interval;
    L7_ushort16 handle;
    unsigned char *ipAddr[] = {   "127.0.0.1",
                            "172.22.75.80",
                            "172.22.64.8",
                            "172.22.192.8"
    };

    addr = ntohl(inet_addr( ipAddr[rand() & 0x3] ));
    count = rand() & 0x1F;
    interval = rand() & 0x7;
    size = rand() & 0xFF;
    dbgPingPrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
            "Calling ASYNC dbgPingExec with addr = %x "
            " count = %d interval = %d\n", addr, count, interval );
    rc = dbgPingExec( L7_FALSE, addr, count, size, interval, &handle );
    if ( rc == L7_SUCCESS )
        pingHandles[handle & 0xF] = handle;
}

/*********************************************************************
* @purpose  Randomly execute ping commands
*
* @param    L7_uint32 num  @b{(input)}  Number of commands to execute
*
* @returns  None
*
* @notes    
*
* @end
*********************************************************************/
void
dbgPingRandom( L7_uint32 num )
{
    L7_uint32 oper;
    L7_ushort16 handle;
    L7_uint32 iterations = 0;
    void *rxArgs[1];
    L7_uint32 taskHandle;

    while ( num != 0 )
    {
        oper = rand() & 0x3;

        switch ( oper )
        {
        case 0:
            if ((taskHandle =  osapiTaskCreate( "dbgPingRandomSync",
                        dbgPingSyncTask, 1, &rxArgs, L7_DEFAULT_STACK_SIZE,
                        L7_DEFAULT_TASK_PRIORITY,
                        L7_DEFAULT_TASK_SLICE)) == L7_FAILURE)
            {
                dbgPingPrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS,
                        "Could not create task\n" );
            }
            break;

        case 1:
            dbgPingASync();
            break;

        case 2:
            handle = pingHandles[ rand() & 0xF ];
            dbgPingPrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                "Calling pingSessionFree with handle = %x\n", handle);
            pingSessionFree( handle );
            break;

        default:
            break;
        }
        if ( iterations++ % 16 == 0 ) {
            dbgPingShowTable( );
        }
        num--;
        osapiSleepMSec(100);
    }
}

/*********************************************************************
* @purpose  Show all ping debug commands
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
dbgPingShowCommands( void )
{
    dbgPingPrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
        "dbgPingExec( L7_BOOL mode, L7_uint32 ipDa, L7_ushort16 count, "
        "L7_ushort16 size, L7_ushort16 interval, L7_ushort16 *handle )\n" );

    dbgPingPrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
            "pingCreateSock( pingSessionTable_t *pingEntry )\n" );
    dbgPingPrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
            "dbgPingQueryExec( L7_ushort16 handle )\n" );
    dbgPingPrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
            "dbgPingWalkReqTable( void )\n" );
    dbgPingPrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
            "dbgPingWalkRespTable( void )\n" );
    dbgPingPrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
            "dbgPingShowTable( void )\n" );
    dbgPingPrintf( SYSAPI_APPLICATION_LOGGING_ALWAYS, 
            "dbgPingRandom()\n" );
}

/*********************************************************************
* @purpose  Set the debug trace flag.
*
* @param    flag  @b{(input)}  The new value for the ping trace flag
*
* @notes
*
* @end
*********************************************************************/

void pingDebugPathTraceFlagSet(L7_BOOL flag)
{
 pingDebugPathTraceFlag = flag;
}

/*********************************************************************
* @purpose  Get the debug trace flag.
*
* @param    
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL pingDebugPathTraceFlagGet()
{
  return pingDebugPathTraceFlag;
}
