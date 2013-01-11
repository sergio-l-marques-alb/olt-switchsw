/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename tacacs_aaa.h
*
* @purpose TACACS+ Client definitions common to all AAA.
*
* @component tacacs+
*
* @comments This file consists of constant definitions common to
*           Authentication, Authorization and Accounting.
*
* @create 03/10/2005
*
* @author gkiran
*         dfowler 06/23/05
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_TACACS_AAA_H
#define INCLUDE_TACACS_AAA_H

#include "user_manager_exports.h"
#include "cli_web_exports.h"
#include "tacacs_exports.h"


#define L7_TACACS_PKT_HEADER_LEN          12
#define L7_TACACS_UNENCRYPTED_FLAG        0x01
#define L7_TACACS_SINGLE_CONNECT_FLAG     0x04

#define L7_TACACS_PRIV_LVL_READ_ONLY      0x01
#define L7_TACACS_PRIV_LVL_READ_WRITE     0x0F

#define L7_TACACS_AUTHEN                  0x01
#define L7_TACACS_AUTHOR                  0x02

#define L7_TACACS_USER_MSG_LEN           256
#define L7_TACACS_USER_DATA_LEN          256
#define L7_TACACS_SRV_DATA_LEN           L7_TACACS_USER_DATA_LEN
#define L7_TACACS_SRV_MSG_LEN            L7_TACACS_USER_MSG_LEN

#define L7_TACACS_SESSION_ID_BYTES       4
#define L7_TACACS_PKT_VERSION_BYTES      1
#define L7_TACACS_SEQ_NO_BYTES           1

#define L7_TACACS_MD5_OUTPUT_LEN         16 /* as per standard*/
#define L7_TACACS_MD5_INPUT_LEN          160
 /* Output Length should be greater
 ** than 4 + KEY_LEN + 1 + 1 +
 ** OUTPUT_LEN as per standard.
 */

#define L7_TACACS_PKT_VERSION_OFFSET     0
#define L7_TACACS_PKT_TYPE_OFFSET        L7_TACACS_PKT_VERSION_OFFSET + 1
#define L7_TACACS_PKT_SEQ_NO_OFFSET      L7_TACACS_PKT_TYPE_OFFSET + 1
#define L7_TACACS_PKT_FLAGS_OFFSET       L7_TACACS_PKT_SEQ_NO_OFFSET + 1
#define L7_TACACS_PKT_SESSION_ID_OFFSET  L7_TACACS_PKT_FLAGS_OFFSET + 1
#define L7_TACACS_PKT_LENGTH_OFFSET      L7_TACACS_PKT_SESSION_ID_OFFSET + 4
#define L7_TACACS_BODY_OFFSET            L7_TACACS_PKT_LENGTH_OFFSET + 4

/* TACACS+ Major & Minor Version Numbers */
#define L7_TACACS_MAJOR_VER              0x0c
#define L7_TACACS_MINOR_VER_DEFAULT      0x00
#define L7_TACACS_MINOR_VER_MASK         0x0F
#define L7_TACACS_MAJOR_VER_SHIFT        4

/* dfowler - I don't like the use of L7_TACACS_PORT_NAME, we should have
   allowed this to be passed in auth API, its supposed to be descriptive
   of where the user is authenticating, too late now. */
#define L7_TACACS_PORT_NAME                  "unknown"
#define L7_TACACS_PORT_NAME_MAX              32 /* should be param in future */

/* Start packet offsets */

#define L7_TACACS_START_ACTION_OFFSET        L7_TACACS_PKT_HEADER_LEN
#define L7_TACACS_START_PRIVLVL_OFFSET       L7_TACACS_START_ACTION_OFFSET + 1
#define L7_TACACS_START_AUTHEN_TYPE_OFFSET   L7_TACACS_START_PRIVLVL_OFFSET + 1
#define L7_TACACS_START_SERVICE_OFFSET       L7_TACACS_START_AUTHEN_TYPE_OFFSET + 1
#define L7_TACACS_START_USR_LEN_OFFSET       L7_TACACS_START_SERVICE_OFFSET + 1
#define L7_TACACS_START_PORT_LEN_OFFSET      L7_TACACS_START_USR_LEN_OFFSET + 1
#define L7_TACACS_START_REM_ADDR_LEN_OFFSET  L7_TACACS_START_PORT_LEN_OFFSET+1
#define L7_TACACS_START_DATA_LEN_OFFSET      L7_TACACS_START_REM_ADDR_LEN_OFFSET + 1
#define L7_TACACS_START_USER_OFFSET          L7_TACACS_START_DATA_LEN_OFFSET+1

