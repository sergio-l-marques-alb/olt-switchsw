/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename nimapi.h
*
* @purpose Network Interface Manager API Functions, Constants and Data Structures
*
* @component nim
*
* @comments none
*
* @create 08/03/2000
*
* @author bmutz
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/

#ifndef INCLUDE_NIMAPI
#define INCLUDE_NIMAPI

#include "l7_common.h"
#include "async_event_api.h"
#include "sysapi.h"
#include "osapi.h"
#include "l7_product.h"
#include "platform_config.h"
#include "portevent_mask.h"


#define L7_NIM_INTF_DESCR_SIZE  60        /* maximum string size for ifdescription */
#define L7_NIM_IFNAME_SIZE      60        /* maximum string size for ifnames */
#define L7_NIM_IF_ALIAS_SIZE    64

/* NIM startup callback priorities. 
 * NSF capable applications use priority 1000 or higher. */
#define NIM_STARTUP_PRIO_DOT1Q          10000
#define NIM_STARTUP_PRIO_PFC             9950
#define NIM_STARTUP_PRIO_DOT1X           9900
#define NIM_STARTUP_PRIO_DOT3AD          9800
#define NIM_STARTUP_PRIO_DOT1S           9700
#define NIM_STARTUP_PRIO_FDB             8000
#define NIM_STARTUP_PRIO_VOICE_VLAN      7500
#define NIM_STARTUP_PRIO_LLDP            7000
#define NIM_STARTUP_PRIO_POE             7000
#define NIM_STARTUP_PRIO_CP              5000
#define NIM_STARTUP_PRIO_DS              5000
#define NIM_STARTUP_PRIO_DOS             4800
#define NIM_STARTUP_PRIO_IGMP_SNOOPING   4500
#define NIM_STARTUP_PRIO_DAI             4000
#define NIM_STARTUP_PRIO_PML             3000
#define NIM_STARTUP_PRIO_PROTECTED_PORT  3000
#define NIM_STARTUP_PRIO_POLICY          3000
#define NIM_STARTUP_PRIO_COS             3000
#define NIM_STARTUP_PRIO_ACL             3000
#define NIM_STARTUP_PRIO_QOS_VOIP        3000
#define NIM_STARTUP_PRIO_DIFFSERV        3000
#define NIM_STARTUP_PRIO_DVLANTAG        3000
#define NIM_STARTUP_PRIO_FILTER          3000
#define NIM_STARTUP_PRIO_PBVLAN          3000
#define NIM_STARTUP_PRIO_MIRROR          2500
#define NIM_STARTUP_PRIO_IPMAP           2000
#define NIM_STARTUP_PRIO_IP6MAP          2000
#define NIM_STARTUP_PRIO_RLIM            1900
#define NIM_STARTUP_PRIO_LINK_DEPENDENCY 1800
#define NIM_STARTUP_PRIO_SIM             1000
#define NIM_STARTUP_PRIO_DEFAULT          500


/*
 * Type used for Correlators and Handles for the event notification mechanism
 */
typedef L7_uint32 NIM_CORRELATOR_t, NIM_HANDLE_t;


/****************************************
*
*  NIM Unit, Slot, Port Structure
*
*****************************************/

typedef struct
{
  L7_uchar8     unit;
  L7_uchar8     slot;
  L7_ushort16  port;

} nimUSP_t;

/*
 * Type used to indicate various reasons for a failure on Port Event Notifications
 */
typedef enum
{
  NIM_ERR_RC_UNUSED = 0,
  NIM_ERR_RC_TIMEOUT,
  NIM_ERR_RC_RESOURCE_BUSY,
  NIM_ERR_RC_INVALID_INTF,
  NIM_ERR_RC_INVALID_EVNT,
  NIM_ERR_RC_FATAL,
  NIM_ERR_RC_LACK_OF_RESOURCES,
  NIM_ERR_RC_NOT_AVAILABLE,
  NIM_ERR_RC_NOT_FOUND,
  NIM_ERR_RC_BUSY,
  NIM_ERR_RC_IGNORED,
  NIM_ERR_RC_INTERNAL,
  NIM_ERR_RC_INVALID_ID,
  NIM_ERR_RC_INVALID_DATA,
  NIM_ERR_RC_INVALID_RQST,
  NIM_ERR_RC_INVALID_HANDLE,
  NIM_ERR_RC_INVALID_CMD,
  NIM_ERR_RC_INVALID_CMD_TYPE,
  NIM_ERR_RC_INVALID_PAIR,
  NIM_ERR_RC_OUT_OF_SEQUENCE,
  NIM_ERR_RC_OTHER,
  NIM_ERR_RC_LAST

} L7_NIM_ERR_RC_t;

typedef enum
{
  NIM_INTERFACE_CREATE_STARTUP,
  NIM_INTERFACE_ACTIVATE_STARTUP
} NIM_STARTUP_PHASE_t;

/*
 * Type used to return status status Asynchronously
 */
typedef struct
{

  L7_RC_t               rc;
  L7_NIM_ERR_RC_t       reason;

} L7_NIM_ASYNC_RESPONSE_t;



/*
 * Type used to return status to the caller of a Port Event Notification
 */
typedef struct NIM_NOTIFY_CB_INFO_s
{

  L7_NIM_ASYNC_RESPONSE_t   response;
  NIM_HANDLE_t              handle;
  L7_uint32                 intIfNum;
  L7_PORT_EVENTS_t          event;

} NIM_NOTIFY_CB_INFO_t;

/*
 * Type used for the Event Notification Status to be returned to the component
 */
typedef void (*NIM_NOTIFY_CB_FUNC_t)(NIM_NOTIFY_CB_INFO_t retVal);

/*
 * Type used to pass data to NIM for event notifications
 */
typedef struct NIM_EVENT_NOTIFY_INFO_s
{

  L7_COMPONENT_IDS_t    component;
  L7_PORT_EVENTS_t      event;
  L7_uint32             intIfNum;
  NIM_NOTIFY_CB_FUNC_t  pCbFunc;

} NIM_EVENT_NOTIFY_INFO_t;

/*
 * Type used to pass data to NIM for event notifications
 */
typedef struct NIM_INTF_CREATE_INFO_s
{

  L7_COMPONENT_IDS_t    component;
  NIM_NOTIFY_CB_FUNC_t  pCbFunc;

} NIM_INTF_CREATE_INFO_t;

/*
 * Type used to return status to NIM by the component at the conclusion of an Event
 */
typedef struct
{

  L7_NIM_ASYNC_RESPONSE_t response;
  NIM_CORRELATOR_t        correlator;
  L7_COMPONENT_IDS_t      component;
  L7_PORT_EVENTS_t        event;
  L7_uint32               intIfNum;

} NIM_EVENT_COMPLETE_INFO_t;

/************************************************************************************************
 * Macro Port Information
 *
 * This structure contains information about interfaces which are made up of a composite of
 * other interfaces.
 *
 *
 * The pointer macroInfo  depends upon the type of macroport.  It may be cast to
 * 1. *nimIntf_t, if the macro port is the physical port or the internal Bridge/Router interface.
 * 2. a pointer to a link aggregation structure, if the macroport is a LAG.
 *
 **************************************************************************************************/

typedef struct
{
  L7_uint32   macroPort;      /* Port Number of owning macroport */
  L7_uint32   macroType;      /* Type of owning macroport */
  L7_uint32   macroMtu;       /* (Layer 3) MTU of owning macroport */
  L7_uint32   macroMaxFrame;  /* (Layer 2) Max Frame Size of owning macroport */
  void        *macroInfo;     /* Pointer to macroport information */

} nimMacroPort_t;

/* Interface Structures for creation */
/* Jumbo Frame Size specification */

typedef struct   nimFrameSizeCapability_s
{
  L7_uint32   frameSize;
  L7_uint32   smallestFrameSize;
  L7_uint32   largestFrameSize;

} nimFrameSizeCapability_t;

/*
 * Type used for saving and retrieving Configuration information across boots
 * In order to create an invalid nimConfigID_t, one should use the memset(&dst,0,sizeof(nimConfigID_t)
 * Comparison of the nimConfigID_t should be done by the NIM_CONFIG_ID_IS_EQUAL
 */
/* Type for the configuration interface ID */
typedef struct
{
  L7_INTF_TYPES_t   type;

  union
  {
    nimUSP_t    usp;
    L7_uint32   vlanId;
    L7_uint32   dot3adIntf;
    L7_uint32   loopbackId;
    L7_uint32   tunnelId;
    L7_uint32   wirelessNetId;
    L7_uint32   l2tunnelId;
  }configSpecifier;

} nimConfigID_t;

