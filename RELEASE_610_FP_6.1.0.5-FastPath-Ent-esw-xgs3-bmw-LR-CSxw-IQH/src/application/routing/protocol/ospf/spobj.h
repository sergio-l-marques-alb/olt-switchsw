/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename           spobj.h
 *
 * @purpose            OSPF SW internal definitions
 *
 * @component          Routing OSPF Component  
 *
 * @comments
 *                     Preparsing LSA Database entries reference model
 *
 * (Router)p_DbEntry->ConnList    ->  List of (Network)p_DbEntry
 * (Network)p_DbEntry->ConnList   ->  List of (Router)p_DbEntry
 * (Router)p_DbEntry->IFO_Id      ->  (TE Router)p_DbEntry
 * (TE Router)p_DbEntry->ConnList ->  List of (TE Link)p_DbEntry
 * (TE Link)p_DbEntry->ConnList   ->  (Router)p_DbEntry or (Network)p_DbEntry

 *
 * @create             02/18/2003
 *
 * @author
 *
 * @end
 *
 * ********************************************************************/
#ifndef spobj_h
#define spobj_h

/* Common to all OSPF files includes */


/* --- external object interfaces --- */

#include <string.h>
#include <stdlib.h>
#include <stddef.h>

#include "std.h"
#include "local.h"
#include "xx.ext"
#include "hl.ext"
#include "avl.ext"
#include "buffer.ext"
#include "frame.ext"
#include "timer.ext"
#include "ll.ext"
#include "sfloat.h"
#include "bitset.ext"
#include "agingtbl.ext"

/* --- specific include files --- */

#include "spapi.ext"
#include "spcfg.h"
#include "sppck.h"

#if L7_IPMS
#include "ipms.ext"
#endif


/*----------------------------------------------------------------
 *
 *             General definitions
 *
 *----------------------------------------------------------------*/

/* Get time interval from the time */
#define GET_INTERVAL(time)                                         \
   (TIMER_SysTime() >= (time) ?                                    \
    TICKS_TO_SECONDS(TIMER_SysTime() - (time)) :                   \
    TICKS_TO_SECONDS(TIMER_SysTime() + ((0xffffffffL - (time)) + 1)))

#define GET_PAYLOAD_PTR(p_Hdr) ((byte *)p_Hdr+sizeof(t_S_PckHeader))

#define GET_LSA_HDR(p_F) ((byte *) B_GetData(F_GetFirstBuf(p_F)))

#define GET_LSA(p_F) (GET_LSA_HDR(p_F) + sizeof(t_S_LsaHeader))

#define OSPF_MAX_TRACE_STR_LEN 255



/*----------------------------------------------------------------
 *
 *             Interface Object  (IFO)
 *
 *----------------------------------------------------------------*/

typedef enum e_IFO_States
{
   IFO_DOWN,
   IFO_LOOP,
   IFO_WAIT,
   IFO_PTPST,
   IFO_DR,
   IFO_BACKUP,
   IFO_DROTHER,
   NUMB_OF_IFO_STATES
}  e_IFO_States;

typedef enum e_IFO_Events
{
   IFO_INTERFACE_UP,
   IFO_WAIT_TIMER,
   IFO_BACKUP_SEEN,
   IFO_NGB_CHANGE,
   IFO_LOOP_IND,
   IFO_UNLOOP_IND,
   IFO_INTERFACE_DOWN,
   IFO_INTERFACE_PASSIVE,
   IFO_INTERFACE_NON_PASSIVE,
   NUMB_OF_IFO_EVENTS
}  e_IFO_Events;

/* LsAck List element structure */
typedef struct t_AckElement
{
	void *next_item;   /* next item in the list of items */
   t_Handle      NBO_Id; /* ack is send to ack LSA received from this NBO */
   t_S_LsaHeader LsaHdr; /* received LSA header */
}  t_AckElement;

/* Metric List element structure */
typedef struct t_MetricEntry
{
   struct t_MetricEntry *next;  /* pointer to the previous entry */
   struct t_MetricEntry *prev;  /* pointer to next entry */
   e_TosTypes  MetricTOS;    /* metric type */
   word        Metric;       /* metric value */
   Bool        IsValid;      /* metric is valid flag */
}  t_MetricEntry;

/* Configurable NBMA routers hash list entry */
typedef struct t_S_NbmaRtrCfg
{
   struct t_S_NbmaRtrCfg *next;  /* pointer to the previous entry */
   struct t_S_NbmaRtrCfg *prev;  /* pointer to next entry */

   SP_IPADR RouterId;        /* NBMA Router Id */
   SP_IPADR IpAdr;           /* NBMA Interface IP address */ 
   Bool     CanBeDr;         /* Can be Designated Router flag */

   Bool        IsValid;      /* NBMA entry is valid flag */
}  t_S_NbmaRtrCfg;

/* Interface Object sturcture */
typedef struct t_IFO
{
   struct t_IFO *next;  /* pointer to the previous entry */
   struct t_IFO *prev;  /* pointer to next entry */

   t_Name         Name;               /* 4 bytes of user-info */
   Bool           Taken;              /* object structure is being used */
    
   /* Interface identifier. Takes different values depending on the interface.
    * For virtual interfaces, this is the router ID of the virtual neighbor.
    * For normal interfaces, this is the primary IP address. If the interface
    * is unnumbered, this is the IfIndex. */
   ulng           IfoId;        

   /* Transit area for this interface. Only applies to virtual links. This field
    * must follow IfoId in the structure, because the combination of IfoId and
    * TransitAreaId is used as the hash key for the area's list of interfaces. 
    * NULL for non-virtual interfaces. */
   ulng           TransitAreaId; 

   t_Handle       TransitARO;         

   SP_IPADR       VrtNgbIpAdr;        /* The IP address of the Virtual Neighbor */       
   t_LIH          VirtIfIndex;        /* physical Interface index for virtual IFOs */

   /* Only applies to interfaces for virtual links. The IP address of the next hop
    * on the route to the virtual neighbor. This becomes the next hop of routes
    * whose virtual next hop is the virtual neighbor. The next hop is found in
    * VlinkStatusUpdate(). */
   SP_IPADR       NextHopToVrtNbr;          

   /* Area to which the interface belongs. The backbone for virtual interfaces. */
   t_Handle       ARO_Id;             

   t_Handle       LowLayerId;         /* low-layer object handler */  
   t_Handle       RTO_Id;             /* Parent Router Object Id */

   t_IFO_Cfg      Cfg;                /* Configuration parameters */

   SP_IPADR       DrId;               /* Designated Router ID  */
   SP_IPADR       BackupId;           /* Backup Router ID      */

   t_Handle       HelloTimer;         /* Hello packets resend timer handler */
   t_Handle       WaitTimer;          /* Stop wait timer */
   t_Handle       NboHl;              /* Neighbor Objects Hash List */
   t_Handle       LsAck;              /* Link State Acknowledgment list */
   t_Handle       AckTimer;           /* Ls Ack  repeatitive timer */

   void           *Hello_TT;          /* pointer to Hello SM transition table */

   t_Handle       MetricHl;           /* Configurable List of interface's metrics. */
 
   t_Handle       NbmaHl;             /* Configurable List of	all NBMA attached routers. */
    
   t_Handle       AuthKeyHl;          /* Authentication Keys list */

   ulng           TimeStamp;          /* time stamp of last sent Hello packet */

   /* LS Update packet awaiting transmission on this interface. */
   void           *PendingUpdate;

   ulng           CfgMetric;          /* Configured Metric Value (saved here) */
   t_PCK_Stats    PckStats;           /* Rx packets statistics */
#ifdef L7_OSPF_TE
   float32        TePdr[4][8];        /* Traffic enginnering last PDRs */
   ulng           TeLinkId;           /* TE LSA Link Id */
   byte           TeCosMask;          /* Last TE LSA CosMask */
   word           TeLsaInst;          /* Instance of TE LSA  */
#endif
   L7_uint32      neighborsOnIface;   /* Neighbors attached on this interface */
     
} t_IFO;

