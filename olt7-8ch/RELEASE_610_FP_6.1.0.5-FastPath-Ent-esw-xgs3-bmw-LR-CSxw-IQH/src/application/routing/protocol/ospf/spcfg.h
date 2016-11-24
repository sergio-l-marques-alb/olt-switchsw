/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename         spcfg.h
 *
 * @purpose          OSPF Configuration structures, variables and
 *                   defaults definitions
 *
 * @component        Routing OSPF Component
 *
 * @comments
 *
 *   File contents:
 *
 * General Variables
 * Area Data Structure
 * Area Stub Metric Table
 * Link State Database
 * Address Range Table
 * Host Table
 * Interface Table
 * Interface Metric Table
 * Virtual Interface Table
 * Neighbor Table
 * Virtual Neighbor Table
 * External Link State Database
 * Aggregate Range Table
 * Traps
 *
 *
 *
 * @create           04/24/2002
 *
 * @author
 *
 * @end
 *
 * ********************************************************************/
#ifndef spcfg_h
#define spcfg_h
#include "iptypes.h"



/*--------------------------------------------------------------------**
**                                                                    **
**                OSPF Architectural Constants and Variables          **
**                                                                    **
**--------------------------------------------------------------------*/

	/* The maximum time between distinct originations of any particular
	LSA.  If the LS	age field of one of the	router's self-originated
	LSAs reaches the value LSRefreshTime, a	new instance of	the LSA
	is originated, even though the contents	of the LSA (apart from
	the LSA	header)	will be	the same.  The value of	LSRefreshTime is
	set to 30 minutes. */

#define LSRefreshTime  (30*60)

	/* The minimum time between distinct originations of any particular
	LSA.  The value	of MinLSInterval is set	to 5 seconds. */

#define MinLSInterval 5

	/* For any	particular LSA,	the minimum time that must elapse
	between	reception of new LSA instances during flooding.	LSA
	instances received at higher frequencies are discarded.	The
	value of MinLSArrival is set to	1 second. */

#define MinLSArrival 1

	/* The maximum age	that an	LSA can	attain.	When an	LSA's LS age
	field reaches MaxAge, it is reflooded in an attempt to flush the
	LSA from the routing domain (See Section 14). LSAs of age MaxAge
	are not	used in	the routing table calculation.	The value of
	MaxAge is set to 1 hour. */

#define MaxAge (60*60)

	/* When the age of	an LSA in the link state database hits a
	multiple of CheckAge, the LSA's	checksum is verified.  An
	incorrect checksum at this time	indicates a serious error.  The
	value of CheckAge is set to 5 minutes. */

#define CheckAge (5*60)

	/* The maximum time dispersion that can occur, as an LSA is flooded
	throughout the AS.  Most of this time is accounted for by the
	LSAs sitting on	router output queues (and therefore not	aging)
	during the flooding process.  The value	of MaxAgeDiff is set to
	15 minutes. */

#define MaxAgeDiff (15*60)

	/* The metric value indicating that the destination described by an
	LSA is unreachable. Used in summary-LSAs and AS-external-LSAs as
	an alternative to premature aging (see Section 14.1). It is
	defined	to be the 24-bit binary	value of all ones: 0xffffff. */

#define LSInfinity 0xffffff

    /* Infinite link cost for links advertised in an LSA */
#define OspfMaxLinkCost 0xffff

	/* The Destination	ID that	indicates the default route.  This route
	is used	when no	other matching routing table entry can be found.
	The default destination	can only be advertised in AS-external-
	LSAs and in stub areas'	type 3 summary-LSAs.  Its value	is the
	IP address 0.0.0.0. Its	associated Network Mask	is also	always
	0.0.0.0. */

#define DefaultDestination 0L
#define AllSpfRouters      ((ulng)((224L << 24)+5))
#define AllDRouters        ((ulng)((224L << 24)+6))
#define OSPF_BACKBONE_AREA_ID 0L

#define OSPF_MAX_IP_MTU      1500

#define OSPF_MAX_ROUTER_LSA_LEN   8000
#define OSPF_MAX_NETWORK_LSA_LEN  8000

/* maximum size of an lsa buffer is set to 8000 */
#define OSPF_MAX_LSA_BUF_LEN      8000

/* max length of individual LSA that we will accept */
#define OSPF_HEADER_LEN 24
#define OSPF_UPDATE_LEN  4
#define OSPF_MAX_LSA_LEN    (OSPF_MAX_LSA_BUF_LEN - (OSPF_MD5_AUTH_DATA_LEN + \
                                                    OSPF_HEADER_LEN + OSPF_UPDATE_LEN))

/* maximum size of the ospf hello packet sent out
   This will allow upto 359 neighbors on an interface */
#define OSPF_MAX_HELLO_PKT_SIZE   1500

/* maximum size of an ospf data buffer is set to 8000 */
#define OSPF_MAX_BUF_SIZE    8000
#define OSPF_MIN_BUF_SIZE     128

/* The minimum length in bytes of an IP packet that can carry a useful
 * non-initial DD packet. 
 *   IP header - 20 bytes
 *   OSPF header - 24 bytes
 *   DD if mtu, Options, flags, seqno - 8 bytes
 *   One LSA header - 20 bytes 
 */
#define OSPF_MIN_DD_LEN 72

/* Allow db size to be 3 * max routes plus a few miscellaneous router and 
   network LSAs */
#define OSPF_MAX_LSAS		((3*L7_L3_ROUTE_TBL_SIZE_TOTAL)+200)
#define OSPF_MAX_NBRS           400
#define OSPF_MAX_NBRS_ON_IFACE  128

/* Maximum number of retransmit list entries. Enforced to protect system from
 * running out of memory. At this time, each retx entry takes 56 bytes (including
 * 20 bytes of chunk overhead). With 24,000 LSAs and even 50 neighbors, we could 
 * use up to 67 Mbytes of memory for retx list entries! With a 30 Mbyte routing 
 * heap, we don't want to use more than about 7 MB for retx lists. That is enough
 * memory for 125,000 retx list entries. We'll be a little conservative and 
 * drop LSAs at 100,000 retx list entries. We may flood locally-originated LSAs
 * even if the number of retransmit entries exceeds this MAX. The max is mainly
 * used to drop LSAs received from neighbors. Compute as 4 * max LSAs. So we 
 * can flood all LSAs to four neighbors simultaneously. */
#define OSPF_MAX_RETX_ENTRIES  (4 * OSPF_MAX_LSAS)

#define OSPF_MAX_ROUTES  L7_L3_ROUTE_TBL_SIZE_TOTAL


#define LOW_LAYER_HEADERS_SIZE (14+20)
#define OSPF_DATA_BUF_NUM    200

#define OSPF_IP_PROTOCOL    89
#define OSPF_VERSION        2

#define OSPF_PUBLIC_VPN_ID  0L

/* Maximum number of incremental routing table calculations for external
 * destinations before scheduling a full recalc. */
#define OSPF_MAX_EXT_INCREMENTALS 10

#define OSPF_MAX_SUM_INCREMENTALS 10

#define OSPF_MAX_RTB_UPDATE_MSECS 2000

/* Maximum number of neighbors simultaneously in Exchange or Loading 
 * state for a given area. */
#define OSPF_MAX_NBRS_EXCH_LOAD 5

	/* The value used for LS Sequence Number when originating the first
	instance of any	LSA. Its value is the signed 32-bit integer
	0x80000001. */

#define InitialSequenceNumber 0x80000001

	/* The maximum value that LS Sequence Number can attain.  Its value
	is the signed 32-bit integer 0x7fffffff. */

#define MaxSequenceNumber 0x7fffffff

#define SPF_HOST_ROUTE 0xffffffff

typedef enum e_TosTypes
{
   TOS_NORMAL_SERVICE        = 0,
   TOS_MINIMIZE_COST         = 2,
   TOS_MAXIMIZE_RELIABILITY  = 4,
   TOS_MAXIMIZE_THROUGHPUT   = 8,
   TOS_MINIMIZE_DELAY        = 16
}  e_TosTypes;

/* Combined VPN and COS structure */
typedef ulng t_VpnCos;

typedef enum e_OspfPhyType
{
   OSPF_ETHERNET    = 0,
   OSPF_FRAME_RELAY = 1,
   OSPF_ATM         = 2
}  e_OspfPhyType;

/* Indices for OSPF processing task message queues. Low priority incoming packets
 * go in the data queue. Hellos and ACKs go in a separate queues. Events 
 * (timer events, config events) go in the event queue. */
typedef enum e_OspfQueueType
{
  OSPF_EVENT_QUEUE  = 0,
  OSPF_DATA_QUEUE,
  OSPF_ACK_QUEUE,
  OSPF_HELLO_QUEUE,
  OSPF_NUM_QUEUES
} e_OspfQueueType;

/*-----------------------------------------------------**
**                                                     **
**                    OSPF Defaults                    **
**                                                     **
**-----------------------------------------------------*/

#define HELLO_INTERVAL_DFLT            10    /* sec */
#define ROUTER_DEAD_INTERVAL_DFLT      40    /* sec */
#define POLL_INTERVAL_DFLT             120   /* sec */
#define INF_TRANS_DELAY_DFLT           1     /* sec */
#define RXMT_INTERVAL_DFLT             5     /* sec */
#define DEF_AGING_INTERVAL             1     /* sec */
#define DEF_EXTERNAL_ROUTING_CAPABILITY  1
#define SILENCE_INTERVAL_DFLT            5   /* sec */
#define HOLDTIME_INTERVAL_DFLT          10   /* sec */
#define RTO_UPDATE_BACKOFF               1   /* sec */
#define ITEMS_IN_TURN_DEF              100
#define DEFAULT_ROUTE_METRIC           10
#define DEF_OSPF_TICK_INTERVAL         1     /* sec */

/* Best if this matches FD_OSPF_MAXIMUM_PATHS */
#define DEFAULT_MAX_PATHS               4   

/*-----------------------------------------------------**
**                                                     **
**              Router Data Structure                  **
**                                                     **
--      These parameters apply globally to the Router's
--      OSPF Process.
**                                                     **
**-----------------------------------------------------*/

#define INTRA_AREA_MULTICAST 1
#define INTER_AREA_MULTICAST 2
#define INTER_AS_MULTICAST   4


