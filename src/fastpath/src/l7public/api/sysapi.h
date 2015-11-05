
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename sysapi.h
*
* @purpose sysapi defines and structure definitions
*
* @component sysapi
*
* @comments none
*
* @create 08/10/2000
*
* @author paulq
* @end
*
**********************************************************************/


#ifndef SYSAPIPROTO_HEADER
#define SYSAPIPROTO_HEADER

#include <stdio.h>
#include "l7_product.h"
#include "platform_counters.h"
#include "log.h"
#include "comm_structs.h"


#define DMA_PORT_SIZE_8BIT          0x0000
#define DMA_PORT_SIZE_16BIT         0x0001
#define DMA_PORT_SIZE_32BIT         0x0002
#define DMA_MEM_TO_MEM              0x0010
#define DMA_MEM_TO_PERIPHERAL       0x0020
#define DMA_PERIPHERAL_TO_MEM       0x0040

#define SYSAPI_BIG_CONFIG_FILENAME SYSAPI_CONFIG_FILENAME    /* Version used for upload/download */
#define SYSAPI_CONFIG_FILENAME     "fastpath.cfg" /* Version used for storing across resets */
#define SYSAPI_CONFIG_RUNING_FILENAME "fastpathRun.cfg"
#define SYSAPI_TXTCFG_FILENAME     "startup-config"
#define SYSAPI_TXTCFG_RUNNING_FILENAME  "running-config"
#define SYSAPI_TXTCFG_BACKUP_FILENAME    "backup-config"
#define SYSAPI_CONFIG_BACKUP_FILENAME    "backup-fastpath"
#define SYSAPI_TXTCFG_SIMPLE_FILENAME        "simple-startup-cfg" /* Last saved simple mode configuration */
#define SYSAPI_TXTCFG_NORMAL_FILENAME        "normal-startup-cfg" /* Last saved normal mode configuration */
#define SYSAPI_TXTCFG_SIMPLE_BACKUP_FILENAME "simple-backup-cfg" /* Last saved simple mode backup configuration */
#define SYSAPI_TXTCFG_NORMAL_BACKUP_FILENAME "normal-backup-cfg" /* Last saved normal mode backup configuration */

#define SYSAPI_SUPPORT_CONFIG_FILENAME   "fpsupras.cfg" /* Version used for storing debug cfg across resets */
#define SYSAPI_KERNEL_FILENAME     "kernel.tar"
#define SYSAPI_ACL_CONFIG_FILENAME "easyacl.xml"

typedef enum eMbufLocation
{
  MBUF_LOC_FREE = 0,
  MBUF_LOC_ALLOC,            /* Allocated, but no more specific information available */
  MBUF_LOC_IP4_FWD_Q,        /* Holds an IPv4 data packet on the msg q for IP forwarding */
  MBUF_LOC_IP4_FWD,          /* Holds an IPv4 data packet being software forwarded */
  MBUF_LOC_ARP_RX,           /* Holds an incoming ARP packet */
  MBUF_LOC_ARP_PEND_Q,       /* Used to queue an IPv4 data packet pending ARP resolution */
  MBUF_LOC_ARP_RES_CB,       /* On ARP resolution callback message queue */
  MBUF_LOC_PDU_TX,           /* Used to tx a packet in dtlPduTransmit() */
  MBUF_LOC_PDU_RX,           /* Mbuf for incoming packet. Reached dtlPduReceive() */
  MBUF_LOC_VRRP_RX,          /* Incoming VRRP packet */
  MBUF_LOC_DHCP_RELAY_RX,    /* Incoming DHCP relay packet */
  MBUF_LOC_UDP_RELAY_TX,     /* mbuf used by IP helper to forward a UDP message */
  MBUF_LOC_CP_WIO_RX,        /* IP packet intercepted by captive portal, wired interface */
  MBUF_LOC_CP_WIO_ARP_RX,    /* ARP packet intercepted by captive portal, wired interface */
  MBUF_LOC_CP_WIO_TX,        /* Captive portal wired interface, outgoing packet */
  MBUF_LOC_DAI,              /* mbuf allocated by dynamic arp inspection */

  MBUF_LOC_LAST

} eMbufLocation;

extern L7_uchar8 *mbufLocNames[MBUF_LOC_LAST];

#define MBUF_HISTORY
#ifdef MBUF_HISTORY

#define MBUF_HISTORY_FILENAME_LEN 16

typedef enum
{
  MBUF_ALLOC = 0,
  MBUF_FREE
} eMbufAction;

typedef struct mbuf_history_s
{
  void *mbuf_ptr;
  eMbufAction mbufAction;
  L7_uint32 timestamp;     /* msec since boot - good for 50 days before wrap. */

  /* file and line where mbuf either allocated or freed */
  L7_uchar8 alloc_file[MBUF_HISTORY_FILENAME_LEN];
  L7_uint32 alloc_line;

} mbuf_history_t;

#endif

/**************************************************************************
*
* @structures SYSAPI_NET_MBUF_HEADER_t
*
* @purpose  net buffer handle
*
* @notes
*
* @end
*************************************************************************/
typedef struct
{
  L7_uint32   applSpecVar;                 /* application specific use */
  L7_uchar8  *bufStart;
  L7_uint32   bufLength;
  void       *osBuffer;
  L7_uint32   taskId;
  L7_uint32   timeStamp;
  L7_BOOL     rxBuffer;  /* set to L7_TRUE if this buffer was allocated on Rx side */
  L7_BOOL     in_use;    /* Buffer is curently allocated. */
  L7_uint32   rxCode;    /* indicates why the pkt was received */
  L7_uchar8   last_file[16]; /* File name that touched the buffer last (debug) */
  L7_uint32   last_line;   /* Line number that touched the buffer last (debug) */
  eMbufLocation mbufLoc;     /* Indicates where mbuf currently is */
} SYSAPI_NET_MBUF_HEADER_t;

#define SYSAPI_PRINTF_QUEUE_MESSAGE_SIZE 1024
#define SYSAPI_MAX_FILE_ENTRIES    ( L7_LAST_COMPONENT_ID * 2 )
#define SYSAPI_BIGC_HEADER_SIZE    L7_MAX_FILENAME+4  /* 4 bytes to hold file size info */
#define SYSAPI_FILETYPE_OTHER      0
#define SYSAPI_FILETYPE_BIGC       1
#define SYSAPI_FILETYPE_CFG        2
#define SYSAPI_CFGFILE_DEBUG       0
#define SYSAPI_CFGFILE_LIST_BUF_SIZE (1024 * 4)

typedef struct
{
    L7_char8  *filename;
    L7_uint32  filesize;
    L7_uint32  filetype;
  void      *file_location;

} SYSAPI_DIRECTORY_t;


#define NET_MBUF_START_OFFSET 16

/*
 * This deals with buffer alignment issues on some processors.
 */
typedef enum
{
  L7_MBUF_FRAME_ALIGNED = 0,
  L7_MBUF_IP_ALIGNED,
} L7_MBUF_ALIGNMENT;

/*
 * Account for necessary alignment. This parm does not effect the ipheader offset and only ensures
 * the data buffer will fall on a natural boundary.
 */
#define L7_MBUF_ALIGN_BOUND   4

/*
 * This aligns the IP addresses in the IP Header
 */
#define L7_MBUF_IP_CORRECTION      2

/* Max  Correction required if the buffer is to be
 * (4 byte boundary + IP aligned )
 */
#define SYSAPI_PKT_BUF_ALIGN_LEN   8