typedef struct  nimIntfConfig_s
{
  L7_enetMacAddr_t   LAAMacAddr;           /* Locally Administered MAC Address  */

  L7_uint32   addrType;                    /* Using L7_BIA or L7_LAA */

  L7_char8    ifAlias[L7_NIM_IF_ALIAS_SIZE+1]; /* User-Defined Name */

  L7_uint32   nameType;                    /* Using L7_SYSNAME or L7_ALIASNAME */

  L7_uint32   ifSpeed;                     /* ifType (MIB II) duplex is included*/

  L7_uint32   negoCapabilities;            /* negotiation capabilities: disabled, 10h, 10f, 100h, 100f, 1000f, all */
  L7_uint32   mgmtAdminState;              /* ifAdminStatus: L7_ENABLE or L7_DISABLE
                                              as configured by management entity. */

  L7_uint32   adminState;                  /* ifAdminStatus: L7_ENABLE or L7_DISABLE -- operational */

  L7_uint32   trapState;                   /* link Trap status: L7_ENABLE or L7_DISABLE */

  L7_uint32   ipMtu;                       /* Maximum size of data portion of a frame on the port.
                                              Derived from cfgMaxFrameSize. Not the IP MTU.
                                              Not configured directly but here for historical
                                              reasons. */

  L7_uint32   encapsType;                  /* Encapsulation Type of frame: L7_ENCAPSULATION_t*/

  L7_uint32   cfgMaxFrameSize;             /* Maximum configurable frame size on the port*/
} nimIntfConfig_t;

typedef struct  nimIntfDescr_s
{

  L7_BOOL          configurable;   /* end user configurable - save configuration across resets */
  L7_BOOL          internal;       /* internal interface only (i.e. CPU, tunnel, or other pseudo-interface */
  L7_uint32        settableParms;  /* settable parameters */
  L7_enetMacAddr_t macAddr;        /* burned-in mac addr */
  L7_enetMacAddr_t bcastMacAddr;      /* bcast addr */
  L7_enetMacAddr_t l3MacAddr;      /* L3 router MAC addr */

  nimFrameSizeCapability_t       frameSize;       /* includes Jumbo Frame specification */

  L7_IANA_INTF_TYPE_t   ianaType;
  L7_PORT_SPEEDS_t      defaultSpeed;

  L7_PHY_CAPABILITIES_t    phyCapability;
  L7_CONNECTOR_TYPES_t     connectorType;

  char           ifDescr[L7_NIM_INTF_DESCR_SIZE]; /* ifDescr -  One of IANA descriptor values */
  char           ifName[L7_NIM_IFNAME_SIZE];      /* ifDescr - for short form of interface */
  char           ifLongName[L7_NIM_IFNAME_SIZE];  /* for long form of the interface*/

  nimMacroPort_t macroPort;    /* MacroPort Information, if any.  */
} nimIntfDescr_t;

/* Details interface  */
typedef struct nimIntfCreateRequest_s
{
  nimConfigID_t       *pIntfIdInfo;  /* unique interface specification info */
  nimIntfDescr_t      *pIntfDescr;   /* unique interface descriptor  info */
  nimIntfConfig_t     *pDefaultCfg;  /* default non-volatile interface configuration info */
  NIM_INTF_CREATE_INFO_t *pCreateInfo; /* the component id of the caller and the callback func */

} nimIntfCreateRequest_t;

typedef struct nimIntfCreateOutput_s
{
  L7_uint32       *intIfNum;  /* the internal interface created */
  NIM_HANDLE_t    *handle;    /* used to match the response with this request */
} nimIntfCreateOutput_t;

/*
 * Macro used to determine if two IDs are equal
 * returns L7_TRUE if equal or L7_FALSE if not equal
 */
#define NIM_CONFIG_ID_IS_EQUAL(_a,_b) ((memcmp(_a,_b,sizeof(nimConfigID_t)) == 0)?L7_TRUE:L7_FALSE)

/*
 * Macro used to copy the contents of one config ID to another
 * both parameters are to be pointers to nimConfigID_t
 * This macro should be used the same as an assignment
 * therefore, the caller should insure that both the src and dst pointers are valid
 */
#define NIM_CONFIG_ID_COPY(_dst,_src) (memcpy(_dst,_src,sizeof(nimConfigID_t)))



/*
 * Enumeration used for the query mechanism
 */
typedef enum L7_NIM_QUERY_RQST_e
{
  L7_NIM_QRY_RQST_FIRST  = 1,

  L7_NIM_QRY_RQST_STATE,
  L7_NIM_QRY_RQST_PRESENT,

  L7_NIM_QRY_RQST_LAST

} L7_NIM_QUERY_RQST_t;

/*
 * Type used to query NIM for information
 */
typedef struct
{

  L7_uint32           intIfNum;             /* input */
  L7_NIM_QUERY_RQST_t request;              /* input */
  L7_NIM_ERR_RC_t     reason;               /* ouput */

  union
  {
    L7_INTF_STATES_t            state;
    L7_BOOL                     present;
  } data;                                     /* output */

} L7_NIM_QUERY_DATA_t;


#if L7_FEAT_SF10GBT

 /* What is proper place to put following definitions to - some platform-specific header ? 
SYS_SFX7101_FW_PN_LENGTH
SYS_SFX7101_FW_REV_LENGTH
  */

#if 1 /* FIXUP TODO RESOLVE LATER */
#define L7_NIM_IF_FW_PART_NUM_LENGTH    8
#define L7_NIM_IF_FW_REV_LENGTH         4
#else
#define L7_NIM_IF_FW_PART_NUM_LENGTH    SYS_SFX7101_FW_PN_LENGTH
#define L7_NIM_IF_FW_REV_LENGTH         SYS_SFX7101_FW_REV_LENGTH
#endif

#define NIM_INTF_PORT_FW_REV_SIZE       (L7_NIM_IF_FW_PART_NUM_LENGTH + L7_NIM_IF_FW_REV_LENGTH)

typedef struct  nimIntfPortFWRev_s
{
  L7_uchar8    ifFwPartNumber[L7_NIM_IF_FW_PART_NUM_LENGTH + 1]; /* Firmware part number*/
  L7_uchar8    ifFwRevision[L7_NIM_IF_FW_REV_LENGTH + 1]; /* Firmware revision*/
} nimIntfPortFWRev_t;

extern nimIntfPortFWRev_t nimIntfPortFWRevUndefined;

#endif

/*****************************************************************************/
/*********************NIM INTERFACE MASK MACROS & DEFINES*********************/
/*****************************************************************************/

/* Number of bytes in mask */
#define NIM_INTF_INDICES   ((L7_MAX_INTERFACE_COUNT - 1) / (sizeof(L7_uchar8) * 8) + 1)

/* Interface storage */
typedef struct
{
  L7_uchar8 value[NIM_INTF_INDICES];
} NIM_INTF_MASK_t;

/*
 * NONZEROMASK returns true if any bit in word mask of NUM length
 * is turned-on.  The result, TRUE or FALSE is stored in
 * result.
 */
#define NIM_INTF_NONZEROMASK(mask, result){                           \
    L7_uint32 _i_;                                                    \
    NIM_INTF_MASK_t *_p_;                                             \
                                                                      \
    _p_ = (NIM_INTF_MASK_t *)&mask;                                   \
    for(_i_ = 0; _i_ < NIM_INTF_INDICES; _i_++)                       \
        if(_p_ -> value[_i_]){                                        \
            result = L7_TRUE;                                         \
            break;                                                    \
        }                                                             \
        else                                                          \
            result = L7_FALSE;                                        \
}



/* Least significant bit/rightmost bit is lowest interface # */
/* this is opposite of what SNMP wants */

/* SETMASKBIT turns on bit index # k in mask j. */
#define NIM_INTF_SETMASKBIT(j, k)                                    \
            ((j).value[((k-1)/(8*sizeof(L7_uchar8)))] \
                         |= 1 << ((k-1) % (8*sizeof(L7_uchar8))))



/* CLRMASKBIT turns off bit index # k in mask j. */
#define NIM_INTF_CLRMASKBIT(j, k)                                    \
           ((j).value[((k-1)/(8*sizeof(L7_uchar8)))]   \
                        &= ~(1 << ((k-1) % (8*sizeof(L7_uchar8)))))



/* MASKEQ sets mask j equal to mask k. */
#define NIM_INTF_MASKEQ(j, k) { \
        L7_uint32 x; \
 \
        for (x = 0; x < NIM_INTF_INDICES; x++) { \
                (j).value[x] = (k).value[x]; \
        } \
}



/* MASKOREQ or's on the bits in mask j that are on in either mask j or k. */
#define NIM_INTF_MASKOREQ(j, k) { \
        L7_uint32 x; \
 \
        for (x = 0; x < NIM_INTF_INDICES; x++) { \
                (j).value[x] |= (k).value[x]; \
        } \
}



/* MASKEXOREQ turns-on the bits in mask j that are on in either mask j and k but not in both. */
#define NIM_INTF_MASKEXOREQ(j, k) { \
        L7_uint32 x; \
 \
        for (x = 0; x < NIM_INTF_INDICES; x++) { \
                j.value[x] ^= k.value[x]; \
        } \
}



