/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename         ospfmib.h
 *
 * @purpose          internal definitions for OSPF MIB tables support
 *                   (according to RFC 1850)
 *
 * @component        Routing OSPF Component
 *
 * @comments
 *
 * @create           05/01/2000
 *
 * @author           Elena Korchemny
 *
 * @end
 *
 * ********************************************************************/


/*#ifdef FILEID_IN_MEMORY
static char __fileid[] = "$Header: lvl7dev\src\application\protocol\ospf\mibospf\ospfmib.h 1.1.2.3 2002/04/24 20:18:00EDT anayar Exp  $";
#endif */

#ifndef _ospfmib_support_h_
#define _ospfmib_support_h_

#include "std.h"
#include "rfc1443.h"
#include "spcfg.h"

/***********************
* Constant definitions
***********************/

#define ospfAreaTableAuthTypeSubId     2
#define ospfAreaTableImpAsExtSubId     3
#define ospfAreaTableAreaSummarySubId  9
#define ospfAreaTableRowStatusSubId    10

#define ospfStubAreaTableRowStatusSubId  4
#define ospfStubAreaTableMetricTypeSubId 5 

#define ospfHostTableRowStatusSubId   4  

#define ospfIfTableAreaId             3
#define ospfIfTableAdminStatusSubId   5
#define ospfIfTableRowStatusSubId     17
#define ospfIfTableMltCastFrwrdSubId  18
#define ospfIfTableIfDemandSubId      19
#define ospfIfTableAuthTypeSubId      20

#define ospfIfMetricTableRowStatusSubId 5

#define ospfVirtIfTableRowStatusSubId  10
#define ospfVirtIfTableAuthTypeSubId   11

#define ospfNbrTableNbrPriorSubId      5
#define ospfNbrTableRowStatusSubId     9
#define ospfNbrTableHelloSuprSubId     11

#define ospfAreaAggrTableRowStatusSubId 5
#define ospfAreaAggrTableEffectSubId    6

/*************************
 * Type definitions
 *************************/

typedef enum e_ospfAdminStatus
{
   enabled = 1,
   disabled   

}  e_ospfAdminStatus;

typedef enum e_ospfNbrPermanence
{
   dynamic = 1,
   permanent

} e_ospfPermanence;

typedef enum e_ospfAreaAggrEffect
{
   advertiseMatching = 1,
   doNotAdvertiseMatching
} e_ospfAreaAggrEffect;

/* 1. ospfAreaTable */

/* ospfAreaTable info - statistical information is comment out here */
/* it will be retrived from the area object */
typedef struct t_ospfAreaInfo
{
/* e_ospfAuthType          authType; - obsolete */
   e_AreaImportExternalCfg importASExt;
/* ulng                    spfRuns,
   ulng                    brdRtrCnt,
   ulng                    ASBrdRtrCnt,
   ulng                    lsaCnt,
   ulng                    lsaChkSum, */
   e_AreaSummaryCfg        areaSummary;
   e_RowStatus             rowStatus;
   Bool                    internalCreated; /* FALSE: created by SNMP
                                               TRUE:  created internally,
                                                      by stack manager    */
} ospfAreaInfo;

typedef struct ospfAreaEntry
{
   struct   ospfAreaEntry *next;
   struct   ospfAreaEntry *prev;
   ulng     ospfAreaId;              /* key for searches in the OLL*/
   t_Handle owner;                   /* handle of the area object ospfAreaObj */

   ospfAreaInfo info;
} ospfAreaEntry;


/* 2. ospfStubAreaTable */

typedef struct t_ospfStubAreaInfo
{
   ulng              metric;
   e_RowStatus       rowStatus;
   e_StubMetricType  metricType;
   Bool              internalCreated; /* FALSE: created by SNMP
                                         TRUE:  created internally */
   Bool              SNMPdisable; /* TRUE: the row is being disabled
                                     by SNMP - do not delete */

} ospfStubAreaInfo;

typedef struct ospfStubAreaEntry
{
   struct   ospfStubAreaEntry *next;
   struct   ospfStubAreaEntry *prev;
   ulng     ospfStubAreaId;   /* 2 keys for searches in the OLL */
   ulng     ospfStubAreaTOS;
   t_Handle owner;

   ospfStubAreaInfo info;
} ospfStubAreaEntry;


/* 3. ospfLsdbTable */