e_Err IFO_SendPacket(t_Handle Id, void *p_Packet, 
                     e_S_PckType Type, t_Handle NboId, Bool IsRetx);
e_Err IFO_SendHello(t_Handle Id, t_Handle NboId, Bool IsReplay);
e_Err IFO_SendPendingUpdate(t_Handle IfoId);
e_Err IFO_LsUpdateTx(t_Handle IfoId, void *p_F);
e_Err InactivityTimerExp( t_Handle Id, t_Handle TimerId, word Flag );

/* Allocate OSPF packet with offset */
void *IFO_AllocPacket(t_IFO *p_IFO, e_S_PckType pktType,
                      word offset, ulng *allocPktLen);
ulng GetMaxOspfBufLength(t_IFO *p_IFO, e_S_PckType pktType);
e_Err UnicastAckTimerExp( t_Handle Id, t_Handle TimerId, word Flag );
e_Err BroadcastAckTimerExp( t_Handle Id, t_Handle TimerId, word Flag );
void DoHelloTransition(t_IFO *p_IFO, e_IFO_Events Event);
word SPF_ComputeIpCheckSum(void *p_Data, word len);



/*----------------------------------------------------------------
 *
 *             Neighboring Object (NBO)
 *
 *----------------------------------------------------------------*/
typedef enum e_NBO_States
{
   NBO_DOWN,
   NBO_ATTEMPT,
   NBO_INIT,
   NBO_2WAY,
   NBO_EXSTART,
   NBO_EXCHANGE,
   NBO_LOADING,
   NBO_FULL,
   NUMB_OF_NBO_STATES
}  e_NBO_States;

extern unsigned char *nbrStateNames[NUMB_OF_NBO_STATES];

typedef enum e_NBO_Events
{
   NBO_HLO_RX,
   NBO_START,
   NBO_2WAY_RX,
   NBO_NEG_DONE,
   NBO_EXC_DONE,
   NBO_BAD_LSREQ,
   NBO_LOAD_DONE,
   NBO_ADJOK,
   NBO_SEQNUM_MISM,
   NBO_1WAY_RX,
   NBO_KILL,
   NBO_INACT_TIMER,
   NBO_LL_DOWN,
   NUMB_OF_NBO_EVENTS
}  e_NBO_Events;

extern unsigned char *nbrEventNames[NUMB_OF_NBO_EVENTS];

/* Neighboring SM Object sturcture */
typedef struct t_NBO
{
   struct t_NBO *next;  /* pointer to the previous entry */
   struct t_NBO *prev;  /* pointer to next entry */

   t_Name   Name;       /* 4 bytes of user-info */
   Bool     Taken;      /* object structure is being used */

   Bool     OperationState; /* NBO operational state enabled/disabled */

   byte     State;      /* NBO SM state */

   ulng     Uptime;     /* Time when neighbor last went to Full. Ticks since boot. */
   
   t_Handle IFO_Id;     /* Corresponded IFO object */
   t_Handle RTO_Id;     /* Corresponded RTO object */
   t_Handle ARO_Id;     /* Corresponded ARO object */
   t_Handle MngId;      /* User application managament id */

   t_Handle InactivityTimer;    /* No Hellos has been seen interval timer */
   ulng     InactivityInterval; /* = RouterDeadInterval */
   t_Handle DdRxmtTimer;        /* DD retransmission timer */
   t_Handle RqRxmtTimer;        /* LS Req retransmission timer */
   t_Handle SlaveLastDdTimer;   /* Last DD retransmission timer for Slave only */
   word     RxmtInterval;       /* DD retransmission Interval */
   ulng     HloIntCount;        /* Hello intervals counter */ 

   Bool     IsMaster;          /* TRUE if this router is Master */
   ulng     DdSeqNum;          /* DD Sequence Number */
   t_S_DbDscr LastRxDd;        /* Last received DD packet */
   void     *p_LastDd;         /* Last sent DD packet */
   void     *p_OutDdEntry;     /* first outstanding DD list entry */

   SP_IPADR RouterId;          /* Neighboring router ID */
   byte     RouterPriority;    /* the router priority of neighboring router */
   SP_IPADR IpAdr;             /* the IP address of neighboring router */
   byte     Options;           /* the optional OSPF capabilities */
   SP_IPADR DrId;              /* the neighbor's DR router-Id   */
   ulng     DrIpAddr;          /* DR Ip Addr get in the nbr's Hello packet*/
   SP_IPADR BackupId;          /* the neighbor's BDR router-Id  */
   ulng     BackupIpAddr;      /* BDR Ip Addr get in nbr's Hello packet */

   t_Handle LsReTx;            /* Link State retransmission list */
   t_Handle LsReTxHead;        /* Head of the ReTx LL, earliest sent */
   t_Handle LsReTxTail;        /* Tail of the ReTx LL, most recently sent */
   ulng     LsReTxSentTime;    /* Last LsReTx packet sent time */
   ulng     LsReTxCount;       /* Number of LSA retransmits to this neighbor. */

   t_Handle DbSum;             /* Data Base Summary list */
   t_Handle LsReq;             /* Link State request list */

   void     *Nbo_TT;           /* pointer to NBO FSM transition table */

   Bool     ExchOrLoadState;   /* NBO state is exchange or loading flag */

   ulng     NbrEvents;         /* The number of times this neighbor relationship
                                  has changed state, or an error has occurred. */

   ulng     dupDDs;            /* number of consecutive duplicate DDs from this neighbor. */

   /* number of hellos received from this neighbor */
   ulng     hellosRx;  

   /* time last hello received from neighbor. Seconds since boot. */
   ulng     lastHelloTime;  

   /* number of hellos received < (hello int / 2) after previous hello */
   ulng     lateHellos;

   Bool     NbrHelloSuppressed; /* Indicates whether Hellos are being  
                                   suppressed to the neighbor */
   Bool     InitDDSent;        /* Init DD packet sent flag */
   ulng     LastRxCryptSeqNum;  /* Last received cryptographic sequence number */
   
   t_SysLabel OspfSysLabel;       /* Timer notification Queue Id */
} t_NBO;