/* MASKANDEQ turns-on the bits in mask j that are on in both mask j and k. */
#define NIM_INTF_MASKANDEQ(j, k) { \
        L7_uint32 x; \
 \
        for (x = 0; x < NIM_INTF_INDICES; x++) { \
                j.value[x] &= k.value[x]; \
        } \
}

/* MASKINV inverts the bits in mask j. */
#define NIM_INTF_MASKINV(j) { \
        L7_uint32 x; \
 \
        for (x = 0; x < NIM_INTF_INDICES; x++) { \
                j.value[x] = ~(j.value[x]); \
        } \
}

/* MASKANDEQINV turns on the bits in mask j that are on in both mask j and the
   bitwise-inverse of mask k. */
#define NIM_INTF_MASKANDEQINV(j, k) { \
        L7_uint32 x; \
 \
        for (x = 0; x < NIM_INTF_INDICES; x++) { \
                j.value[x] &= ~(k.value[x]); \
        } \
}


/* FHMASKBIT finds the index of the most-significant bit turned-on in
   mask j and returns that index in k.  Since this is a 1-based
   mask, 0 is returned for "no bits set". */
#define NIM_INTF_FHMASKBIT(j, k) { \
        L7_int32 x; \
 \
        for (x = (NIM_INTF_INDICES - 1); x >= 0; x--) { \
                if ( j.value[x] ) \
                        break; \
        }; \
        k = 0; \
        if (x >= 0) { \
/* This is for i960
                asm volatile ("scanbit %1,%0" : "=d"(k) : "d"(j.value[x])); */ \
/* This is not */ \
                L7_int32 i; \
                for (i = 7; i >= 0; i--) { \
                    if ( j.value[x] & (1 << i)) { \
                       k = i + 1 + (x * (8 * sizeof(L7_uchar8))); \
                       break; \
                    } \
                } \
/* End non-i960 */ \
        }; \
}



/* ISMASKBITSET returns 0 if the interface k is not set in mask j */
#define NIM_INTF_ISMASKBITSET(j, k)                               \
        ((j).value[((k-1)/(8*sizeof(L7_uchar8)))]  \
                         & ( 1 << ((k-1) % (8*sizeof(L7_char8)))) )



/****************************************
*
*  NIM API Functions
*
*****************************************/
/* PTin added: ptin_intf
   Check if nim message queue is empty */
extern L7_BOOL nimIntfRequestsDone(void);

/*********************************************************************
  * @purpose  Returns the descripion for port event
  *
  * @param    event     interface event
  *
  * @returns  description for the event

  * @notes    none
  *
  * @end
  *********************************************************************/
extern L7_char8 *nimGetIntfEvent(L7_PORT_EVENTS_t event);


/*********************************************************************
* @purpose  Register a routine to be called when a link state changes.
*
* @param    registrar_ID   @b{(input)} routine registrar id  (See L7_COMPONENT_ID_t)
* @param    *notify        @b{(input)} pointer to a routine to be invoked for link state
*                          changes.  Each routine has the following parameters:
*                          (internal interface number, event(L7_UP, L7_DOWN,
*                           etc.)).
* @param    *startup_notify @b{(input)} pointer to a routine to be invoked at startup.
*                          Each routine has the following parameters:
*                          (startup_phase(NIM_INTERFACE_CREATE_STARTUP,
*                                         NIM_INTERFACE_ACTIVATE_STARTUP)).
* @param    priority       @b{(input)} priority of the startup notification.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
typedef  void (*StartupNotifyFcn)(NIM_STARTUP_PHASE_t startup_phase);

extern L7_RC_t nimRegisterIntfChange( L7_COMPONENT_IDS_t registrar_ID,
                                     L7_RC_t (*notify)(L7_uint32 intIfNum, L7_uint32 event,NIM_CORRELATOR_t correlator),
                                     StartupNotifyFcn startupFcn,
                                     L7_uint32 priority);

/*******************************************************************************
* @purpose  To allow components to register only for port events that it processes
*
* @param    registrar_ID     @b{(input)} routine registrar id  (See L7_COMPONENT_ID_t)
* @param    registeredEvents @b{(input)} Bit mask of port events that component requests
*                              notification
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*******************************************************************************/
L7_RC_t nimRegisterIntfEvents(L7_COMPONENT_IDS_t  registrar_ID,
                              PORTEVENT_MASK_t    registeredEvents);

/*********************************************************************
* @purpose  De-Register a routine to be called when a link state changes.
*
* @param    registrar_ID   @b{(input)} routine registrar id  (See L7_COMPONENT_ID_t)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t nimDeRegisterIntfChange( L7_COMPONENT_IDS_t registrar_ID);

/*********************************************************************
* @purpose  Send message to nim to Notifies registered routines of interface changes.
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    event       @b{(input)} all port events,
*                       (@b{  L7_CREATE,
*                             L7_DELETE,
*                             L7_PORT_DISABLE,
*                             L7_PORT_ENABLE,
*                             L7_UP,
*                             L7_DOWN,
*                             L7_ENABLE,
*                             L7_DISABLE,
*                             L7_DIAG_DISABLE,
*                             L7_FORWARDING,
*                             L7_NOT_FORWARDING,
*                             L7_CREATE,
*                             L7_DELETE,
*                             L7_ACQUIRE,
*                             L7_RELEASE,
*                             L7_SPEED_CHANGE,
*                             L7_LAG_CFG_CREATE,
*                             L7_LAG_CFG_MEMBER_CHANGE,
*                             L7_LAG_CFG_REMOVE,
*                             L7_LAG_CFG_END,
*                             L7_PROBE_SETUP,
*                             L7_PROBE_TEARDOWN,
*                             L7_SET_INTF_SPEED,
*                             L7_SET_MTU_SIZE,
*                             L7_VRRP_TO_MASTER or
*                             L7_VRRP_FROM_MASTER})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t nimNotifyIntfChange(L7_uint32 intIfNum, L7_uint32 event);


/*********************************************************************
* @purpose  Returns the ifIndex associated with the
*           internal interface number
*
* @param    intIfNum    @b{(input)}  internal interface number
* @param    ifIndex     @b{(output)} pointer to ifIndex,
*                       (@b{Returns: ifIndex}).
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t nimGetIntfIfIndex(L7_uint32 intIfNum, L7_uint32 *ifIndex);


/*********************************************************************
* @purpose  Returns the internal interface number
*           associated with the **external** interface number, or ifIndex
*
* @param    extIfNum    @b{(input)}  external interface number
* @param    intIfNum    @b{(output)} pointer to Internal Interface Number,
*                       (@b{Returns: Internal Interface Number}).
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t nimGetIntfNumber(L7_uint32 extIfNum, L7_uint32 *intIfNum);


/*********************************************************************
* @purpose  Returns the Unit-Slot-Port
*           associated with the internal interface number
*
* @param    intIfNum    @b{(input)}  internal interface number
* @param    usp         @b{(output)} pointer to nimUSP_t structure,
*                       (@b{Returns: nimUSP_t structure}).
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t nimGetUnitSlotPort(L7_uint32 intIfNum, nimUSP_t *usp);


/*********************************************************************
* @purpose  Returns the internal interface number
*           associated with the Unit-Slot-Port
*
* @param    usp         @b{(input)}  pointer to nimUSP_t structure
* @param    intIfNum    @b{(output)} pointer to internal interface number,
*                       (@b{Returns: Internal Interface Number}).
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t nimGetIntIfNumFromUSP(nimUSP_t* usp, L7_uint32 *intIfNum);


/*********************************************************************
* @purpose  Obtain the next **external** interface number, or ifIndex
*
* @param    extIfNum      @b{(input)}  external interface number
* @param    nextextIfNum  @b{(output)} pointer to next external interface number
*                       (@b{Returns: Next External Interface Number}).
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t nimGetNextExtIfNumber(L7_uint32 extIfNum, L7_uint32 *nextExtIfNum);


/*********************************************************************
* @purpose  Determine if this **external** interface number, or ifIndex
*           exists
*
* @param    extIfNum      @b{(input)} external interface number
*
* @returns  L7_SUCCESS     if interface exists
* @returns  L7_ERROR       if interface does not exist
* @returns  L7_FAILURE     if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t nimCheckExtIfNumber(L7_uint32 extIfNum);


/*********************************************************************
* @purpose  Determine if this internal interface number is valid
*
* @param    intIfNum      @b{(input)} internal interface number
*
* @returns  L7_SUCCESS     if interface exists
* @returns  L7_ERROR       if interface does not exist
* @returns  L7_FAILURE     if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t nimCheckIfNumber(L7_uint32 intIfNum);


/*********************************************************************
* @purpose  Returns the internal interface type
*           associated with the internal interface number
*
* @param    intIfNum    @b{(input)}  internal interface number
* @param    sysIntfType @b{(output)} pointer to internal interface type,
*                       (@b{Returns: Internal Interface Type}).
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t nimGetIntfType(L7_uint32 intIfNum, L7_INTF_TYPES_t *sysIntfType);


/*********************************************************************
* @purpose  Sets the speed of the specified interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    intfSpeed   @b{(input)} Interface Speed,
*                       (@b{  L7_PORTCTRL_PORTSPEED_FULL_100TX,
*                             L7_PORTCTRL_PORTSPEED_HALF_100TX,
*                             L7_PORTCTRL_PORTSPEED_FULL_10T,
*                             L7_PORTCTRL_PORTSPEED_HALF_10T,
*                             L7_PORTCTRL_PORTSPEED_FULL_100FX,
*                             L7_PORTCTRL_PORTSPEED_FULL_1000SX,
*                             L7_PORTCTRL_PORTSPEED_UNKNOWN})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t nimSetIntfSpeed(L7_uint32 intIfNum, L7_uint32 intfSpeed);


/*********************************************************************
* @purpose  Sets the default speed of the specified interface -- NO it doesn't
*           This function really parameter checks the speed asked for vs. port
*           capabilities.
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    intfSpeed   @b{(input)} Interface Speed,
*                       (@b{  L7_PORTCTRL_PORTSPEED_FULL_100TX,
*                             L7_PORTCTRL_PORTSPEED_HALF_100TX,
*                             L7_PORTCTRL_PORTSPEED_FULL_10T,
*                             L7_PORTCTRL_PORTSPEED_HALF_10T,
*                             L7_PORTCTRL_PORTSPEED_FULL_100FX,
*                             L7_PORTCTRL_PORTSPEED_FULL_1000SX,
*                             L7_PORTCTRL_PORTSPEED_UNKNOWN})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    Unlike nimSetIntfSpeed(), this function does not
*           call DTL to actually set the speed of the interface.
*           It only sets the ifSpeed field in the NIM port config structure.
*
* @end
*********************************************************************/
extern L7_RC_t nimSetDefaultIntfSpeed(L7_uint32 intIfNum, L7_uint32 intfSpeed);