typedef struct t_S_RouterCfg
{
/*     ospfRouterId OBJECT-TYPE                               */
/*         ACCESS   read-write                                */
/*         DESCRIPTION                                        */
/*            "A  32-bit  integer  uniquely  identifying  the */
/*            router in the Autonomous System.                */
/*                                                            */
/*            By  convention,  to  ensure  uniqueness,   this */
/*            should  default  to  the  value  of  one of the */
/*            router's IP interface addresses."               */
	SP_IPADR RouterId;

/*     ospfAdminStat OBJECT-TYPE                              */
/*         ACCESS   read-write                                */
/*         DESCRIPTION                                        */
/*            "The  administrative  status  of  OSPF  in  the */
/*            router.   The  value 'enabled' denotes that the */
/*            OSPF Process is active on at least  one  inter- */
/*            face;  'disabled'  disables  it  on  all inter- */
/*            faces."                                         */
   Bool  AdminStat;

/*     ospfVersionNumber OBJECT-TYPE                         */
/*         ACCESS   read-only                                */
/*         DESCRIPTION                                       */
/*           "The current version number of the OSPF  proto- */
/*            col is 2."                                     */
   word  VersionNumber;

/*     ospfAreaBdrRtrStatus OBJECT-TYPE                       */
/*         ACCESS   read-only                                 */
/*         DESCRIPTION                                        */
/*            "A flag to note whether this router is an  area */
/*            border router."                                 */
   Bool  AreaBdrRtrStatus;

/*     ospfASBdrRtrStatus OBJECT-TYPE                         */
/*         ACCESS   read-write                                */
/*         DESCRIPTION                                        */
/*            "A flag to note whether this router is  config- */
/*            ured as an Autonomous System border router."    */
   Bool  ASBdrRtrStatus;

/*     "The number of opaque AS (LS type 11)                  */
/*     link-state advertisments"                              */
   ulng  OpaqueASLsaCount;
   
/*     ospfExternLsaCount OBJECT-TYPE                         */
/*         ACCESS   read-only                                 */
/*         DESCRIPTION                                        */
/*            "The number of external (LS type 5)  link-state */
/*            advertisements in the link-state database."     */
/*            This does not include type 11 opaque LSAs. It   */
/*            does include T5 LSAs with the default destination */
   ulng  ExternLsaCount;

/*     "The number of self originated external (LS type 5)    */
/*     link-state advertisments"                              */
   ulng  SelfOrigExternLsaCount;
   
/*     ospfExternLsaCksumSum OBJECT-TYPE                      */
/*         ACCESS   read-only                                 */
/*         DESCRIPTION                                        */
/*            "The 32-bit unsigned sum of the LS checksums of */
/*            the  external  link-state  advertisements  con- */
/*            tained in the link-state  database.   This  sum */
/*            can  be  used  to determine if there has been a */
/*            change in a router's link state  database,  and */
/*            to  compare  the  link-state  database  of  two */
/*            routers."                                       */
   ulng  ExternLsaCksumSum;

/*     ospfTOSSupport OBJECT-TYPE                             */
/*         ACCESS   read-write                                */
/*         DESCRIPTION                                        */
/*            "The router's support for type-of-service rout- */
/*            ing."                                           */
   Bool  TOSSupport;

/*     ospfOriginateNewLsas OBJECT-TYPE                       */
/*         ACCESS   read-only                                 */
/*         DESCRIPTION                                        */
/*            "The number of  new  link-state  advertisements */
/*            that  have been originated.  This number is in- */
/*            cremented each time the router originates a new */
/*            LSA."                                           */
   ulng  OriginateNewLsas;

/*     ospfRxNewLsas OBJECT-TYPE                              */
/*         ACCESS   read-only                                 */
/*         DESCRIPTION                                        */
/*            "The number of  link-state  advertisements  re- */
/*            ceived  determined  to  be  new instantiations. */
/*            This number does not include  newer  instantia- */
/*            tions  of self-originated link-state advertise- */
/*            ments."                                         */
   ulng  RxNewLsas;

/*     ospfExtLsdbLimit OBJECT-TYPE                           */
/*         ACCESS   read-write                                */
/*         DESCRIPTION                                        */
/*            "The  maximum   number   of   non-default   AS- */
/*            external-LSAs entries that can be stored in the */
/*            link-state database.  If the value is -1,  then */
/*            there is no limit.                              */
/*                                                            */
/*            When the number of non-default AS-external-LSAs */
/*            in   a  router's  link-state  database  reaches */
/*            ospfExtLsdbLimit, the router  enters  Overflow- */
/*            State.   The   router  never  holds  more  than */
/*            ospfExtLsdbLimit  non-default  AS-external-LSAs */
/*            in  its  database. OspfExtLsdbLimit MUST be set */
/*            identically in all routers attached to the OSPF */
/*            backbone  and/or  any regular OSPF area. (i.e., */
/*            OSPF stub areas and NSSAs are excluded)."       */
/*        DEFVAL { -1 }                                       */
   long  ExtLsdbLimit;

/* The maximum number of next hops that OSPF can report for   */
/* a single destination. If 1, ECMP is effectively disabled.  */
   ulng MaximumPaths;

/*     ospfMulticastExtensions OBJECT-TYPE                    */
/*         ACCESS   read-write                                */
/*         DESCRIPTION                                        */
/*            "A Bit Mask indicating whether  the  router  is */
/*            forwarding  IP  multicast  (Class  D) datagrams */
/*            based on the algorithms defined in  the  Multi- */
/*            cast Extensions to OSPF.                        */
/*                                                            */
/*            Bit 0, if set, indicates that  the  router  can */
/*            forward  IP multicast datagrams in the router's */
/*            directly attached areas (called intra-area mul- */
/*            ticast routing).                                */
/*                                                            */
/*            Bit 1, if set, indicates that  the  router  can */
/*            forward  IP  multicast  datagrams  between OSPF */
/*            areas (called inter-area multicast routing).    */
/*                                                            */
/*            Bit 2, if set, indicates that  the  router  can */
/*            forward  IP  multicast  datagrams between Auto- */
/*            nomous Systems (called inter-AS multicast rout- */
/*            ing).                                           */
/*                                                            */
/*            Only certain combinations of bit  settings  are */
/*            allowed,  namely: 0 (no multicast forwarding is */
/*            enabled), 1 (intra-area multicasting  only),  3 */
/*            (intra-area  and  inter-area  multicasting),  5 */
/*            (intra-area and inter-AS  multicasting)  and  7 */
/*            (multicasting  everywhere). By default, no mul- */
/*            ticast forwarding is enabled."                  */
/*        DEFVAL { 0 }                                        */
   ulng  MulticastExtensions;

/*-------------------------------------------------------------------------**
**     The application joins a multicast group on an interface-independent **
**     basis.                                                              **
**     1 - interface independed fashion                                    **
**     0 - application joines on some interface                            **
**     DEFVAL(1)                                                           **
**-------------------------------------------------------------------------*/
   Bool  GroupJoinFashion;

/*    Indicates whether the router is running the           */
/*    Opaque option (i.e., capable of storing Opaque LSAs). */
   Bool  OpaqueCapability;

/*     ospfExitOverflowInterval OBJECT-TYPE                   */
/*         SYNTAX   PositiveInteger                           */
/*         ACCESS   read-write                                */
/*         DESCRIPTION                                        */
/*            "The number of  seconds  that,  after  entering */
/*            OverflowState,  a  router will attempt to leave */
/*            OverflowState. This allows the router to  again */
/*            originate  non-default  AS-external-LSAs.  When */
/*            set to 0, the router will not  leave  Overflow- */
/*            State until restarted."                         */
/*        DEFVAL { 0 }                                        */
   ulng  ExitOverflowInterval;

/*     ospfDemandExtensions OBJECT-TYPE                  */
/*         ACCESS   read-write                           */
/*         DESCRIPTION                                   */
/*            "The router's support for demand routing." */
   Bool  DemandExtensions;

 /* Controls the preference rules used in Section 16.4 when
 choosing among multiple AS-external-LSAs advertising the
 same destination. When set to "enabled", the preference
 rules remain those specified by RFC	1583 ([Ref9]). When set
 to "disabled", the preference rules	are those stated in
 Section 16.4.1, which prevent routing loops	when AS-
 external-LSAs for the same destination have	been originated
 from different areas. Set to "enabled" by default. */

   Bool  RFC1583Compatibility;

/*     SilenceInterval                                        */
/*         SYNTAX   PositiveInteger                           */
/*         ACCESS   read-write                                */
/*         DESCRIPTION                                        */
/*            "The number of seconds  between two subsequend  */
/*             changes of LSAs, during that the Routing Table */
/*             calculation is delayed                         */
/*        DEFVAL { 5 }                                        */
   word  SilenceInterval;

/*     Holdtime                                               */
/*         SYNTAX   PositiveInteger                           */
/*         ACCESS   read-write                                */
/*         DESCRIPTION                                        */
/*            "The number of seconds  between two             */
/*             consecutinve spf calculation."                 */
/*        DEFVAL { 10 }                                        */
   word  Holdtime;

/*     StartUpSilenceInterval                                 */
/*         SYNTAX   PositiveInteger                           */
/*         ACCESS   read-write                                */
/*         DESCRIPTION                                        */
/*            "The number of seconds starting from Router     */
/*             initialization during that the Routing Table   */
/*             calculation is delayed. Recomended value is    */
/*             ROUTER_DEAD_INTERVAL."                         */
/*        DEFVAL { 40}                                        */
   word  StartUpSilenceInterval;

/*     StagedPathComp                                         */
/*         SYNTAX   Boolean                                   */
/*         ACCESS   read-write                                */
/*         DESCRIPTION                                        */
/*            "Routing table path computation in stages."     */
/*        DEFVAL {TRUE}                                       */
   Bool StagedPathComp;

/*     ItemsNumInTurn                                         */
/*         SYNTAX   PositiveInteger                           */
/*         ACCESS   read-write                                */
/*         DESCRIPTION                                        */
/*            "number of items (topology database entries or  */
/*             routing table entires) processed in one turn." */
/*        DEFVAL {100}                                        */
   word ItemsNumInTurn;

/*     DelAreaOnLastIfDel                                     */
/*         SYNTAX   Boolean                                   */
/*         ACCESS   read-write                                */
/*         DESCRIPTION                                        */
/*            "Delete Area Object if last interface in this   */
/*             area is deleted."                              */
/*        DEFVAL {FALSE}                                      */
   Bool DelAreaOnLastIfDel;

   /* Defaults to 30 minutes. Only changed for testing/debugging. */
   word LsRefreshTime;

   /* Max number of entries on all neighbor LSA retransmission lists. */
   ulng MaxLsaRetxEntries;

/*     RouterStatus OBJECT-TYPE                               */
/*         ACCESS   read-create                               */
/*         DESCRIPTION                                        */
/*            "This variable displays the status of  the  en- */
/*            try.  Setting it to 'invalid' has the effect of */
/*            rendering it inoperative.  The internal  effect */
/*            (row removal) is implementation dependent."     */
   e_A_RowStatus  RouterStatus;

}  t_S_RouterCfg;


/*-----------------------------------------------------**
**                                                     **
**                Area Data Structure                  **
**                                                     **
**-----------------------------------------------------*/
typedef enum e_AreaImportExternalCfg
{
   AREA_IMPORT_EXTERNAL    = 1,
   AREA_IMPORT_NO_EXTERNAL = 2,
   AREA_IMPORT_NSSA        = 3
}  e_AreaImportExternalCfg;

typedef enum e_AreaSummaryCfg
{
   AREA_NO_SUMMARY    = 1,
   AREA_SEND_SUMMARY  = 2
}  e_AreaSummaryCfg;

