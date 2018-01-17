 /********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename      std.h
 *
 * @purpose       standard typedefs and defines for all objects
 *
 * @component     Routing Utils Component
 *
 * @comments
 *
 * @create        
 *
 * @author
 *
 * @end
 *
 * ********************************************************************/
#ifndef std_h
#define std_h

/* typedefs */
#ifndef GEN_USE_TYPES       /* IB because of type definitions collision */
#define GEN_USE_TYPES

/* general typedefs */


#ifndef UNUSED
#define UNUSED(p) ((void)p)
#endif

/*
 * --------------------- *
 *     error codes
 * --------------------- *
 */

typedef enum
{
   E_OK = 0
   , E_RES_UNAVAIL
   , E_STATE_NOT_APPL
   , E_FAILED
   , E_DOWN
   , E_BADPARM
   , E_TOOMANY
   , E_NOMEMORY
   , E_PORTDOWN
   , E_INVALID_FRAME
   , E_NO_ROOM
   , E_NO_MATCH
   , E_BUSY
   , E_NOT_FOUND
   , E_NO_ROUTING
   , E_IN_MATCH
   , E_DISCARD

/* frame relay */
   , E_INVALID_CLLM
   , E_NOT_MULTI
   , E_TOO_SHORT
   , E_PROTOCOL_ERROR
   , E_UNKNOWN_IE
   , E_SEQUENCE_ERROR
   , E_UNKNOWN_REPORT

/* ATM */
   , E_INVALID_CELL
   , E_OVERFLOW
   , E_EXISTS

/* driver */
   , E_INVALID_PAR
   , E_NO_MORE
   , E_NO_INTERNAL_MEM
   , E_NOT_SUPPORTED
   , E_TOO_LARGE
   , E_SEMAPHORE_STUCK
   , E_BAD_ALIGNMENT

/* 860sar API */
    , E_ILL_SCC
    , E_SCC_NOT_FREE
    , E_ACTIVE_CHANNEL
    , E_NO_AVAILABLE_CHANNEL
    , E_NOT_INITIALIZED_SCC
    , E_ILL_ADDRESS_MAPPING
    , E_CHANNEL_NOT_IN_ADDRESS_MAPPING_TABLES
    , E_NO_FREE_BLOCK
    , E_ILL_UTOPIA_SCC
    , E_BUSY_CHANNEL
    , E_ILL_MPHY

/* MGAL */
    , E_ATTR_NOT_SUPPORTED
    , E_EVENT_NOT_SUPPORTED
    , E_SIGNAL_NOT_SUPPORTED
    , E_BAD_SIGNAL_PARMS
    , E_ALREADY_ONHOOK
    , E_ALREADY_OFFHOOK
    , E_NO_DIGIT_MAP
    , E_NO_CONNECTION
    , E_NO_CALLID
    , E_BAD_CONN_MODE
    , E_NO_RMT_ADDRESS 

   , E_DUMMY = -1
} e_Err;

typedef e_Err  eErr;

#ifndef NULLP
#define NULLP 0L
#endif

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

typedef unsigned char            Bool;
typedef unsigned char            byte;
typedef unsigned short           word;
typedef unsigned long            ulng;
typedef struct { char c[4]; }    t_Name;
typedef void *                   t_Handle;
typedef t_Handle                 t_Mutex;
/* t_HandleAsLong type is an integer of the same size as a pointer */
/* Might require tuning on some platforms                          */
typedef unsigned long            t_HandleAsLong;

/* Logical interface ID. Only least significant 31 bits can be used, */
/* because the 0xffffffff (LIH_API) value has a special meaning.     */
typedef ulng t_LIH;

/* Local API Logical Interface Handle */
#define LIH_API 0xffffffff

/*------------------------------------------------------------------------------
Software IEC/IEEE floating-point types.
-------------------------------------------------------------------------------*/
typedef unsigned int             float32;

/* The following types may be defined here or somewhere else */
#ifndef PORTABLE_TYPES_IN_STDH
#   ifndef __atmos__
#      define PORTABLE_TYPES_IN_STDH  1
#   endif
#endif

#if PORTABLE_TYPES_IN_STDH

   typedef unsigned char            U8;
   typedef signed char              S8;
   typedef unsigned short           U16;
   typedef signed short             S16;
   typedef unsigned long            U32;
   typedef signed long              S32;

#endif

typedef word   t_Dlci;        /* 2-octet dlci's only */
typedef ulng   t_Vcc;         /* VPI/VCI */

/* 64-bit scalar value */
typedef struct
{
   ulng    MS_Long;      /* most significant 32 bits */
   ulng    LS_Long;      /* least significant 32 bits */
} t_LongLong;