/* ospfLsdbInfo table - "dynamic" information will be retrieved */
/* from the Lsa data base entry                                 */
typedef struct t_ospfLsdbInfo
{
   ulng  seqNo;
   word  lsaAge;
   ulng  chkSum;
   byte  *advertisement;
   word  advertLength;
} ospfLsdbInfo;

typedef struct ospfLsdbEntry
{
   struct ospfLsdbEntry *next;
   struct ospfLsdbEntry *prev;
   ulng   ospfLsdbAreaId;   /* 4 keys for searches in the OLL*/
   ulng   ospfLsdbType;
   ulng   ospfLsdbLsId;
   ulng   ospfLsdbRtrId;

   t_Handle owner;    /* Handle of the area object (ARO) */ 

   ospfLsdbInfo info;
} ospfLsdbEntry;

/* 5. ospfHostTable */

typedef struct t_ospfHostInfo
{
   ulng         metric;
   e_RowStatus  rowStatus;
   ulng         areaId;
   Bool         internalCreated; /* FALSE: created by SNMP
                                    TRUE:  created internally */
   Bool         SNMPdisable; /* TRUE: the row is being disabled
                                by SNMP - do not delete */

} ospfHostInfo;

typedef struct ospfHostEntry
{
   struct ospfHostEntry *next;
   struct ospfHostEntry *prev;
   ulng   ospfHostIpAddres;   /* 2 keys for searches in the OLL*/
   ulng   ospfHostTOS;

   ospfHostInfo info;
} ospfHostEntry;


/* 6. ospfIfTable */

typedef struct t_ospfIfInfo
{
   ulng                areaId;
   e_IFO_Types         ifType;   /* 1 - brdcast, 2- nbma, 3-p_to_p, 5- p-to_mp */
   e_ospfAdminStatus   adminStatus;
   byte                rtrPriority;
   word                transitDelay;
   word                retransInterval;
   word                helloInterval;
   ulng                deadInterval;
   ulng                pollInterval;
/*   e_ospfIfState     ifState,      */  /* State of OSPF Interface state machine */
/*   ulng              desRtr,       */
/*   ulng              backupDesRtr, */
/*   ulng              events,       */
   e_IFO_AuTypes       authType;
   byte                authKey[8];
   byte                keyLen;        /* Length of the key up to 255 */
   e_RowStatus         rowStatus;
   e_MulticastForwConf mtCastForwarding;
   e_TruthValue        demand;
   Bool                internalCreated; /* FALSE: created by SNMP
                                           TRUE:  created internally */
   Bool                SNMPdisable; /* TRUE: the row is being disabled
                                       by SNMP - do not delete */
} ospfIfInfo;


typedef struct ospfIfEntry
{
   struct ospfIfEntry *next;
   struct ospfIfEntry *prev;
   ulng   ospfIfIpAddres;    /* 2 keys for searches in the OLL */
   ulng   ospfAddrLessIf;
   t_Handle owner;           /* handle of the interface object (IFO) */

   ospfIfInfo info;
} ospfIfEntry;


/* 7. ospfIfMetricTable */
/* --------------------*/

/* ospfIfMetric table info */
typedef struct t_ospfIfMetricInfo
{
   word              metricValue;
   e_RowStatus       rowStatus;
   Bool              internalCreated; /* FALSE: created by SNMP
                                         TRUE:  created internally */

}  ospfIfMetricInfo;

typedef struct ospfIfMetricEntry
{
   struct ospfIfMetricEntry *next;
   struct ospfIfMetricEntry *prev;
   ulng   ospfIfMetrIpAddres;   /* 3 keys for searches in the OLL*/
   ulng   ospfIfMetrAddrLessIf;
   ulng   ospfIfMetricTOS;

   ospfIfMetricInfo info;
} ospfIfMetricEntry;


/* 8. ospfVirtIfTable */
/*--------------------*/
/* ospf virtual interface table info */
typedef struct t_ospfVirtIfInfo
{
   word              transitDelay;
   word              retransInterval;   
   word              helloInterval;
   ulng              deadInterval;
/* e_ospfIfState     ifState;  
   ulng              eventCount;  */
   e_IFO_AuTypes     authType;

#if LVL7_ORIGINAL_VENDOR_CODE
   /* no space allocated for the authKey, so this
      causes a machine check when accessed */
   byte             *authKey;
#else
   byte              authKey[8];
#endif
   byte              keyLen;   /* Length of the key up to 255 bytes*/
   e_RowStatus       rowStatus;
   Bool              internalCreated; /* FALSE: created by SNMP
                                         TRUE:  created internally */
   Bool              SNMPdisable; /* TRUE: the row is being disabled
                                     by SNMP - do not delete */
} ospfVirtIfInfo;