typedef struct t_S_AreaCfg
{
/*     ospfAreaId OBJECT-TYPE                                 */
/*         ACCESS   read-only                                 */
/*         DESCRIPTION                                        */
/*            "A 32-bit integer uniquely identifying an area. */
/*             Area ID 0.0.0.0 is used for the OSPF backbone. */
/*             If the	area represents a subnetted network,  */
/*             the IP network number of the subnetted network */
/*             may be used for the Area ID.                   */
   SP_IPADR      AreaId;

	/*  Whether AS-external-LSAs will be flooded into/throughout the
	    area.  If AS-external-LSAs are excluded from the area, the
	    area is called a "stub".  Internal to stub areas, routing to
	    external destinations will be based	solely on a default
	    summary route.  The	backbone cannot	be configured as a stub
	    area.  Also, virtual links cannot be configured through stub
	    areas.  For	more information, see Section 3.6. */

/*     ospfImportAsExtern OBJECT-TYPE                         */
/*         SYNTAX   INTEGER    {                              */
/*                     importExternal (1),                    */
/*                     importNoExternal (2),                  */
/*                     importNssa (3)                         */
/*                   }                                        */
/*         ACCESS   read-create                               */
/*         DESCRIPTION                                        */
/*            "The area's support for importing  AS  external */
/*            link- state advertisements."                    */
/*       DEFVAL { importExternal }                            */
	e_AreaImportExternalCfg    ExternalRoutingCapability;

/*     ospfSpfRuns OBJECT-TYPE                                */
/*         ACCESS   read-only                                 */
/*         DESCRIPTION                                        */
/*            "The number of times that the intra-area  route */
/*            table  has  been  calculated  using this area's */
/*            link-state database.  This  is  typically  done */
/*            using Dijkstra's algorithm."                    */
   ulng  SpfRuns;

/*     ospfAreaBdrRtrCount OBJECT-TYPE                        */
/*         ACCESS   read-only                                 */
/*         DESCRIPTION                                        */
/*            "The total number of area border routers reach- */
/*            able within this area.  This is initially zero, */
/*            and is calculated in each SPF Pass."            */
   ulng  AreaBdrRtrCount;

/*     ospfAsBdrRtrCount OBJECT-TYPE                          */
/*         ACCESS   read-only                                 */
/*         DESCRIPTION                                        */
/*            "The total number of Autonomous  System  border */
/*            routers  reachable  within  this area.  This is */
/*            initially zero, and is calculated in  each  SPF */
/*            Pass."                                          */
   ulng  AsBdrRtrCount;

/*     ospfAreaLsaCount OBJECT-TYPE                           */
/*         ACCESS   read-only                                 */
/*         DESCRIPTION                                        */
/*            "The total number of link-state  advertisements */
/*            in  this  area's link-state database, excluding */
/*            AS External LSA's."                             */
   ulng  AreaLsaCount;

/*     ospfAreaLsaCksumSum OBJECT-TYPE                        */
/*         ACCESS   read-only                                 */
/*         DESCRIPTION                                        */
/*            "The 32-bit unsigned sum of the link-state  ad- */
/*            vertisements'  LS  checksums  contained in this */
/*            area's link-state database.  This sum  excludes */
/*            external (LS type 5) link-state advertisements. */
/*            The sum can be used to determine if  there  has */
/*            been  a  change  in a router's link state data- */
/*            base, and to compare the link-state database of */
/*            two routers."                                   */
/*        DEFVAL   { 0 }                                      */
   ulng  AreaLsaCksumSum;

/*     ospfAreaSummary OBJECT-TYPE                            */
/*         SYNTAX   INTEGER    {                              */
/*                     noAreaSummary (1),                     */
/*                     sendAreaSummary (2)                    */
/*                   }                                        */
/*         ACCESS   read-create                               */
/*         DESCRIPTION                                        */
/*            "The variable ospfAreaSummary controls the  im- */
/*            port  of  summary LSAs into stub areas.  It has */
/*            no effect on other areas.                       */
/*                                                            */
/*            If it is noAreaSummary, the router will neither */
/*            originate  nor  propagate summary LSAs into the */
/*            stub area.  It will rely entirely  on  its  de- */
/*            fault route.                                    */
/*                                                            */
/*            If it is sendAreaSummary, the router will  both */
/*            summarize and propagate summary LSAs."          */
/*        DEFVAL   { noAreaSummary }                          */
   e_AreaSummaryCfg          AreaSummary;

   /* Defines if Traffic engineering extension is supported for this area */
   Bool TEsupport;

/*     ospfAreaStatus OBJECT-TYPE                             */
/*         ACCESS   read-create                               */
/*         DESCRIPTION                                        */
/*            "This variable displays the status of  the  en- */
/*            try.  Setting it to 'invalid' has the effect of */
/*            rendering it inoperative.  The internal  effect */
/*            (row removal) is implementation dependent."     */
   e_A_RowStatus  AreaStatus;

}  t_S_AreaCfg;


/*-----------------------------------------------------------------**
**                 Area Stub Metric Table                          **
**                                                                 **
**     If the area	has been configured as a stub area, and	the **
**     router itself is an	area border router, then the            **
**     StubDefaultCost indicates the cost of the default summary-  **
**     LSA	that the router	should advertise into the area.      **
**-----------------------------------------------------------------*/

typedef enum e_StubMetricType
{
   STUB_OSPF_METRIC      = 1,
   STUB_COMPARABLE_COST  = 2,
   STUB_NON_COMPARABLE   = 3
}  e_StubMetricType;

typedef struct t_S_StubAreaEntry
{
/*     ospfStubAreaId OBJECT-TYPE                             */
/*         ACCESS   read-only                                 */
/*         DESCRIPTION                                        */
/*            "The 32 bit identifier for the Stub  Area.   On */
/*            creation,  this  can  be  derived  from the in- */
/*            stance."                                        */
   SP_IPADR      AreaId;

/*     ospfStubTOS OBJECT-TYPE                                */
/*         ACCESS   read-only                                 */
/*         DESCRIPTION                                        */
/*            "The  Type  of  Service  associated  with   the */
/*            metric.   On creation, this can be derived from */
/*            the instance."                                  */
   e_TosTypes  StubTOS;

/*     ospfStubMetric OBJECT-TYPE                             */
/*         ACCESS   read-create                               */
/*         DESCRIPTION                                        */
/*            "The metric value applied at the indicated type */
/*            of  service.  By default, this equals the least */
/*            metric at the type of service among the  inter- */
/*            faces to other areas."                          */
   ulng  StubMetric;

/*     ospfStubStatus OBJECT-TYPE                             */
/*         ACCESS   read-create                               */
/*         DESCRIPTION                                        */
/*            "This variable displays the status of  the  en- */
/*            try.  Setting it to 'invalid' has the effect of */
/*            rendering it inoperative.  The internal  effect */
/*            (row removal) is implementation dependent."     */
   e_A_RowStatus  StubStatus;

/*     ospfStubMetricType OBJECT-TYPE                                */
/*         SYNTAX   INTEGER    {                                     */
/*                     ospfMetric (1),                -- OSPF Metric */
/*                     comparableCost (2),        -- external type 1 */
/*                     nonComparable  (3)        -- external type 2  */
/*                   }                                               */
/*         ACCESS   read-create                                      */
/*         DESCRIPTION                                               */
/*            "This variable displays the type of metric  ad-        */
/*            vertised as a default route."                          */
/*        DEFVAL   { ospfMetric }                                    */
   e_StubMetricType  StubMetricType;

}  t_S_StubAreaEntry;

/*-----------------------------------------------------**
**                                                     **
**                Link State Database                  **
**                                                     **
**-----------------------------------------------------*/

typedef struct t_S_LsDbEntry
{

/*     ospfStubAreaId OBJECT-TYPE                             */
/*         ACCESS   read-only                                 */
/*         DESCRIPTION                                        */
/*            "The 32 bit identifier for the Stub  Area.   On */
/*            creation,  this  can  be  derived  from the in- */
/*            stance."                                        */
   SP_IPADR      AreaId;

/*     ospfLsdbType OBJECT-TYPE                                        */
/*         SYNTAX   INTEGER    {                                       */
/*                     routerLink (1),                                 */
/*                     networkLink (2),                                */
/*                     summaryLink (3),                                */
/*                     asSummaryLink (4),                              */
/*                     asExternalLink (5),                             */
/*                     multicastLink (6),                              */
/*                     nssaExternalLink (7)                            */
/*                   }                                                 */
/*         ACCESS   read-only                                          */
/*         DESCRIPTION                                                 */
/*            "The type  of  the  link  state  advertisement.          */
/*            Each  link state type has a separate advertise-          */
/*            ment format."                                            */
/*    External Link State Advertisements are permitted                 */
/*    for backward compatibility, but should be displayed in           */
/*    the ospfExtLsdbTable rather than here.                           */
   word  LsdbType;

/*     ospfLsdbLsid OBJECT-TYPE                               */
/*         ACCESS   read-only                                 */
/*         DESCRIPTION                                        */
/*            "The Link State ID is an LS Type Specific field */
/*            containing either a Router ID or an IP Address; */
/*            it identifies the piece of the  routing  domain */
/*            that is being described by the advertisement."  */
   SP_IPADR LsdbLsid;

/*     ospfLsdbRouterId OBJECT-TYPE                           */
/*         ACCESS   read-only                                 */
/*         DESCRIPTION                                        */
/*            "The 32 bit number that uniquely identifies the */
/*            originating router in the Autonomous System."   */
   SP_IPADR LsdbRouterId;

/*     ospfLsdbSequence OBJECT-TYPE                           */
/*         ACCESS   read-only                                 */
/*         DESCRIPTION                                        */
/*            "The sequence number field is a  signed  32-bit */
/*            integer.   It  is used to detect old and dupli- */
/*            cate link state advertisements.  The  space  of */
/*            sequence  numbers  is  linearly  ordered.   The */
/*            larger the sequence number the more recent  the */
/*            advertisement."                                 */
/*   Note that the OSPF Sequence Number is a 32 bit signed    */
/*   integer.  It starts with the value '80000001'h,          */
/*   or -'7FFFFFFF'h, and increments until '7FFFFFFF'h        */
/*   Thus, a typical sequence number will be very negative.   */
   ulng  LsdbSequence;

/*     ospfLsdbAge OBJECT-TYPE                                */
/*         ACCESS   read-only                                 */
/*         DESCRIPTION                                        */
/*            "This field is the age of the link state adver- */
/*            tisement in seconds."                           */
   word  LsdbAge;

/*     ospfLsdbChecksum OBJECT-TYPE                           */
/*         ACCESS   read-only                                 */
/*         DESCRIPTION                                        */
/*            "This field is the  checksum  of  the  complete */
/*            contents  of  the  advertisement, excepting the */
/*            age field.  The age field is excepted  so  that */
/*            an   advertisement's  age  can  be  incremented */
/*            without updating the  checksum.   The  checksum */
/*            used  is  the same that is used for ISO connec- */
/*            tionless datagrams; it is commonly referred  to */
/*            as the Fletcher checksum."                      */
   word  LsdbChecksum;

   byte  LsdbOptions;

/*     ospfLsdbAdvertisement OBJECT-TYPE                      */
/*         ACCESS   read-only                                 */
/*         DESCRIPTION                                        */
/*            "The entire Link State Advertisement, NOT including */
/*            its header."                                    */
   /* THIS SHOULD NOT BE USED, BECAUSE IT IS A POINTER TO DATA
    * WITHIN THE LSDB, WHICH MAY CHANGE BEFORE THE CALLER GETS
    * AROUND TO DEREFERENCING IT. */
   byte *LsdbAdvertisement;

/* Length of the Link State Advertisement including the header*/
/* length                                                     */
   word LsdbAdvertLength;


   /* For hash list browsing the previous entry should be defined or
      NULL if it is the first required */
   t_Handle PrevEntry;

}  t_S_LsDbEntry;

/*-----------------------------------------------------**
**                                                     **
**         AS-External routes configuration            **
**    (causes to AS_External LSA & NSSA LSA            **
**     creation in the Area LSA Database)              **
**                                                     **
**-----------------------------------------------------*/
typedef struct t_S_AsExternalCfg
{
   struct tagt_Value *next;  /* pointer to the previous entry */
   struct tagt_Value *prev;  /* pointer to next entry */

   SP_IPADR DestNetIpAdr;
   SP_IPADR DestNetIpMask;
   SP_IPADR LsId;

   t_VpnCos VpnCos;
   Bool     IsVpnSupported;
   e_OspfPhyType PhyType;   /* ETHERNET/ATM/FRAME RELAY */

   byte     TosType;
   Bool     IsMetricType2;
   ulng     MetricValue;
   SP_IPADR ForwardingAdr;
   SP_IPADR Nexthop;
   ulng     ExtRouteTag;

   Bool     SupportMulticast; /* External link multicasting support flag */

   e_A_RowStatus  AsExtStatus;

   /* For hash list browsing the previous entry should be defined or
      NULL if it is the first required */
   t_Handle PrevEntry;

}  t_S_AsExternalCfg;

/* NSSA Change */
typedef enum e_NSSAMetricType
{
   NSSA_COMPARABLE_COST     = 1,
   NSSA_NON_COMPARABLE_COST = 2
}  e_NSSAMetricType;

typedef enum e_NSSATranslatorRole
{
   NSSA_TRANSLATOR_ALWAYS    = 1,
   NSSA_TRANSLATOR_CANDIDATE = 2
}  e_NSSATranslatorRole;