/* Continue packet offsets */
#define L7_TACACS_CONT_USER_MSG_LEN_OFFSET   L7_TACACS_PKT_HEADER_LEN
#define L7_TACACS_CONT_DATA_LEN_OFFSET       L7_TACACS_CONT_USER_MSG_LEN_OFFSET + 2
#define L7_TACACS_CONT_FLAGS_OFFSET          L7_TACACS_CONT_DATA_LEN_OFFSET + 2
#define L7_TACACS_CONT_USER_MSG_OFFSET       L7_TACACS_CONT_FLAGS_OFFSET + 1

/* Authorization request attribute/value pairs, currently hardcoded to support L7_LOGIN_ACCESS_READ_ONLY */
#define L7_TACACS_REQUEST_ATTR_VALUE_ARG_CNT  3
#define L7_TACACS_REQUEST_ATTR_VALUE_SIZE_MAX 16
#define L7_TACACS_REQUEST_ATTR_VALUE_SERVICE  "service=shell"
#define L7_TACACS_REQUEST_ATTR_VALUE_CMD      "cmd="  /* NULL cmd refers to shell itself */
#define L7_TACACS_REQUEST_ATTR_VALUE_PRIV_LVL "priv-lvl=15"

/* Request packet offsets, we only define those that are different from START packet */
#define L7_TACACS_REQUEST_AUTHEN_METHOD_OFFSET  L7_TACACS_PKT_HEADER_LEN
#define L7_TACACS_REQUEST_ARG_CNT_OFFSET        L7_TACACS_START_REM_ADDR_LEN_OFFSET + 1
#define L7_TACACS_REQUEST_USER_OFFSET           L7_TACACS_REQUEST_ARG_CNT_OFFSET + L7_TACACS_REQUEST_ATTR_VALUE_ARG_CNT + 1

/* Reply  packet offsets */
#define L7_TACACS_REPLY_STATUS_OFFSET        L7_TACACS_PKT_HEADER_LEN
#define L7_TACACS_REPLY_FLAGS_OFFSET         L7_TACACS_REPLY_STATUS_OFFSET + 1
#define L7_TACACS_REPLY_SRV_MSG_LEN_OFFSET   L7_TACACS_REPLY_FLAGS_OFFSET + 1
#define L7_TACACS_REPLY_DATA_LEN_OFFSET      L7_TACACS_REPLY_SRV_MSG_LEN_OFFSET + 2
#define L7_TACACS_REPLY_SRV_MSG_OFFSET       L7_TACACS_REPLY_DATA_LEN_OFFSET + 2

/* Response packet offsets, we only define those that are different from REPLY packet */
#define L7_TACACS_RESPONSE_ARG_CNT_OFFSET       L7_TACACS_REPLY_STATUS_OFFSET + 1
#define L7_TACACS_RESPONSE_ATTR_VALUE_ARG_CNT   10   /* its possible for server to send back more than we send */
#define L7_TACACS_RESPONSE_ATTR_VALUE_SIZE_MAX  255  /* max according to spec for attr-value pairs */

/*Authentication Flags */
#define L7_TACACS_CONT_ABORT_FLAG            0x01
#define L7_TACACS_NO_ECHO_FLAG               0x01

#define L7_TACACS_REPLY_FIXED_FIELDS         6
#define L7_TACACS_CONT_FIXED_FIELDS          5
#define L7_TACACS_START_FIXED_FIELDS         8
#define L7_TACACS_REQUEST_FIXED_FIELDS       8
#define L7_TACACS_AUTHEN_SEQ_NO_MAX          0xFF

/* Authentication actions */
#define L7_TACACS_AUTHEN_LOGIN               0x01
#define L7_TACACS_AUTHEN_SENDPASS            0x03
#define L7_TACACS_AUTHEN_TERMINATE           0x05