/*********************************************************************
* @purpose  Sets the management administrative state of the specified interface.
*
* @param    intIfNum Internal Interface Number
*
* @param    adminState admin state,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t nimSetIntfMgmtAdminState(L7_uint32 intIfNum, L7_uint32 adminState);


/*********************************************************************
* @purpose  Sets the administrative state of the specified interface.
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    adminState  @b{(input)} admin state,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t nimSetIntfAdminState(L7_uint32 intIfNum, L7_uint32 adminState);


/*********************************************************************
* @purpose  Sets the locally administered address
*           of the specified interface.
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    macAddr     @b{(input)} Pointer to LAA MAC Address.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t nimSetIntfLAA(L7_uint32 intIfNum, L7_uchar8 *macAddr);


/*********************************************************************
* @purpose  Sets either the burned-in or locally administered address
*           of the specified interface.
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    addrType    @b{(input)} address type,
*                       (@b{  L7_BIA or
*                             L7_LAA})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes none
*
* @end
*********************************************************************/
extern L7_RC_t nimSetIntfAddrType(L7_uint32 intIfNum, L7_uint32 addrType);


/*********************************************************************
* @purpose  Sets the link trap configuration of the specified interface.
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    trapState   @b{(input)} trap state,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    This routine should call SNMP trap manager routine.  It is
*           here for completeness and to refer the user to that routine.
*
* @end
*********************************************************************/
extern L7_RC_t nimSetIntfLinkTrap(L7_uint32 intIfNum, L7_uint32 trapState);


/*********************************************************************
* @purpose  Sets the ifAlias name of the specified interface.
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    ifAlias     @b{(input)} pointer to string containing alias name
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes none
*
* @end
*********************************************************************/
extern L7_RC_t nimSetIntfifAlias(L7_uint32 intIfNum, L7_uchar8 *ifAlias);


/*********************************************************************
* @purpose  Sets either the system or alias name
*           of the specified interface.
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    nameType    @b{(input)} name type,
*                       (@b{  L7_SYSNAME or
*                             L7_ALIASNAME})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes none
*
* @end
*********************************************************************/
extern L7_RC_t nimSetIntfNameType(L7_uint32 intIfNum, L7_uint32 nameType);


/*********************************************************************
* @purpose  Gets the physical link state of the specified interface.
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    intfSpeed   @b{(output)} pointer to Interface Speed,
*                       (@b{Returns:  L7_PORTCTRL_PORTSPEED_AUTO_NEG   = 1,
*                             L7_PORTCTRL_PORTSPEED_HALF_100TX,
*                             L7_PORTCTRL_PORTSPEED_FULL_100TX,
*                             L7_PORTCTRL_PORTSPEED_HALF_10T,
*                             L7_PORTCTRL_PORTSPEED_FULL_10T,
*                             L7_PORTCTRL_PORTSPEED_FULL_100FX,
*                             L7_PORTCTRL_PORTSPEED_FULL_1000SX,
*                             L7_PORTCTRL_PORTSPEED_UNKNOWN})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t nimGetIntfSpeedStatus(L7_uint32 intIfNum, L7_uint32 *intfSpeed);

/*********************************************************************
*
* @purpose  Gets the status of the speed if congigured to auto.
*
* @param intIfNum  L7_uint32 the internal interface number
* @param *val      L7_uint32 pointer to link configured state
*
* @returns  L7_TRUE, if speed is configured to auto
* @returns  L7_FALSE, if other failure
*
* @end
*********************************************************************/
L7_BOOL nimIsIntfSpeedAuto(L7_uint32 intIfNum);


/*********************************************************************
* @purpose  Gets the link configured state of the specified interface.
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    intfSpeed   @b{(output)} pointer to Interface Speed,
*                       (@b{Returns:   L7_PORTCTRL_PORTSPEED_AUTO_NEG,
*                                      L7_PORTCTRL_PORTSPEED_FULL_100TX,
*                                      L7_PORTCTRL_PORTSPEED_HALF_100TX,
*                                      L7_PORTCTRL_PORTSPEED_FULL_10T,
*                                      L7_PORTCTRL_PORTSPEED_HALF_10T,
*                                      L7_PORTCTRL_PORTSPEED_FULL_100FX,
*                                      L7_PORTCTRL_PORTSPEED_FULL_1000SX})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t nimGetIntfSpeed(L7_uint32 intIfNum, L7_uint32 *intfSpeed);


/*********************************************************************
* @purpose  Gets the auto-negotiation admin status of the specified interface.
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    intfSpeed   @b{(output)} pointer to Auto-negotiation admin status,
*                       (@b{Returns:   L7_ENABLE,
*                                      L7_DISABLE})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t nimGetIntfAutoNegAdminStatus(L7_uint32 intIfNum, L7_uint32 *autoNegAdminStatus);


/*********************************************************************
* @purpose  Gets the physical link simplex data rate of the specified interface.
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    intfRate    @b{(output)} pointer to Interface Data Rate
*                       (@b{Returns: link data rate in Mbps})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    The data rate is determined from the ifSpeed encode maintained
*           by NIM.
*
* @end
*********************************************************************/
extern L7_RC_t nimGetIntfSpeedSimplexDataRate(L7_uint32 intIfNum, L7_uint32 *intfSpeed);


/*********************************************************************
* @purpose  Gets the physical link data rate of the specified interface.
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    intfRate    @b{(output)} pointer to Interface Data Rate
*                       (@b{Returns: link data rate in Mbps})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    The data rate is determined from the ifSpeed encode maintained
*           by NIM.  Full duplex links are reported as having twice the
*           data rate as their half duplex counterparts.
*
* @end
*********************************************************************/
extern L7_RC_t nimGetIntfSpeedDataRate(L7_uint32 intIfNum, L7_uint32 *intfSpeed);