typedef enum e_NSSATranslatorState
{
   NSSA_TRANS_STATE_ENABLED  = 1,
   NSSA_TRANS_STATE_ELECTED  = 2,
   NSSA_TRANS_STATE_DISABLED = 3
}  e_NSSATranslatorState;

/*-----------------------------------------------------**
**                                                     **
**         NSSA configuration                          **
**                                                     **
**-----------------------------------------------------*/
typedef struct t_S_NssaCfg
{

/*     ospfAreaId OBJECT-TYPE                                                   */
/*         ACCESS   read-only                                                   */
/*         DESCRIPTION                                                          */
/*            "A 32-bit integer uniquely identifying an area.                   */
/*             Area ID 0.0.0.0 is used for the OSPF backbone.                   */
/*             If the	area represents a subnetted network,                    */
/*             the IP network number of the subnetted network                   */
/*             may be used for the Area ID.                                     */
  SP_IPADR         AreaId;

/*     ospfNSSATranslatorRole OBJECT-TYPE                                       */
/*         ACCESS read-create                                                   */
/*         DESCRIPTION                                                          */
/*            "Specifies whether or not an NSSA border router will              */
/*             unconditionally translate Type-7 LSAs into Type-5 LSAs.  When    */
/*             it is set to Always, an NSSA border router always translates     */
/*             Type-7 LSAs into Type-5 LSAs regardless of the translator state  */
/*             of other NSSA border routers.  When it is set to Candidate, an   */
/*             NSSA border router participates in the translator election       */
/*             process described in Section 3.1.  The default setting is        */
/*             Candidate."                                                      */
  e_NSSATranslatorRole  NSSATranslatorRole;

/*     ospfNSSATranslatorState OBJECT-TYPE                                      */
/*         ACCESS read-only                                                     */
/*         DESCRIPTION                                                          */
/*            "Determined by the NSSA Translator Role.  This value is set       */
/*             up after NSSA Translator election. Possible values are enable/   */
/*             disable/ elected.                                                */
  e_NSSATranslatorState  NSSATranslatorState;

/*     ospfTranslatorStabilityInterval OBJECT-TYPE                              */
/*         ACCESS read-create                                                   */
/*         DESCRIPTION                                                          */
/*            "Defines the length of time an elected Type-7 translator will     */
/*             continue to perform its translator duties once it has            */
/*             determined that its translator status has been deposed by        */
/*             another NSSA border router translator as described in Section    */
/*             3.1 and 3.3.  The default setting is 40 seconds                  */
  ulng             TranslatorStabilityInterval;

/*     ospfImportSummaries OBJECT-TYPE                                          */
/*         ACCESS read-create                                                   */
/*         DESCRIPTION                                                          */
/*            "When set to enabled, OSPF's summary routes are imported into     */
/*             the NSSA as Type-3 summary-LSAs.  When set to disabled, summary  */
/*             routes are not imported into the NSSA.  The default setting is   */
/*             enabled."                                                        */
  Bool             ImportSummaries;

  /* Value configured from area default-cost command. The metric advertised in
   * a default T3 LSA when area nssa no-summary. */
  ulng DefaultCost;

/*     ospfNSSARedistribute OBJECT-TYPE                                         */
/*         ACCESS read-create                                                   */
/*         DESCRIPTION                                                          */
/*            "When set to enabled NSSA ASBRs will not redistribute external    */
/*             routes into the NSSA.  I.e. they will cease to originate Type-7  */
/*             external LSAs for external routes"                               */
  Bool             NSSARedistribute;

/*     ospfNSSADefInfoOrig OBJECT-TYPE                                          */
/*         ACCESS read-create                                                   */
/*         DESCRIPTION                                                          */
/*            "Determines if the router will originate a Type-7 default LSA     */
/*             into the NSSA.  This command will cause a default LSA to be      */
/*             originated only if the router is an NSSA ABR or ASBR."           */

  Bool             NSSADefInfoOrig;

/*     ospfNSSADefaultMetric OBJECT-TYPE                                        */
/*         ACCESS read-create                                                   */
/*         DESCRIPTION                                                          */
/*            "Defines the metric value advertised in the Type-7 default LSA    */
/*             originated by this router"                                       */
  ulng             NSSADefMetric;

/*     ospfNSSAMetricType OBJECT-TYPE                                           */
/*        ACCESS read-create                                                    */
/*        DESCRIPTION                                                           */
/*            "Defines the metric type advertised in the Type-7 default LSA     */
/*             originated by this router."                                      */
  e_NSSAMetricType NSSADefMetricType;

/*     ospfNSSAStatus OBJECT-TYPE                                               */
/*         ACCESS   read-create                                                 */
/*         DESCRIPTION                                                          */
/*            "This variable displays the status of  the  en-                   */
/*            try.  Setting it to 'invalid' has the effect of                   */
/*            rendering it inoperative.  The internal  effect                   */
/*            (row removal) is implementation dependent."                       */
  e_A_RowStatus  NSSAStatus;

}  t_S_NssaCfg;

/*-----------------------------------------------------**
**                                                     **
**         Aggregate (Address) Range Table             **
**                                                     **
**-----------------------------------------------------*/

#define AGGREGATE_SUMMARY_LINK       3
#define AGGREGATE_NSSA_EXTERNAL_LINK 7

#define AGGREGATE_ADVERTISE          1
#define AGGREGATE_DO_NOT_ADVERTISE   2

typedef struct t_S_AreaAggregateEntry
{
   struct tagt_Value *next;  /* pointer to the previous entry */
   struct tagt_Value *prev;  /* pointer to next entry */

/*     ospfAreaAggregateAreaID OBJECT-TYPE                    */
/*         ACCESS   read-only                                 */
/*         DESCRIPTION                                        */
/*            "The Area the Address Aggregate is to be  found */
/*             within."                                       */
   SP_IPADR      AreaId;


/*------------------------------------------------------------**
**     ospfAreaAggregateLsdbType OBJECT-TYPE                  **
**         SYNTAX   INTEGER    {                              **
**                     summaryLink (3),                       **
**                     nssaExternalLink (7)                   **
**                   }                                        **
**         ACCESS   read-only                                 **
**         DESCRIPTION                                        **
**            "The type of the Address Aggregate.  This field **
**            specifies  the  Lsdb type that this Address Ag- **
**            gregate applies to."                            **
**           "OSPF Version 2, Appendix A.4.1 The  Link  State **
**           Advertisement header"                            **
**------------------------------------------------------------*/
   word  LsdbType;

/*------------------------------------------------------------**
**     ospfAreaAggregateNet OBJECT-TYPE                       **
**         SYNTAX   IpAddress                                 **
**         ACCESS   read-only                                 **
**         DESCRIPTION                                        **
**            "The IP Address of the Net or Subnet  indicated **
**            by the range."                                  **
**------------------------------------------------------------*/
   SP_IPADR NetIpAdr;


/*------------------------------------------------------------**
**     ospfAreaAggregateMask OBJECT-TYPE                      **
**         SYNTAX   IpAddress                                 **
**         ACCESS   read-only                                 **
**         DESCRIPTION                                        **
**            "The Subnet Mask that pertains to  the  Net  or **
**            Subnet."                                        **
**------------------------------------------------------------*/
   SP_IPADR SubnetMask;


/*------------------------------------------------------------**
**     ospfAreaAggregateStatus OBJECT-TYPE                    **
**         ACCESS   read-create                               **
**         DESCRIPTION                                        **
**            "This variable displays the status of  the  en- **
**            try.  Setting it to 'invalid' has the effect of **
**            rendering it inoperative.  The internal  effect **
**            (row removal) is implementation dependent."     **
**------------------------------------------------------------*/
   e_A_RowStatus  AggregateStatus;

/*------------------------------------------------------------**
**     ospfAreaAggregateEffect OBJECT-TYPE                    **
**         SYNTAX   INTEGER    {                              **
**                     advertiseMatching (1),                 **
**                     doNotAdvertiseMatching (2)             **
**                   }                                        **
**         ACCESS   read-create                               **
**         DESCRIPTION                                        **
**            "Subnets subsumed by ranges either trigger  the **
**            advertisement  of  the indicated aggregate (ad- **
**            vertiseMatching), or result in the subnet's not **
**            being advertised at all outside the area."      **
**        DEFVAL   { advertiseMatching }                      **
**------------------------------------------------------------*/
   word  AggregateEffect;

   /* For hash list browsing the previous entry should be defined or
      NULL if it is the first required */
   t_Handle PrevEntry;      /* for hash list browsing */

}  t_S_AreaAggregateEntry;

/*-----------------------------------------------------**
**                                                     **
**                Host Table                           **
**                                                     **
**-----------------------------------------------------*/
typedef struct t_S_HostRouteCfg
{
   struct tagt_Value *next;  /* pointer to the previous entry */
   struct tagt_Value *prev;  /* pointer to next entry */

/*------------------------------------------------------------**
**     ospfHostAreaID OBJECT-TYPE                             **
**         ACCESS   read-only                                 **
**         DESCRIPTION                                        **
**            "The Area the Host Entry is to be found within. **
**            By  default, the area that a subsuming OSPF in- **
**            terface is in, or 0.0.0.0"                      **
**------------------------------------------------------------*/
   SP_IPADR      AreaId;

/*------------------------------------------**
**     ospfHostIpAddress OBJECT-TYPE        **
**         ACCESS   read-only               **
**         DESCRIPTION                      **
**            "The IP Address of the Host." **
**------------------------------------------*/
	SP_IPADR HostIpAdr;

/*------------------------------------------------------------**
**     ospfHostTOS OBJECT-TYPE                                **
**         ACCESS   read-only                                 **
**         DESCRIPTION                                        **
**            "The Type of Service of the route being config- **
**            ured."                                          **
**------------------------------------------------------------*/
   e_TosTypes  HostTOS;

/*-------------------------------------------**
**     ospfHostMetric OBJECT-TYPE            **
**         ACCESS   read-create              **
**         DESCRIPTION                       **
**            "The Metric to be advertised." **
**-------------------------------------------*/
   word  HostMetric;

   t_VpnCos VpnCos;
   Bool     IsVpnSupported;
   e_OspfPhyType PhyType;   /* ETHERNET/ATM/FRAME RELAY */

   t_LIH    IfIndex;    /* IP interface connected  to this Host route */

/*------------------------------------------------------------**
**     ospfHostStatus OBJECT-TYPE                             **
**         ACCESS   read-create                               **
**         DESCRIPTION                                        **
**            "This variable displays the status of  the  en- **
**            try.  Setting it to 'invalid' has the effect of **
**            rendering it inoperative.  The internal  effect **
**            (row removal) is implementation dependent."     **
**------------------------------------------------------------*/
   e_A_RowStatus  HostStatus;

   /* For hash list browsing the previous entry should be defined or
      NULL if it is the first required */
   t_Handle PrevEntry;

}  t_S_HostRouteCfg;

/*-----------------------------------------------------**
**                                                     **
**                Interface Table                      **
**      (plays the role of Virtual Links also)         **
**-----------------------------------------------------*/

typedef enum e_IFO_Types
{
   IFO_ILLEGAL_TYPE,
   IFO_BRDC, /* Broadcast */
   IFO_NBMA, /* NBMA */
   IFO_PTP,  /* Point-to-Point */
   IFO_VRTL, /* Virtual */
   IFO_PTM,  /* Point-to-Multipoint */
   IFO_LOOPBACK /* Loopback */
}  e_IFO_Types;

typedef enum e_IFO_AuTypes
{
   IFO_NULL_AUTH,  /* Null Authentication */
   IFO_PASSWORD,   /* Simple Password */
   IFO_CRYPT       /* Cryptographic Authentication */
}  e_IFO_AuTypes;

