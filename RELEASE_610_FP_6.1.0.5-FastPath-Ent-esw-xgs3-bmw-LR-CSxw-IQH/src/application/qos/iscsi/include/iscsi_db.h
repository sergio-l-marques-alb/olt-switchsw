/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename iscsi_db.h
*
* @purpose ISCSI component session and connection database functions
*
* @component iSCSI
*
* @comments none
*
* @create 05/18/2008
*
* @end
*
**********************************************************************/
#ifndef ISCSI_DB_H
#define ISCSI_DB_H

#include "datatypes.h"
#include "osapi.h"
#include "iscsi.h"

typedef struct
{
  L7_BOOL              inUse;
  L7_uchar8            targetName[ISCSI_NAME_LENGTH_MAX+1];     /* save room for terminating '\0' */
  L7_uchar8            initiatorName[ISCSI_NAME_LENGTH_MAX+1];  /* save room for terminating '\0' */
  L7_uchar8            isid[6];
  L7_clocktime         startTime;         /* time when session first detected */
  L7_clocktime         lastActivityTime;  /* time when activity detected on session */
  L7_uint32            silentTime;        /* number of seconds since activity detected on session */
  L7_ulong64           counterData;       /* hit count on connection rules for aging */
  L7_uint32            connectionCount;   /* number of connections associated with this session */
} iscsiSessionRecord_t;

typedef struct
{
  L7_BOOL              inUse;
  L7_uint32            targetIpAddress;
  L7_uint32            targetTcpPort;
  L7_uint32            initiatorIpAddress;
  L7_uint32            initiatorTcpPort;
  L7_uint32            cid;                /* CID from iSCSI protocol */
  L7_uint32            sessionIndex;       /* index into sessionRecord table entry for this connection */
  L7_uint32            targetPortTableId;  /* identifier of which target TCP port table entry caused this connection to be made, used for
                                              clean up of connections when configuration changed */
} iscsiConnectionRecord_t;

L7_int32 iscsiDbSessionRecordLookup(L7_uchar8 *targetName,
                                    L7_uchar8 *initiatorName,
                                    L7_uchar8 *isid);
L7_int32 iscsiDbSessionRecordCreate(L7_uchar8 *targetName,
                                    L7_uchar8 *initiatorName,
                                    L7_uchar8 *isid);
void iscsiDbSessionRecordDelete(L7_uint32 sessionId);
L7_RC_t iscsiDbSessionDelete(L7_uint32 sessionId);
void iscsiDbConnectionDelete(L7_uint32 connectionId);

L7_int32 iscsiDbConnectionRecordLookup(L7_uint32 targetIpAddress,
                                       L7_uint32 targetTcpPort,
                                       L7_uint32 initiatorIpAddress,
                                       L7_uint32 initiatorTcpPort);
L7_int32 iscsiDbConnectionRecordCreate(L7_uint32 targetIpAddress,
                                       L7_uint32 targetTcpPort,
                                       L7_uint32 initiatorIpAddress,
                                       L7_uint32 initiatorTcpPort,
                                       L7_uint32 cid,
                                       L7_uint32 sessionTableIndex,
                                       L7_uint32 targetTcpPortTableId);
void iscsiDbConnectionRecordDelete(L7_uint32 connectionId);
L7_RC_t iscsiDbConnectionCreate(L7_uchar8 *targetName,
                                L7_uchar8 *initiatorName,
                                L7_uchar8 *isid,
                                L7_uint32 targetIpAddress,
                                L7_uint32 targetTcpPort,
                                L7_uint32 initiatorIpAddress,
                                L7_uint32 initiatorTcpPort,
                                L7_uint32 cid,
                                L7_int32  *connectionId,
                                L7_int32  *sessionId,
                                L7_uint32 targetTcpPortTableId);

L7_int32 iscsiDbSessionRecordCompare(iscsiSessionRecord_t *r1, iscsiSessionRecord_t *r2);
L7_BOOL iscsiDbSessionIdInUse(L7_uint32 sessionId);
L7_RC_t iscsiDbSessionFirstGet(L7_uint32 *sessionId);
L7_RC_t iscsiDbSessionNextGet(L7_uint32 sessionId, L7_uint32 *nextSessionId);

L7_RC_t iscsiDbSessionTargetNameGet(L7_uint32 sessionId, L7_uchar8 *name);
L7_RC_t iscsiDbSessionInitiatorNameGet(L7_uint32 sessionId, L7_uchar8 *name);
L7_RC_t iscsiDbSessionStartTimeGet(L7_uint32 sessionId, L7_clocktime *time);
L7_RC_t iscsiDbSessionSilentTimeGet(L7_uint32 sessionId, L7_uint32 *seconds);
L7_RC_t iscsiDbSessionActivityTimeUpdate(L7_uint32 sessionId);
L7_RC_t iscsiDbSessionIsidGet(L7_uint32 sessionId, L7_uchar8 *isid);
L7_RC_t iscsiDbSessionIterate(L7_int32 sessionId, L7_int32 *nextSessionId);

L7_RC_t iscsiDbSessionCounterGet(L7_uint32 sessionId, L7_ulong64 *countData);
L7_RC_t iscsiDbSessionCounterSet(L7_uint32 sessionId, L7_ulong64 *countData);
L7_RC_t iscsiDbSessionConnectionCountGet(L7_uint32 sessionId, L7_uint32 *count);

L7_int32 iscsiDbConnectionRecordCompare(iscsiConnectionRecord_t *r1, iscsiConnectionRecord_t *r2);
L7_BOOL iscsiDbConnectionIdInUse(L7_uint32 connectionId);
L7_RC_t iscsiDbConnectionFirstGet(L7_uint32 sessionId, L7_uint32 *connectionId);
L7_RC_t iscsiDbConnectionNextGet(L7_uint32 sessionId, L7_uint32 connectionId, L7_uint32 *nextConnectionId);
L7_RC_t iscsiDbConnectionIterate(L7_int32 connectionId, L7_int32 *nextConnectionId);
L7_RC_t iscsiDbConnectionTargetIpAddressGet(L7_uint32 connectionId, L7_uint32 *ipAddr);
L7_RC_t iscsiDbConnectionInitiatorIpAddressGet(L7_uint32 connectionId, L7_uint32 *ipAddr);
L7_RC_t iscsiDbConnectionTargetTcpPortGet(L7_uint32 connectionId, L7_uint32 *port);
L7_RC_t iscsiDbConnectionInitiatorTcpPortGet(L7_uint32 connectionId, L7_uint32 *port);
L7_RC_t iscsiDbConnectionCidGet(L7_uint32 connectionId, L7_uint32 *cid);

L7_RC_t iscsiDbConnectionDataRetrieve(L7_uint32 connectionId,
                                      L7_uchar8 *targetName, 
                                      L7_uchar8 *initiatorName, 
                                      L7_uint32 *targetIpAddress,
                                      L7_uint32 *targetTcpPort, 
                                      L7_uint32 *initiatorIpAddress, 
                                      L7_uint32 *initiatorTcpPort, 
                                      L7_uint32 *isid, 
                                      L7_uint32 *cid);
L7_int32 iscsiDbConnectionSessionIdGet(L7_uint32 connectionId);
L7_int32 iscsiDbConnectionTargetTcpPortIdGet(L7_uint32 connectionId);

L7_RC_t iscsiDbSessionDataStructureCreate(void);
void iscsiDbSessionDataStructureDestroy(void);
#endif

