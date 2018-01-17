/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename dot1x_exports.h
*
* @purpose  Defines constants and feature definitions that are shared by Management and the application 
*
* @component 
*
* @comments 
*           
*
* @Notes   
*
* @created 03/26/2008
*
* @author akulkarn    
* @end
*
**********************************************************************/

#ifndef __DOT1X_EXPORTS_H_
#define __DOT1X_EXPORTS_H_


/* DOT1X Component Feature List */
typedef enum
{
  L7_DOT1X_FEATURE_ID = 0,                   /* general support statement */
  L7_DOT1X_SET_KEYTXENABLED_FEATURE_ID,      /* Key Transmission Enabled as read/write field */
  L7_DOT1X_SET_CONTROLDIR_FEATURE_ID,       /* Control Direction as read/write field */
  L7_DOT1X_VLANASSIGN_FEATURE_ID,           /* RFC 3580 VLAN Assignments via dot1x */
  L7_DOT1X_MACBASEDAUTH_FEATURE_ID,         /* Mac Based Authentication support*/
  L7_DOT1X_SUPPLICANT_CONTROL_ON_AUTH_PORTS_FEATURE_ID,/* Supplicant Control on Auth ports support */
  L7_DOT1X_DYNAMIC_VLANASSIGN_FEATURE_ID,           
  L7_DOT1X_FEATURE_ID_TOTAL,                /* total number of enum values */
} L7_DOT1X_FEATURE_IDS_t;



/******************************************************************/
/*************       Start DOT1X types and defines        *********/
/******************************************************************/

/* Port protocol version */
typedef enum
{
  L7_DOT1X_PAE_PORT_PROTOCOL_VERSION_1 = 1
} L7_DOT1X_PAE_PORT_PROTOCOL_VERSION_t;

/* Port controlled directions */
typedef enum
{
  L7_DOT1X_PORT_DIRECTION_BOTH = 0,
  L7_DOT1X_PORT_DIRECTION_IN
} L7_DOT1X_PORT_DIRECTION_t;

/* Port authorization status */
typedef enum
{
  L7_DOT1X_PORT_STATUS_AUTHORIZED = 1,
  L7_DOT1X_PORT_STATUS_UNAUTHORIZED,
	L7_DOT1X_PORT_STATUS_NA
} L7_DOT1X_PORT_STATUS_t;

/* Port authorization mode */
typedef enum
{
  L7_DOT1X_PORT_FORCE_UNAUTHORIZED = 1,
  L7_DOT1X_PORT_AUTO,
  L7_DOT1X_PORT_FORCE_AUTHORIZED,
  L7_DOT1X_PORT_AUTO_MAC_BASED,
	  L7_DOT1X_PORT_NA
} L7_DOT1X_PORT_CONTROL_t;

/* Port PAE capabilities bitmask values */
#define L7_DOT1X_PAE_PORT_AUTH_CAPABLE 0x01
#define L7_DOT1X_PAE_PORT_SUPP_CAPABLE 0x02

/* Authenticator PAE State Machine (APM) states (needed by UI) */
typedef enum
{
  L7_DOT1X_APM_INITIALIZE = 1,
  L7_DOT1X_APM_DISCONNECTED,
  L7_DOT1X_APM_CONNECTING,
  L7_DOT1X_APM_AUTHENTICATING,
  L7_DOT1X_APM_AUTHENTICATED,
  L7_DOT1X_APM_ABORTING,
  L7_DOT1X_APM_HELD,
  L7_DOT1X_APM_FORCE_AUTH,
  L7_DOT1X_APM_FORCE_UNAUTH
} L7_DOT1X_APM_STATES_t;

/* Backend Authenticator State Machine (BAM) states (needed by UI) */
typedef enum
{
  L7_DOT1X_BAM_REQUEST = 1,
  L7_DOT1X_BAM_RESPONSE,
  L7_DOT1X_BAM_SUCCESS,
  L7_DOT1X_BAM_FAIL,
  L7_DOT1X_BAM_TIMEOUT,
  L7_DOT1X_BAM_IDLE,
  L7_DOT1X_BAM_INITIALIZE
} L7_DOT1X_BAM_STATES_t;