e_Err NBO_Init(t_Handle IFO_Id, SP_IPADR RouterId, 
                SP_IPADR SrcIpAdr, t_IFO_Cfg *p_Cfg, t_Handle *p_Object);
Bool NBO_Delete(t_Handle NBO_Id, ulng flag);
e_Err NBO_LsReTxInsert(t_Handle NBO_Id, byte *key, t_Handle entry);
e_Err NBO_LsReTxDelete(t_Handle NBO_Id, byte *key, t_Handle entry);
e_Err NBO_LsRxmtExpire(t_Handle NBO_Id);
e_Err NBO_LsReTxUpdateSentTime(t_Handle NBO_Id, t_Handle entry);
void DbDescrReceived(t_NBO *p_NBO, t_S_PckHeader *p_Hdr);
void LsReqReceived(t_NBO *p_NBO, t_S_PckHeader *p_Hdr);
void LsUpdateReceived(t_NBO *p_NBO, t_S_PckHeader *p_Hdr);
void LsAckReceived(t_NBO *p_NBO, t_S_PckHeader *p_Hdr);
void DoNboTransition(t_NBO *p_NBO, e_NBO_Events Event, void *p_Prm);

/* NBO's LSA request list entry */
typedef struct t_LsReqEntry
{
   struct t_LsReqEntry *next;  /* pointer to the previous entry */
   struct t_LsReqEntry *prev;  /* pointer to next entry */

   t_S_LsaHeader        Lsa;   /* header of neighbour's LSA copy */ 

}  t_LsReqEntry;


/*----------------------------------------------------------------
 *
 *                  Area Object (ARO)
 *
 *----------------------------------------------------------------*/

/* Area DB MT entry value structure */
typedef struct t_A_DbEntry
{
   t_S_LsaHeader Lsa; /* standard 20-byte LSA header */
   byte     *p_Lsa;   /* pointer to the LSA contents following the 20 byte LSA header */
   ulng     OrgTime;  /* last update time stamp     */
   ulng     RcvTime;  /* receiving time */
   ulng     SentTime; /* used only for rfc 2328 section 13) item 8) */
   word     InitAge; 
   word     AgeIndex;  /* Index into the aging object */

   word     dbFlags;   /* flags for this db entry that include bit settings
                        * encoded for IsCur, SeqNoOver, flushingHeld fields */

   t_Handle ARO_Id;   /* owner Area Object handler */
   t_Handle IFO_Id;   /* owner IFO Object handler for Link-Opaque LSAs,
                         for Router LSA points to TE Router LSA if any present */
   t_Handle ConnList; /* preparsing feature connectivity list, 
                         for RouterLSA points to list of Networks LSAs,
                         for Network LSA point to list of Router LSAs,
                         for TE Router Opaque LSA points to list of TE Link Opaque LSAs 
                         for TE Link Opaque LSA points to TE Router or Network LSA */
}  t_A_DbEntry;

/* Area DB MT entry key structure */
typedef struct t_A_DbKey
{
   byte           LsId[4];
   byte           AdvertisingRouter[4];
}  t_A_DbKey;

/* NBO's LSA retx list entry. Use OspfRetxEntryAlloc() to allocate. */
typedef struct t_RetxEntry
{
   struct t_RetxEntry *next;  /* pointer to the previous entry */
   struct t_RetxEntry *prev;  /* pointer to next entry */
	
   struct t_RetxEntry *nextSent;  /* pointer to the previous entry sent */
   struct t_RetxEntry *prevSent;  /* pointer to next entry sent */

   byte	  LsType;
   byte   LsId[4];
   byte   OrigId[4];
   ulng   SentTime;
   t_A_DbEntry *DbEntry;
}  t_RetxEntry;

typedef enum e_DbInstComp
{
   LSA_EQUAL,
   LSA_LESS_RECENT,
   LSA_MORE_RECENT
}  e_DbInstComp;

/* Delayed LSA list entry structure */
typedef struct t_DelayedLsa
{
   struct t_DelayedLsa *next;  /* pointer to the previous entry */
   struct t_DelayedLsa *prev;  /* pointer to next entry */

   e_S_LScodes LsaType;
   SP_IPADR    LsId;
   Bool        IsValid;
   t_Handle    ARO_Id;
   void        *p_F;
   ulng        OrgTime;
   byte        Option;
}  t_DelayedLsa;

typedef struct t_DlyLsaKey
{
   e_S_LScodes LsaType;
   SP_IPADR    LsId;
}  t_DlyLsaKey;

/* structure used as key for ifo hash table lookup. */
typedef struct t_IfoHashKey
{
    /* IP address of interface */
    SP_IPADR IfAddr;

    /* Area to which interface belongs */
    t_Handle aro;
} t_IfoHashKey;

/* Preparsing list entry structure */
typedef struct t_ParseEntry
{
   struct t_ParseEntry *next;  /* pointer to the previous entry */
   struct t_ParseEntry *prev;  /* pointer to next entry */
   
   t_A_DbEntry *p_DbEntry;
   word metric;

   /* Interface identifier. LinkData of link in rtr LSA. Only set for parse   
    * entries created from the local router LSA. Used to find the next hop via
    * the link corresponding to the parse entry. */
   ulng ifoId;  

   /* Type of link represented by this parse entry */
   e_LinkTypes linkType;

}  t_ParseEntry;

typedef enum e_McAreasPrecendence
{
   McSourceIntraArea, 
   McSourceInterArea2,
   McSourceInterArea1, 
   McSourceExternal,
   McSourceStubExternal
}  e_McAreasPrecendence;