/* L7 NET MBUF MACRO: */
/* Get a NET MBUF - netMbufHandle output */
#define SYSAPI_NET_MBUF_GET(netMbufHandle)                      \
 {                                                              \
   netMbufHandle = sysapiNetMbufGetTrack(__FILE__, __LINE__);   \
   if (netMbufHandle == L7_NULL)                                \
   {                                                            \
     L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID, "Out of system buffers.\n");                       \
   }                                                            \
 }

#define SYSAPI_NET_RX_MBUF_GET(netMbufHandle, priority, alignType)                      \
{                                                              \
 netMbufHandle = sysapiNetRxMbufGetTrack(__FILE__, __LINE__, priority, alignType);   \
 if (netMbufHandle == L7_NULL)                                \
 {                                                            \
   L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID, "Out of system buffers.\n");                       \
 }                                                            \
}

/* L7 NET MBUF MACRO: */
/* Get a aligned NET MBUF - netMbufHandle output */

#define SYSAPI_BUF_ALIGN(x, align)  (((L7_uint32) (x) + (align - 1)) & ~(align - 1))

#define SYSAPI_NET_ALIGN_MBUF_GET(netMbufHandle,__ALIGN_TYPE__)         \
{                                                                       \
  netMbufHandle = sysapiNetMbufAlignGet(__FILE__,__LINE__,__ALIGN_TYPE__); \
  if (netMbufHandle == L7_NULL)                                         \
  {                                                                     \
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID, "Out of system buffers.\n");                                \
  }                                                                     \
}

/* Align buffer to 4 byte boundary  */
#define SYSAPI_ALIGN_4_BYTE_BOUND_LENGTH   (L7_MBUF_ALIGN_BOUND -1)

#define SYSAPI_ALIGN_4_BYTE_BOUNDARY(charBuf)  \
  SYSAPI_BUF_ALIGN(charBuf,L7_MBUF_ALIGN_BOUND)

/* L7 NET MBUF MACRO: */
/* Get a NET MBUF (without logging message) - netMbufHandle output */
#define SYSAPI_NET_MBUF_GET_NOLOG(netMbufHandle)                \
 {                                                              \
   (netMbufHandle) = sysapiNetMbufGetTrack(__FILE__, __LINE__); \
 }


/* L7 NET MBUF MACRO: */
/* Free a NET MBUF - netMbufHandle input */
#define SYSAPI_NET_MBUF_FREE(netMbufHandle)                             \
  do                                                                    \
  {                                                                     \
   if ((netMbufHandle) != 0)                                            \
   {                                                                    \
     sysapiNetMbufFreeTrack((netMbufHandle), __FILE__, __LINE__);       \
   }                                                                    \
   else                                                                 \
   {                                                                    \
      sysapiPrintf("SYSAPI_NET_MBUF_FREE ERROR Line : %d Function : %s", \
             __LINE__,__FUNCTION__);                                    \
   }                                                                    \
  } while (0);



/* L7 NET MBUF MACRO: */
/* Get data pointer for a NET MBUF - netMbufHandle input, dataStart output */
#define SYSAPI_NET_MBUF_GET_DATASTART(netMbufHandle, dataStart) \
 dataStart = ((SYSAPI_NET_MBUF_HEADER_t *)netMbufHandle)->bufStart



/* L7 NET MBUF MACRO: */
/* Set data pointer for a NET MBUF - netMbufHandle input, dataStart input */
#define SYSAPI_NET_MBUF_SET_DATASTART(netMbufHandle, dataStart) \
 ((SYSAPI_NET_MBUF_HEADER_t *)netMbufHandle)->bufStart = dataStart



/* L7 NET MBUF MACRO: */
/* Get data length for a NET MBUF - netMbufHandle input, dataLength output */
#define SYSAPI_NET_MBUF_GET_DATALENGTH(netMbufHandle, dataLength) \
 dataLength = ((SYSAPI_NET_MBUF_HEADER_t *)netMbufHandle)->bufLength



/* L7 NET MBUF MACRO: */
/* Set data length for a NET MBUF - netMbufHandle input, dataLength input */
#define SYSAPI_NET_MBUF_SET_DATALENGTH(netMbufHandle, dataLength) \
 ((SYSAPI_NET_MBUF_HEADER_t *)netMbufHandle)->bufLength = dataLength

/* Set MBUF location */
#define SYSAPI_NET_MBUF_SET_LOC(netMbufHandle, loc) \
 ((SYSAPI_NET_MBUF_HEADER_t *)netMbufHandle)->mbufLoc = loc

/* Get MBUF location */
#define SYSAPI_NET_MBUF_GET_LOC(netMbufHandle) \
  ((SYSAPI_NET_MBUF_HEADER_t *)netMbufHandle)->mbufLoc


/* Align buffer to 4 byte boundary  */

/**************************************************************************
*
* System Statistics Management
*
*
*************************************************************************/


typedef struct SYSAPI_IP_STATS_s
{
    L7_uint32   ipInDiscards;
    L7_uint32   ipInDelivers;
    L7_uint32   ipOutRequests;
    L7_uint32   ipOutDiscards;
    L7_uint32   ipOutNoRoutes;
} SYSAPI_IP_STATS_t;


/*---------------------------------*/
/*          MACROS                 */
/*---------------------------------*/
#define SYSAPI_IP_STATS_INCREMENT(intIfNum, counterID)  \
  {                                                     \
                                                        \
   extern SYSAPI_IP_STATS_t sysapiIPStats;              \
                                                        \
   switch(counterID)                                    \
   {                                                    \
       case L7_PLATFORM_CTR_RX_IP_IN_DISCARDS:          \
            sysapiIPStats.ipInDiscards++;               \
            break;                                      \
       case L7_PLATFORM_CTR_RX_IP_IN_DELIVERS:          \
            sysapiIPStats.ipInDelivers++;               \
            break;                                      \
       case L7_PLATFORM_CTR_TX_IP_OUT_DISCARDS:         \
            sysapiIPStats.ipOutDiscards++;              \
            break;                                      \
                                                        \
       default:                                         \
           break;                                       \
  }                                                     \
}




#define SYSAPI_IP_STATS_GET(val,intIfNum, counterID)    \
  {                                                     \
                                                        \
   extern SYSAPI_IP_STATS_t sysapiIPStats;              \
                                                        \
   switch(counterID)                                    \
   {                                                    \
       case L7_PLATFORM_CTR_RX_IP_IN_DISCARDS:          \
            *(val) = sysapiIPStats.ipInDiscards;        \
            break;                                      \
       case L7_PLATFORM_CTR_RX_IP_IN_DELIVERS:          \
            *(val) = sysapiIPStats.ipInDelivers;        \
            break;                                      \
       case L7_PLATFORM_CTR_TX_IP_OUT_DISCARDS:         \
            *(val) = sysapiIPStats.ipOutDiscards;       \
            break;                                      \
                                                        \
       default:                                         \
           break;                                       \
  }                                                     \
}

/* Start of SYSAPI Application Layer Logging Defines                                            */
/* Start of SYSAPI Application Layer Logging Defines                                            */
/* Start of SYSAPI Application Layer Logging Defines                                            */
/* Start of SYSAPI Application Layer Logging Defines                                            */
/* Start of SYSAPI Application Layer Logging Defines                                            */