typedef struct t_OspfAuthKey
{
   struct tagt_Value *next;  /* pointer to the previous entry */
   struct tagt_Value *prev;  /* pointer to next entry */

   byte  KeyId;
   byte  KeyValue[16];
   ulng  KeyStartAccept;
   ulng  KeyStopAccept;
   ulng  KeyStartGenerate;
   ulng  KeyStopGenerate;

   e_A_RowStatus KeyStatus;

   /* For hash list browsing the previous entry should be defined or
      NULL if it is the first required */
   t_Handle PrevEntry;

}  t_OspfAuthKey;

typedef enum e_MulticastForwConf
{
   IF_MLTCST_BLOCKED   = 1,
   IF_MLTCST_MULTICAST = 2,
   IF_MLTCST_UNICAST   = 3
}  e_MulticastForwConf;

#define IFO_MAX_SRLG 4
#define IFO_MAX_LD   2

#define IFO_MAX_IP_ADDRS 32

typedef struct t_IFO_Addrs
{
  SP_IPADR ipAddr;   /* IP Address  */
  SP_IPADR ipMask;   /* Subnet Mask */

} t_IFO_Addrs;

typedef struct t_IFO_Cfg
{

/*-----------------------------------------------------**
**     ospfIfIpAddress OBJECT-TYPE                     **
**         ACCESS   read-only                          **
**         DESCRIPTION                                 **
**            "The IP address of this OSPF interface." **
**-----------------------------------------------------*/
   SP_IPADR       IpAdr;

/*------------------------------------------------------------**
**     ospfAddressLessIf OBJECT-TYPE                          **
**         ACCESS   read-only                                 **
**         DESCRIPTION                                        **
**            "For the purpose of easing  the  instancing  of **
**            addressed   and  addressless  interfaces;  This **
**            variable takes the value 0 on  interfaces  with **
**            IP  Addresses,  and  the corresponding value of **
**            ifIndex for interfaces having no IP Address."   **
**------------------------------------------------------------*/
   t_LIH          IfIndex;

	/*  Also referred to as	the subnet/network mask, this indicates
	    the	portion	of the IP interface address that identifies the
	    attached network.  Masking the IP interface	address	with the
	    IP interface mask yields the IP network number of the
	    attached network.  On point-to-point networks and virtual
	    links, the IP interface mask is not	defined. On these
	    networks, the link itself is not assigned an IP network
	    number, and	so the addresses of each side of the link are
	    assigned independently, if they are	assigned at all.*/
   SP_IPADR       IpMask;

/*------------------------------------------------------------**
**     ospfIfAreaId OBJECT-TYPE                               **
**         ACCESS   read-create                               **
**         DESCRIPTION                                        **
**            "A 32-bit integer uniquely identifying the area **
**            to  which  the  interface  connects.   Area  ID **
**            0.0.0.0 is used for the OSPF backbone."         **
**        Note: Virtual Interfaces must be created with       **
**              Area ID equal to 0.0.0.0 (backbone)           **
**        DEFVAL   { '00000000'H }    -- 0.0.0.0              **
**------------------------------------------------------------*/
   SP_IPADR       AreaId;

   /* proprietary VPN supporting parameters */
   t_VpnCos       VpnCos;
   Bool           IsVpnSupported;
   e_OspfPhyType      PhyType;   /* ETHERNET/ATM/FRAME RELAY */

/*------------------------------------------------------------**
**     VirtTransitAreaId OBJECT-TYPE                          **
**         ACCESS   read-create                               **
**         DESCRIPTION                                        **
**            "A 32-bit integer uniquely identifying the      **
**            transit Area ID for a virtual link.             **
**            Must be not backbone(!=0.0.0.0)."               **
**------------------------------------------------------------*/
   SP_IPADR       VirtTransitAreaId;

/*-----------------------------------------------------**
**     ospfVirtIfNeighbor OBJECT-TYPE                  **
**         ACCESS   read-create                        **
**         DESCRIPTION                                 **
**            "The Router ID of the Virtual Neighbor." **
**-----------------------------------------------------*/
   SP_IPADR       VirtIfNeighbor;

/*------------------------------------------------------------**
**     ospfIfType OBJECT-TYPE                                 **
**         SYNTAX   INTEGER    {                              **
**                     broadcast (1),                         **
**                     nbma (2),                              **
**                     pointToPoint (3),                      **
**                     virtual (4),                           **
**                     pointToMultipoint (5)                  **
**                   }                                        **
**         ACCESS   read-create                               **
**         DESCRIPTION                                        **
**            "The OSPF interface type.                       **
**            By way of a default, this field may be intuited **
**            from the corresponding value of ifType.  Broad- **
**            cast LANs, such as  Ethernet  and  IEEE  802.5, **
**            take  the  value  'broadcast', X.25 and similar **
**            technologies take the value 'nbma',  and  links **
**            that  are  definitively point to point take the **
**            value 'pointToPoint'."                          **
**------------------------------------------------------------*/
   e_IFO_Types    Type;

/*------------------------------------------------------------**
**     ospfIfAdminStat OBJECT-TYPE                            **
**         ACCESS   read-create                               **
**         DESCRIPTION                                        **
**            "The OSPF  interface's  administrative  status. **
**            The  value formed on the interface, and the in- **
**            terface will be advertised as an internal route **
**            to  some  area.   The  value 'disabled' denotes **
**            that the interface is external to OSPF."        **
**        DEFVAL { enabled }                                  **
**------------------------------------------------------------*/
   Bool  AdminStat;

/*------------------------------------------------------------**
**     ospfIfRtrPriority OBJECT-TYPE                          **
**         ACCESS   read-create                               **
**         DESCRIPTION                                        **
**            "The  priority  of  this  interface.   Used  in **
**            multi-access  networks,  this  field is used in **
**            the designated router election algorithm.   The **
**            value 0 signifies that the router is not eligi- **
**            ble to become the  designated  router  on  this **
**            particular  network.   In the event of a tie in **
**            this value, routers will use their Router ID as **
**            a tie breaker."                                 **
**        DEFVAL { 1 }                                        **
**------------------------------------------------------------*/
   byte  RouterPriority;

/*------------------------------------------------------------**
**     ospfIfTransitDelay OBJECT-TYPE                         **
**         ACCESS   read-create                               **
**         DESCRIPTION                                        **
**            "The estimated number of seconds  it  takes  to **
**            transmit  a  link state update packet over this **
**            interface."                                     **
**        DEFVAL { 1 }                                        **
**------------------------------------------------------------*/
   word  InfTransDelay;

/*------------------------------------------------------------**
**     ospfIfRetransInterval OBJECT-TYPE                      **
**         ACCESS   read-create                               **
**         DESCRIPTION                                        **
**            "The number of seconds between  link-state  ad- **
**            vertisement  retransmissions,  for  adjacencies **
**            belonging to this  interface.   This  value  is **
**            also used when retransmitting database descrip- **
**            tion and link-state request packets."           **
**        DEFVAL { 5 }                                        **
**------------------------------------------------------------*/
   word  RxmtInterval;

/*------------------------------------------------------------**
**     ospfIfHelloInterval OBJECT-TYPE                        **
**         ACCESS   read-create                               **
**         DESCRIPTION                                        **
**            "The length of time, in  seconds,  between  the **
**            Hello  packets that the router sends on the in- **
**            terface.  This value must be the same  for  all **
**            routers attached to a common network."          **
**        DEFVAL { 10 }                                       **
**------------------------------------------------------------*/
   word  HelloInterval;

/*------------------------------------------------------------**
**     ospfIfRtrDeadInterval OBJECT-TYPE                      **
**         ACCESS   read-create                               **
**         DESCRIPTION                                        **
**            "The number of seconds that  a  router's  Hello **
**            packets  have  not been seen before it's neigh- **
**            bors declare the router down.  This  should  be **
**            some  multiple  of  the  Hello  interval.  This **
**            value must be the same for all routers attached **
**            to a common network."                           **
**        DEFVAL { 40 }                                       **
**------------------------------------------------------------*/
   ulng  RouterDeadInterval;

/*------------------------------------------------------------**
**     ospfIfPollInterval OBJECT-TYPE                         **
**         ACCESS   read-create                               **
**         DESCRIPTION                                        **
**            "The larger time interval, in seconds,  between **
**            the  Hello  packets  sent  to  an inactive non- **
**            broadcast multi- access neighbor."              **
**        DEFVAL { 120 }                                      **
**------------------------------------------------------------*/
   ulng  NbmaPollInterval;

	/*  The	number of seconds between LSA Acknowledgment packets
	    transmissions. Must be less than RxmtInterval: 1 seconds.*/
   word           AckInterval;

/*-------------------------------------------------**
**     ospfIfState OBJECT-TYPE                     **
**         SYNTAX   INTEGER    {                   **
**                     down (1),                   **
**                     loopback (2),               **
**                     waiting (3),                **
**                     pointToPoint (4),           **
**                     designatedRouter (5),       **
**                     backupDesignatedRouter (6), **
**                     otherDesignatedRouter (7)   **
**                   }                             **
**         ACCESS   read-only                      **
**         DESCRIPTION                             **
**            "The OSPF Interface State."          **
**        DEFVAL { down }                          **
**-------------------------------------------------*/
   word  State;

/*-------------------------------------------------------**
**     ospfIfDesignatedRouter OBJECT-TYPE                **
**         ACCESS   read-only                            **
**         DESCRIPTION                                   **
**            "The IP Address of the Designated Router." **
**        DEFVAL   { '00000000'H }    -- 0.0.0.0         **
**-------------------------------------------------------*/
   SP_IPADR    DrId;

/*------------------------------------------------------------**
**     ospfIfBackupDesignatedRouter OBJECT-TYPE               **
**         ACCESS   read-only                                 **
**         DESCRIPTION                                        **
**            "The  IP  Address  of  the  Backup   Designated **
**            Router."                                        **
**        DEFVAL   { '00000000'H }    -- 0.0.0.0              **
**------------------------------------------------------------*/
   SP_IPADR    BackupId;

/*------------------------------------------------------------**
**     ospfIfEvents OBJECT-TYPE                               **
**         ACCESS   read-only                                 **
**         DESCRIPTION                                        **
**            "The number of times this  OSPF  interface  has **
**            changed its state, or an error has occurred."   **
**------------------------------------------------------------*/
   ulng  EventsCounter;

/*-----------------------------------------------------------------**
**     ospfIfAuthType OBJECT-TYPE                                  **
**         SYNTAX   INTEGER (0..255)                               **
**                     -- none (0),                                **
**                     -- simplePassword (1)                       **
**                     -- md5 (2)                                  **
**                     -- reserved for specification by IANA (> 2) **
**         ACCESS   read-create                                    **
**         DESCRIPTION                                             **
**            "The authentication type specified for  an  in-      **
**            terface.   Additional  authentication types may      **
**            be assigned locally."                                **
**       DEFVAL { 0 }        -- no authentication, by default      **
**-----------------------------------------------------------------*/
   e_IFO_AuTypes  AuType;

/*--------------------------------------------------------------**
**     ospfIfAuthKey OBJECT-TYPE                                **
**         ACCESS   read-create                                 **
**         DESCRIPTION                                          **
**            "The Authentication Key.  If the Area's Author-   **
**            ization  Type  is  simplePassword,  and the key   **
**            length is shorter than 8 octets, the agent will   **
**            left adjust and zero fill to 8 octets.            **
**                                                              **
**            Note that unauthenticated  interfaces  need  no   **
**            authentication key, and simple password authen-   **
**            tication cannot use a key of more  than  8  oc-   **
**            tets.  Larger keys are useful only with authen-   **
**            tication mechanisms not specified in this docu-   **
**            ment.                                             **
**                                                              **
**            When read, ospfIfAuthKey always returns an  Oc-   **
**            tet String of length zero."                       **
**       DEFVAL   { '0000000000000000'H }    -- 0.0.0.0.0.0.0.0 **
**--------------------------------------------------------------*/
   byte           AuKey[8];

/*------------------------------------------------------------**
**     ospfIfStatus OBJECT-TYPE                               **
**         ACCESS   read-create                               **
**         DESCRIPTION                                        **
**            "This variable displays the status of  the  en- **
**            try.  Setting it to 'invalid' has the effect of **
**            rendering it inoperative.  The internal  effect **
**            (row removal) is implementation dependent."     **
**------------------------------------------------------------*/
   e_A_RowStatus  IfStatus;

/*------------------------------------------------------------**
**     ospfIfDemand OBJECT-TYPE                               **
**         ACCESS   read-create                               **
**         DESCRIPTION                                        **
**            "Indicates whether Demand OSPF procedures (hel- **
**            lo supression to FULL neighbors and setting the **
**            DoNotAge flag on proogated LSAs) should be per- **
**            formed on this interface."                      **
**        DEFVAL { false }                                    **
**------------------------------------------------------------*/
   Bool  IfDemand;

   /* If interface is passive or not */
   Bool  passiveMode;

/*------------------------------------------------------------------------------**
**     ospfIfMulticastForwarding OBJECT-TYPE                                    **
**         SYNTAX   INTEGER    {                                                **
**                             blocked (1),       -- no multicast forwarding    **
**                             multicast (2),     -- using multicast address    **
**                             unicast (3)        -- to each OSPF neighbor      **
**                   }                                                          **
**         ACCESS   read-create                                                 **
**         DESCRIPTION                                                          **
**            "The way multicasts should  forwarded  on  this                   **
**            interface;  not  forwarded,  forwarded  as data                   **
**            link multicasts, or forwarded as data link uni-                   **
**            casts.   Data link multicasting is not meaning-                   **
**            ful on point to point and NBMA interfaces,  and                   **
**            setting ospfMulticastForwarding to 0 effective-                   **
**            ly disables all multicast forwarding."                            **
**        DEFVAL { blocked }                                                    **
**------------------------------------------------------------------------------*/
   e_MulticastForwConf  MulticastForwarding;

/*     ospfIfLsaCount OBJECT-TYPE                             */
/*         ACCESS   read-only                                 */
/*         DESCRIPTION                                        */
/*            "The total number of Local Link Opaque LSAs"    */
   ulng  LinkLsaCount;

/*     ospfIfLsaCksumSum OBJECT-TYPE                          */
/*         ACCESS   read-only                                 */
/*         DESCRIPTION                                        */
/*            "The checksum of Local Link Opaque LSAs"        */
/*        DEFVAL   { 0 }                                      */
   ulng  LinkLsaCksum;

/*------------------------------------------------------------**
**     MaxIpMTUsize OBJECT-TYPE                               **
**         ACCESS   read-create                               **
**         DESCRIPTION                                        **
**            "Maximum IP MTU size on this interface."        **
**        DEFVAL { OSPF_MAX_IP_MTU }                          **
**------------------------------------------------------------*/
   word  MaxIpMTUsize;

/*------------------------------------------------------------**
**     MtuIgnore OBJECT-TYPE                                 **
**         ACCESS   read-create                               **
**         DESCRIPTION                                        **
**            "MTU Ignore flag on this interface."    **
**        DEFVAL { }                        **
**------------------------------------------------------------*/
   Bool  MtuIgnore;

/*------------------------------------------------------------**
**     DisableFlooding OBJECT-TYPE                            **
**         ACCESS   read-create                               **
**         DESCRIPTION                                        **
**            "Disable flooding on this interface."           **
**        DEFVAL { FALSE }                                    **
**------------------------------------------------------------*/
   Bool  DisableFlooding;

/*------------------------------------------------------------**
**                                                            **
**                                                            **
**         Traffic engineering parameters                     **
**                                                            **
**                                                            **
**------------------------------------------------------------*/

#if L7_OSPF_TE
   /* Traffic engineering metric change Threshold (default - 25 %) */
   word TeThreshold;

   /* Traffic Engineering metric */
   ulng TeMetric;

   /* Traffic enginnering initial maximum bandwidth PDRs per class */
   float32 TeMaxBandWidth;

   /* Traffic Engineering max reservable bandwidth (PDR) */
   float32 TeMaxReservBandWidth;

   /* Link Mux Capability */
   e_LinkMuxCap LinkMuxCap;

   /* Outgoing Interface Identifier */
   t_LIH OutIfIndex;

   /* Incoming Interface Identifier */
   t_LIH IncIfIndex;

   /* Maximum LSP bandwidth         */
   float32 MaxLspBndw[8];

   /* Link Protection Type */
   byte ProtType;  /* Link protection types are defined in l7_rtm.ext*/
   byte ProtPri;

   /* Link Descriptor     */
   e_LinkDescriptor LinkDescr[IFO_MAX_LD];
   byte LdPri[IFO_MAX_LD];
   float32 LdMinBndw[IFO_MAX_LD];
   float32 LdMaxBndw[IFO_MAX_LD];

   /* Shared Risk Link Group */
   ulng Srlg[IFO_MAX_SRLG];

#endif  /* L7_OSPF_TE */

   /* Support secondary addresses */

   t_IFO_Addrs    addrs[IFO_MAX_IP_ADDRS];
   byte			  DupNet[IFO_MAX_IP_ADDRS];

}  t_IFO_Cfg;

