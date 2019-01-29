
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  l3_bgp_commdefs.h
*
* @purpose   Common defines, enums and data structures for packets
*
* @component
*
* @comments  none
*
* @create    02/18/2002
*
* @author
*
* @end
*
**********************************************************************/

#ifndef L3_BGP_COMMDEFS_H
#define L3_BGP_COMMDEFS_H

#include "l7_common.h"
#include <datatypes.h>



/**************************************
*
*     BGP DEFINES
*
***************************************/


/*--------------------------------------*/
/*  BGP Constants                       */
/*--------------------------------------*/

#define L7_BGP_DOWN     1
#define L7_BGP_UP       2


/*------------------*/
/* BGP TRAP TYPES  */
/*------------------*/

#define     L7_NULL_BGP_TRAP        0
#define     L7_BGP_ESTABLISHED_TRAP 1
#define     L7_BGP_BACK_TRANS_TRAP      2
#define     L7_BGP_ROUTE_INFO_PRINT 4
#define     L7_BGP_NLRI_INFO_PRINT      8
#define     L7_BGP_EVENT_TRAP       16
#define     L7_BGP_RX_PACKET_TRAP       32

#define     L7_BGP_CLASSA_NETWORK_LENGTH        8
#define     L7_BGP_CLASSB_NETWORK_LENGTH        16
#define     L7_BGP_CLASSC_NETWORK_LENGTH        24

#define     L7_BGP_POLICY_RANGE_EQUAL_MAX       4294967294u
#define     L7_BGP_POLICY_RANGE_EQUAL_MIN       0


/*------------------*/
/* BGP ENUMS  */
/*------------------*/

typedef enum
{
  L7_BGP_ROW_ACTIVE = 1,       /* - in call:   activate the object  */
                                /*   in return: the object is active */
  L7_BGP_ROW_NOT_IN_SERVICE,   /* - in call:   deactivate the object */
                                /* - in return: the object is not active */
  L7_BGP_ROW_NOT_READY,        /* - in return only: the object not ready */
  L7_BGP_ROW_CREATE_AND_GO,    /* - in call only: init and run the object */
  L7_BGP_ROW_CREATE_AND_WAIT,  /* - in call only: init the object */
  L7_BGP_ROW_DESTROY           /* - in call only: delete the object */

} L7_BGP_ROW_STATUS_t;


typedef enum
{
 L7_BGP_STOP =1,
 L7_BGP_START

}L7_BGP_PEER_STATE_t;

typedef enum
{
  L7_BGP4_ADVERTISE = 1,
  L7_BGP4_DONOT_ADVERTISE

}L7_BGP_AGGR_EFFECT_t;

#define L7_BGP4_MULTI_PROTO     0x01
#define L7_BGP4_LABLE_DIST      0x02
#define L7_BGP4_VPNCOS          0x04
#define L7_BGP4_ROUTE_REFLECT   0x08
#define L7_BGP4_COMMUNITY       0x10
#define L7_BGP4_CONFED          0x20
#define L7_BGP4_ROUTE_REFRESH   0x40
#define L7_BGP4_ALL             0x7F

#define L7_BGP4_CAP_ENABLE      0x80

typedef enum
{
  L7_BGP_PEER_STATE_ILG = 0,
  L7_BGP_PEER_STATE_IDLE,
  L7_BGP_PEER_STATE_CONNECT,
  L7_BGP_PEER_STATE_ACTIVE,
  L7_BGP_PEER_STATE_OPENSENT,
  L7_BGP_PEER_STATE_OPENCONFIRM,
  L7_BGP_PEER_STATE_ESTABLISHED

}L7_BGP4_PEER_STATE_t;

typedef enum
{
    L7_BGP_PATHATTR_ORIGIN_IGP,
    L7_BGP_PATHATTR_ORIGIN_EGP,
    L7_BGP_PATHATTR_ORIGIN_INCOMPLETE
}L7_BGP4_PATHATTR_ORIGIN_t;