typedef struct t_ARO
{
   struct t_ARO *next;  /* pointer to the previous entry */
   struct t_ARO *prev;  /* pointer to next entry */

   t_Name    Name;   /* 4 bytes of user-info */
   Bool      Taken;  /* object structure is being used */

   Bool      OperationState; /* ARO Operational state enabled/disabled */

   t_Handle  RTO_Id; /* Corresponded RTO object */

   SP_IPADR  AreaId; /* Area Identifier */
   t_Handle  MngId;  /* User application managament id */

   t_Handle	 IfoHl;        /* Associated IFO objects list */

   /* This hash list contains both type 3 summary and type 7 NSSA 
    * ranges. Unfortunately, when the HL was expanded to include T7 
    * ranges, the HL key wasn't expanded to cover the range type 
    * (t_AgrEntry.LsdbType). The OSPF UI and mapping layer both accept
    * a T3 and a T7 range with the same prefix and mask, but obviously
    * we can't put both in this HL. Some small changes would fix this; 
    * however, it's unlikely that a user would want to configure both a 
    * T3 and T7 range with same prefix and mask. So I'm not making these
    * changes at the moment. This means that any FindFirst()/FindNext()
    * operation on this HL must be followed by a check to verify that 
    * the range is of the desired type. */
   t_Handle  AdrRangeHl;   

   t_Handle  HostHl;       /* hosts list attached to this area */

	/* Area's LSA Database */

   t_Handle  RouterLsaHl;  /* Router LSAs DB list */ 		 
   t_Handle  NetworkLsaHl; /* Network LSAs DB list */
   t_Handle  NetSummaryLsaHl; /* Network Summary LSAs DB list */	 
   t_Handle  AsBoundSummaryLsaHl; /* AS boundary routers Summary LSAs list */	 
   t_Handle  Tmp0;             /* AS-Ext LSA dummy replacer */
   t_Handle  GroupMembLsaHl;  /* group-membership-LSAs */
   t_Handle  NssaLsaHl;        /* NSSA LSAs DB list */
   t_Handle  Tmp2;             /* not implemented LSAs */
   t_Handle  LinkOpaqueLsaHl;  /* Local-Link-Opaque-LSAs */
   t_Handle  AreaOpaqueLsaHl;  /* Local-Area-Opaque-LSAs */
   
   t_Handle  SpfHl;        /* The Shortest-path tree list */		
   t_Handle  CandidateHl;  /* The Candidate list for SPF calculation */		

   Bool      TransitCapability; /* calculated by spf algorithm and defines 
                                   transit data carrying capability */
   Bool      TransitCapabilityOld; /* TransitCapability of this area prior
                                    * to the re-run of the spf, used to find
                                    * the change in the transit-capability 
                                    * after spf */

   e_AreaImportExternalCfg ExternalRoutingCapability; /* defines (2) if this
                                           area is the stub */
   t_Handle StubDefaultCostHl; /* If this area is the stub, this
                                 parameter defines area's
                                 default cost*/
   Bool     StubDefLSAOriginated; /* If the area is stub area, this
                                 parameter states that def. summary LSA
                                 was already originated */
   e_AreaSummaryCfg FilterSumLsa; /* send/filter summary LSAs for a stub area 
                                  ** (same as importSummaries - OSPF NSSA RFC 3101)
                                  */
   ulng  ExchOrLoadState;  /* NBOs which state is exchange or loading counter */

   ulng  SpfRuns;          /* The number of times that the intra-area  route  
                              table  has  been  calculated                    */
   ulng  AreaBdrRtrCount;  /* The total number of area border routers reachable
                              within this area. */
   ulng  AsBdrRtrCount;    /* The number of Autonomous  System  border 
                              routers  reachable  within  this area */
   ulng  AreaIntRtrCount;  /* The number of Area Internal routers  reachable
                              within  this area */
   ulng  AreaLsaCount;     /* The number of link-state  advertisements
                              in  this  area's link-state database, excluding
                              AS External LSA's */
   ulng  AreaLsaStats[S_LAST_LSA_TYPE]; /* per LSA type stats for this area */
   ulng  SelfOrigNSSALsaCount;  /* The number of  self originated type-7 LSAs */
   ulng  AreaLsaCksumSum;  /* The cheksum of link-state  advertisements
                              in  this  area's link-state database, excluding
                              AS External LSA's */ 

   t_Handle DelayedLsa;    /* delayed LSAs queue handler */
   t_Handle DlyLsaTimer;   /* delayed LSAs timer handler */

   t_SysLabel OspfSysLabel;  /* Timer exp messages notification queue id */

   Bool       HasVirtLink;
   Bool       IsAgrInit;

   Bool       IsRtrLsaOrg;  /* service flag for Router LSA origination */
   Bool       IsNtwLsaOrg;  /* service flag for Network LSA origination */

#ifdef L7_OSPF_TE
   /* Defines if Traffic engineering extension is supported for this area */
   Bool TEsupport;
   ulng TeSpfRuns; /* Counter of QOS SPF runs */
   t_Handle  TeLsaHl;  /* TE LSAs hash list for TE Links with key as Router ID+Link ID */
   t_Handle  TeRtrHl;   /* TE LSAs hash list for TE Router Opaque LSAs with key as Router ID */
#endif

   ulng       DataBaseSize; /* Area topology database size */

   ulng       NSSATranslatorRole;
   ulng       NSSATranslatorState;
   ulng       TranslatorStabilityInterval;
   t_Handle   TranslatorStabilityTimer;
   Bool       ImportSummaries;
   ulng       DefaultCost;           /* metric for T3 LSA when nssa no-summary */
   Bool             NSSARedistribute;
   Bool             NSSADefInfoOrig;
   ulng             NSSADefMetric;
   e_NSSAMetricType NSSADefMetricType;

}   t_ARO;


e_Err ARO_Init(t_Handle IFO_Id, t_Handle *p_Object);
Bool ARO_Delete(t_Handle ARO_Id, ulng flag);
Bool FlushLsa(t_Handle db, ulng aroId, Bool noRtrIdChk);
Bool FlushAsLsa(t_Handle db, ulng rtoId);
e_DbInstComp LsaCompare(t_S_LsaHeader *lsa1, t_S_LsaHeader *lsa2);
e_Err LsaFlooding(t_ARO *p_ARO, t_NBO *p_SourceNBO, 
                  t_A_DbEntry *p_DbEntry, Bool *p_FloodedBack);
word FletcherCheckSum(byte *buffer, int len, int offset);
Bool DlyLsaDelete(t_Handle Dly, ulng prm);
void LsaParseDel(t_ARO *p_ARO, t_A_DbEntry *p_DbEntry);
e_Err UpdateAgeOfDbEntry (t_A_DbEntry *p_DbEntry);
L7_BOOL AdjInExchOrLoad(t_ARO *p_ARO, L7_uint32 nbrId);
e_Err RemoveFromRetxtLists(t_ARO *p_ARO, t_A_DbEntry *p_DbEntry);

/* NSSA Change */
e_Err TranslatorStabilityTimerExp(t_Handle Id, t_Handle TimerId, word Flag);
Bool CheckLsaEquivalent(t_A_DbEntry *p_DbEntry, t_A_DbEntry *p_AsExtEntry);
Bool NssaTranslatorElect(t_Handle ARO_Id, ulng flag);
Bool AsExtAndNssaRqFree(t_Handle rq, ulng tmp);
e_Err RefreshTranslatedLsa(t_Handle RTO_Id, t_A_DbEntry *p_DbEntry);

e_Err ospfAgingTick( t_Handle userId, t_Handle AgingObjId);

typedef e_Err (*f_Originate)(t_Handle Id, ulng prm);
extern f_Originate LsaReOriginate[];