/*---------------------------------------------------------------**
**                                                               **
**                     Interface Metric Table                    **
**                                                               **
**    The Metric Table describes the metrics to be advertised    **
**    for a specified interface at the various types of service. **
**    As such, this table is an adjunct of the OSPF Interface    **
**    Table.                                                     **
**                                                               **
**  Types of service, as defined by RFC 791, have the ability    **
**  to request low delay, high bandwidth, or reliable linkage.   **
**                                                               **
**  For the purposes of this specification, the measure of       **
**  bandwidth                                                    **
**                                                               **
**       Metric = 10^8 / ifSpeed                                 **
**                                                               **
**  is the default value.  For multiple link interfaces, note    **
**  that ifSpeed is the sum of the individual link speeds.       **
**  This yields a number having the following typical values:    **
**                                                               **
**       Network Type/bit rate   Metric                          **
**                                                               **
**       >= 100 MBPS                 1                           **
**       Ethernet/802.3             10                           **
**        E1                        48                           **
**        T1 (ESF)                  65                           **
**        64 KBPS                 1562                           **
**        56 KBPS                 1785                           **
**        19.2 KBPS               5208                           **
**        9.6 KBPS               10416                           **
**                                                               **
**  Routes that are not specified use the default (TOS 0) metric **
**---------------------------------------------------------------*/
typedef struct t_S_IfMetric
{

/*------------------------------------------------------------**
**     ospfIfMetricIpAddress OBJECT-TYPE                      **
**         ACCESS   read-only                                 **
**         DESCRIPTION                                        **
**            "The IP address of this OSPF interface.  On row **
**            creation,  this  can  be  derived  from the in- **
**            stance."                                        **
**------------------------------------------------------------*/
   SP_IPADR       IpAdr;

/*------------------------------------------------------------**
**     ospfIfMetricAddressLessIf OBJECT-TYPE                  **
**         ACCESS   read-only                                 **
**         DESCRIPTION                                        **
**            "For the purpose of easing  the  instancing  of **
**            addressed   and  addressless  interfaces;  This **
**            variable takes the value 0 on  interfaces  with **
**            IP  Addresses, and the value of ifIndex for in- **
**            terfaces having no IP Address.   On  row  crea- **
**            tion, this can be derived from the instance."   **
**------------------------------------------------------------*/
   t_LIH          IfIndex;

/*------------------------------------------------------------**
**     ospfIfMetricTOS OBJECT-TYPE                            **
**         ACCESS   read-only                                 **
**         DESCRIPTION                                        **
**            "The type of service metric  being  referenced. **
**            On  row  creation, this can be derived from the **
**            instance."                                      **
**------------------------------------------------------------*/
   e_TosTypes  MetricTOS;

/*------------------------------------------------------------**
**     ospfIfMetricValue OBJECT-TYPE                          **
**         ACCESS   read-create                               **
**         DESCRIPTION                                        **
**            "The metric of using this type  of  service  on **
**            this interface.  The default value of the TOS 0 **
**            Metric is 10^8 / ifSpeed."                      **
**------------------------------------------------------------*/
   word  Metric;

/*------------------------------------------------------------**
**     ospfIfMetricStatus OBJECT-TYPE                         **
**         ACCESS   read-create                               **
**         DESCRIPTION                                        **
**            "This variable displays the status of  the  en- **
**            try.  Setting it to 'invalid' has the effect of **
**            rendering it inoperative.  The internal  effect **
**            (row removal) is implementation dependent."     **
**------------------------------------------------------------*/
   e_A_RowStatus  MetricStatus;

   /* For hash list browsing the previous entry should be defined or
      NULL if it is the first required */
   t_Handle       PrevEntry;
}  t_S_IfMetric;

/*-----------------------------------------------------**
**                                                     **
**          Configurable NBMA routers hash list entry  **
**                                                     **
**-----------------------------------------------------*/
typedef struct t_S_IfNbmaCfg
{
   SP_IPADR RouterId;
   SP_IPADR IpAdr;
   Bool     CanBeDr;
   e_A_RowStatus  NbmaStatus;
   /* For hash list browsing the previous entry should be defined or
      NULL if it is the first required */
   t_Handle       PrevEntry;
}  t_S_IfNbmaCfg;

/*-----------------------------------------------------**
**                                                     **
**                Neighbor Table                       **
**       (play role of Virtual Neighbor also)          **
**-----------------------------------------------------*/