/*
* These defines for the application layer must only occupy the upper
* 16 bits of the mask. The lower 16 bits are reserved for platform
* hardware driver logging mask bits.
*/
#define SYSAPI_APPLICATION_LOGGING_ALWAYS        0x00010000
#define SYSAPI_APPLICATION_LOGGING_IPMAP         0x00020000
#define SYSAPI_APPLICATION_LOGGING_IPMAP_ERROR   0x00040000
#define SYSAPI_APPLICATION_LOGGING_OSPFMAP       0x00080000
#define SYSAPI_APPLICATION_LOGGING_OSPFMAP_ERROR 0x00100000
#define SYSAPI_APPLICATION_LOGGING_RIPMAP        0x00200000
#define SYSAPI_APPLICATION_LOGGING_RIPMAP_ERROR  0x00400000
#define SYSAPI_APPLICATION_LOGGING_MPLSMAP       0x00800000
#define SYSAPI_APPLICATION_LOGGING_MPLSMAP_ERROR 0x01000000
#define SYSAPI_APPLICATION_LOGGING_BGPMAP        0x02000000
#define SYSAPI_APPLICATION_LOGGING_BGPMAP_ERROR  0x04000000
#define SYSAPI_APPLICATION_LOGGING_QOS           0x08000000
#define SYSAPI_APPLICATION_LOGGING_QOS_ERROR     0x10000000
#define SYSAPI_APPLICATION_LOGGING_DVMRPMAP_ERROR 0x20000000
#define SYSAPI_APPLICATION_LOGGING_DVMRPMAP      0x20000000
#define SYSAPI_APPLICATION_LOGGING_IGMPMAP       0x40000000
#define SYSAPI_APPLICATION_LOGGING_IGMPMAP_ERROR 0x40000000
#define SYSAPI_APPLICATION_LOGGING_PIMDMMAP      0x80000000
#define SYSAPI_APPLICATION_LOGGING_PIMDMMAP_ERROR 0x80000000
#define SYSAPI_APPLICATION_LOGGING_PIMSMMAP       0xA0000000
#define SYSAPI_APPLICATION_LOGGING_PIMSMMAP_ERROR 0xA0000000

#define SYSAPI_APPLICATION_LOGGING_MASK  (SYSAPI_APPLICATION_LOGGING_ALWAYS | \
                              SYSAPI_APPLICATION_LOGGING_IPMAP              | \
                              SYSAPI_APPLICATION_LOGGING_IPMAP_ERROR        | \
                              SYSAPI_APPLICATION_LOGGING_OSPFMAP            | \
                              SYSAPI_APPLICATION_LOGGING_OSPFMAP_ERROR      | \
                              SYSAPI_APPLICATION_LOGGING_RIPMAP             | \
                              SYSAPI_APPLICATION_LOGGING_RIPMAP_ERROR       | \
                              SYSAPI_APPLICATION_LOGGING_MPLSMAP            | \
                              SYSAPI_APPLICATION_LOGGING_MPLSMAP_ERROR      | \
                              SYSAPI_APPLICATION_LOGGING_BGPMAP             | \
                              SYSAPI_APPLICATION_LOGGING_BGPMAP_ERROR       | \
                              SYSAPI_APPLICATION_LOGGING_QOS                | \
                              SYSAPI_APPLICATION_LOGGING_QOS_ERROR          | \
                              SYSAPI_APPLICATION_LOGGING_DVMRPMAP           | \
                              SYSAPI_APPLICATION_LOGGING_DVMRPMAP_ERROR     | \
                              SYSAPI_APPLICATION_LOGGING_IGMPMAP            | \
                              SYSAPI_APPLICATION_LOGGING_IGMPMAP_ERROR      | \
                              SYSAPI_APPLICATION_LOGGING_PIMDMMAP           | \
                              SYSAPI_APPLICATION_LOGGING_PIMDMMAP_ERROR     | \
                              SYSAPI_APPLICATION_LOGGING_PIMSMMAP           | \
                              SYSAPI_APPLICATION_LOGGING_PIMSMMAP_ERROR     )

#if (SYSAPI_APPLICATION_LOGGING_MASK & 0x0000FFFF)
#error illegal logging mask error in sysapi.h
#endif
/* End of SYSAPI Application Layer Logging Defines                                              */
/* End of SYSAPI Application Layer Logging Defines                                              */
/* End of SYSAPI Application Layer Logging Defines                                              */
/* End of SYSAPI Application Layer Logging Defines                                              */
/* End of SYSAPI Application Layer Logging Defines                                              */


typedef enum
{
  SYSAPI_APP_IP_REASON_NONE = 0,
  SYSAPI_APP_IP_REASON_NO_LOOPBACK_INTERFACE,
  SYSAPI_APP_IP_REASON_NO_IP_ADDRESS,
  SYSAPI_APP_IP_REASON_GLOBAL_ROUTING_DISABLED,
  SYSAPI_APP_IP_REASON_NO_ACTIVE_IF_FOR_STATICALLY_CONFIGURED_IP
} SYSAPI_APP_IP_REASON_t;

EXT_API L7_RC_t FlashStoreCreate(void);
extern L7_uint32 sysapiWriteToFlash;

/**************************************************************************
*
* @purpose  Start various system services.
*
* @param    none
*
* @returns  none.
*
* @notes This function must be envoked from bootos.c before
*        the configurator is started.
*
* @end
*
*************************************************************************/
void sysapiInit (void (*box_reset)(void));

/**************************************************************************
*
* @purpose  This function takes the string given to it and uses XIO
*           functions to set the up/down result string.
*
* @param    string
*
* @returns  Void
*
* @notes
*
* @end
*
*************************************************************************/
extern void   result_string(L7_char8 *string);

/**************************************************************************
*
* @purpose  Task to save configuration data when request is from SNMP
*
* @param    None
*
* @returns  Never returns
*
* @notes
*
* @end
*
*************************************************************************/
void sysapiWriteConfigToFlashTask();

/**************************************************************************
*
* @purpose  Status for save configuration command when request is from SNMP
*
* @param    None
*
* @returns  Returns enum from L7_SAVE_CONFIG_CODE_t
*
* @notes
*
* @end
*
*************************************************************************/
L7_SAVE_CONFIG_CODE_t sysapiWriteConfigToFlashStatus();

/**************************************************************************
*
* @purpose  Function to start save configuration command when request is from SNMP
*
* @param    None
*
* @returns  None
*
* @notes
*
* @end
*
*************************************************************************/
void sysapiWriteConfigToFlashStart();

/**************************************************************************
*
* @purpose  Retrieve an mbuf to the caller
*
* @param    isRx if the mbuff is Rx 
*
* @returns  A ptr to an mbuf
* @returns  L7_NULL if none are available
*
* @notes    Use the Mutex semaphore to inhibit global variable corruption
*
* @end
*
*************************************************************************/
L7_uint32 *sysapiMbufGet(L7_BOOL isRx );

/**************************************************************************
*
* @purpose  Return an mbuf to the mbuf pool
*
* @param    *mbuf ptr to the mbuf to return
* @param    isRx if the mbuf is Rx
*
*
* @returns  none.
*
* @notes    Use the Mutex semaphore to inhibit global variable corruption
*
* @end
*
*************************************************************************/
void      sysapiMbufFree( L7_uint32 *mbuf, L7_BOOL isRx );