#define LSA_FLOODING(p_RTO, p_ARO, p_NBO, p_DbEntry, p_FloodedBack) \
{                                                            \
   if((p_DbEntry->Lsa.LsType != S_AS_EXTERNAL_LSA) &&        \
      (p_DbEntry->Lsa.LsType != S_AS_OPAQUE_LSA))            \
      LsaFlooding(p_ARO, p_NBO, p_DbEntry, p_FloodedBack);   \
   else                                                      \
      AsExtLsaFlooding(p_RTO, p_NBO, p_DbEntry, p_FloodedBack);\
}


/*----------------------------------------------------------------
 *
 *             Routing Table Object (RTB)
 *
 *----------------------------------------------------------------*/

/* Size of circular buffer of SPF stats. One more than the number of
 * stats we want to display, since one slot is always zeroed for next
 * SPF. */
#define OSPF_SPF_STATS_NUM 21

typedef enum e_SpfReason
{
  R_ROUTER_LSA = 0x1,
  R_NETWORK_LSA = 0x2,
  R_SUMMARY_LSA = 0x4,
  R_ASBR_SUM_LSA = 0x8,
  R_EXT_LSA = 0x10
} e_SpfReason;

typedef struct t_SpfStats
{
  /* When the SPF ran. Number of milliseconds since boot. */
  L7_uint32   SpfTime;

  /* How long the SPF took. Milliseconds. */
  L7_uint32   SpfDuration;

  /* Reasons the SPF ran. */
  e_SpfReason SpfReason;

} t_SpfStats;

typedef enum e_PathCompStage
{
   RTB_STAGE_IDLE,
   RTB_STAGE_INTRA_1,
   RTB_STAGE_INTRA_2,
   RTB_STAGE_INTRA_BACKBONE1,
   RTB_STAGE_INTRA_BACKBONE2,
   RTB_STAGE_INTER,
   RTB_STAGE_TRANSIT,
   RTB_STAGE_ASEXT,
   RTB_STAGE_RT_RESET,
   RTB_STAGE_NT_RESET,
   RTB_STAGE_RT_CHECK,
   RTB_STAGE_NT_CHECK
}  e_PathCompStage;

typedef enum e_RtbUpdState
{
  RTB_UPD_DONE,
  RTB_UPD_START,
  RTB_UPD_NET_PEND,
  RTB_UPD_NET_FLUSH,
  RTB_UPD_RTR_PEND,
  RTB_UPD_RTR_FLUSH,
  RTB_UPD_LAST
} e_RtbUpdState;

typedef struct t_RTB
{
   struct t_RTB   *next;           /* pointer to the previous entry */
   struct t_RTB   *prev;           /* pointer to next entry */

   t_Name         Name;            /* 4 bytes of user-info */
   Bool           Taken;           /* object structure is being used */

   t_VpnCos       VpnCos;          /* entry VPN/COS identifier */

   /* Binary (AVL) trees of routes to routers (Rt) and routes to networks (Nt).
    * For each type of routes, we allocate two trees, one to maintain the
    * routes learned in the previous SPF run and one to maintain the routes
    * computed in the current SPF run. When an SPF completes, the routes just
    * computed are compared to the routes previously computed to determine
    * what OSPF needs to tell RTO about additions, deletions, and changes.
    * RtbRtBtOld and RtbNtBtOld are always empty except within RTB_EntireTableCalc().
    */
   t_Handle       RtbRtBt;         
   t_Handle       RtbNtBt;         
   t_Handle       RtbRtBtOld;
   t_Handle       RtbNtBtOld;

   t_Handle       RTO_Id;          /* parent RTO object handler */
   Bool           IsUsed;          /* RTB calculation service flag */
   e_RtChange     RtbChanged;      /* RTB table change service flag */
   t_Handle       RecalcTimer;     /* RTB recalculation timer (spf-delay) */
   word           CalcDelay;       /* Spf Calculation delay interval    */
   ulng           tsSpf;           /* Time stamp when previous spf calculation ended    */
   Bool           FirstCalc;       /* First time calculation flag   */

   t_Handle       LocGroupHl;      /* Local Group List */
   t_Handle       ForwCacheHl;     /* Forwarding cache */

   ulng           RtComputeTime;   /* Last Whole routing table computation time interval */
   ulng           SumIncComputeTime;   /* Last Summary LSA incremental computation time interval */
   ulng           AsExtIncComputeTime; /* Last AS-Ext LSA incremental computation time interval */

   e_RtbUpdState  RtbUpdateState;  /* post-SPF RTB to RTO update processing state */
   t_Handle       RtePendingUpdate;       /* next RTE to process for the current RTB update */   

   t_Handle        NextCompItem;   /* Next item to compute */
   t_Handle        AroId;          /* next ARO handler to compute */
   t_Handle        Backbone;       /* saved backbone ARO handler */
   byte            SumType;        /* Summary LSA type for inter calc */
   word            NtNum;          /* counter of network items in browse loop */
   word            RtNum;          /* counter of router items in browse loop */

   /* The number of times AsExtIncrementalCalc() has been called since
    * the last full AsExtCalc(). When this reaches a certain limit,
    * schedule a full calculation, in the hope that a full calculation for
    * a number of destinations is better than a large number of incrementals. */
   word            IncExtCalcNum;

   /* Similar to above for incremental summary calculations. */
   word            IncSumCalcNum;

   /* Index of spfStats where stats are written for the next SPF run. */
   word            SpfStatsIndex;

   /* Circular buffer of SPF statistics for "show ip ospf statistics." An entry 
    * is made each time the full SPF runs. An entry contains valid data if spfTime
    * is non-zero. Reasons are stored when the SPF is scheduled (before the SPF
    * runs. */
   t_SpfStats      SpfStats[OSPF_SPF_STATS_NUM];

   /* Number of times the SPF has been delayed waiting for the RTO update
    * from the previous SPF to complete. */
   L7_uint32       spfWaitForRto;

}  t_RTB;

typedef enum e_OspfPathType
{
   OSPF_INTRA_AREA,
   OSPF_INTER_AREA,
   OSPF_TYPE_1_EXT,
   OSPF_TYPE_2_EXT,
   OSPF_NSSA_TYPE_1_EXT,  /* NSSA Change */
   OSPF_NSSA_TYPE_2_EXT,  /* NSSA Change */ 
   OSPF_PATH_TYPE_ILG
}  e_OspfPathType;

typedef struct t_RoutingTableKey
{
   SP_IPADR DstAdr;
   SP_IPADR Prefix;
}  t_RoutingTableKey;

#define MAX_RTR_CHAIN_LEN  10