typedef enum
{
    L7_BGP4_LESS_SPECIFICROUTE_NOT_SELECTED = 1,
    L7_BGP4_LESS_SPECIFICROUTE_SELECTED
}L7_BGP4_ATMIC_AGGREGATE_t;

typedef enum
{
  L7_BGP4_AFI_IP = 1,
  L7_BGP4_AFI_IPv6,
  L7_BGP4_AFI_NSAP,
  L7_BGP4_AFI_HDLC,
  L7_BGP4_AFI_BBN,
  L7_BGP4_AFI_802,
  L7_BGP4_AFI_E163,
  L7_BGP4_AFI_E164,
  L7_BGP4_AFI_F69,
  L7_BGP4_AFI_X121,
  L7_BGP4_AFI_IPX,
  L7_BGP4_AFI_APPLETALK,
  L7_BGP4_AFI_DECNET,
  L7_BGP4_AFI_BANYAN_WINES

}L7_BGP4_AFI_t;

typedef enum
{
  L7_BGP4_SAFI_UNICAST = 1,
  L7_BGP4_SAFI_MULTICAST,
  L7_BGP4_SAFI_UNICAST_MULITICAST,
  L7_BGP4_SAFI_LABEL_DISTRIBUTION
}L7_BGP4_SAFI_t;

typedef enum
{
  L7_BGP4_ORIGIN_IGP = 1,
  L7_BGP4_ORIGIN_EGP,
  L7_BGP4_ORIGIN_INCOMPLETE
}L7_BGP4_ORIGIN_t;

typedef enum
{
    L7_IBGP,
    L7_EBGP
} L7_BGP_ROUTE_TYPE_t;

#define L7_BGP_AUTH_DATA_LENGTH              16

typedef enum
{
    L7_BGP_NULL_AUTH,
    L7_BGP_PASSWD_AUTH,
    L7_BGP_CRYPT_AUTH
}L7_BGP_AUTH_t;

typedef enum
{
    L7_BGP4_BEST_FALSE = 1,
    L7_BGP4_BEST_TRUE
}L7_BGP4_BEST_t;

/*------------------------------*/
/* BGP TRAP TYPES  */
/*------------------------------*/

/* The following flags are to be used by user-interface routines to
   enable/disable or get the mode of the specified BGP trap */

#define L7_BGP_TRAP_ALL                                 0x03
#define L7_BGP_TRAP_FSM_ESTABLISHED                     0x01
#define L7_BGP_TRAP_BACKWARD_TRANSITION                 0x02


/*------------------*/
/* BGP4 RANGE SIZES */
/*------------------*/
#define L7_BGP4_PREFIX_LEN_MIN          0
#define L7_BGP4_PREFIX_LEN_MAX          32

#define L7_BGP4_AS_NUMBER_MAX           65535
#define L7_BGP4_AS_NUMBER_MIN           1

#define L7_BGP4_CONFED_ID_MAX           65535
#define L7_BGP4_CONFED_ID_MIN           0

#define L7_BGP4_DAMP_FACTOR_MAX         65535
#define L7_BGP4_DAMP_FACTOR_MIN         1

#define L7_BGP4_FLAP_TIME_MAX         65535
#define L7_BGP4_FLAP_TIME_MIN           1

#define L7_BGP4_PENALTY_INC_MAX         65535
#define L7_BGP4_PENALTY_INC_MIN         1

#define L7_BGP4_REUSE_LIMIT_MAX         65535
#define L7_BGP4_REUSE_LIMIT_MIN         1

#define L7_BGP4_REUSE_SIZE_MAX         65535
#define L7_BGP4_REUSE_SIZE_MIN          1

#define L7_BGP4_SUPPRESS_LIMIT_MAX         65535
#define L7_BGP4_SUPPRESS_LIMIT_MIN          1

#define L7_BGP4_TIMER_RESOLUTION_MAX         65535
#define L7_BGP4_TIMER_RESOLUTION_MIN            1

#define L7_BGP4_AS_ORIG_INTERVAL_MAX         32767
#define L7_BGP4_AS_ORIG_INTERVAL_MIN            1

#define L7_BGP4_ROUTE_ADV_INTERVAL_MAX         32767
#define L7_BGP4_ROUTE_ADV_INTERVAL_MIN          1