/*********************************************************************
* @purpose  Indicates whether a physical link is full duplex
*
* @param    intIfNum    @b{(input)} Internal Interface Number
*
* @returns  L7_TRUE     if link is full duplex
* @returns  L7_FALSE    if link is half duplex, or if interface is invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL nimIsIntfSpeedFullDuplex(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Retrieves the duplex value of the port i.e., auto | full | half
*
* @param UnitIndex  L7_uint32 the unit for this operation
* @param intIfNum    Internal Interface Number
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimIntfDuplexGet(L7_uint32 intIfNum,L7_uint32 *duplexvalue);

/*********************************************************************
* @purpose  Retrieves the speed value of the port
*
* @param UnitIndex  L7_uint32 the unit for this operation
* @param intIfNum    Internal Interface Number
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimIntfSpeedGet(L7_uint32 intIfNum, L7_uint32 *speedvalue);

/*********************************************************************
* @purpose  Gets the negotiated Tx and Rx pause status of the specified interface.
*
* @param    intIfNum    Internal Interface Number
* @param    rxPauseStatus  L7_TRUE if autonegotiation result allows Rx pause
* @param    txPauseStatus  L7_TRUE if autonegotiation result allows Tx pause
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimGetIntfPauseStatus(L7_uint32 intIfNum, L7_BOOL *rxPauseStatus, L7_BOOL *txPauseStatus);

/*********************************************************************
* @purpose  Gets the management administrative state of the specified interface.
*
* @param    intIfNum Internal Interface Number
*
* @param    adminState admin state,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t nimGetIntfMgmtAdminState(L7_uint32 intIfNum, L7_uint32 *adminState);

/*********************************************************************
* @purpose  Gets the admin state of the specified interface.
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    adminState  @b{(output)} pointer to Admin State,
*                       (@b{Returns: L7_DISABLE,
*                                    L7_ENABLE
*                                    or L7_DIAG_DISABLE})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t nimGetIntfAdminState(L7_uint32 intIfNum, L7_uint32 *adminState);


/*********************************************************************
* @purpose  Gets the link state of the specified interface.
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    linkState   @b{(output)} pointer to Link State,
*                       (@b{Returns: L7_UP
*                                    or L7_DOWN})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    A physical port has link up when the PHY has link up.
* @notes    A LAG has link up when at least one of the member ports has link up.
* @notes    A VLAN interface has link up when at least one of the member ports of the VLAN has link up.
*
* @end
*********************************************************************/
extern L7_RC_t nimGetIntfLinkState(L7_uint32 intIfNum, L7_uint32 *linkState);


/*********************************************************************
* @purpose  Gets the active state of the specified interface.
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    activeState @b{(output)} pointer to Active State,
*                       (@b{Returns: L7_ACTIVE
*                                    or L7_INACTIVE})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    An interface is considered active if it both has link up and is in forwarding state.
*
* @end
*********************************************************************/
extern L7_RC_t nimGetIntfActiveState(L7_uint32 intIfNum, L7_uint32 *activeState);


/*********************************************************************
* @purpose  Gets the active of the specified interface.
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
*
* @returns  NIM interface state
*
* @notes    This is an API for internal function nimUtilIntfStateGet
*
* @end
*********************************************************************/
L7_INTF_STATES_t nimGetIntfState(L7_uint32 intIfNum);


/*********************************************************************
*
* @purpose  Gets the (Layer 3) MTU size of the specified interface
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    mtuSize     @b{(output)} pointer to mtu Size,
*                       (@b{Returns: mtu size})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t nimGetIntfMtuSize(L7_uint32 intIfNum,L7_uint32 *mtuSize);


/*********************************************************************
* @purpose  Gets the (Layer 2) max frame size of the specified interface
*
* @param    intIfNum       @b{(input)}  Internal Interface Number
* @param    maxFrameSize   @b{(output)} pointer to Max Frame Size,
*                         (@b{Returns: max frame size})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t nimGetIntfMaxFrameSize(L7_uint32 intIfNum, L7_uint32 *maxFrameSize);

/*********************************************************************
* @purpose  Gets the configured (Layer 2) max frame size of the specified interface
*
* @param    intIfNum       @b{(input)}  Internal Interface Number
* @param    maxFrameSize   @b{(output)} pointer to Max Frame Size
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t nimGetIntfConfigMaxFrameSize(L7_uint32 intIfNum, L7_uint32 *maxFrameSize);

/*********************************************************************
* @purpose  Sets the (Layer 2) max frame size of the specified interface
*
* @param    intIfNum       Internal Interface Number
* @param    maxFrameSize   pointer to Max Frame Size,
*                         (@b{Returns: max frame size})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimSetIntfConfigMaxFrameSize(L7_uint32 intIfNum, L7_uint32 maxFrameSize);

/*********************************************************************
* @purpose  Gets either the burned-in or locally administered address
*           of the specified interface, as requested
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    addrType    @b{(input)} address type requested (L7_SYSMAC_BIA, L7_SYSMAC_LAA,
*                       or L7_NULL) L7_NULL will return currently configured
*                       MAC Address
* @param    macAddr     @b{(output)} pointer to MAC Address,
*                       (@b{Returns: 6 byte mac address})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t nimGetIntfAddress(L7_uint32 intIfNum, L7_uint32 addrType, L7_uchar8 *macAddr);

/* PTin added: MAC address */
#if 1
/*********************************************************************
* @purpose  Sets a new MAC address to the specified interface
*
* @param    intIfNum    Internal Interface Number
* @param    addrType    address type requested (L7_SYSMAC_BIA, L7_SYSMAC_LAA,
*                       or L7_NULL) L7_NULL will return currently configured
*                       MAC Address
* @param    macAddr     pointer to MAC Address,
*                       (@b{Returns: 6 byte mac address})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t nimSetIntfAddress(L7_uint32 intIfNum, L7_uint32 addrType, L7_uchar8 *macAddr);
#endif

/*********************************************************************
* @purpose  Gets the L3 router MAC Address of the specified interface
*
* @param    intIfNum    Internal Interface Number
* @param    addrType    address type requested (L7_SYSMAC_BIA, L7_SYSMAC_LAA,
*                       or L7_NULL) L7_NULL will return currently configured
*                       MAC Address
* @param    macAddr     pointer to MAC Address,
*                       (@b{Returns: 6 byte mac address})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    L3 applications use the router MAC address as the source address.
*           This API provides 2 options: (a) use one single burned-in/LAA address
*           for all protocols enabled on this interface, OR (b) use burned-in/LAA
*           address for L2 switching protocols, and (burned-in/LAA+1) address for
*           L3 routing protocols.
*
* @end
*********************************************************************/

extern L7_RC_t nimGetIntfL3MacAddress(L7_uint32 intIfNum, L7_uint32 addrType, L7_uchar8 *macAddr);

/*********************************************************************
* @purpose  Gets the address type being used, either the burned-in or
*           locally administered address of the specified interface.
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    addrType    @b{(output)} address type,
*                       (@b{  Returns: L7_BIA or
*                             L7_LAA})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes none
*
* @end
*********************************************************************/
extern L7_RC_t nimGetIntfAddrType(L7_uint32 intIfNum, L7_uint32 *addrType);


/*********************************************************************
*
* @purpose  Gets either the system name or alias name
*           of the specified interface, as requested
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    nameType    @b{(input)} name type requested (L7_SYSNAME, L7_ALIASNAME or L7_NULL)
*                       L7_NULL will return currently configured ifName
* @param    ifName      @b{(output)} pointer to Interface Name,
*                       (@b{Returns: MAX_INTF_NAME byte interface name})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t nimGetIntfName(L7_uint32 intIfNum, L7_uint32 nameType, L7_uchar8 *ifName);

/*********************************************************************
*
* @purpose  Define the system ifName and ifDescr for the specified interface
*
* @param    configId        @b{(input)} NIM configID for the interface
* @param    configId        @b{(input)} NIM configID for the interface
* @param    *ifName         @b{(output)}  Ptr to buffer to contain name
* @param    *ifNameDescr    @b{(output)}  Ptr to buffer to contain description
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    Buffer must be of size L7_NIM_IFNAME_SIZE 
*           and L7_NIM_INTF_DESCR_SIZE, respectively
*
* @notes    The IANA type is passed in order to give the most flexibility for 
*           naming an interface.
*
* @end
*********************************************************************/
extern void nimIfDescrInfoSet(nimConfigID_t *configId, L7_IANA_INTF_TYPE_t ianaType,
                                 L7_uchar8 *ifName,L7_uchar8 *ifDescr,L7_uchar8 *ifLongName);

/*********************************************************************
* @purpose  Gets the link trap configuration of the specified interface.
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    trapState   @b{(output)} pointer to Trap State,
*                       (@b{Returns: L7_DISABLE
*                                    or L7_ENABLE})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    This routine should call SNMP trap manager routine.  It is
*           here for completeness and to refer the user to that routine.
*
* @end
*********************************************************************/
extern L7_RC_t nimGetIntfLinkTrap(L7_uint32 intIfNum, L7_uint32 *trapState);


/*********************************************************************
* @purpose  Sets up the mirroring.
*
* @param    probedIntf  @b{(input)} Internal Interface Number to probe
* @param    probingIntf @b{(input)} Internal Interface Number which is probing
* @param    probeType   @b{(input)} Type of mirroring requested,
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t nimSetupMirror(L7_uint32 probedIntf, L7_uint32 probingIntf, L7_uint32 probeType);


/*********************************************************************
* @purpose  Tears down the mirroring.
*
* @param    probedIntf  @b{(input)} Internal Interface Number to probe
* @param    probingIntf @b{(input)} Internal Interface Number which is probing
* @param    probeType   @b{(input)} Type of mirroring requested,
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes none
*
* @end
*********************************************************************/
extern L7_RC_t nimTeardownMirror(L7_uint32 probedIntf, L7_uint32 probingIntf, L7_uint32 probeType);