/* Physical interface type */
typedef enum
{
     T1 = 0         /* T1-based interface                */
   , E1             /* E1-based interface                */
   , UTOPIA_SPHY    /* Utopia-based single PHY interface */
   , UTOPIA_MPHY    /* Utopia-based multi PHY interface  */
   , T1_56KBS       /* n x 56 Kbs T1 interface */
   , PCM_3TS        /* three time slots-based interface  */
                    /* for 2B+D(ISDN) applications family*/
} e_PhysicalInterface;


typedef  byte  t_IPAddr[4];     /* IP Address */
typedef  byte  t_MACAddr[6];    /* MAC Address */

typedef struct tagt_IPEndPt
{
   t_IPAddr    subNetMask;
   t_IPAddr    ipAddr;
   t_IPAddr    gateWay;
} t_IPEndPt;


#define MAXLAY3ADDR 4

/* Layer 3 address structure */
typedef struct t_Lay3Addr
{
   byte lay3Addr[MAXLAY3ADDR];  /* layer 3 address */
   byte protocolId;             /* protocol ID returned by  */
}t_Lay3Addr;

/* Layer 3 prefix structure */
typedef struct t_Lay3Pref
{
   byte prefLength;             /* number of prefix bits (prefix width) */
   t_Lay3Addr lay3Addr;         /* layer 3 address to be &-ed with
                                   prefix mask */
}t_Lay3Pref;

#endif   /* GEN_USE_TYPES */

/*
 * module ID's
 * used for signing match table ownership for
 * diagnostic/debugging purposes only
 */
#define FR_MODULE       1        /* frame relay */
#define FRN_MODULE      2        /* frame relay network interworking */
#define FRS_MODULE      3        /* frame relay service interworking */
#define ATM_MODULE      4        /* ATM layer module */
#define ADX_MODULE      5        /* ATM-DXI module */




/*
 * Software Module
 * Each module is represented by the following structure.
 * It is used to bind different modules together.
 */
typedef enum { Up = 0, Down } e_UpDown;

typedef struct
{
   /* pointer to a module's bind routine */
   e_Err  (* f_Bind)(t_Handle, e_UpDown, t_Handle,
                           void  (*f_Unbind)(t_Handle),
                           e_Err (*f_Data)(t_Handle, void *, word),
                           void  (*f_Status)(t_Handle, word, word) );

   /* remove an object's routing information */
   void   (* f_Unbind)(t_Handle);

   /* pointer to a module's transmit routine */
   e_Err  (* f_Transmit)(t_Handle, void *, word);

   /* status indications from upper neighbour */
   void   (* f_StatusFromUpper)(t_Handle, word, word);

   /* pointer to a module's receive routine */
   e_Err  (* f_Receive)(t_Handle, void *, word);

   /* status indications from lower neighbour */
   void   (* f_StatusFromLower)(t_Handle, word, word);
   
   /* Changes the system management handle */
   t_Handle (*f_ChangeSysMngr)(t_Handle , t_Handle);
   
} t_SwModule; 

#ifdef __atmos__
#include "config.h"
#endif
#include "local.h"
#include "common.h"

/* Structure TLV header */
typedef struct t_StructTlvHdr
{
   word tlvLen;
   word tlvType;
   word tlvStruct;
   byte tlvVersion;
} t_StructTlvHdr;

/*  It's desirable to supply the below set of routines for every  
    structure that requires "transfer capability" through distributed 
    system.  
    The unique tag has to be assigned to each structure and additionally
    structure version has to be assigned to structure as well (in case
    the reformat capability is required for a structure).
    For every structure the next target routines could be supplied:
    - copy data routine;
    - clone data copy routine (create and copy);
    - free data;
    - data to frame encode routine;
    - frame to data decode routine;
    - print data routine;
    - list of reformat data routines
    Structure tag, version and routines set have to be placed in the ext file
    when structure is defined. For each ext file the unique tag space has
    to be defined.
    Initialization API routine can be supplied for every such module in
    order to fill the structure below with an appropriate set of routines.
*/  
/* Helper procedure prototypes */

typedef enum 
{
    DELETE_ALL = 0,
    DELETE_SUB_ELEMENTS = 1
} e_DelType;

typedef e_Err (*F_CopyStruct)    (IN      t_Handle   data,
                                  IN OUT  t_Handle   newdata);

typedef e_Err (*F_CloneStruct)   (IN      t_Handle   data,
                                  OUT     t_Handle   *p_data);