typedef struct t_RoutingTableEntry
{
   /* destination key = following two fields */
   SP_IPADR       DestinationId;   /* Router Id or network IP address */
   SP_IPADR       IpMask;          /* network IP mask */
   
   /* This field is a bit mask and may have multiple bits set. However, not
    * all combinations are possible. If the destination is a router, the 
    * router, ABR, and ASBR bits may be simultaneously set. If the destination
    * is a network, then either the network OR the stub network bit is set. The 
    * code uses the test (DestinationType < DEST_TYPE_IS_NETWORK) to determine
    * if the destination is a network or a router.  */
   e_OspfDestType DestinationType; 

   SP_IPADR       AreaId;          /* entry originator's Area */
   Bool           BackboneReachable; /* This entry is reachable from Backbone area */
   byte           OptionalCapabilities; /* ability to process AS-external LSAs */
   Bool pBitSet;     /* TRUE if T7 route and T7 LSA has p-bit set */

   e_OspfPathType PathType;        /* four possible types of path */
   ulng           Cost;            /* link cost */
   ulng           Type2Cost;       /* cost valid only for type 2 path */

   /* Despite the assumption to the contrary made by much of the code, this
    * field may be NULL. It is NULL whenever the LSA from which the route
    * was derived has been deleted or when a more recent LSA is received
    * with max age. */
   t_A_DbEntry    *p_DbEntry;       

   t_Handle       VrtlIfo;         /* for virtual links this is virtual IFO handler */
   Bool           DirAttachNet;    /* Root's Directly attached network flag */
   Bool           isRejectRoute;   /* If this entry corresponds to reject route */

   byte           PathNum;         /* MultiPath elements number */
   byte           ErNum;           /* Total ER number in all paths */
   t_OspfPathPrm  *LastUsedPath;   /* last used path for data loading balance algorithm */
   t_OspfPathPrm  *PathPrm;        /* MultiPaths list */

} t_RoutingTableEntry;

/* Stub Metric list element structure */
typedef struct t_StubMetricEntry
{
   struct t_StubMetricEntry *next;       /* pointer to the previous entry */
   struct t_StubMetricEntry *prev;       /* pointer to next entry */
   e_TosTypes            MetricTOS;      /* metric type */
   word                  Metric;         /* metric value */
   e_StubMetricType      StubMetricType; /* stub metric type */
   t_RoutingTableEntry   StubMetricRte;  /* routing information used */
                                         /*  for LsaOriginate call   */ 
   Bool                  IsValid;        /* metric is valid flag */
}  t_StubMetricEntry;


/* Aggregation List Entry structure */
typedef struct t_AgrEntry
{
   struct t_AgrEntry *next;  /* pointer to the previous entry */
   struct t_AgrEntry *prev;  /* pointer to next entry */

   SP_IPADR NetIpAdr;
   SP_IPADR SubnetMask;
   word     LsdbType;        /* The type of address range this entry 
                             ** represents: AGGREGATE_SUMMARY_LINK      
                             **             AGGREGATE_NSSA_EXTERNAL_LINK 
                             */
   e_A_RowStatus  AggregateStatus;  
   word  AggregateEffect;

   Bool  alive;         /* TRUE if at least one destination belongs */
                        /* to the aggregation; FALSE otherwise      */

   long  LargestCost;

   void  *LcRte;                       /* Largest cost contributing route (RFC 2328) */
   t_Handle AgrRteHl;
   t_RoutingTableEntry  AggregateRte;  /* routing information used */
                                       /*  for LsaOriginate call   */
}  t_AgrEntry;

e_Err RTB_Find(t_RTB *p_RTB, t_RoutingTableKey *rtbkey, t_RoutingTableEntry **p_Rte, Bool best_match);
t_S_LinkDscr *RtrLsaCheckPresense(t_S_RouterLsa *p_Lsa, SP_IPADR Id, SP_IPADR Mask);
Bool NetLsaCheckPresense(t_A_DbEntry* p_DbEntry, SP_IPADR Id);
e_Err RecalcTimerExp( t_Handle Id, t_Handle TimerId, word Flag );
Bool RteRelease(byte *rte, ulng prm);
Bool RteOfAreaDeleteAndNotify(byte *rte, ulng area);


Bool RteOfNeighborDeleteAndNotify(byte *rte, ulng neighbor);

void PathAdd(t_RoutingTableEntry  *Rte, t_IFO *p_IFO, SP_IPADR nextHop, ulng linkId);
Bool PathCompare(t_RoutingTableEntry  *Rte1, t_RoutingTableEntry *Rte2);
Bool CheckDestPath(t_RoutingTableEntry *p_Rte, t_Handle ifoid);
void NssaHandleRouteChange(t_RTB *p_RTB, t_RoutingTableEntry *p_Rte,
                           e_RtChange chType, t_AgrEntry *p_AdrRange);
e_Err RTB_AddSpfReason(t_RTB *p_RTB, e_SpfReason reason);

#define CheckVpnValues(VpnCos1, VpnCos2) \
   (((VpnCos2 == 0) || \
     ((VpnCos1 & 0x3FFFFFFFL) == (VpnCos2 & 0x3FFFFFFFL)) || \
     (!(VpnCos1 & 0x3FFFFFFFL) && (VpnCos2 & 0x3FFFFFFFL))) ? TRUE:FALSE)


/* Multicast stuff definitions */
typedef e_Err (*f_LocForward) (t_Handle Id, void *p_Packet, ulng prm);

typedef struct t_LocGroup
{
   struct t_LocGroup *next;        /* pointer to the previous entry */
   struct t_LocGroup *prev;        /* pointer to next entry */

   /* destination key = following three fields */
   SP_IPADR       McstGroupId;      /* Multicast group address */
   SP_IPADR       NetIdOrAppId;     /* Attached network IP address or AppHandler */
   SP_IPADR       NetMaskOrAppClbk; /* Attached network IP mask or F_LocForward */

   Bool           IsValid;          /* validation flag */
   Bool           IsLocalApp;

   t_Handle       RTB_Id;           /* Parent RTB object */   
   t_Handle       IGMP_Id;          /* Parent IGMP object */
   ulng           OrgTime;          /* origination tme for Entry's age calculation */
   byte           GroupState;       /* IGMP protocol state  */
   ulng           LeaveTimeout;     /* Leave timeout        */
   word           LastMemberQueryCount;
   ulng           V1HostTimeout;    /* V1 Host Timeout      */
   
} t_LocGroup;

typedef struct t_ForwCache
{
   struct t_ForwCache *next;        /* pointer to the previous entry */
   struct t_ForwCache *prev;        /* pointer to next entry */

   /* destination key = following three fields */
   SP_IPADR       McstGroupId;      /* Destination Multicast group address */
   SP_IPADR       NetIdOrAppId;     /* Source network IP address */
   SP_IPADR       NetMaskOrAppClbk; /* Source network IP mask */

   SP_IPADR       NetAreaId;        /* Source network Area Id */
   e_OspfPathType SrcType;          /* Source network is intra(0)/inter(1)/external(2,3) */
   t_A_DbEntry    *p_DbEntry;       /* Source network origin LSA */
   
   SP_IPADR       UpNode;           /* Upstream node: The attached network/
                                       neighboring router */
   SP_IPADR       UpNodeMask;
   t_Handle       DownStreamList;   /* Down stream interfaces/neighbors linked list */

   t_Handle       RTB_Id;           /* Parent RTB object */   
} t_ForwCache;