/**************************************************************************
*
* @purpose  General purpose printf routine
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*
*************************************************************************/
/*
* These defines for the hardware platform must only occupy the lower
* 16 bits of the mask. The upper 16 bits are reserved for application
* logging mask bits.
*/
#define SYSAPI_LOGGING_ALWAYS           0x00000001
#define SYSAPI_LOGGING_MASK_1           0x00000002
#define SYSAPI_LOGGING_MASK_2           0x00000004
#define SYSAPI_LOGGING_MASK_3           0x00000008
#define SYSAPI_LOGGING_DAPI             0x00000010
#define SYSAPI_LOGGING_DAPI_ERROR       0x00000020
#define SYSAPI_LOGGING_HAPI             0x00000040
#define SYSAPI_LOGGING_HAPI_ERROR       0x00000080
#define SYSAPI_LOGGING_MDA_ERROR        0x00000100
#define SYSAPI_LOGGING_MASK_9           0x00000200
#define SYSAPI_LOGGING_MASK_10          0x00000400
#define SYSAPI_LOGGING_MASK_11          0x00000800
#define SYSAPI_LOGGING_MASK_12          0x00001000
#define SYSAPI_LOGGING_MASK_13          0x00002000
#define SYSAPI_LOGGING_MASK_14          0x00004000
#define SYSAPI_LOGGING_MASK_15          0x00008000

#define SYSAPI_LOGGING_MASK  (SYSAPI_LOGGING_ALWAYS         | \
                              SYSAPI_LOGGING_DAPI_ERROR     | \
                              SYSAPI_LOGGING_HAPI_ERROR     | \
                              SYSAPI_LOGGING_MDA_ERROR )

#if (SYSAPI_LOGGING_MASK & 0xFFFF0000)
  #error illegal logging mask error in sysapi.h
#endif