typedef void (*F_FreeStruct)     (IN      t_Handle   data,
                                  IN      e_DelType  delFlag);
 
typedef word (*F_EncodeStruct)   (IN      t_Handle   data,      
                                  IN OUT  word       *p_offset,
                                  IN OUT  t_Handle   frame);

typedef word (*F_DecodeStruct)   (IN      t_Handle   frame,
                                  IN      word       frameSize,
                                  IN      byte       tlvVersion,
                                  IN      word       tlvStruct,
                                  IN OUT  word       *p_offset,
                                  OUT     t_Handle   *p_data);

typedef word (*F_PrintStruct)    (IN      t_Handle   data,
                                  IN      word       prtFlag);

#if L7_REFORMAT
typedef void (*F_ReformatStruct) (IN      t_Handle   data,
                                  OUT     t_Handle   *p_newdata);

typedef struct tagt_ReformatStruct
{
   struct tagt_ReformatStruct *next;
   struct tagt_ReformatStruct *prev;
   byte                       versionId;  /* Current version ID           */
   F_ReformatStruct           f_Reformat; /* Reformat routine from the 
                                             previous type version to the 
                                             current type version         */
} t_ReformatStruct;
#endif

typedef struct t_StructSet
{
   word              tagId;               /* Unique structure ID          */
   byte              versionId;           /* Version ID                   */
   F_CopyStruct      f_Copy;              /* Copy data target             */
   F_CloneStruct     f_Clone;             /* Clone data target            */
   F_FreeStruct      f_Free;              /* Free data target             */
   F_EncodeStruct    f_Encode;            /* Encode data to frame target  */
   F_DecodeStruct    f_Decode;            /* Decode frame to data target  */
   F_PrintStruct     f_Print;             /* Print data target            */
#if L7_REFORMAT  
   t_ReformatStruct  *reformatList;       /* List of reformat routines    */
#endif
} t_StructSet; 

enum 
{
   T_QOSE,
   T_L2FS,
   T_LAY3ADDR,
   T_LAY3PREF,
   T_LAY3FILTERSPEC,
   T_LAY3FLOWSPEC,
   T_MPLSGENLABEL,
   P_USERPARAMS,
   T_MPLSLABELSETELM,
   T_NHRFE,
   T_MPLSLABELPRIMITIVE,
   T_NHLFE,                 
   T_SIGNUSERTLV,
   T_SIGNUSERTLVS,
   T_MPLSDIFFSRVMAPINFO,
   T_MPLSLABELREQUEST,
   T_MPLSLOOPDETECT,
   T_LSPPARAMS,
   T_REQRPSLSPACTIVE,
   T_RSPRPSLSPACTIVE,
   T_REQRPSLSPBACKUP,
   T_RSPRPSLSPBACKUP,
   T_REQACTIVE,
   T_RSPACTIVE,
   T_REQBACKUP,
   T_RSPBACKUP,
   T_FIBFILTERSPEC,
   T_FTNEINTINFO,
   T_ILMEINTINFO,
   T_URFEINTINFO,
   T_MRFEINTINFO,
   T_FQEINTINFO,
   T_ARPEINTINFO,
   T_RSVPADSPEC,
   T_RSVPRRSO,
   T_RSVPMPLSSESSION,
   T_PSBDISCARDPARMS,
   T_PSBBLOCKADEPARMS,
   T_RSVPNRO,
   T_RSVPLSPTUNINFID,
   T_RSVPCOMPINFID,
   T_RSVPINSTANCE,
   T_RSVPNEIGHBOR,
   T_PSB,
   T_ANINFO,
   T_ERINFO,
   T_BELSPINFO,
   T_BELSPINCSGM,
   T_BELSPOUTSGM,
   T_TELSPINFO,
   T_LDPSESSOBJ,
   T_LDPLSPINFO,
   T_LAY3ADDRLIST,
   T_SAVEDLDPMSG,
   T_LDPIFOBJ,
   T_ROUTEINFO,
   T_RESSPECINFO,
   T_TOPCONSTRAINS,
   T_NEXTHOPINFO,
   T_SRLGINFO,
   T_IPMSREG,
   T_IPMSOBJREG,
   T_IPMSDECFUNCREG,
   T_MPLSATMVPIVCIRANGE,
   T_MPLSFRDLCIRANGE,
   T_MPLSCTRLIFPARAMS,
   T_RSVPINTERFACEPARAMS,
   T_LDPINTERFACEPARAMS
};

#if !L7_RPS
#define IS_ACTIVE_SIDE()     TRUE
#define IS_SIDE_BUSY()       FALSE
#endif

#endif

/* --- end of file std.h --- */