typedef struct t_S_NeighborCfg
{
/*------------------------------------------------------------**
**     ospfNbrIpAddr OBJECT-TYPE                              **
**         ACCESS   read-only                                 **
**         DESCRIPTION                                        **
**            "The IP address this neighbor is using  in  its **
**            IP  Source  Address.  Note that, on addressless **
**            links, this will not be 0.0.0.0,  but  the  ad- **
**            dress of another of the neighbor's interfaces." **
**------------------------------------------------------------*/
   SP_IPADR       IpAdr;

/*------------------------------------------------------------**
**     ospfNbrAddressLessIndex OBJECT-TYPE                    **
**         ACCESS   read-only                                 **
**         DESCRIPTION                                        **
**            "On an interface having an  IP  Address,  zero. **
**            On  addressless  interfaces,  the corresponding **
**            value of ifIndex in the Internet Standard  MIB. **
**            On  row  creation, this can be derived from the **
**            instance."                                      **
**------------------------------------------------------------*/
   t_LIH       IfIndex;

/*------------------------------------------------------------**
**     ospfNbrRtrId OBJECT-TYPE                               **
**         ACCESS   read-only                                 **
**         DESCRIPTION                                        **
**            "A 32-bit integer (represented as a type  IpAd- **
**            dress)  uniquely  identifying  the  neighboring **
**            router in the Autonomous System."               **
**        DEFVAL   { '00000000'H }    -- 0.0.0.0              **
**------------------------------------------------------------*/
   SP_IPADR    NbrRtrId;

/*------------------------------------------------------------**
**     ospfNbrOptions OBJECT-TYPE                             **
**         ACCESS   read-only                                 **
**         DESCRIPTION                                        **
**            "A Bit Mask corresponding to the neighbor's op- **
**            tions field.                                    **
**                                                            **
**            Bit 0, if set, indicates that the  system  will **
**            operate  on  Type of Service metrics other than **
**            TOS 0.  If zero, the neighbor will  ignore  all **
**            metrics except the TOS 0 metric.                **
**                                                            **
**            Bit 1, if set, indicates  that  the  associated **
**            area  accepts and operates on external informa- **
**            tion; if zero, it is a stub area.               **
**                                                            **
**            Bit 2, if set, indicates that the system is ca- **
**            pable  of routing IP Multicast datagrams; i.e., **
**            that it implements the Multicast Extensions  to **
**            OSPF.                                           **
**                                                            **
**            Bit 3, if set, indicates  that  the  associated **
**            area  is  an  NSSA.  These areas are capable of **
**            carrying type 7 external advertisements,  which **
**            are  translated into type 5 external advertise- **
**            ments at NSSA borders."                         **
**       DEFVAL { 0 }                                         **
**------------------------------------------------------------*/
   ulng        NbrOptions;

/*------------------------------------------------------------**
**     ospfNbrPriority OBJECT-TYPE                            **
**         ACCESS   read-create                               **
**         DESCRIPTION                                        **
**            "The priority of this neighbor in the designat- **
**            ed router election algorithm.  The value 0 sig- **
**            nifies that the neighbor is not eligible to be- **
**            come  the  designated router on this particular **
**            network."                                       **
**        DEFVAL { 1 }                                        **
**------------------------------------------------------------*/
   byte        NbrPriority;

/*------------------------------------------------------------**
**        Area Id                                             **
**------------------------------------------------------------*/
   byte        NbrArea;

/*------------------------------------------------------------**
**        Dead Time Remaining                                 **
**------------------------------------------------------------*/
   ulng        NbrDeadTimerRemaining;

/* number of seconds since adjacency last went to Full. */
   ulng        NbrUptime;

/*------------------------------------------------------------**
**     ospfNbrState OBJECT-TYPE                               **
**         SYNTAX   INTEGER    {                              **
**                     down (1),                              **
**                     attempt (2),                           **
**                     init (3),                              **
**                     twoWay (4),                            **
**                     exchangeStart (5),                     **
**                     exchange (6),                          **
**                     loading (7),                           **
**                     full (8)                               **
**                   }                                        **
**         ACCESS   read-only                                 **
**         DESCRIPTION                                        **
**            "The State of the relationship with this Neigh- **
**            bor."                                           **
**        REFERENCE                                           **
**           "OSPF Version 2, Section 10.1 Neighbor States"   **
**       DEFVAL   { down }                                    **
**------------------------------------------------------------*/
   word     NbrState;

/*------------------------------------------------------------**
**     ospfNbrEvents OBJECT-TYPE                              **
**         ACCESS   read-only                                 **
**         DESCRIPTION                                        **
**            "The number of times this neighbor relationship **
**            has changed state, or an error has occurred."   **
**------------------------------------------------------------*/
   ulng     NbrEvents;

/*------------------------------------------------------------**
**     ospfNbrLsRetransQLen OBJECT-TYPE                       **
**         ACCESS   read-only                                 **
**         DESCRIPTION                                        **
**            "The  current  length  of  the   retransmission **
**            queue."                                         **
**------------------------------------------------------------*/
   ulng     NbrLsRetransQLen;

/*------------------------------------------------------------**
**     ospfNbmaNbrStatus OBJECT-TYPE                          **
**         ACCESS   read-create                               **
**         DESCRIPTION                                        **
**            "This variable displays the status of  the  en- **
**            try.  Setting it to 'invalid' has the effect of **
**            rendering it inoperative.  The internal  effect **
**            (row removal) is implementation dependent."     **
**------------------------------------------------------------*/
   e_A_RowStatus  NbmaNbrStatus;

/*---------------------------------------------------------------------**
**     ospfNbmaNbrPermanence OBJECT-TYPE                               **
**         SYNTAX   INTEGER    {                                       **
**                     dynamic (1),        -- learned through protocol **
**                     permanent (2)       -- configured address       **
**                   }                                                 **
**         ACCESS   read-only                                          **
**         DESCRIPTION                                                 **
**            "This variable displays the status of  the  en-          **
**            try.   'dynamic'  and  'permanent' refer to how          **
**            the neighbor became known."                              **
**        DEFVAL { permanent }                                         **
**---------------------------------------------------------------------*/
   word     NbmaNbrPermanence;

/*------------------------------------------------------------**
**     ospfNbrHelloSuppressed OBJECT-TYPE                     **
**         ACCESS   read-only                                 **
**         DESCRIPTION                                        **
**            "Indicates whether Hellos are being  suppressed **
**            to the neighbor"                                **
**------------------------------------------------------------*/
   Bool     NbrHelloSuppressed;

}  t_S_NeighborCfg;

/*-----------------------------------------------------**
**                                                     **
**                   Traps                             **
**                                                     **
**-----------------------------------------------------*/
typedef enum e_OspfTrapType
{
    VIRT_IF_STATE_CHANGE,           /* 1     */
    NBR_STATE_CHANGE,               /* 2     */
    VIRT_NBR_STATE_CHANGE,          /* 4     */
    IF_CONFIG_ERROR,                /* 8     */
    VIRT_IF_CONFIG_ERROR,           /* 10    */
    IF_AUTH_FAILURE,                /* 20    */
    VIRT_IF_AUTH_FAILURE,           /* 40    */
    RX_BAD_PACKET,                  /* 80    */
    VIRT_IF_RX_BAD_PACKET,          /* 100   */
    TX_RETRANSMIT,                  /* 200   */
    VIRT_IF_TX_RETRANSMIT,          /* 400   */
    ORIGINATE_LSA,                  /* 800   */
    MAX_AGE_LSA,                    /* 1000  */
    LS_DB_OVERFLOW,                 /* 2000  */
    LS_DB_APPROACHING_OVERFLOW,     /* 4000  */
    IF_STATE_CHANGE,                /* 8000  */
    IF_RX_PACKET,                   /* 10000 */
    RTB_ENTRY_INFO                  /* 20000 */
}  e_OspfTrapType;

#define GET_TRAP_BITMASK(trap) (1 << trap)

typedef enum e_TrapConfigErrors
{
   CONFIG_OK               = 0,
   BAD_VERSION             = 1,
   AREA_MISMATCH           = 2,
   UNKNOWN_NBMA_NBR        = 3, /* Router is Dr eligible */
   UNKNOWN_VIRTUAL_NBR     = 4,
   AUTH_TYPE_MISMATCH      = 5,
   AUTH_FAILURE            = 6,
   NET_MASK_MISMATCH       = 7,
   HELLO_INTERVAL_MISMATCH = 8,
   DEAD_INTERVAL_MISMATCH  = 9,
   OPTION_MISMATCH         = 10,
   BAD_PACKET              = 11,
   NBR_IPADDR_MISMATCH     = 12,
   UKNOWN_ERROR            /* must be last */
}  e_TrapConfigErrors;


/*------------------------------------------------------------**
**     ospfSetTrap OBJECT-TYPE                                **
**         ACCESS   read-write                                **
**         DESCRIPTION                                        **
**            "A four-octet string serving as a bit  map  for **
**            the trap events defined by the OSPF traps. This **
**            object is used to enable and  disable  specific **
**            OSPF   traps   where  a  1  in  the  bit  field **
**            represents enabled.  The right-most bit  (least **
**            significant) represents trap 0."                **
**------------------------------------------------------------*/
typedef ulng t_S_TrapControl;

/*-----------------------------------------------------------------**
**     ospfVirtIfStateChange NOTIFICATION-TYPE                     **
**         DESCRIPTION                                             **
**            "An ospfIfStateChange trap signifies that there      **
**            has  been a change in the state of an OSPF vir-      **
**            tual interface.                                      **
**            This trap should be generated when  the  inter-      **
**            face  state  regresses  (e.g., goes from Point-      **
**            to-Point to Down) or progresses to  a  terminal      **
**            state (i.e., Point-to-Point)."                       **
**-----------------------------------------------------------------*/
typedef struct t_VirtIfStateChange
{
   SP_IPADR RouterId;    /* The originator of the trap */
   SP_IPADR VirtIfAreaId;
   SP_IPADR VirtIfNeighbor;
   word     VirtIfState;  /* The new state           */
   word     Event;
}  t_VirtIfStateChange;

/*-----------------------------------------------------------------**
**     ospfNbrStateChange NOTIFICATION-TYPE                        **
**         DESCRIPTION                                             **
**            "An  ospfNbrStateChange  trap  signifies   that      **
**            there  has been a change in the state of a non-      **
**            virtual OSPF neighbor.   This  trap  should  be      **
**            generated  when  the  neighbor  state regresses      **
**            (e.g., goes from Attempt or Full  to  1-Way  or      **
**            Down)  or progresses to a terminal state (e.g.,      **
**            2-Way or Full).  When an  neighbor  transitions      **
**            from  or  to Full on non-broadcast multi-access      **
**            and broadcast networks, the trap should be gen-      **
**            erated  by the designated router.  A designated      **
**            router transitioning to Down will be  noted  by      **
**            ospfIfStateChange."                                  **
**-----------------------------------------------------------------*/
typedef struct t_NbrStateChange
{
   SP_IPADR RouterId; /* The originator of the trap */
   SP_IPADR NbrIpAddr;
   t_LIH    NbrAddressLessIndex;
   SP_IPADR NbrRtrId;
   word     NbrState;  /* The new state              */
   word     Event;
}  t_NbrStateChange;

/*-----------------------------------------------------------------**
**     ospfVirtNbrStateChange NOTIFICATION-TYPE                    **
**         DESCRIPTION                                             **
**            "An ospfIfStateChange trap signifies that there      **
**            has  been a change in the state of an OSPF vir-      **
**            tual neighbor.  This trap should  be  generated      **
**            when  the  neighbor state regresses (e.g., goes      **
**            from Attempt or  Full  to  1-Way  or  Down)  or      **
**            progresses to a terminal state (e.g., Full)."        **
**-----------------------------------------------------------------*/

typedef struct t_VirtNbrStateChange
{
   SP_IPADR RouterId; /* The originator of the trap */
   SP_IPADR VirtNbrArea;
   SP_IPADR VirtNbrRtrId;
   word     VirtNbrState;  /* The new state */
   word     Event;
}  t_VirtNbrStateChange;


/*-----------------------------------------------------------------**
**     ospfIfConfigError NOTIFICATION-TYPE                         **
**         DESCRIPTION                                             **
**            "An ospfIfConfigError  trap  signifies  that  a      **
**            packet  has  been received on a non-virtual in-      **
**            terface  from  a  router  whose   configuration      **
**            parameters  conflict  with this router's confi-      **
**            guration parameters.  Note that the  event  op-      **
**            tionMismatch  should  cause  a  trap only if it      **
**            prevents an adjacency from forming."                 **
**-----------------------------------------------------------------*/

typedef struct t_IfConfigError
{
   SP_IPADR RouterId; /* The originator of the trap */
   SP_IPADR IfIpAddress;
   t_LIH    AddressLessIf;
   SP_IPADR PacketSrc;  /* The source IP address  */
   word     PacketType;
   e_TrapConfigErrors ConfigErrorType; /* Type of error     */
}  t_IfConfigError;

/*-----------------------------------------------------------------**
**     ospfVirtIfConfigError NOTIFICATION-TYPE                     **
**         DESCRIPTION                                             **
**            "An ospfConfigError trap signifies that a pack-      **
**            et  has  been  received  on a virtual interface      **
**            from a router  whose  configuration  parameters      **
**            conflict   with   this  router's  configuration      **
**            parameters.  Note that the event optionMismatch      **
**            should  cause a trap only if it prevents an ad-      **
**            jacency from forming."                               **
**-----------------------------------------------------------------*/

typedef struct t_VirtIfConfigError
{
   SP_IPADR RouterId; /* The originator of the trap */
   SP_IPADR VirtIfAreaId;
   SP_IPADR VirtIfNeighbor;
   word     PacketType;
   e_TrapConfigErrors ConfigErrorType; /* Type of error     */
}  t_VirtIfConfigError;