/*********************************************************************
* @purpose  Given a usp, get the interface type associated with the slot
*
* @param    usp         @b{(input)}  pointer to nimUSP_t structure
* @param    sysIntfType @b{(output)} pointer to a parm of L7_INTF_TYPES_t
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t nimGetIntfTypeFromUSP(nimUSP_t* usp, L7_INTF_TYPES_t *sysIntfType);

/*********************************************************************
* @purpose  Returns the internal interface number of the LAG of which
*           this interface is a member.  If it is not a member, returns
*           this interface number
*
* @param    intIntfNum  @b{(input)}  Internal Interface Number
* @param    lagIntfNum  @b{(output)} pointer to Lag Interface Number,
*                       (@b{Returns: Internal Interface Number of LAG})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes none
*
* @end
*********************************************************************/
extern L7_RC_t nimGetLagIntfNum(L7_uint32 intIfNum, L7_uint32 *lagIntfNum);


/*********************************************************************
* @purpose  Return Internal Interface Number of next valid port
*
* @param    intIfNum     @b{(input)}  Internal Interface Number
* @param    nextintIfNum @b{(output)} Internal Interface Number,
*                       (@b{Returns: next valid internal interface number})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes none
*
* @end
*********************************************************************/
extern L7_RC_t nimNextValidIntfNumber(L7_uint32 intIfNum, L7_uint32 *nextIntIfNum);


/*********************************************************************
* @purpose  Return Internal Interface Number of the first valid port
*
* @param    firstIntIfNum   @b{(output)} Internal Interface Number
*                       (@b{Returns: first valid internal interface number})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes none
*
* @end
*********************************************************************/
extern L7_RC_t nimFirstValidIntfNumber(L7_uint32 *firstIntIfNum);


/*********************************************************************
* @purpose  Return Internal Interface Number of next valid interface for
*           the specified system interface type.
*
* @param    sysIntfType  @b{(input)}  Internal Interface Type
* @param    intIfNum     @b{(input)}  Internal Interface Number
* @param    nextintIfNum @b{(output)} Internal Interface Number,
*                       (@b{Returns: next valid internal interface number})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes none
*
* @end
*********************************************************************/
extern L7_RC_t nimNextValidIntfNumberByType(L7_INTF_TYPES_t sysIntfType, L7_uint32 intIfNum, L7_uint32 *nextIntIfNum);


/*********************************************************************
* @purpose  Return Internal Interface Number of the first valid interface for
*           the specified interface type.
*
* @param    sysIntfType  @b{(input)}  Internal Interface Type
* @param    intIfNum     @b{(output)} Internal Interface Number
*                       (@b{Returns: first valid internal interface number})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes none
*
* @end
*********************************************************************/
extern L7_RC_t nimFirstValidIntfNumberByType(L7_INTF_TYPES_t sysIntfType, L7_uint32 *firstIntIfNum);

/*********************************************************************
* @purpose  Clear all counter for an intIfNum
*
* @param    intIfNum @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t nimClearCounters(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Obtain time of last reset of counters
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    *ts         @b{(output)} Pointer to time structure L7_timespec
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t nimCountersLastResetTime(L7_uint32 intIfNum, L7_timespec *ts);


/*********************************************************************
* @purpose  Obtain raw time of last link change, in seconds
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    *seconds    @b{(output)} Pointer to value to hold time
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t nimGetRawLinkChangeTime(L7_uint32 intIfNum, L7_uint32 *seconds);


/*********************************************************************
*
* @purpose  Get the max IF number
*
* @param    *maxintf  @b{(output)} Get the max IF number
*
* @returns  L7_SUCCESS
*
* @notes    The number of network interfaces (regardless of
*           their current state) present on this system
*
* @end
*********************************************************************/
extern L7_RC_t nimIfNumberGet(L7_uint32 *maxintf);


/*********************************************************************
*
* @purpose  Returns a textual string containing information about the
*           interface
*
* @param    intIfNum  @b{(input)}   Internal Interface Number
*           descr     @b{(output)}  string containing the information
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    None
*
* @end
*********************************************************************/
extern L7_RC_t nimGetIfDescr(L7_uint32 intIfNum, L7_uchar8 *descr);


/*********************************************************************
* @purpose  Returns the internal interface type (IANAifType)
*
* @param    intIfNum    @b{(input)}  internal interface number
* @param    IfType      @b{(output)} pointer to type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    The type of interface, distinguished according to
*           the physical/link protocol(s) immediately `below'
*           the network layer in the protocol stack."
*
* @end
*********************************************************************/
extern L7_RC_t nimGetIfType(L7_uint32 intIfNum, L7_uint32 *IfType);


#if L7_FEAT_SF10GBT
/*********************************************************************
* @purpose  Returns the interface transceiver firmware revision
*
* @param    UnitIndex    unit for operation 
* @param    intIfNum    internal interface number
* @param    ifFWRevision      pointer to FW revision 
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist, or operation not supported 
* @returns  L7_FAILURE  if other failure
*
* @notes    Is valid only for 10G non-stacking ports with 
*           Solarflare SFX7101 transceiver
*
* @end
*********************************************************************/
extern L7_RC_t nimGetIfTransceiverFwRevision(L7_uint32 UnitIndex, L7_uint32 intIfNum, nimIntfPortFWRev_t* ifFWRevision);


#endif


/*********************************************************************
* @purpose  Returns the internal interface physical type
*
* @param    intIfNum    @b{(input)}  internal interface number
* @param    IfType      @b{(output)} pointer to type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t nimGetIfPhyType(L7_uint32 intIfNum, L7_uint32 *IfType);


/*********************************************************************
* @purpose  Get the interface in loopback mode.
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    *mode       @b{(output)} mode
*                       (@b{  L7_PORTCTRL_LOOPBACK_MAC,
*                             L7_PORTCTRL_LOOPBACK_PHY,
*                             L7_PORTCTRL_LOOPBACK_NONE} }
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
*
* @notes    In release 1, mode will be L7_ENABLE or L7_DISABLE.
*
* @end
*********************************************************************/
extern L7_RC_t nimGetLoopbackMode(L7_uint32 intIfNum, L7_uint32 *mode);


/*********************************************************************
* @purpose  Set the interface in loopback mode.
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    mode        @b{(input)} link state
*                       (@b{  L7_PORTCTRL_LOOPBACK_MAC,
*                             L7_PORTCTRL_LOOPBACK_PHY,
*                             L7_PORTCTRL_LOOPBACK_NONE} }
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
*
* @notes    None.
*
* @end
*********************************************************************/
extern L7_RC_t nimSetLoopbackMode(L7_uint32 intfType, L7_uint32 mode);


/*********************************************************************
* @purpose  Assigns an interface to a macroport.
*
* @param    intIfNum      @b{(input)}  internal interface number
* @param    macroPortIntf @b{(output)} pointer to interface type,
*                         (@b{Returns: Internal Interface Type}).
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t nimSetMacroPortAssignment(L7_uint32 intIfNum, nimMacroPort_t *macroPortIntf);


/*********************************************************************
* @purpose  Get interface macroport information
*
* @param    intIfNum      internal interface number
* @param    macroPortIntf pointer to interface type,
*                         (@b{Returns: Internal Interface Type}).
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t nimGetMacroPortAssignment(L7_uint32 intIfNum, nimMacroPort_t *macroPortIntf);

/*********************************************************************
* @purpose  Release an interface from a macroport.
*
* @param    intIfNum    @b{(input)} internal interface number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t nimResetMacroPortAssignment(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Gets the encapsulation type
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    *encapType  @b{(output)}Encapsulation Type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t nimEncapsulationTypeGet(L7_uint32 intIfNum, L7_uint32 *encapType);



/*********************************************************************
* @purpose  Sets the encapsulation type
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    encapType  @b{(output)}Encapsulation Type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t nimEncapsulationTypeSet(L7_uint32 intIfNum, L7_uint32 encapType);



/*********************************************************************
* @purpose  Gets the broadcast MAC address for an interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    *bcastAddr  @b{(output)}Broadcast MAC address
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t nimGetBroadcastMACAddr(L7_uint32 intIfNum, L7_uchar8 *bcastAddr);

/*********************************************************************
* @purpose  return the highest assigned inteface number
*
* @param    highestIntfNumber   @b{(output)} pointer to return value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*
*********************************************************************/
extern L7_RC_t nimGetHighestIntfNumber(L7_uint32 *highestIntfNumber);