#define L7_BGP4_LOCAL_ORIGIN_MAX         3
#define L7_BGP4_LOCAL_ORIGIN_MIN            1

#define L7_BGP4_LOCAL_MED_MAX         2147483647
#define L7_BGP4_LOCAL_MED_MIN           -1

#define L7_BGP4_LOCAL_PREF_MAX         2147483647
#define L7_BGP4_LOCAL_PREF_MIN          -1

#define L7_BGP4_NEXTHOP_ADDR_LEN_MAX         32
#define L7_BGP4_NEXTHOP_ADDR_LEN_MIN            1

#define L7_BGP4_AUTH_KEY_LEN            L7_BGP_AUTH_DATA_LENGTH

#define L7_BGP4_CONN_RETRY_INT_MAX         65535
#define L7_BGP4_CONN_RETRY_INT_MIN          1

#define L7_BGP4_HOLD_TIME_MAX         65535
#define L7_BGP4_HOLD_TIME_MIN           0

#define L7_BGP4_KEEP_ALIVE_TIME_MAX         21845
#define L7_BGP4_KEEP_ALIVE_TIME_MIN         0

#define L7_BGP4_LOCAL_PORT_MAX         65535
#define L7_BGP4_LOCAL_PORT_MIN           0

#define L7_BGP4_REMOTE_PORT_MAX         65535
#define L7_BGP4_REMOTE_PORT_MIN          0

#define L7_BGP4_MSG_SEND_LIMIT_MAX         100
#define L7_BGP4_MSG_SEND_LIMIT_MIN          1

#define L7_BGP4_TX_DELAY_INT_MAX          5
#define L7_BGP4_TX_DELAY_INT_MIN            1

/* Threshold value in neighbor maximum-prefix command */
#define L7_BGP4_MAX_PFX_THRESH_MIN  0
#define L7_BGP4_MAX_PFX_THRESH_MAX  100

#define L7_BGP4_ROUTE_COMMUNITY_MAX         65535
#define L7_BGP4_ROUTE_COMMUNITY_MIN         1

#define L7_BGP4_SNPA_LEN_MAX         128
#define L7_BGP4_SNPA_LEN_MIN            1

#define L7_BGP4_VAL_MIN                  1
#define L7_BGP4_VAL_MAX                  65535

#define L7_BGP4_AS_PATH_MIN              1
#define L7_BGP4_AS_PATH_MAX              65535

#define L7_BGP4_LOCAL_PREFERENCE_MIN     1
#define L7_BGP4_LOCAL_PREFERENCE_MAX     65535

#define L7_BGP4_MULTI_EXIT_DISC_MIN      0
#define L7_BGP4_MULTI_EXIT_DISC_MAX      2147483647

#define L7_BGP4_AS_PATH_LEN_MIN          1
#define L7_BGP4_AS_PATH_LEN_MAX          65535

#define L7_BGP4_OSPF_DESTIN_TYPE_MIN     1
#define L7_BGP4_OSPF_DESTIN_TYPE_MAX     65535

#define L7_BGP4_AGGREGATOR_AS_MIN        0
#define L7_BGP4_AGGREGATOR_AS_MAX        65535

#define L7_BGP4_CONFEDERATION_ID_MIN     1
#define L7_BGP4_CONFEDERATION_ID_MAX     65535

#define L7_BGP4_POLICY_RANGE_MIN         0
#define L7_BGP4_POLICY_RANGE_MAX         4294967295UL

#define L7_BGP4_ACCESS_LIST_MIN          L7_MIN_ACL_ID
#define L7_BGP4_ACCESS_LIST_MAX          L7_MAX_ACL_ID


/*------------------*/
/* BGP4 MISC DEFINES*/
/*------------------*/

#define L7_BGP_LOCAL_MED_ABSENT   L7_BGP4_LOCAL_MED_MIN
#define L7_BGP_LOCAL_PREF_ABSENT  L7_BGP4_LOCAL_PREF_MIN