typedef struct t_McstKey
{
   SP_IPADR       McstGroupId;      /* Multicast group address */
   SP_IPADR       NetIdOrAppId;     /* Attached network IP address */
   SP_IPADR       NetMaskOrAppClbk; /* Attached network IP mask */
}  t_McstKey;


/* Multicast SPF tree vertex structure */

typedef enum e_McLinkType
{
   ILNone      ,/* the vertex is not on the shortest-path tree */
   ILExternal  ,/* OSPF AS external links */
   ILSummary   ,/* OSPF summary links */
   ILNormal    ,/* either router-to-router or router-to-network links*/
   ILDirect    ,/* vertex is directly attached to SourceNet*/
   ILVirtual    /* virtual links */
}  e_McLinkType;

typedef struct t_McVertex
{
   struct t_McVertex *next;        /* pointer to the previous entry */
   struct t_McVertex *prev;        /* pointer to next entry */

   /* destination key = following two fields */
   SP_IPADR       VertexId;        /* Router Id or network IP address */
   SP_IPADR       VertexMask;      /* network IP mask */
   t_VpnCos       VpnCos;          /* entry originator's VPN and COS */
   
   Bool           IsVpnMarked;     /* VPN/COS presense flag */
   byte           VertexType;      /* Set to 1 for routers, 2 for transit networks */
   ulng           Cost;            /* Vertex cost */
   t_A_DbEntry    *p_DbEntry;      /* destination directly LSA reference */    
   t_Handle       *p_Parent;       /* immediately preceding vertex in SPF */
   e_McLinkType   IncLinkType;     /* Vertex's incomming link type */
   word           Ttl;             /* number of routers between calculating router and vertex */
   t_Handle       AssIfoNbo;       /*  Associated Interface/Neighbor handler */

} t_McVertex;


/*----------------------------------------------------------------
 *
 *             Router Object  (RTO)
 *
 *----------------------------------------------------------------*/

typedef enum e_OSPF_TRACE_FLAGS
{
   OSPF_TRACE_HELLO = 0x1, 
   OSPF_TRACE_DD = 0x2,
   OSPF_TRACE_LS_REQ = 0x4,
   OSPF_TRACE_LS_UPDATE = 0x8,
   OSPF_TRACE_LS_ACK = 0x10,
   OSPF_TRACE_NBR_STATE = 0x20,
   OSPF_TRACE_SPF = 0x40,
   OSPF_TRACE_VIRT_LINK = 0x80,
   OSPF_TRACE_NSSA = 0x100,
   OSPF_TRACE_RX = 0x200,
   OSPF_TRACE_AGING = 0x800,
   OSPF_TRACE_SPF_DETAIL = 0x1000,   
   OSPF_TRACE_SPF_PREPARSE = 0x2000,
   OSPF_TRACE_REDIST = 0x4000,
   OSPF_TRACE_LSA_ORIG = 0x8000,
   OSPF_TRACE_DR_CALC = 0x10000
} e_OSPF_TRACE_FLAGS;

typedef enum e_OSPF_RES_CHECK
{
  /* Resources exist for new LSA */
  OSPF_RESOURCE_AVAIL = 0x0, 

  /* LSDB contains OSPF_MAX_LSAS and can't store a new LSA */
  OSPF_LSDB_FULL, 

  /* System has the max number of retx entries already in use. */
  OSPF_RETX_LIMIT

} e_OSPF_RES_CHECK;

/* Router Instance Object sturcture */

typedef struct t_RTO
{
   struct t_RTO *next;        /* pointer to the previous entry */
   struct t_RTO *prev;        /* pointer to next entry */
#if L7_IPMS
   Bool      isStub;         /* Real or Stub Object (don't change offset
                                without t_GeneralObject (ipms.h) changing)*/
   IPMS_OBJ_TYPE objType;    /* IPMS object type */
#endif
   t_SysLabel OspfSysLabel;  /* OSPF's thread handler (don't change offset
                                without t_GeneralObject (ipms.h) changing)*/
/* Don't change upper fields without t_GeneralObject (ipms.h) changing) */

   Bool      Taken;          /* object structure is being used */

   t_SysLabel OspfRtbThread; /* OSPF routing table process thread */
   t_Handle  MngId;          /* Upper Managment object handler */
   t_S_Callbacks Clbk;       /* Callback structure */
   t_S_RouterCfg Cfg;        /* The router configuration */

   Bool       RtbOwnThread; 

   t_S_TrapControl TrapControl; /* trap enable/disable bitmask */ 
   
   Bool      IsOverflowed;   /* AS-Ext LSAs number reaches the max */
   t_Handle  OverflowTimer;  /* Overflow state exit timer */

   /* Unlike t_S_RouterCfg.ExternLsaCount, this counter does not include
    * external LSAs to 0/0. Only counts T5 LSAs. Used to determine when
    * the router has reached external LSA overflow. */
   ulng      NonDefExtLsaCount;

   /* LsdbOverload is TRUE when the router is in overload state. The router
    * enters the overload state when the LSDB is full and the router failed
    * to store an LSA. The router's LSDB is therefore out of sync with the 
    * rest of the network. In this state, the router LSA always sets the 
    * cost of non-stub links to LSInfinity. See RFC 3137.  IsOverflowed 
    * above considers only external LSAs. */
   Bool      LsdbOverload;

   /* Number of LSAs in LSDB. */
   ulng      ActiveLsas;

   /* High water mark for number of LSAs in db (ActiveLsas) */
   ulng      lsaHighWater;

   /* Total number of entries on retransmit lists for all neighbors */
   ulng      retxEntries;
   ulng      retxHighWater;   /* high water mark for retxEntries */

   t_Handle  AsExtRoutesHl;  /* hash list of AS-external routes */
   t_Handle  AsExternalLsaHl;/* AS-external LSAs list */	 
   t_Handle  AsOpaqueLsaHl;  /* AS-Opaque-LSAs */

   /* Since AS scope LSAs don't belong to a specific area, we need a global
    * delayed LSA list and timer. */
   t_Handle DelayedLsa;    /* delayed LSAs queue handler */
   t_Handle DlyLsaTimer;   /* delayed LSAs timer handler */

   t_Handle  RtbHl;          /* Routing table objects hash list */

   /* IFO objects indexed by Layer 2 index hash list. Does NOT include 
    * virtual interfaces. */
   t_Handle  IfoIndexHl;    

   t_Handle  FaIndexHl;      /* FA objects hash list */

   t_Handle  AroHl;          /* Attached Area Objects hash list */
   ulng      AreasNum;       /* areas number counter */

   t_Handle  AgingObj;       /* Aging object - keeps track of all LSA age */

   t_Handle  TickTimer;      /* OSPF tick timer */

   t_Handle  TeLsaInstBS;    /* Bitset obj used for allocation TELsa instance*/

   /* When LSAs are flooded, we prefer to bundle many LSAs into a single
    * LS Update, rather than sending a single LSU per LSA. To accomplish this,
    * we define BundleLsas. A function that calls LsaFlooding() sets this to 
    * TRUE if it knows it will call LsaFlooding() for more than one LSA. 
    * When BundleLsas is TRUE, LsaFlooding() does not immediately send an
    * LS Update (unless it is full). Instead, it simply adds an LSA to an
    * LS Update for each interface where the LSA should be flooded. The caller
    * can trigger LS Updates to be sent by calling LsUpdatesSend(). Defaults to
    * FALSE. Only TRUE while a single function is flooding multiple LSAs. 
    * A function that sets BundleLsas to TRUE must reset it to FALSE by
    * calling LsUpdatesSend() after calling LsaFlooding for all the LSAs
    * it is currently processing. */
   Bool      BundleLsas;   

   /* bit mask to turn on OSPF debug tracing. Bits correspond to the 
    * e_OSPF_TRACE_FLAGS enum. */
   ulng      ospfTraceFlags;

   /* AS OPAQUE(Type-11) LSA checksum Sum */
   ulng      OpaqueASLsaCksumSum;


#if L7_MIB_OSPF
   t_Handle  ospfMibHandle;  /* corresponding MIB object handle, has to */
                             /* set by stack manager                    */
#endif
#if L7_IPMS
   t_Handle  ipmsHandle;                    /* IPMS handle */
   t_Handle  decodeFunc[NUM_DECODE_FUNC];   /* Decoding functions */
#endif
   L7_uint32 neighborsToRouter; /* Number of neighbor objects created for this router */

} t_RTO;