/*-----------------------------------------------------------------**
**     ospfIfRxBadPacket NOTIFICATION-TYPE                         **
**         DESCRIPTION                                             **
**            "An ospfIfRxBadPacket trap  signifies  that  an      **
**            OSPF  packet has been received on a non-virtual      **
**            interface that cannot be parsed."                    **
**-----------------------------------------------------------------*/
typedef struct t_IfRxBadPacket
{
   SP_IPADR RouterId; /* The originator of the trap */
   SP_IPADR IfIpAddress;
   t_LIH    AddressLessIf;
   SP_IPADR PacketSrc;  /* The source IP address */
   word     PacketType;
}  t_IfRxBadPacket;

/*-----------------------------------------------------------------**
**     ospfVirtIfRxBadPacket NOTIFICATION-TYPE                     **
**         DESCRIPTION                                             **
**            "An ospfRxBadPacket trap signifies that an OSPF      **
**            packet has been received on a virtual interface      **
**            that cannot be parsed."                              **
**-----------------------------------------------------------------*/
typedef struct t_VirtIfRxBadPacket
{
   SP_IPADR RouterId; /* The originator of the trap */
   SP_IPADR VirtIfAreaId;
   SP_IPADR VirtIfNeighbor;
   word     PacketType;
}  t_VirtIfRxBadPacket;

/*-----------------------------------------------------------------**
**     ospfTxRetransmit NOTIFICATION-TYPE                          **
**         DESCRIPTION                                             **
**            "An ospfTxRetransmit  trap  signifies  than  an      **
**            OSPF  packet  has  been retransmitted on a non-      **
**            virtual interface.  All packets that may be re-      **
**            transmitted  are associated with an LSDB entry.      **
**            The LS type, LS ID, and Router ID are  used  to      **
**            identify the LSDB entry."                            **
**-----------------------------------------------------------------*/
typedef struct t_TxRetransmit
{
   SP_IPADR RouterId; /* The originator of the trap */
   SP_IPADR IfIpAddress;
   t_LIH    AddressLessIf;
   SP_IPADR NbrRtrId; /* Destination  */
   word     PacketType;
   word     LsdbType;
   ulng     LsdbLsid;
   SP_IPADR LsdbRouterId;
   ulng     LsdbSeqNum;
}  t_TxRetransmit;

/*-----------------------------------------------------------------**
**     ospfVirtIfTxRetransmit NOTIFICATION-TYPE                    **
**         DESCRIPTION                                             **
**            "An ospfTxRetransmit  trap  signifies  than  an      **
**            OSPF packet has been retransmitted on a virtual      **
**            interface.  All packets that may be retransmit-      **
**            ted  are  associated with an LSDB entry. The LS      **
**            type, LS ID, and Router ID are used to identify      **
**            the LSDB entry."                                     **
**-----------------------------------------------------------------*/
typedef struct t_VirtTxRetransmit
{
   SP_IPADR RouterId; /* The originator of the trap */
   SP_IPADR VirtIfAreaId;
   SP_IPADR VirtIfNeighbor;
   word     PacketType;
   word     LsdbType;
   ulng     LsdbLsid;
   SP_IPADR LsdbRouterId;
   ulng     LsdbSeqNum;
}  t_VirtTxRetransmit;

/*-----------------------------------------------------------------**
**     ospfOriginateLsa NOTIFICATION-TYPE                           **
**         DESCRIPTION                                              **
**            "An ospfOriginateLsa trap signifies that a  new       **
**            LSA  has  been originated by this router.  This       **
**            trap should not be invoked for simple refreshes       **
**            of  LSAs  (which happesn every 30 minutes), but       **
**            instead will only be invoked  when  an  LSA  is       **
**            (re)originated due to a topology change.  Addi-       **
**            tionally, this trap does not include LSAs  that       **
**            are  being  flushed  because  they have reached       **
**            MaxAge."                                              **
**-----------------------------------------------------------------*/
typedef struct t_OriginateLsa
{
   SP_IPADR RouterId; /* The originator of the trap */
   SP_IPADR LsdbAreaId;  /* 0.0.0.0 for AS Externals */
   word     LsdbType;
   ulng     LsdbLsid;
   SP_IPADR LsdbRouterId;
   ulng     LsdbSeqNum;
}  t_OriginateLsa;

/*-----------------------------------------------------------------**
**     ospfMaxAgeLsa NOTIFICATION-TYPE                              **
**         DESCRIPTION                                              **
**            "An ospfMaxAgeLsa trap signifies  that  one  of       **
**            the LSA in the router's link-state database has       **
**            aged to MaxAge."                                      **
**-----------------------------------------------------------------*/
typedef t_OriginateLsa t_MaxAgeLsa;

/*-----------------------------------------------------------------**
**     ospfLsdbOverflow NOTIFICATION-TYPE                          **
**         DESCRIPTION                                             **
**            "An ospfLsdbOverflow trap  signifies  that  the      **
**            number of LSAs in the router's link-state data-      **
**            base has exceeded ospfExtLsdbLimit."                 **
**-----------------------------------------------------------------*/
typedef struct t_LsdbOverflow
{
   SP_IPADR RouterId; /* The originator of the trap */
   ulng     ExtLsdbLimit;
}  t_LsdbOverflow;

/*-----------------------------------------------------------------**
**     ospfLsdbApproachingOverflow NOTIFICATION-TYPE               **
**         DESCRIPTION                                             **
**            "An ospfLsdbApproachingOverflow trap  signifies      **
**            that  the  number of LSAs in the router's link-      **
**            state database has exceeded ninety  percent  of      **
**            ospfExtLsdbLimit."                                   **
**-----------------------------------------------------------------*/

typedef t_LsdbOverflow t_LsdbApproachingOverflow;

/*-----------------------------------------------------------------**
**     ospfIfStateChange NOTIFICATION-TYPE                         **
**         DESCRIPTION                                             **
**            "An ospfIfStateChange trap signifies that there      **
**            has been a change in the state of a non-virtual      **
**            OSPF interface. This trap should  be  generated      **
**            when  the interface state regresses (e.g., goes      **
**            from Dr to Down) or progresses  to  a  terminal      **
**            state  (i.e.,  Point-to-Point, DR Other, Dr, or      **
**            Backup)."                                            **
**-----------------------------------------------------------------*/
typedef struct t_IfStateChange
{
   SP_IPADR RouterId; /* The originator of the trap */
   SP_IPADR IfIpAddress;
   t_LIH    AddressLessIf;
   word     IfState;   /* The new state */
   word     Event;
}  t_IfStateChange;


/*-----------------------------------------------------------------**
**     t_RtbEntryInfo    NOTIFICATION-TYPE                         **
**         DESCRIPTION                                             **
**            "proprietary Routing Table Entry Information         **
**             trap."                                              **
**-----------------------------------------------------------------*/
typedef struct t_RtbEntryInfo
{
   SP_IPADR       DestinationId;   /* Router Id or network IP address */
   SP_IPADR       IpMask;          /* network IP mask */
   int            DestinationType; /* Router or Network */
   SP_IPADR       AreaId;          /* entry originator's Area */
   word           OptionalCapabilities; /* ability to process AS-external LSAs */
   int            PathType;        /* four possible types of path */
   ulng           Cost;            /* link cost */
   ulng           Type2Cost;       /* cost valid only for type 2 path */
   void           *p_DbEntry;      /* destination directly LSA reference */
   int            Status;          /* entry status (added, removed, modifyed) */
   SP_IPADR       NextIpAdr;       /* Next Hop Ip Address */
   word           NextHopNum;      /* Number of equal next hops */
   SP_IPADR       RtrChain[20];    /* Router Id table */
   ulng           IfIndex[20];     /* NextHop Interface Index table */
}  t_RtbEntryInfo;

typedef union u_OspfTrap
{
   t_VirtIfStateChange         VirtIfStateChange;
   t_NbrStateChange            NbrStateChange;
   t_VirtNbrStateChange        VirtNbrStateChange;
   t_IfConfigError             IfConfigError;
   t_VirtIfConfigError         VirtIfConfigError;
   t_IfRxBadPacket             IfRxBadPacket;
   t_VirtIfRxBadPacket         VirtIfRxBadPacket;
   t_TxRetransmit              TxRetransmit;
   t_VirtTxRetransmit          VirtTxRetransmit;
   t_OriginateLsa              OriginateLsa;
   t_MaxAgeLsa                 MaxAgeLsa;
   t_LsdbOverflow              LsdbOverflow;
   t_LsdbApproachingOverflow   LsdbApproachingOverflow;
   t_IfStateChange             IfStateChange;
   t_IfRxBadPacket             IfRxPacket;
   t_RtbEntryInfo              RtbEntryInfo;
}  u_OspfTrap;

/* IGMP V.2 Timers and variables defaults */
#define ROBUSTNESS_DEF         2
#define QUERY_INT_DEF          125 /*sec*/
#define QUERY_RSP_INT_DEF      10  /*sec*/
#define GROUP_MEMBER_INT_DEF   (ROBUSTNESS_DEF*QUERY_INT_DEF + QUERY_RSP_INT_DEF)
#define OTHER_QUERIER_INT_DEF  (ROBUSTNESS_DEF*QUERY_INT_DEF + QUERY_RSP_INT_DEF/2)
#define STARTUP_QUERY_INT_DEF  (QUERY_INT_DEF/4)
#define STARTUP_QUERY_COUNT_DEF ROBUSTNESS_DEF
#define LAST_MEMBER_QUERY_INT_DEF 1 /*sec*/
#define LAST_MEMBER_QUERY_COUNT_DEF ROBUSTNESS_DEF
#define UNSOLICITED_REPORT_INT_DEF 10 /*sec*/
#define VER1_ROUTER_PRES_TIMEOUT_DEF 400 /*sec*/
#define USE_IGMP_V1            FALSE

/* IGMP V.2 Timers and variables parameters */
typedef struct t_OspfRouterIgmpCfg
{
   /* The Robustness Variable allows tuning for the expected packet loss on a subnet */
   word  Robustness;
   /* The Query Interval is the interval between General Queries sent  by the Querier */
   word  QueryInt;
   /* The Max Response Time inserted into the periodic General Queries */
   word  QueryRspInt;
   /* The Group Membership Interval is the amount of time that must passbefore a
      multicast router decides there are no more members of agroup on a network.
      This value MUST be ((the Robustness Variable)times (the Query Interval))
      plus (one Query Response Interval).  */
   word  GroupMemberInt;
   /* The Other Querier Present Interval is the length of time that must pass before
      a multicast router decides that there is no longer another multicast router
      which should be the querier.  */
   word  OtherQuerierInt;
   /* The Startup Query Interval is the interval between General Queries sent
      by a Querier on startup.  */
   word  StartupQueryInt;
   /* The Startup Query Count is the number of Queries sent out on startup,
      separated by the Startup Query Interval.  */
   word  StartupQueryCount;
   /* The Last Member Query Interval is the Max Response Time inserted into
      Group-Specific Queries sent in response to Leave Group messages, and
      is also the amount of time between Group-Specific Query messages.  */
   word  LastMemberQueryInt;
   /* The Last Member Query Count is the number of Group-Specific
      Queriessent before the router assumes there are no local members.  */
   word  LastMemberQueryCount;
   /* The Unsolicited Report Interval is the time between repetitions of
      a host's initial report of membership in a group.  */
   word  UnsolicitedReportInt;
   /* The Version 1 Router Present Timeout is how long a host must
      waitafter hearing a Version 1 Query before it may send any IGMPv2 messages.  */
   word  Ver1RouterPresTimeout;
   /* The configuration switch to ignore Version 1 messages completely.  */
   Bool  UseIgmpV1;
}  t_OspfRouterIgmpCfg;

#endif /* spcfg_h */

/* --- end of file spcfg.h --- */