/* For neighbor maximum-prefixes command, to indicate no limit. */
#define L7_BGP_NBR_MAX_PFX_NOLIMIT 0xFFFFFFFFUL

/* Flapping entry states */
#define BGP_FLAP_NOT_SUPPRESSED             0
#define BGP_FLAP_SUPPRESSED                 1
#define BGP_FLAP_REUSED                     2

/* Flapping events */
#define BGP_FLAP_ADD_EVENT                  2
#define BGP_FLAP_WITHDRAW_EVENT             1
#define BGP_FLAP_INIT_EVENT                 0


#define L7_BGP4_PROTOCOL_ID_MIN 1
#define L7_BGP4_PROTOCOL_ID_MAX 255

#define L7_BGP4_ATOMIC_AGGR_MIN 1
#define L7_BGP4_ATOMIC_AGGR_MAX 2

#define L7_BGP4_POLICY_INDEX_MAX   65535
#define L7_BGP4_POLICY_INDEX_MIN   1


#define FD_RTR_RTO_PREF_BGP                                       170

/*policy related constats */
#define MAX_NUM_OF_POLICIES 128
#define MAX_NUM_OF_LABELS   6

typedef enum
{
    L7_BGP_AS_PATH = 1,
    L7_BGP_ORIGIN,
    L7_BGP_LOCAL_PREFERENCE,
    L7_BGP_MULTI_EXIT_DISC,
    L7_BGP_COMMUNITY,
    L7_BGP_CONFEDERATION_ID,
    L7_BGP_CLUSTER_NUMBER,
    L7_BGP_FLOWSPEC,
    L7_BGP_DEST_IP_PREF,
    L7_BGP_NEXT_HOP,
    L7_BGP_MAX_SETUP_PRIO,
    L7_BGP_MAX_HOLD_PRIO,
    L7_BGP_POLICY_STRING,
    L7_BGP_USER_DEFINE,
    L7_BGP_AS_PATH_LEN,
    L7_BGP_PROTOCOL_ID,
    L7_BGP_OSPF_DESTINATION_TYPE,
    L7_BGP_PEER,
    L7_BGP_ATOMIC_AGGREGATE,
    L7_BGP_AGGREGATOR_AS,
    L7_BGP_AGGREGATOR_ID,
    L7_BGP_ROUTE_ACL,
    L7_BGP_AS_PATH_REGEX,
    L7_BGP_COMMUNITY_REGEX
}L7_BGP_MATCH_TYPE_t;

typedef enum
{
    L7_BGP_POLICY_EQUAL = 1,
    L7_BGP_POLICY_BETWEEN,
    L7_BGP_POLICY_ADDRESS,
    L7_BGP_POLICY_GREATER_THAN,
    L7_BGP_POLICY_LESS_THAN,
    L7_BGP_POLICY_MATCH
}L7_BGP_POLICY_RANGE_t;

typedef enum
{
    L7_BGP_POLICY_MODIFY = 1,
    L7_BGP_POLICY_REMOVE,
    L7_BGP_POLICY_ADD
}L7_BGP_POLICY_ACTION_t;

typedef enum
{
    L7_BGP_POLICY_DENY,
    L7_BGP_POLICY_PERMIT
}L7_BGP_POLICY_ACCESS_t;

typedef enum
{
    L7_BGP_INTERNAL_IN = 1,
    L7_BGP_INTERNAL_OUT,
    L7_BGP_ASYNC_OUT,
    L7_BGP_OSPF,
    L7_BGP_MPLS
}L7_BGP_POLICY_PROTOCOL_t;

typedef enum
{
    L7_BGP_FLAP_STATE_NOT_SUPPRESSED ,
    L7_BGP_FLAP_STATE_SUPPRESSED,
    L7_BGP_FLAP_STATE_REUSED
}L7_BGP_DAMP_STATE_t;

typedef enum
{
    L7_BGP_FLAP_INIT_EVENT ,
    L7_BGP_FLAP_WITHDRAW_EVENT,
    L7_BGP_FLAP_ADD_EVENT
}L7_BGP_DAMP_EVENT_STATE_t;

#endif /* L3_BGP_COMMDEFS_H */