/* Vlan Assignment Mode */
typedef enum
{
  L7_DOT1X_DEFAULT_ASSIGNED_VLAN = 1,
  L7_DOT1X_RADIUS_ASSIGNED_VLAN,
  L7_DOT1X_UNAUTHENTICATED_VLAN,
  L7_DOT1X_GUEST_VLAN,
  L7_DOT1X_VOICE_VLAN,
  L7_DOT1X_MONITOR_MODE_VLAN,
  L7_DOT1X_NOT_ASSIGNED
} L7_DOT1X_VLAN_ASSIGNED_MODE_t;

/* Radius Termination Action (needed by UI)*/
typedef enum 
{
    L7_DOT1X_TERMINATION_ACTION_DEFAULT = 1,
    L7_DOT1X_TERMINATION_ACTION_RADIUS
}L7_DOT1X_TERMINATION_ACTION_t;


/* dot1x range values */

#define L7_DOT1X_PORT_MIN_QUIET_PERIOD     0
#define L7_DOT1X_PORT_MAX_QUIET_PERIOD     65535

#define L7_DOT1X_PORT_MIN_TX_PERIOD        1
#define L7_DOT1X_PORT_MAX_TX_PERIOD        65535

#define L7_DOT1X_PORT_MIN_SUPP_TIMEOUT     1
#define L7_DOT1X_PORT_MAX_SUPP_TIMEOUT     65535

#define L7_DOT1X_PORT_MIN_SERVER_TIMEOUT   1
#define L7_DOT1X_PORT_MAX_SERVER_TIMEOUT   65535

#define L7_DOT1X_PORT_MIN_MAX_REQ          1
#define L7_DOT1X_PORT_MAX_MAX_REQ          10

#define L7_DOT1X_PORT_MIN_REAUTH_PERIOD    1
#define L7_DOT1X_PORT_MAX_REAUTH_PERIOD    65535

#define L7_DOT1X_PORT_MIN_GUESTVLAN_PERIOD 1
#define L7_DOT1X_PORT_MAX_GUESTVLAN_PERIOD 300

#define L7_DOT1X_PORT_MIN_MAC_USERS        1
#define L7_DOT1X_PORT_MAX_MAC_USERS        16

#define L7_DOT1X_USER_NAME_LEN             L7_LOGIN_SIZE
#define L7_DOT1X_CHALLENGE_LEN             32

#define L7_DOT1X_FILTER_NAME_LEN           256
/******************************************************************/
/*************       End DOT1X types and defines          *********/
/******************************************************************/


/* Supplicant state machines */
typedef enum
{
  L7_DOT1X_SPM_LOGOFF=0,
  L7_DOT1X_SPM_DISCONNECTED,
  L7_DOT1X_SPM_CONNECTING,
  L7_DOT1X_SPM_AUTHENTICATING,
  L7_DOT1X_SPM_HELD,
  L7_DOT1X_SPM_AUTHENTICATED,
  L7_DOT1X_SPM_RESTART,
  L7_DOT1X_SPM_FORCE_AUTH,
  L7_DOT1X_SPM_FORCE_UNAUTH
} L7_DOT1X_SPM_STATES_t;

/* Supplicant Backend State Machine (SBM) states  */
typedef enum
{
  L7_DOT1X_SBM_REQUEST = 0,
  L7_DOT1X_SBM_RESPONSE,
  L7_DOT1X_SBM_SUCCESS,
  L7_DOT1X_SBM_FAIL,
  L7_DOT1X_SBM_TIMEOUT,
  L7_DOT1X_SBM_IDLE,
  L7_DOT1X_SBM_INITIALIZE,
  L7_DOT1X_SBM_RECEIVE
} L7_DOT1X_SBM_STATES_t;

/* dot1x Supplicant range values */

#define L7_DOT1X_PORT_MIN_MAX_START          1
#define L7_DOT1X_PORT_MAX_MAX_START          10

#define L7_DOT1X_PORT_MIN_START_PERIOD       1
#define L7_DOT1X_PORT_MAX_START_PERIOD       65535

#define L7_DOT1X_PORT_MIN_HELD_PERIOD        1
#define L7_DOT1X_PORT_MAX_HELD_PERIOD        65535

#define L7_DOT1X_PORT_MIN_AUTH_PERIOD        1
#define L7_DOT1X_PORT_MAX_AUTH_PERIOD        65535


/******************** conditional Override *****************************/

#ifdef INCLUDE_DOT1X_EXPORTS_OVERRIDES
#include "dot1x_exports_overrides.h"
#endif

#endif /* __DOT1X_EXPORTS_H_*/