typedef struct ospfVirtIfEntry
{
   struct ospfVirtIfEntry *next;
   struct ospfVirtIfEntry *prev;
   ulng   ospfVirtIfAreaId;   /* 3 keys for searches in the OLL*/
   ulng   ospfVirtIfNbr;
   t_Handle owner;

   ospfVirtIfInfo info;
} ospfVirtIfEntry;

/* 9. ospfNbrTable */
/* ----------------*/
typedef struct t_ospfNbrInfo
{
   ulng                 rtrId;
/* ulng                 options; */
   byte                 priority;
/* e_ospfNbrState       nbrState;
   ulng                 eventCount;
   ulng                 lsRetransQLen; */
   e_RowStatus          rowStatus;
   e_ospfPermanence     permanence;
/* Bool                 helloSuppressed; */
   Bool                 internalCreated;

} ospfNbrInfo;

typedef struct ospfNbrEntry
{
   struct ospfNbrEntry *next;
   struct ospfNbrEntry *prev;
   ulng   ospfNbrIpAddres;   /* 2 keys for searches in the OLL*/
   ulng   ospfNbrAddrLessIf;
   t_Handle owner;

   ospfNbrInfo info;
} ospfNbrEntry;


/* 10. ospfVirtNbrTable  */
/*     ----------------  */
/* Ospf virtual neighbor table info. State of the neighbor state machine */
/* and statistics are retrieved from the neighbor object.                */
typedef struct t_ospfVirtNbrInfo
{
/* ulng              IpAddress; */
/* ulng              options; */
/* e_ospfNbrState    state;
   ulng              events;
   ulng              lsRetransQLen; */
   e_RowStatus       rowStatus;  /* just a dummy: not used */
/* Bool              helloSuppressed; */
   
} ospfVirtNbrInfo;

typedef struct ospfVirtNbrEntry
{
   struct ospfVirtNbrEntry *next;
   struct ospfVirtNbrEntry *prev;
   ulng   ospfVirtNbrArea;   /* 2 keys for searches in the OLL*/
   ulng   ospfVirtNbrRtrId;
   t_Handle  owner;

   ospfVirtNbrInfo info;
} ospfVirtNbrEntry;


/* 11. ospfExtLsdbTable  */
/*    -----------------  */

/* ospf external link state data base */
/* All the info is retrieved from the LSA data base entry*/
/* crated when LSA message is received                   */
typedef struct t_ospfExtLsdbInfo
{
     ulng     seqNo;
     ulng     lsaAge;
     ulng     chkSum;
     byte *   advertisement;
     word     advertLength;
} ospfExtLsdbInfo;

typedef struct ospfExtLsdbEntry
{
   struct ospfExtLsdbEntry *next;
   struct ospfExtLsdbEntry *prev;
   ulng   ospfExtLsdbType;    /* 3 keys for searches in the OLL*/
   ulng   ospfExtLsdbLsId;
   ulng   ospfExtLsdbRtrId;

   ospfExtLsdbInfo info;
} ospfExtLsdbEntry;


/* 12. ospfAreaAggregateTable      */
/* --------------------------      */
/* ospf area aggregate table info  */
typedef struct t_ospfAreaAggrInfo
{
   e_RowStatus          rowStatus;
   e_ospfAreaAggrEffect effect;
   Bool                 internalCreated; /* FALSE: created by SNMP
                                            TRUE:  created internally */
   Bool                 SNMPdisable; /* TRUE: the row is being disabled
                                        by SNMP - do not delete */
} ospfAreaAggrInfo;

typedef struct ospfAreaAggrEntry
{
   struct ospfAreaAggrEntry *next;
   struct ospfAreaAggrEntry *prev;
   ulng   ospfAreaAggrAreaId;   /* 4 keys for searches in the OLL*/
   ulng   ospfAreaAggrLsdbType;
   ulng   ospfAreaAggrNet;
   ulng   ospfAreaAggrMask;

   ospfAreaAggrInfo info;
} ospfAreaAggrEntry;


#endif  /* _ospfmib_support_h_ */