/*********************************************************************
* @purpose  Returns the internal interface iftype
*           associated with the internal interface number
*
* @param    intIfNum    @b{(input)}  internal interface number
* @param    ifType      @b{(output)} pointer to internal interface type,
*                       (@b{Returns: Internal Interface Type}).
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t nimGetIntfifType(L7_uint32 intIfNum, L7_uint32 *ifType);


/*********************************************************************
* @purpose  set the internal interface iftype
*
* @param    intIfNum    @b{(input)}  internal interface number
* @param    ifType      @b{(output)} pointer to internal interface type,
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t nimSetIntfifType(L7_uint32 intIfNum, L7_uint32 *ifType);



/*********************************************************************
* @purpose  Get phy capability of the specified interface
*
* @param    intIfNum      @b{(input)}  Internal Interface Number
* @param    phyCapability @b{(output)} pointer to phyCapability,
*                       (@b{Returns: L7_PHY_CAPABILITIES_t})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t nimGetIntfPhyCapability(L7_uint32 intIfNum, L7_uint32 *phyCapability);

/*********************************************************************
* @purpose  Set phy capability of the specified interface
*
* @param    intIfNum      @b{(input)}  Internal Interface Number
* @param    phyCapability @b{(output)} pointer to phyCapability,
*                       (@b{Returns: L7_PHY_CAPABILITIES_t})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t nimSetIntfPhyCapability(L7_uint32 intIfNum, L7_uint32 phyCapability);


/*********************************************************************
* @purpose  Get connector type of the specified interface
*
* @param    intIfNum        @b{(input)}  Internal Interface Number
* @param    connectorType   @b{(output)} pointer to phyCapability,
*                       (@b{Returns: L7_CONNECTOR_TYPE_t})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t nimGetIntfConnectorType(L7_uint32 intIfNum, L7_uint32 *connectorType);

/*********************************************************************
* @purpose  Convert nim bitmasks to SNMP octet-strings
*
* @param    in     @b{(input)}  mask to be converted
*           out    @b{(output)} pointer to storage to hold the converted mask
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t nimReverseMask(NIM_INTF_MASK_t in,
                              NIM_INTF_MASK_t* out);

/*********************************************************************
* @purpose  Convert a mask to a densely packed list of internal interface numbers
*
* @param    mask    @b{(input)}  mask to be converted
* @param    list[]  @b{(output)} densely packed list
* @param    numList @b{(output)} number of interfaces in the above list
*
* @returns  L7_SUCCESS
*
* @notes    mask is filter oriented i.e. lsb/rightmost bit represents interafce 1
*
* @end
*********************************************************************/
extern L7_RC_t nimMaskToList(NIM_INTF_MASK_t *mask,
                             L7_uint32 list[],
                             L7_uint32 *numList);

/*********************************************************************
* @purpose  Convert a densely packed list of internal interface numbers to a mask
*
* @param    list[]   @b{(input)}  densely packed list
* @param    numList  @b{(input)}  number of interfaces in the above list
* @param    mask     @b((output)) mask to be generated
*
* @returns  L7_SUCCESS
*
* @notes    mask is filter oriented i.e. lsb/rightmost bit represents interafce 1
*
* @end
*********************************************************************/
extern L7_RC_t nimListToMask(L7_uint32 list[],
                             L7_uint32 numList,
                             NIM_INTF_MASK_t *mask);


/*********************************************************************
* @purpose  check if the port is a macro port
*
* @param    intIfNum    @b{(input)} internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_BOOL nimIsMacroPort(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Return the first interface in the unit
*
* @param    unit        @b{(input)} The unit to act on
* @param    *intIfNum   @b{(output)} Internal interface number returned after completion
* @param    *usp        @b{(output)} USP returned after completion
*
* @returns  L7_SUCCESS
*
* @notes    none
*
*
* @end
*********************************************************************/
extern L7_RC_t nimFirstIntfOnUnitGet(L7_uchar8 unit, L7_uint32 *intIfNum, nimUSP_t *usp);



/*********************************************************************
*
* @purpose  Return the next interface in the unit
*
* @param    *usp        @b{(input)} previous usp found using the nimFirstIntfOnUnitGet
*                   needs to be a fully qualified usp
* @param    *intIfNum   @b{(output)} Internal interface number returned after completion
*
* @returns  L7_SUCCESS
*
* @notes    none
*
*
* @end
*********************************************************************/
extern L7_RC_t nimNextIntfOnUnitGet(nimUSP_t *usp,L7_uint32 *intIfNum);


/*********************************************************************
*
* @purpose  Return the first interface in the slot
*
* @param    unit        @b{(input)}   The unit to act on
* @param    slot        @b{(input)}   The slot to act on
* @param    *intIfNum   @b{(output)} Internal interface number returned after completion
* @param    *usp        @b{(output)} USP returned after completion
*
* @returns  L7_SUCCESS
*
* @notes    none
*
*
* @end
*********************************************************************/
extern L7_RC_t nimFirstIntfOnSlotGet(L7_uchar8 unit,L7_uchar8 slot, L7_uint32 *intIfNum, nimUSP_t *usp);


/*********************************************************************
*
* @purpose  Return the first interface in the slot
*
* @param    *usp        @b{(input)}   previous usp found using the nimFirstIntfOnUnitGet
*                   needs to be a fully qualified usp, returns the usp
*                   of the interface found also
* @param    *intIfNum   @b{(output)} Internal interface number returned after completion
*
* @returns  L7_SUCCESS
*
* @notes    none
*
*
* @end
*********************************************************************/
extern L7_RC_t nimNextIntfOnSlotGet(nimUSP_t *usp,L7_uint32 *intIfNum );

/*********************************************************************
*
* @purpose  Get the configuration ID for the given Internal Interface ID
*
* @param    intIfNum  @b{(input)}  Internal Interface number
* @param    configId        @b{(output)} Pointer to the new config ID
*
* @returns  L7_SUCCESS     if interface exists
* @returns  L7_ERROR       if interface does not exist
* @returns  L7_FAILURE     if other failure
*
* @notes    none
*
*
* @end
*********************************************************************/
extern L7_RC_t nimConfigIdGet(L7_uint32 intIfNum,nimConfigID_t *configId);

/*********************************************************************
*
* @purpose  Get the USP for the given Configuration ID
*
* @param    configId        @b{(input)}  Configuration Interface ID
* @param    usp             @b{(output)} Pointer to the USP
*
* @returns  L7_SUCCESS     if interface exists
* @returns  L7_ERROR       if interface does not exist
* @returns  L7_FAILURE     if other failure
*
* @notes    none
*
*
* @end
*********************************************************************/
extern L7_RC_t nimUspFromConfigIDGet(nimConfigID_t *configId,nimUSP_t *usp);

/*********************************************************************
*
* @purpose  Get the USP for the given Configuration ID
*
* @param    configId        @b{(input)}  Configuration Interface ID
* @param    intIfNum        @b{(output)} Pointer to the intIfNum
*
* @returns  L7_SUCCESS     if interface exists
* @returns  L7_ERROR       if interface does not exist
* @returns  L7_FAILURE     if other failure
*
* @notes    none
*
*
* @end
*********************************************************************/
extern L7_RC_t nimIntIfFromConfigIDGet(nimConfigID_t *configId,L7_uint32 *intIfNum);

/*********************************************************************
*
* @purpose  A deep copy for the Configuration ID
*
* @param    src             @b{(input)}  Pointer to the source configID
* @param    dst             @b{(output)} Pointer to the destination configID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
*
* @end
*********************************************************************/
extern L7_RC_t nimConfigIdCopy(nimConfigID_t *src,nimConfigID_t *dst);

/*********************************************************************
*
* @purpose  Status callback from components to NIM for PORT EVENT Notifications
*
* @param    status        @b{(output)}  Status from the component
*
* @returns  void
*
* @notes    At the conclusion of processing a PORT Event, each component must
*           must call this function with the correlator, intf, status, and
*           the component ID
*
* @end
*********************************************************************/
extern void nimEventStatusCallback(NIM_EVENT_COMPLETE_INFO_t status);

/*********************************************************************
*
* @purpose  Notify all interested components of an Interface Change event
*
* @param    cbInfo        @b{(output)}  The event information
* @param    pHandle       @b{(output)}  A handle that identifies this request
*
* @returns  void
*
* @notes    If the caller is interested in being notified at when the event is
*           completed, they must put a callback function in the cbInfo.pCbFunc
*
* @notes    Depending on the event, the interface will either be an intIfNum or
*           a nimUSP_t.  Presently, only the L7_CREATE may have an interface other
*           than the intIfNum
*
* @end
*********************************************************************/
extern L7_RC_t nimEventIntfNotify(NIM_EVENT_NOTIFY_INFO_t cbInfo, NIM_HANDLE_t *pHandle);