void sysapiPrintf(const char *format, ...);
#define SYSAPI_PRINTF(FLG,format,args...)        \
{                                                \
  /*printf("%s(%d) sysapiPrintf: \"%s\"\r\n",__FUNCTION__,__LINE__,format);*/ \
  if (( SYSAPI_LOGGING_MASK & FLG) || ( SYSAPI_APPLICATION_LOGGING_MASK & FLG)) \
  {                                              \
    sysapiPrintf(format, ##args);                \
  }                                              \
}

/**************************************************************************
*
* @purpose  console printf routine
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @notes    This routine is for the use of the console in the event that
*           the user interface is not present or not initialized
*
* @end
*
*************************************************************************/
void sysapiPrintfConsole (const char *format, ...);

/**************************************************************************
*
* @purpose  Enable or disable sysapi debug display mode
*
* @param    mode    L7_ENABLE or L7_DISABLE
*
* @returns  void
*
* @notes   The setting of this mode controls whether certain types of messages
*          (e.g. internal trace messages, debug dumps) may be displayed on
*          CLI login sessions which are enabled for such display.

*
*          This mode must be set to enabled if there are any sessions enabled
*          for debug display.  Otherwise, set to disabled.
*
*
* @end
*
*************************************************************************/
void sysapiDebugSupportDisplayModeSet(L7_uint32 mode);

/**************************************************************************
*
* @purpose  Get sysapi debug display mode
*
* @param    void
*
* @returns  mode    L7_ENABLE or L7_DISABLE
*
* @notes   The setting of this mode controls whether certain types of messages
*          (e.g. internal trace messages, debug dumps) may be displayed on
*          CLI login sessions which are enabled for such display.
*
* @end
*
*************************************************************************/
L7_uint32 sysapiDebugSupportDisplayModeGet(void);

/**************************************************************************
*
* @purpose  Get display mode of hidden support commands for a session
*
* @param    session   CLI session
*
* @returns  mode    L7_ENABLE or L7_DISABLE
*
* @notes   The setting of this mode controls whether hidden support commands
*          are available from the user interface.
*
* @end
*
*************************************************************************/
L7_uint32 sysapiDebugSupportFeatureHiddenModeGet(L7_int32 session);

/**************************************************************************
*
* @purpose  Initialize the sysapi component
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t sysapiSystemInit(void);

/**************************************************************************
*
* @purpose  Retrieve a network mbuf to the caller (and track the caller)
*
* @param    none.
*
* @returns  A ptr to a network mbuf handle
* @returns  0 if none are available
*
* @notes
*
* @end
*
*************************************************************************/
L7_netBufHandle sysapiNetMbufGetTrack(L7_uchar8 *file, L7_uint32 line);

/**************************************************************************
*
* @purpose  Retrieve a network mbuf to the caller
*
* @param    none.
*
* @returns  A ptr to a network mbuf handle
* @returns  0 if none are available
*
* @notes
*
* @end
*
*************************************************************************/
L7_netBufHandle sysapiNetMbufGet( void );

/**************************************************************************
*
* @purpose  Retrieve a network mbuf to the caller, for use on receiving packets
*
* @param    L7_MBUF_RX_PRIORITY priority
*
* @returns  A ptr to a network mbuf handle
* @returns  0 if none are available
*
* @notes
*
* @end
*
*************************************************************************/
typedef enum
{
  L7_MBUF_RX_PRIORITY_HIGH = 1,
  L7_MBUF_RX_PRIORITY_MID0 = 2,
  L7_MBUF_RX_PRIORITY_MID1 = 3,
  L7_MBUF_RX_PRIORITY_MID2 = 4,
  L7_MBUF_RX_PRIORITY_NORMAL = 5
} L7_MBUF_RX_PRIORITY;

/**************************************************************************
*
* @purpose  Retrieve a network rx mbuf to the caller (and track the caller)
* 
* @param    priority    @b{(input)}  Priority Indicator, for the mbuf
* @param    alignType   @b{(input)}  Alignment indicator, for IP or frame
*
* @returns  A ptr to a network mbuf handle
* @returns  0 if none are available
*
* @notes
*
* @end
*
*************************************************************************/
L7_netBufHandle sysapiNetRxMbufGetTrack(L7_uchar8 *file, L7_uint32 line, L7_MBUF_RX_PRIORITY priority, L7_MBUF_ALIGNMENT  alignType);

extern L7_netBufHandle sysapiRxNetMbufGet( L7_MBUF_RX_PRIORITY priority,
    L7_MBUF_ALIGNMENT  alignType);


/**************************************************************************
 * @purpose  Retrieve an aligned network mbuf to the caller
 *
 * @param    align   @b{(input)}  Alignment indicator, for IP or frame
 *
 * @returns  A ptr to a network mbuf handle
 * @returns  0 if none are available
 *
 * @note     All mbufs are 4 byte aligned
 *
 * @end
 *************************************************************************/
L7_netBufHandle sysapiNetMbufAlignGet(L7_uchar8 *file, L7_uint32 line,
    L7_MBUF_ALIGNMENT alignType);

/**************************************************************************
*
* @purpose  Free a network mbuf
*
* @param    Ptr to network mbuf handle
*
* @returns  void
*
* @notes
*
* @end
*
*************************************************************************/
void sysapiNetMbufFree( L7_netBufHandle netMbufHandle );

/**************************************************************************
*
* @purpose  Free a network mbuf with debug information.
*
* @param    Ptr to network mbuf handle
*
* @returns  void
*
* @notes
*
* @end
*
*************************************************************************/
void sysapiNetMbufFreeTrack( L7_netBufHandle netMbufHandle, L7_uchar8 *file, L7_uint32 line);

/**************************************************************************
*
* @purpose  Retrieve start of data portion of a network mbuf
*
* @param    A ptr to a network mbuf handle
*
* @returns  A ptr to start of data portion of a network mbuf
* @returns  0 if none are available
*
* @notes
*
* @end
*
*************************************************************************/
L7_uchar8 *sysapiNetMbufGetDataStart( L7_netBufHandle netMbufHandle );

/**************************************************************************
*
* @purpose  Set start of data portion of a network mbuf
*
* @param    A ptr to a network mbuf handle
* @param    A ptr to start of data portion
*
* @returns  void
*
* @notes
*
* @end
*
*************************************************************************/
void sysapiNetMbufSetDataStart(  L7_netBufHandle netMbufHandle, L7_uchar8 *dataStart );

/**************************************************************************
*
* @purpose  Retrieve length of network mbuf
*
* @param    A ptr to a network mbuf handle
*
* @returns  Length of data in network mbuf
*
* @notes
*
* @end
*************************************************************************/
L7_uint32 sysapiNetMbufGetDataLength(  L7_netBufHandle netMbufHandle );

/**************************************************************************
* @purpose  Set length of network mbuf
*
* @param    A ptr to a network mbuf handle
* @param    Size of data in network mbuf
*
* @returns  void
*
* @notes
*
* @end
*
*************************************************************************/
void sysapiNetMbufSetDataLength(  L7_netBufHandle netMbufHandle, L7_uint32 size );

/**************************************************************************
*
* @purpose  Get next buffer from network mbuf
*
* @param    A ptr to a network mbuf handle
* @param    A ptr to a buffer handle
* @param    Size of data in network mbuf
*
* @returns  void
*
* @notes    For Device Driver use Only
*
* @end
*
*************************************************************************/
void sysapiNetMbufGetNextBuffer( L7_netBufHandle netMbufHandle, L7_netBlockHandle *blockHandle, L7_uchar8 **bufData, L7_uint32 *bufSize );

/**************************************************************************
*
* @purpose  Get the totals frame size of a network mbuf
*
* @param    A ptr to an network mbuf handle
*
* @returns  void
*
* @notes    For Device Driver use Only
*
* @end
*
*************************************************************************/
L7_uint32 sysapiNetMbufGetFrameLength( L7_netBufHandle netMbufHandle );

/**************************************************************************
*
* @purpose  Configure service port ip
*
* @param    ipAddr         32bit ip address
* @param    ipNetMask      32bit ip net mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t sysapiConfigServicePortIp (L7_uint32 ipAddr, L7_uint32 ipNetMask);

/**************************************************************************
*
* @purpose  Configure switch ip
*
* @param    ipAddr         32bit ip address
* @param    ipNetMask      32bit ip net mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t sysapiConfigSwitchIp (L7_uint32 ipAddr, L7_uint32 ipNetMask);

/*********************************************************************
 * @purpose  Use prefix length to isolate network number of a network address
 *
 * @param    prefixLen   @b{(input)}  prefix length (in bits)
 * @param    bufSize     @b{(input)}  number of bytes in output buffer
 * @param    *mask       @b{(inout)}  network address buffer location
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments Caller must supply a mask buffer at least as large as the
 *           bufSize parm indicates.
 *
 * @comments The nwtwork address is always assumed to be in network byte order
 *           for bufSize number of bytes.  A typical bufSize of 4 is used for
 *           IPv4 and 16 for IPv6 addresses.
 *
 * @end
 *********************************************************************/
L7_RC_t sysapiPrefixNetMaskApply(L7_uint32 prefixLen, L7_uint32 bufSize, L7_uchar8 *mask);

/*********************************************************************
 * @purpose  Create a network address mask from an arbitrary prefix length
 *
 * @param    prefixLen   @b{(input)}  prefix length (in bits)
 * @param    bufSize     @b{(input)}  number of bytes in output buffer
 * @param    *mask       @b{(output)} address mask output buffer location
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments Caller must supply a mask buffer at least as large as the
 *           bufSize parm indicates.
 *
 * @comments The mask is always built in network byte order for bufSize
 *           number of bytes.  A typical bufSize of 4 is used for IPv4
 *           and 16 for IPv6 mask creation.
 *
 * @end
 *********************************************************************/
L7_RC_t sysapiPrefixLenToNetMask(L7_uint32 prefixLen, L7_uint32 bufSize, L7_uchar8 *mask);

/*********************************************************************
*
* @purpose  Convert Prefix Length to IPv4 Subnet Mask
*
* @param    prefixLen   @b{(input)}  prefix length (in bits)
* @param    mask        @b{(output)} Mask in the format of L7_IP_MASK_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The mask is always built in host byte order
*
* @end
*
*********************************************************************/
L7_RC_t sysapiIPv4PrefixLenToAddrMask (L7_uchar8 prefixLen, L7_IP_MASK_t *mask);

/*********************************************************************
*
* @purpose  Convert IPv4 Subnet Mask to Prefix Length
*
* @param    mask        @b{(input)} Mask in the format of L7_IP_MASK_t
* @param    prefixLen   @b{(output)} prefix length (in bits)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The mask is always built in host byte order
*
* @end
*
*********************************************************************/
L7_RC_t sysapiIPv4AddrMaskToPrefixLen (L7_IP_MASK_t mask, L7_uchar8 *prefixLen);

/**************************************************************************
 * @purpose  Add service port IPv6 Prefix
 *
 * @param    ip6Addr        @b{(input)}   IPv6 prefix address
 * @param    ip6PrefixLen   @b{(input)}   IPv6 prefix mask length
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments    none.
 *
 * @end
 *************************************************************************/
L7_RC_t sysapiConfigServicePortIPV6PrefixAdd (L7_in6_addr_t *ip6Addr, L7_uint32 ip6PrefixLen);

/**************************************************************************
 * @purpose  Remove service port IPv6 Prefix
 *
 * @param    ip6Addr        @b{(input)}   IPv6 prefix address
 * @param    ip6PrefixLen   @b{(input)}   IPv6 prefix mask length
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments    none.
 *
 * @end
 *************************************************************************/
L7_RC_t sysapiConfigServicePortIPV6PrefixRemove (L7_in6_addr_t *ip6Addr, L7_uint32 ip6PrefixLen);

/**************************************************************************
 * @purpose  Add network port IPv6 Prefix
 *
 * @param    ip6Addr        @b{(input)}   IPv6 prefix address
 * @param    ip6PrefixLen   @b{(input)}   IPv6 prefix mask length
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments    none.
 *
 * @end
 *************************************************************************/
L7_RC_t sysapiConfigSystemIPV6PrefixAdd (L7_in6_addr_t *ip6Addr, L7_uint32 ip6PrefixLen);

/**************************************************************************
 * @purpose  Remove network port IPv6 Prefix
 *
 * @param    ip6Addr        @b{(input)}   IPv6 prefix address
 * @param    ip6PrefixLen   @b{(input)}   IPv6 prefix mask length
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments    none.
 *
 * @end
 *************************************************************************/
L7_RC_t sysapiConfigSystemIPV6PrefixRemove (L7_in6_addr_t *ip6Addr, L7_uint32 ip6PrefixLen);


/**************************************************************************
*
* @purpose  Ping an IP Address
*
* @param    *ipHost   An IP Address to ping
* @param    count     Number of times to issue a ping request
*
* @returns  Number of packets echoed
*
* @notes
*
* @end
*
*************************************************************************/
L7_uint32 sysapiPing (L7_char8 *ipHost, L7_int32 count);


/**************************************************************************
*
* @purpose  Determine if system has an IP address configured on any interface
*
* @param    void
*
* @returns  L7_TRUE     if system has at least one IP address configured
* @returns  L7_FALSE    otherwise
*
* @notes
*
* @end
*
*************************************************************************/
L7_BOOL sysapiIPConfigValid(void);

/* @p1261 start */
/*********************************************************************
* @purpose  checks IP Address and NetMask for subnet conflicts on the system
*
* @param    intIfNum      L7_uint32 internal interface number of the port ifIpAddress and ifNetMask are associated with
* @param    ifIpAddress   L7_uint32 IP Address to check for conflict
* @param    ifNetMask     L7_uint32 NetMask for ifIpAddress
* @param    errorMsg      L7_uint32 pointer to hold localized error message number if L7_TRUE is returned.
*
*
* @returns  L7_TRUE       if there is a conflict
* @returns  L7_FALSE      if there are no conflicts
*
* @notes    use FD_CNFGR_NIM_MIN_SERV_PORT_INTF_NUM for Service port interface
*           use FD_CNFGR_NIM_MIN_CPU_INTF_NUM for CPU interface
*
* @end
*********************************************************************/
L7_BOOL sysapiIPConfigConflict(L7_uint32 intIfNum, L7_uint32 ifIpAddress, L7_uint32 ifNetMask, L7_uint32 *errorMsg);
/* @p1261 end */

/*********************************************************************
* @purpose  Finds first valid IP address on this system
*
* @param    ipAddress    local address
*
* @notes    returns 0.0.0.0 if no valid IP addresses are found
*
* @end
*********************************************************************/
extern void sysapiIPFirstLocalAddress(L7_uint32 *ipAddress);

/**************************************************************************
*
* @purpose  Retrieve Application Specific Variable of network mbuf
*
* @param    A ptr to a network mbuf handle
*
* @returns  Application Specific Variable
*
* @notes
*
* @end
*
*************************************************************************/
L7_uint32 sysapiNetMbufGetApplSpecVar( L7_netBufHandle netMbufHandle );

/**************************************************************************
*
* @purpose  Set Application Specific Variable of network mbuf
*
* @param    A ptr to a network mbuf handle
* @param    Application Specific Variable
*
* @returns  void
*
* @notes
*
* @end
*
*************************************************************************/
void sysapiNetMbufSetApplSpecVar( L7_netBufHandle netMbufHandle, L7_uint32 value );

/**************************************************************************
*
* @purpose  Task that creates the application timer function
*
* @param    none
*
* @returns  none.
*
* @notes    If the task is already created, just return.
*
* @end
*
*************************************************************************/
void sysapiTimerTaskStart(void);

/*********************************************************************
*
* @purpose Combine all the '.cfg' files into one Big Config file
*          and save it as SYSAPI_BIG_CONFIG_FILENAME on the ramdisk
*
* @returns L7_SUCCESS
* @returns L7_ERROR  there were problems
*
* @end
*
*********************************************************************/
L7_RC_t   sysapiCfgFileCombine(void);


/*********************************************************************
*
* @purpose Re-read configuration from NVRAM into run-time storage.
*
* @returns none
*
* @end
*
*********************************************************************/
L7_RC_t sysapiCfgFileReload(L7_uchar8* filename);

/*********************************************************************
*
* @purpose Read the Big Config file from the ramdisk and parse it out
*          into all the component config files on the ramdisk
*
* @param   bigCfn   The Big Config File Name
*
* @returns L7_SUCCESS
* @returns L7_ERROR
*
* @end
*
*********************************************************************/
L7_RC_t   sysapiCfgFileDeploy(L7_char8 *bigCfn);

/*********************************************************************
*
* @purpose Determine the size of all the '.cfg' files and
*          also add in bytes for headers, crc and num files.  This
*          will tell the caller how much memory needs to be allocated
*          for the buffer used to combine all '.cfg' files.
*
* @param   pFileSize Pointer to total number of bytes needed for the
*                    Big Config file
*
* @returns L7_SUCCESS
*          L7_ERROR
*
* @end
*
*********************************************************************/
L7_RC_t sysapiBigcFileSizeGet(L7_uint32 *pFileSize);

/*********************************************************************
*
* @purpose Retrieve the directory listing and store in usable form
*
* @param tree Pointer to the directory listing structure
* @param pFileNum Pointer to the number of files in the directory
*
* @returns L7_SUCCESS
* @returns L7_ERROR
*
* @end
*
*********************************************************************/
L7_RC_t sysapiFileListGet(SYSAPI_DIRECTORY_t *tree, L7_uint32 *pFileNum);

/*********************************************************************
*
* @purpose Malloc space for all the filenames in the SYSAPI_DIRECTORY_t tree
*
* @param *tree Pointer to the directory listing structure
*
* @returns L7_SUCCESS
* @returns L7_ERROR
*
* @end
*
*********************************************************************/
L7_RC_t  sysapiTreeMalloc(SYSAPI_DIRECTORY_t *tree);

/*********************************************************************
*
* @purpose Free space of all the filenames in the SYSAPI_DIRECTORY_t tree
*
* @param *tree Pointer to the directory listing structure
*
* @returns nothing
*
* @end
*
*********************************************************************/
void      sysapiTreeFree(SYSAPI_DIRECTORY_t *tree);


/**************************************************************************
*
* @purpose  Initialize the sysapi network support layer
*
* @param    void
*
* @returns  L7_SUCCESS, if successful
* @returns  L7_FAILURE, otherwise
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t sysapiIfNetInit( void );

/*********************************************************************
* @purpose  Set the MAC address for the specified interface
*
* @param    portMac     port mac address to be set
* @param    sysMac      system base mac address
* @param    portNumber  port number
*
*
* @returns  None
*
* @notes    none
*
* @end
*********************************************************************/
void sysapiSetMacAddress(L7_uchar8  *portMac,L7_uchar8 *sysMac, L7_uint32 portNumber);


/*********************************************************************
*
* @purpose Allocate memory to store individual component config files
*
* param    Void
*
* @returns L7_SUCCESS
* @returns L7_ERROR
*
* @end
*
*********************************************************************/
L7_RC_t sysapiCfgFileSysInit(void);

/*********************************************************************
* @purpose  Invalidate run-time configuration.
*
* @param    none
*
* @returns  none
*
* @notes
*
* @end
*********************************************************************/
void sysapiCfgFileInvalidate (void);

/*********************************************************************
* @purpose  Write data to the file. Create/Open file if does not exist.
*
* @param    component_id  component ID of the file owner.
* @param    filename      file to write data to
* @param    buffer        actual data to write to file
* @param    nbytes        number of bytes to write to file
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sysapiCfgFileWrite(L7_COMPONENT_IDS_t component_id, L7_char8 *filename,
                           L7_char8 *buffer, L7_uint32 nbytes);


/*********************************************************************
 * @purpose  Write binary data to the file. Create/Open file if does not exist.
 *
 * @param    component_id  Component ID of the file owner.
 * @param    filename      file to write data to
 * @param    buffer        actual data to write to file
 * @param    nbytes        number of bytes to write to file
 *
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t sysapiBinaryCfgFileWrite( L7_COMPONENT_IDS_t component_id, L7_char8 *filename,
                                  L7_char8 *buffer, L7_uint32 nbytes);

/*********************************************************************
* @purpose  Write raw data to the file. Create/Open file if does not exist.
*
* @param    component_id  Component ID of the file owner.
* @param    filename      file to write data to
* @param    buffer        actual data to write to file
* @param    nbytes        number of bytes to write to file
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sysapiBinaryCfgFileRawWrite(L7_COMPONENT_IDS_t component_id, L7_char8 *filename,
                                    L7_char8 *buffer, L7_uint32 nbytes);

/*********************************************************************
*
* @purpose Combine all `*.cfg` files into one Big Config file and
*          save it as SYSAPI_CONFIG_FILENAME on the ramdisk
*
* @param   Void
*
* @returns L7_SUCCESS
* @returns L7_ERROR
*
* @end
*
*********************************************************************/
L7_RC_t sysapiCfgFileSave(L7_uchar8* filename);


/*********************************************************************
* @purpose  Read configuration data from the data location pointer to
*           by the config file management system
*
* @param    component_id   component ID of the file owner.
* @param    fileName       name of file to load
* @param    buffer         pointer to location file will be placed
* @param    bufferSize     size of buffer
* @param    checkSum       pointer to location of checksum in buffer
* @param    version        expected software version of file (current version)
* @param    defaultBuild   function ptr to factory default build routine
*                          if L7_NULL do not build default file
* @param    migrateBuild   function ptr to config migration routine
*                          if config is outdated
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Routine assumes checkSum is last 4 bytes of buffer.
*           Routine assumes first structure in the buffer is L7_fileHdr_t
*
* @end
*********************************************************************/
#ifdef L7_PRODUCT_SMARTPATH
#define sysapiCfgFileGet(a,b,c,d,e,f,g,h) sysapiCfgFileGetImpl(a,b,c,d,e,f,g,L7_NULLPTR)
#else
#define sysapiCfgFileGet(a,b,c,d,e,f,g,h) sysapiCfgFileGetImpl(a,b,c,d,e,f,g,h)
#endif
L7_RC_t sysapiCfgFileGetImpl(L7_COMPONENT_IDS_t component_id ,  L7_char8 *fileName,
                         L7_char8 *buffer, L7_uint32 bufferSize,
                         L7_uint32 *checkSum, L7_uint32 version,
                         void (*defaultBuild)(L7_uint32),
                         void (*migrateBuild)(L7_uint32, L7_uint32, L7_char8 *));


/*********************************************************************
* @purpose  Read configuration data from the data location pointer to
*           by the config file management system and return the filesize
*
* @param    component_id  Component ID of the file owner.
* @param    fileName       name of file to return buffer size of
* @param    bufferSize     pointer to location of buffer size
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t sysapiCfgFileSizeGet( L7_COMPONENT_IDS_t component_id, L7_char8 *fileName, L7_uint32 *bufferSize);

/*********************************************************************
* @purpose  Read configuration data from the data location pointer to
*           by the config file management system
*
* @param    component_id   Component ID of the file owner.
* @param    fileName       name of file to load
* @param    buffer         pointer to location file will be placed
* @param    bufferSize     size of buffer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t sysapiCfgFileRawGet( L7_COMPONENT_IDS_t component_id, L7_char8 *fileName,
                            L7_char8 *buffer, L7_uint32 *bufferSize);



/*********************************************************************
* @purpose  displays the configuration file header information
*
* @param    pFileHdr    Pointer to an L7_fileHdr_t structure
*
* @returns  NONE
*
* @notes
*
* @end
*********************************************************************/
void sysapiCfgFileHeaderDump (L7_fileHdr_t  *pFileHdr);


/*********************************************************************
* @purpose  Initialize the keyable features mask and read out the saved enabled mask.
*
* @param    none.
*
* @returns  none.
*
* @end
*********************************************************************/
void FeatureKeyInit();

/*********************************************************************
* @purpose  Set a Feature key's value.
*
* @param    CompID        Component ID to set.
* @param    enabled       L7_TRUE or L7_FALSE.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t sysapiFeatureKeySet(L7_COMPONENT_IDS_t CompID, L7_BOOL enabled);

/*********************************************************************
* @purpose  Get the value of a feature key.
*
* @param    CompID   Component ID to get.
* @param    enabled  ptr to place value of key (L7_TRUE or L7_FALSE).
* @param    keyable  ptr to return status if component ID requested is keyable (L7_TRUE or L7_FALSE).
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if CompID is out of range.
*
* @end
*********************************************************************/
L7_RC_t sysapiFeatureKeyGet(L7_COMPONENT_IDS_t CompID, L7_BOOL *enabled, L7_BOOL *keyable);

/*********************************************************************
* @purpose  Get the address of the static feature array mask.
*
* @param    FeatureKeyMask Ptr to place address of Feature Key Mask.
*
* @returns  none.
*
* @end
*********************************************************************/
void sysapiFeatureKeyMaskGet(ComponentMask **FeatureKeyMask);

/*********************************************************************
* @purpose  Returns the presence of feature keys
*
* @param    none
*
* @returns  L7_TRUE   if the first key is not L7_LAST_COMPONENT_ID
*           L7_FALSE  if it is
*
* @end
*********************************************************************/
L7_BOOL sysapiFeatureKeyPresent();

/*********************************************************************
*
* @purpose Select an IP address for an application component,
*          intended for components that need a common method to
*          select an IP address to communicate with external devices.
*
* @param    pIpAddr        @b{(input/output)} current/new IP address
* @param    pIpMask        @b{{input/output}} current/new IP subnet mask
* @param    pRtrMode       @b{(input/output)} current/new routing mode
* @param    pIntIfNum      @b{(input/output)} current/new internal interface number
* @param    pLoopbackInUse @b{(input/output)} indicates if application current/new using loopback
* @param    pReason        @b{(output)} reason code to indicate why no IP address selected
*
* @returns L7_SUCCESS, IP address selected after NULL IP.
*          L7_FAILURE, IP address not selected, or IP address change after previous IP.
*
* @notes  currently this function returns failure and a failure reason code on an IP
*         address change, there should be an IP address change reason code instead.
*
* @end
*
*********************************************************************/
extern L7_RC_t sysapiApplicationIPAddressSelect(L7_uint32               *pIpAddr,
                                                L7_uint32               *pIpMask,
                                                L7_uint32               *pRtrMode,
                                                L7_uint32               *pIntIfNum,
                                                L7_BOOL                 *pLoopbackInUse,
                                                SYSAPI_APP_IP_REASON_t  *pReason);

/******************************************************************************
*
* @purpose Verify if configured static IP address for wireless application
*          component is same as network interface for switching package or
*          a routing interface on a routing package.
*
* @param    staticIPAddr        @b{(input)} Configured static IP address
*
* @returns L7_SUCCESS, Configured IP address match found.
*          L7_FAILURE, Configured IP address match not found.
*
* @notes
*
* @end
*
*********************************************************************/
extern L7_RC_t sysapiApplicationIPAddressVerify(L7_uint32  staticIPAddr);

/*********************************************************************
* @purpose  Sets the Clear Config Flag.
*
* @param    none
*
* @returns
*
* @notes The clearConfig flag will be set to TRUE when user clears the configuration
         and will be set to FALSE when user saves the configuration to startup-config.
*
* @end
*
*********************************************************************/
void sysapiClearConfigFlagSet(L7_BOOL clearConfig);

/*********************************************************************
* @purpose  Gets the Clear Config Flag.
*
* @param    none
*
* @returns
*
* @notes The clearConfig flag will be having TRUE when user clears the configuration
         and will be having FALSE when user saves the configuration to startup-config.
*
* @end
*
*********************************************************************/
void sysapiClearConfigFlagGet(L7_BOOL *val);

/*********************************************************************
* @purpose  Execute commands on a driver shell
*
* @param    str    string needed to be executed on the debug shell
*
* @returns
*
* @notes
*
* @end
*********************************************************************/
void sysapiDebugDriverShell(const L7_uchar8 str[]);


/* Reason codes indicating why the packets was received */
#define  L7_MBUF_RX_REASON_NONE  0x0
#define  L7_MBUF_RX_REASON_ICMP_REDIR  0x1 /* IP Packet hit ICMP redirect scenario */
#define  L7_MBUF_RX_REASON_L3_MISS  0x2    /* IP packet failed L3 lookup */
#define  L7_MBUF_RX_SAMPLE_SOURCE  0x4     /* packet ingress sampled to the CPU */
#define  L7_MBUF_RX_SAMPLE_DEST    0x8     /* packet ingress sampled to the CPU */
#define  L7_MBUF_RX_MIRROR         0x10    /* packet mirrored to the CPU */

/**************************************************************************
* @purpose  Retrieve the receive reason code(s) of network mbuf
*
* @param    A ptr to a network mbuf handle
*
* @returns  Reason code(s)
*
* @comments Valid only for RX mbufs and not TX mbufs.
*
* @end
*************************************************************************/
L7_uint32 sysapiNetMbufGetRxReasonCode( L7_netBufHandle netMbufHandle );

/**************************************************************************
* @purpose  Set the reason code(s) of RX network mbuf
*
* @param    A ptr to a network mbuf handle
* @param    Reason code(s)
*
* @returns  void
*
* @comments Valid only for RX mbufs and not TX mbufs. Invoked by Driver when
*           a packet is received (before passing the packet to application).
*
* @end
*************************************************************************/
void sysapiNetMbufSetRxReasonCode( L7_netBufHandle netMbufHandle,
                                   L7_uint32  rxCode);

L7_BOOL sysapiTxtCfgEnableGet(void);
void sysapiTxtCfgEnableSet(L7_BOOL val);
L7_BOOL sysapiTxtCfgValidGet (void);
L7_char8 *sysapiTxtCfgFileGet (void);
L7_char8 *sysapiTxtRunCfgFileGet(void);
L7_RC_t sysapiUnconfig (void);
void sysapiTxtCfgValidSet(L7_BOOL val);

/* CPU Utilization defines and prototypes */

/* Utilization sampling period */
#define L7_CPU_UTIL_SAMPLE_PERIOD_SECONDS  5

#define L7_CPU_UTIL_MAX_TASKS 200

#define L7_CPU_RISING_TRAP_MAX_TASKS_REPORT 3

/* How long to keep the utilization for deleted tasks */
#define L7_CPU_UTIL_TASK_AGE_TIME_SEC   (180)

/* Scaling constant to get better precision */
#define L7_CPU_UTIL_SCALAR 10000


/* Max number of periods over which cpu utilization is monitored. 
 * By default, utilization over three periods are monitored: 5 sec, 60 sec, 300 sec.
 * One period can be configured by the user via cpu util monitoring
 * feature.
 */
#define L7_CPU_UTIL_MEASURE_MAX_PERIODS  4 

#include "transfer_exports.h"

typedef struct
{
  L7_uint32 util;
  L7_uint32 taskId;
} cpuTaskUtilSort_t;

typedef struct
{
  /* Time period (secs) over which cpu utilization is being measured. 0 indicates
   * that this time-period is not active.
   */
  L7_uint32 timePeriod;

  /* Scaled Exponential moving avg for this period */
  L7_int32 expMovingAvg;

  /* Time (sec) at which measurement started for this period */
  L7_uint32  measureStartTime;

  /* L7_TRUE indicates that avg for first period has been calculated */ 
  L7_BOOL   firstAvgDone; 
  
} cpuPeriodUtilInfo_t;

typedef struct
{
  L7_int32 taskId;

  /* Indicates that the task entry needs to be aged out as task has been deleted */
  L7_BOOL  pendingAge;

  /* Number of time-periods being monitored for the task */
  L7_int32 numPeriods;

  /* Task utilization info for various periods. The period table is kept sorted
   * based on the timePeriod.
   */
  cpuPeriodUtilInfo_t periodInfo[L7_CPU_UTIL_MEASURE_MAX_PERIODS];

} sysapiTaskCpuUtil_t;

typedef struct
{
  /* Info for this period */
  cpuPeriodUtilInfo_t periodInfo;  

  /* Traps is generated if util crosses this threshold. A value of 0 indicates
   * that no threshold is configured 
   */
  L7_uint32            risingThreshold;

  /* L7_TRUE indicates that rising threshold has been crossed and trap was sent.
   * Now the falling threshold will be monitored and trap would be sent if the
   * util falls below the fallingThreshold.
   */
  L7_BOOL              risingTrapSent;


  /* If the cpu threshold falls below this, then another trap is sent */
  L7_uint32            fallingThreshold;

  /* falling measurement info */
  cpuPeriodUtilInfo_t fallingPeriodInfo;

} sysapiTotalCpuUtil_t;


void sysapiCpuUtilLockTake(void);
void sysapiCpuUtilLockGive(void);
void sysapiTaskCpuUtilTableSort();
L7_BOOL sysapiTaskCpuUtilTableFind(L7_int32 taskId, L7_uint32 *index);
L7_uint32 sysapiTaskCpuUtilTableInsert(L7_int32 taskId);
L7_BOOL sysapiCpuUtilMonitorActive();
void sysapiCpuUtilTaskAge(void);
void sysapiTaskCpuUtilUpdate(L7_uint32 taskIdx, L7_int32 scaledUtil);
void sysapiTotalCpuUtilUpdate(L7_int32 scaledUtil);
L7_RC_t sysapiTotalCpuUtilTablePeriodInsert(L7_uint32 risingPeriod, 
                                            L7_uint32 risingThreshold,
                                            L7_uint32 fallingPeriod,
                                            L7_uint32 fallingThreshold);
L7_RC_t sysapiTotalCpuUtilTablePeriodRemove(L7_uint32 risingPeriod);
                                           

/*********************************************************************
* @purpose  Start task that measures system-wide task utilization.
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t sysapiCpuUtilTaskStart (void);

/*********************************************************************
*
* @purpose Returns the total CPU utilization for various periods
*
* @param cpuUtil {(input)} Pointer to memory to store cpu util. The memory
*                          must be large enough to store info for up to
*                          L7_CPU_UTIL_MEASURE_MAX_PERIODS.
*
* @returns Number of entries in the array.
*
* @notes Return utilization values are scaled with L7_CPU_UTIL_SCALAR
*
* @end
*
*********************************************************************/
L7_uint32 sysapiTotalCpuUtilGet (cpuPeriodUtilInfo_t *cpuUtil);

/*********************************************************************
* @purpose  Fill print buffer with string describing the
*           system-wide task utilization.
*
* @param    outputBuf : pointer to print buffer
* @param    bufferSize: size of print buffer
* @param    tasksFlag : display task utilization
*
* @returns  number of characters placed in print buffer
*
* @end
*
*********************************************************************/
int sysapiTaskUtilPrint (L7_char8 *outputBuf,
                         L7_uint32 bufferSize);
                         

/*********************************************************************
* @purpose  Fill print buffer with string describing the
*           task utilization for the specified task index.
*
* @param    processIndex
* @param    taskNameString
* @param    taskNameSize
* @param    taskPercentString
* @param    taskPercentSize
*
* @returns
*
* @end
*
*********************************************************************/
L7_RC_t sysapiTaskSpecificUtilizationGet (L7_uint32 processIndex,
                                          L7_uchar8 *taskNameString, L7_uint32 taskNameSize,
                                          L7_uchar8 *taskPercentString, L7_uint32 taskPercentSize);

/*********************************************************************
* @purpose  Get the next task ID.
*
* @param    processIndex
* @param    taskIdString
* @param    taskIdSize
*
* @returns
*
* @end
*
*********************************************************************/
L7_RC_t sysapiTaskIdGet (L7_uint32 processIndex,  L7_char8 *taskIdString,
                         L7_uint32 taskIdSize);

/*********************************************************************
* @purpose  Returns the value of the service port debug flag.
*
* @param    None
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL sysapiServicePortDebugFlagGet(void);

#endif