/* Authentication types, there are more, this is what we support */
#define L7_TACACS_AUTHEN_TYPE_NOT_USED 0x00
#define L7_TACACS_AUTHEN_TYPE_ASCII    0x01

/* Authentication services, there are more, this is what we support */
#define L7_TACACS_AUTHEN_SVC_NONE      0x00
#define L7_TACACS_AUTHEN_SVC_LOGIN     0x01
#define L7_TACACS_AUTHEN_SVC_ENABLE    0x02

/* Authentication methods, there are more, this is what we support */
#define L7_TACACS_AUTHEN_METH_TACPLUS  0x06

/* Authentication reply status */
#define L7_TACACS_AUTHEN_STATUS_PASS         0x01
#define L7_TACACS_AUTHEN_STATUS_FAIL         0x02
#define L7_TACACS_AUTHEN_STATUS_GETDATA      0x03
#define L7_TACACS_AUTHEN_STATUS_GETUSER      0x04
#define L7_TACACS_AUTHEN_STATUS_GETPASS      0x05
#define L7_TACACS_AUTHEN_STATUS_RESTART      0x06
#define L7_TACACS_AUTHEN_STATUS_ERROR        0x07
#define L7_TACACS_AUTHEN_STATUS_FOLLOW       0x21

/* Authorization response status */
#define L7_TACACS_AUTHOR_STATUS_PASS_ADD     0x01
#define L7_TACACS_AUTHOR_STATUS_PASS_REPL    0x02
#define L7_TACACS_AUTHOR_STATUS_FAIL         0x10
#define L7_TACACS_AUTHOR_STATUS_ERROR        0x11
#define L7_TACACS_AUTHOR_STATUS_FOLLOW       0x21



                                     /* REQUEST is largest if we need to pass
                                        more data in future, update this size */
#define TACACS_TX_PKT_BUF_SIZE       (L7_TACACS_PKT_HEADER_LEN + \
                                      L7_TACACS_REQUEST_FIXED_FIELDS + \
                                      L7_TACACS_USER_NAME_MAX + \
                                      L7_TACACS_PORT_NAME_MAX + \
                                      L7_TACACS_REQUEST_ATTR_VALUE_ARG_CNT + \
                                      (L7_TACACS_REQUEST_ATTR_VALUE_SIZE_MAX * \
                                       L7_TACACS_REQUEST_ATTR_VALUE_ARG_CNT))

#define TACACS_RX_PKT_BUF_SIZE       (L7_TACACS_PKT_HEADER_LEN + \
                                      L7_TACACS_REPLY_FIXED_FIELDS + \
                                      L7_TACACS_SRV_MSG_LEN + \
                                      L7_TACACS_SRV_DATA_LEN + \
                                      L7_TACACS_RESPONSE_ATTR_VALUE_ARG_CNT + \
                                      (L7_TACACS_RESPONSE_ATTR_VALUE_SIZE_MAX * \
                                       L7_TACACS_RESPONSE_ATTR_VALUE_ARG_CNT))

typedef struct tacacsRxPacket_s
{
  L7_uint32   socket;       /* socket we are reading this packet from */
  L7_uint32   bytesRead;    /* bytes read, either partial header, or partial body */
  L7_uint32   length;       /* body length, excludes header, 0 if header not read yet */
  L7_uchar8   packet[TACACS_RX_PKT_BUF_SIZE];  /* entire packet, includes header */
} tacacsRxPacket_t;

typedef struct tacacsTxPacket_s
{
  L7_uint32   bytesWritten;    /* bytes written to socket */
  L7_uint32   length;          /* total packet length, including header */
  L7_uchar8   packet[TACACS_TX_PKT_BUF_SIZE];  /* entire packet, includes header */
} tacacsTxPacket_t;

typedef enum
{
  TACACS_SERVER_STATUS_DISCONNECTED = 0,
  TACACS_SERVER_STATUS_CONNECT_WAIT,    /* a session has initiated single connection */
  TACACS_SERVER_STATUS_CONNECTED        /* single connection established */
} tacacsServerStatus_t;