/* RTO object list  */
extern t_RTO        *RTO_List;    

L7_BOOL o2LsaTypeValid(byte lsaType);
t_Handle GET_OWNER_HL(t_ARO *p_ARO, e_S_LScodes type);

void LsaAgedSendTrap(t_RTO *p_RTO, t_ARO *p_ARO, t_A_DbEntry *p_DbEntry);
Bool LsaRelease(t_RTO *p_RTO, t_Handle db, ulng flag);
word PathCopy(t_RTO *p_RTO, t_RoutingTableEntry *p_Rte, t_RoutingTableEntry *p_ForwardRtr,
                SP_IPADR OrgAdr, SP_IPADR ForwAdr);

e_Err OverflowTimerExp( t_Handle Id, t_Handle TimerId, word Flag );
e_Err LsUpdatesSend(t_RTO *p_RTO);
e_Err ospfTraceFlagsSet(ulng traceFlags);
void ospfTraceFlagsSetAll();
void ospfTraceFlagsSetNone();
e_Err ospfTraceFlagsGet(ulng *traceFlags);
e_Err RTO_InterfaceName(t_IFO *p_IFO, char *ifName);
e_Err RTO_TraceWrite(char *inputStr);
void OspfOptionsStr(byte options, unsigned char *optStr);
Bool RTO_AddressIsLocallyConfigured(t_RTO *p_RTO, SP_IPADR *addr);
e_Err RTO_VerifyForwardingAddrs(t_RTO *p_RTO);
t_RetxEntry *RTO_RetxEntryAlloc(t_RTO *p_RTO);
e_OSPF_RES_CHECK RTO_LsaResourceCheck(t_RTO *p_RTO, Bool replaceFormer);

Bool routerIsBorder(t_RTO *p_RTO);
Bool CheckAggrCostUpdate(t_RTO *p_RTO, long rteCost, long aggrCost);
e_Err AddRouteToT7Range(t_RTO *p_RTO, t_AgrEntry *p_AggrEntry, 
                        t_RoutingTableEntry *p_Rte);
e_Err RemoveRouteFromT7Range(t_RTO *p_RTO, t_AgrEntry *p_AggrEntry, 
                             t_RoutingTableEntry *p_Rte);
word LsRefreshTimeGet(t_RTO *p_RTO);
ulng RTO_MaxLsaRetxEntriesGet(t_RTO *p_RTO);
Bool HasAsFloodingScope(e_S_LScodes lsaType);
t_A_DbEntry *LsaInstall(t_RTO *p_RTO, t_ARO *p_ARO, void *p_Lsa, t_A_DbEntry *p_former);
e_Err UpdateStatCounters(t_RTO *p_RTO, t_ARO *p_ARO, t_A_DbEntry *p_DbEntry,
                         Bool EntryIsAdded);
e_Err FindSummaryOrASExtLsa(t_RTO *p_RTO, t_ARO *p_ARO, e_S_LScodes LsType,
                           SP_IPADR DestAddr, SP_IPADR NetMask,
                           t_A_DbEntry **p_foundLsa, SP_IPADR *p_LsId);

e_Err AsExtLsaFlooding(t_RTO *p_RTO, t_NBO *p_SourceNBO, 
                  t_A_DbEntry *p_DbEntry, Bool *p_FloodedBack);
e_Err LsaRefresh (t_RTO *p_RTO, t_ARO *p_ARO, t_A_DbEntry *p_DbEntry);

#if L7_OSPF_TE
e_Err TE_RouterInfoUpdate(t_Handle RTO_Id, t_Handle ARO_Id);
void TE_SupportIsStarted(t_ARO *p_ARO);
void TE_SupportIsStopped(t_ARO *p_ARO);
t_RoutingTableEntry *TE_GetNextHop(t_RTB *p_RTB, t_NextHopParam *p_NextHopParam);
Bool TE_FaCleanup(t_Handle FaId, ulng prm);

/* TE Link LSA database entry structure */                 
typedef struct t_TeLink
{
   byte RouterId[4];
   byte LinkId[4];
   t_A_DbEntry *p_DbEntry;
}  t_TeLink;

/* TE Router Opaque LSA database entry structure */                 
typedef struct t_TeRtr
{
   byte RouterId[4];
   t_A_DbEntry *p_DbEntry;
   
}  t_TeRtr;

/* TE Preparsing entry structure */
typedef struct t_TeParseEntry
{
   struct t_TeParseEntry *next;  /* pointer to the previous entry */
   struct t_TeParseEntry *prev;  /* pointer to next entry */
   
   t_TeLink *Link;
   ulng metric;

}  t_TeParseEntry;

/* TE FA Link entry structure */                 
typedef struct t_FA
{
   struct t_FA    *next;     /* pointer to the previous entry */
   struct t_FA    *prev;     /* pointer to next entry */

   t_Name         Name;      /* 4 bytes of user-info */
   Bool           Taken;     /* object structure is being used */
    
   t_LIH          FaIndex;
   t_Handle       ARO_Id;    /* ARO Object Id     */
   t_Handle       RTO_Id;    /* Router Object Id  */
   t_ForwAdjPrm   faPrm;     /* FA parameters     */

   word           TeLsaInst;  /* Instance of TE LSA  */ 

}  t_FA;
                 
#endif

#endif /* spobj_h */

/* --- end of file spobj.h --- */