/*********************************************************************
* @purpose  This function provides information about a component.
*           The use of this function is optional.
*
* @param    pQueryData   - @b{(inputoutput)}pointer to data where the
*                                           component will place
*                                           the requested information
*                                           for a component.
*
* @returns  L7_SUCCESS   - function completed succesfully. pQueryData
*                          contains valid information
* @returns  L7_ERROR     - function failed. pQueryData contain reason
*                        - for failure.
*
* @notes    The following are valid error reason code:
* NIM_ERR_RC_INVALID_INTF
* NIM_ERR_RC_INTERNAL       : Could be fatal
* NIM_ERR_RC_INVALID_DATA
* NIM_ERR_RC_INVALID_RSQT
*
* @notes    This function excecute synchronously.
*
* @end
*********************************************************************/
extern L7_RC_t nimIntfQuery(L7_NIM_QUERY_DATA_t *pQueryData);

/*********************************************************************
* @purpose  Provide the caller with an NIM_INTF_MASK of all created interfaces
*
* @param    mask    @b{(output)}  mask to be converted
*
* @returns  L7_SUCCESS
*
* @notes    mask is filter oriented i.e. lsb/rightmost bit represents interafce 1
*
* @end
*********************************************************************/
extern L7_RC_t nimIntfMaskOfCreatedGet(NIM_INTF_MASK_t *mask);

/*********************************************************************
* @purpose  Provide the caller with an NIM_INTF_MASK of all present interfaces
*
* @param    mask    @b{(output)}  mask to be converted
*
* @returns  L7_SUCCESS
*
* @notes    mask is filter oriented i.e. lsb/rightmost bit represents interafce 1
*
* @end
*********************************************************************/
extern L7_RC_t nimIntfMaskOfPresentGet(NIM_INTF_MASK_t *mask);


/*********************************************************************
* @purpose  Provide the caller with an NIM_INTF_MASK of all CPU interfaces
*
* @param    mask    @b{(output)}  mask to be converted
*
* @returns  L7_SUCCESS
*
* @notes    mask is filter oriented i.e. lsb/rightmost bit represents interafce 1
*
* @end
*********************************************************************/
extern L7_RC_t nimCpuIntfMaskGet(NIM_INTF_MASK_t *mask);

/*********************************************************************
* @purpose  Provide the caller with an NIM_INTF_MASK of all Physical interfaces
*
* @param    mask    @b{(output)}  mask to be converted
*
* @returns  L7_SUCCESS
*
* @notes    mask is filter oriented i.e. lsb/rightmost bit represents interafce 1
*
* @end
*********************************************************************/
extern L7_RC_t nimPhysicalIntfMaskGet(NIM_INTF_MASK_t *mask);

/*********************************************************************
* @purpose  Provide the caller with an NIM_INTF_MASK of all lag interfaces
*
* @param    mask    @b{(output)}  mask to be converted
*
* @returns  L7_SUCCESS
*
* @notes    mask is filter oriented i.e. lsb/rightmost bit represents interafce 1
*
* @end
*********************************************************************/
extern L7_RC_t nimLagIntfMaskGet(NIM_INTF_MASK_t *mask);

/*********************************************************************
* @purpose  Provide the caller with an NIM_INTF_MASK of all vlan interfaces
*
* @param    mask    @b{(output)}  mask to be converted
*
* @returns  L7_SUCCESS
*
* @notes    mask is filter oriented i.e. lsb/rightmost bit represents interafce 1
*
* @end
*********************************************************************/
extern L7_RC_t nimVlanIntfMaskGet(NIM_INTF_MASK_t *mask);

/*********************************************************************
* @purpose  Provide the caller with an NIM_INTF_MASK of all l2 tunnel interfaces
*
* @param    mask    @b{(output)}  mask to be converted
*
* @returns  L7_SUCCESS
*
* @notes    mask is filter oriented i.e. lsb/rightmost bit represents interafce 1
*
* @end
*********************************************************************/
extern L7_RC_t nimL2TnnlIntfMaskGet(NIM_INTF_MASK_t *mask);

/*********************************************************************
* @purpose  Get valid range of intIfNumers for a given interface type
*
* @param    intfType  @b{(input)}  one of L7_INTF_TYPES_t
* @param    *min      @b{(output)} pointer to parm to store min value
* @param    *max      @b{(output)} pointer to parm to store max value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    If the action fails, the intIfNum will be set to zero
*
* @end
*********************************************************************/
extern L7_RC_t nimIntIfNumRangeGet(L7_INTF_TYPES_t intfType, L7_uint32 *min, L7_uint32 *max);

/*********************************************************************
* @purpose  Request to create an interface
*
* @param    *pRequest   @b{(input)}   pointer to nimIntfCreateRequest_t structure
* @param    *pOutput    @b{(output)}  The returned data for this create request
*
*
* @returns  one of L7_RC_t enum values
*
* @notes  This routine is used to request the creation of an interface.
*         All interfaces must be created using this mechanism.
*
*
*         *pIntfIdInfo - This should contain sufficient information to uniquely
*                       identify the interface in the system.
*
*         *pIntfDescr  - This is a pointer to static information that describes this
*                       interface. This information MUST be completely specified
*                       with the create request.
*                       based on FD_NIM_DEFAULT* values.
*
*         *pDefaultCfg - This is a pointer to the information that would be stored
*                       in this particular port's configuration file if configuration
*                       were reset to default values.
*
*                       If this pointer is null, NIM presumes default configuration
*                       based on FD_NIM_DEFAULT* values.
*
*                       Note that all ports of the same type should have the same
*                       default configuration, although this methodology gives a
*                       system integrator flexibility on different devices.
*
*                       The default configuration will be applied on a subsequent
*                       L7_ATTACH command if no non-volatile configuration exists for
*                       this interface.
*
*
*
*         If the interface is successfully created, an L7_CREATE notification
*         is propagated
*
* @end
*********************************************************************/
extern L7_RC_t   nimIntfCreate(nimIntfCreateRequest_t *pRequest, nimIntfCreateOutput_t *pOutput);

/*********************************************************************
*
* @purpose  Get the Configuration Mask Offset
*
* @param    intIfNum      @b{(input)}  Internal Interface Number
* @param    maskOffset    @b{(output)} The config mask offset within a mask
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR      If the internal interface is not valid
* @returns  L7_FAILURE    If there was no config for the interface in the past
*
* @notes    The config files using a config mask must be in sync with NIM
*           or the mask offset will be invalid
*
* @end
*********************************************************************/
extern L7_RC_t nimConfigIdMaskOffsetGet(L7_uint32 intIfNum,L7_uint32 *maskOffset);

/*********************************************************************
* @purpose  Determine whether NIM is in a state ready to process interface requests.
*
* @param     none
*
* @returns  L7_TRUE  - Nim is ready.
* @returns  L7_FALSE - Nim is not ready.
*
* @notes
*
* @end
*********************************************************************/
extern L7_BOOL nimPhaseStatusCheck(void);

/*********************************************************************
* @purpose  Deteremine whether a set of parameters are all settable
*           for a given interface.
*
* @param    intIfNum      @b{(input)}  Internal Interface Number
* @param    setParms      @b{(input)}  Mask of L7_INTF_PARM_TYPES_t values
*
* @returns  L7_TRUE  - All parameters are settable.
* @returns  L7_FALSE - At least one parameter is not settable.
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_BOOL nimIntfParmCanSet(L7_uint32 intIfNum, L7_uint32 setParms);

/*********************************************************************
*
* @purpose  Sets a textual string containing information about the
*           interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t nimIfDescrSet(L7_uint32 intIfNum, L7_uchar8 *descr);
/*********************************************************************
*
* @purpose  Status callback from components to NIM for startup complete
*           Notifications
*
* @param    componentId  @b{(input)} component ID of the caller.
*
* @returns  void
*
* @notes    At the conclusion of processing a startup Event, each component must
*           must call this function.
*
* @end
*********************************************************************/
extern void nimStartupEventDone(L7_COMPONENT_IDS_t componentId);

/*********************************************************************
*
* @purpose  Invoke startup callbacks for registered components
*
* @param    phase     @b{(input)}  startup phase - create or activate
*
* @returns  void
*
* @notes    Startup's are invoked serially, waiting for each one to
*           complete before invoking the next component's startup.
*
* @end
*********************************************************************/
void nimStartupCallbackInvoke(NIM_STARTUP_PHASE_t phase);


/***********************************************************************
* @purpose  Get auto-negotiation status and the negotiation capabilities
*           of the specified interface
*
* @param    intIfNum    Internal Interface Number
* @param    negotiation pointer to negotiation capabilities
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
************************************************************************/
L7_RC_t nimGetIntfAutoNegoStatusCapabilities(L7_uint32 intIfNum, L7_uint32 *negotiation);

/*******************************************************************************
* @purpose  Enable/disable auto-negotiation and set the negotiation capabilities
*           of the specified interface
*
* @param    intIfNum           Internal Interface Number
* @param    negoCapabilities   negotiation capabilities,
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*******************************************************************************/
L7_RC_t nimSetIntfAutoNegoStatusCapabilities(L7_uint32 intIfNum, L7_uint32 negoCapabilities);

#endif /* INCLUDE_NIMAPI */