typedef struct tacacsServerOprData_s
{
  L7_uchar8              status;  /* tacacsServerStatus_t */
  L7_uint32              socket;  /* single-connection, uses one socket per server */
} tacacsServerOprData_t;

/* Authentication session states */
typedef enum tacacsSessionStates_s
{
  TACACS_AUTH_STATE_BEGIN,
  TACACS_AUTH_STATE_CONNECT_WAIT,
  TACACS_AUTH_STATE_SINGLE_CONNECT_WAIT,
  TACACS_AUTH_STATE_CONNECTED,
  TACACS_AUTH_STATE_READY_TO_SEND,
  TACACS_AUTH_STATE_RESPONSE_WAIT,
  TACACS_AUTH_STATE_RESPONSE_RECEIVED,
  TACACS_AUTH_STATE_SOCKET_ERROR
} tacacsSessionStates_t;

typedef struct tacacsAuthSession_s
{
  tacacsSessionStates_t state;         /* current session state */
  L7_uint32            componentId;    /* the requesting component */
  L7_uint32            correlator;     /* used to map request -> response by requesting component */
  L7_uchar8            username[L7_TACACS_USER_NAME_MAX];    /* user to authenticate */
  L7_uchar8            password[L7_TACACS_PASSWORD_MAX];     /* password to authenticate */
  L7_uchar8            accessLevel;    /* current authenticated access level */
  L7_uint32            sessionId;      /* random identifier for session */
  L7_uint32            server;         /* ip address of current server */
  L7_uchar8            sequenceNo;     /* START = 1, current seqNo in conversation */
  L7_uint32            connectTaskId;  /* set when connection task running for this session */
  L7_uint32            connectTimer;   /* timer set for connection task */
  L7_uint32            connectArgv[3]; /* copies of server ip, port, socket for connection task */
  L7_uint32            socket;         /* TCP socket for communication */
  tacacsTxPacket_t     txPacket;       /* packet ready for transmit */
  L7_ACCESS_LEVEL_t    mode;
  L7_uchar8           *port;             /* Name of port -- may be NULL */
  L7_uchar8           *rem_addr;         /* remote address of client -- may be NULL */
  L7_BOOL              mayChallenge;     /* L7_TRUE if may be challenged -- input */
  L7_uchar8           *pState;           /* challenge state -- I/O */
  L7_BOOL             *isChallenged;     /* L7_TRUE if challenged -- output */
  L7_uchar8           *pChallengePhrase; /* phrase to display to user -- output */
  L7_uint32           *challengeFlags;   /* flags returned by authentication method
                                            currently only 0x01 -- NOECHO is supported */
  L7_SERVICE_TYPE     servType;          /* AUTHENTICATION/AUTHORIZATION */
} tacacsAuthSession_t;

typedef struct tacacsOprData_s
{
  L7_BOOL                unencrypted; /* for debug/support send packets unencrypted */
  tacacsServerOprData_t  servers[L7_TACACS_MAX_SERVERS];  /* indexes in sync with configuration data */
  tacacsAuthSession_t    sessions[L7_TACACS_MAX_AUTH_SESSIONS];
  tacacsRxPacket_t       rxPackets[L7_TACACS_MAX_AUTH_SESSIONS];
} tacacsOprData_t;

/******************************************************
**        Internal function prototypes               **
******************************************************/

L7_RC_t tacacsServerIndexGet(L7_uint32 ip, L7_uint32 *index);
L7_RC_t tacacsServerHostIndexGet(L7_uchar8   *serverAddress,
                                 L7_uint32   *index);
L7_RC_t tacacsServerIPHostNameIndexGet(L7_IP_ADDRESS_TYPE_t type,
                                       L7_uchar8   *serverAddress,
                                       L7_uint32   *index);
L7_RC_t tacacsServerSocketOpen(tacacsServerCfg_t *server,
                               L7_uint32         *socket);
L7_uint32 tacacsSessionIdGenerate(void);
void tacacsEncryptDecrypt(L7_uchar8 *packet,
                          L7_uint32  length,  /* excludes packet header */
                          L7_char8  *key);
L7_RC_t tacacsServerToContactNextGet(L7_uint32  ip,
                                     L7_uint32 *serverIdx);


#endif /* INCLUDE_TACACS_AAA_H */
