/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename           arp.h
 *
 * @purpose            Definitions internal to the ARP object
 *
 * @component          ARP Component  
 *
 * @comments
 *
 * @create             02/13/2002
 *
 * @author
 *
 * @end
 *
 * ********************************************************************/
#ifndef l7_arp_header_file
#define l7_arp_header_file

#include "arp.ext"
#include "l3_defaultconfig.h"

#define ARP_SEQ_NUM_INIT      1   /* skip 0 for normal usage */
#define ARP_SEQ_NUM_INVALID   0   /* special value when addr entry is freed */

/* ARP defaults used in absence of init parameters */
#define ARP_DEF_AGETIME       FD_IP_DEFAULT_ARP_AGE_TIME      /* default ARP age time */
#define ARP_DEF_RESPTIME      FD_IP_DEFAULT_ARP_RESP_TIME     /* default ARP request timeout */
#define ARP_DEF_RETRNMB       FD_IP_DEFAULT_ARP_RETRIES       /* default ARP number of retries */
#define ARP_DEF_DYNAMICRENEW  FD_IP_DEFAULT_ARP_DYNAMIC_RENEW /* default ARP dynamic renew mode */

#define ARP_PURGE_TIMEOUT         360    /* 6 minutes */
#define ARP_PURGE_THRESHOLD_HIGH  80 /* 80% of total cache size */
#define ARP_MAX_WINNOW_TIME       100   /* msec. Max time looking for old entries. */

/*
 * Choosing the Gateway ARP Request timeout to be less than min bridge timeout
 * to ensure the bridge MAC entries for GW are not aged out.
 * ARP requests will be triggered every ARP_GW_RQSTTIME time for the GW entries.
 * FD_FDB_DEFAULT_AGING_TIMEOUT is the default bridge aging timeout.
 */
#define ARP_GW_RQSTTIME   ((FD_FDB_DEFAULT_AGING_TIMEOUT) / 3)

#define MACADDRLENGTH sizeof(t_MACAddr)
#define IPADDRLENGTH sizeof(t_IPAddr)

#define MAX_INTERFACES        (L7_RTR_MAX_RTR_INTERFACES +1)

#define SRC_IP_OFFSET 12
#define DST_IP_OFFSET 16
#define SRC_IP_OFF_ETH 26
#define DST_IP_OFF_ETH 30
#define ARP_TRANSMIT_QUEUE_SIZE 10
#define ARP_REQUESTS_QUEUE_SIZE 10

#define GET_DSTIP_FROM_FRAME(p_frame, p_ip) (void)F_Read(p_ip, p_frame, (word)DST_IP_OFFSET, (word)IPADDRLENGTH)
#define GET_SRCIP_FROM_FRAME(p_frame, p_ip) (void)F_Read(p_ip, p_frame, (word)SRC_IP_OFFSET, (word)IPADDRLENGTH)
#define GET_DSTIP_FROM_ETH_FRAME(p_frame,p_ip) (void)F_Read(p_ip,p_frame, (word)DST_IP_OFF_ETH, (word)IPADDRLENGTH)
#define GET_SRCIP_FROM_ETH_FRAME(p_frame,p_ip) (void)F_Read(p_ip,p_frame, (word)SRC_IP_OFF_ETH, (word)IPADDRLENGTH)

/* ARP frame definition */
typedef struct t_ARPFrame
{
   byte       hwType[2]; /* hardware type */
   byte       prType[2]; /* protocol type */
   byte       hwLen;     /* HW addr length */
   byte       prLen;     /* Protocol addr length */
   byte       opCode[2]; /* operation */
   byte       srcMAC[6]; /* source MAC address */
   byte       srcIP[4];  /* source IP address  */
   byte       dstMAC[6]; /* target MAC address */
   byte       dstIP[4];  /* target IP address  */
}t_ARPFrame;

/* Source ADDR entry structure definition */
typedef struct t_ADDREnt
{
    struct t_ADDREnt  *next;
    struct t_ADDREnt  *prev;
    t_MACAddr         macAddr;
    t_IPAddr          ipAddr;       /* network byte order */
    addrConflictStats_t  addrConflictStats; /* address conflict statistics
                                             * for this IP address */
    word              intfNum;
}t_ADDREnt;

/* Layer 2 interface structure */
typedef struct t_ARPIf
{
   F_ARPTransmit    f_Transmit;  /* low layer transmit routine pointer */
   t_Handle         lowId;       /* low layer transmit Id */
   t_LIH            number;      /* interface number */
   Bool             addrValid;   /* local interface IP address is assigned */
}t_ARPIf;

/* ARP user structure definition */
typedef struct t_ARPUser
{
    struct t_ARPUser  *next;
    struct t_ARPUser  *prev;
    F_ARPEventCallback f_User;
    t_Handle           userId;
    struct t_ARP      *arp;
}t_ARPUser;

/* ARP object states */
typedef enum
{
    ARP_STATE_IDLE     = 0,
    ARP_STATE_ACTIVE   = 1,
    ARP_STATE_DISABLED = 2
}t_ARPState;

/* ARP object instance */
typedef struct t_ARP
{
   struct t_ARP     *next;       
   struct t_ARP     *prev;
   word             status;      /* current status */
   word             state;       /* ARP object state */
   t_Handle         userId;      /* user Id */
   t_ADDREnt        *addrEnt;    /* object address entry */ 
   word             arpAgeTime;  /* arp age time (in seconds) */
   word             arpRespTime; /* arp response timeout (in seconds) */
   word             retrNmb;     /* number of retransmitions */
   word             arpCacheSize;/* max number of ARP entries */
   word             dynamicRenew;/* dynamic entry renew mode */
   F_ARPNAKCallback f_NAK;       /* NAK callback pointer */
   t_Handle         arpCacheId;  /* ARP cache (sorted by IP address) */
   t_ARPIf          *p_if[MAX_INTERFACES];
   word             queueSize;   /* frame queue size */
   ulng             arpReqRecvCt; /* debug ctr: ARP requests received */
   ulng             arpRepRecvCt; /* debug ctr: ARP replies received */
   ulng             arpReqSentCt; /* debug ctr: ARP requests sent */
   ulng             arpRepSentCt; /* debug ctr: ARP replies sent */
   ulng             gratArpSentCt; /* debug ctr: Gratuitous ARPs sent */
   ulng             earlyDeletes;  /* Number of entries deleted prior to aging 
                                    * out in order to make space for newer 
                                    * entries */
}t_ARP;

/* request entry structure definition */
typedef struct t_RQE
{
   struct  t_RQE    *next;
   struct  t_RQE    *prev;
   struct  t_ADR    *dstAddrId; /* destination address entry pointer */
   t_Handle         userId;     /* request user Id */
   t_Lay3Addr       l3addr;     /* layer 3 requested address */
}t_RQE;

/* destination ADR entry structure definition */
typedef struct t_ADR
{
   struct t_ADR  *next;
   struct t_ADR  *prev;
   void          *pNode;       /* back pointer to container node construct */
   t_ARP         *p_obj;       /* ARP object pointer */
   t_RQE         *request;     /* request entry pointer */
   word          status;       /* current status */

   /* Incremented each time this entry is used for software forwarding. 
    * When it reaches a threshold, send an out-of-cycle ARP entry to 
    * repopulate the L2 table, in case an L2 table miss is the reason 
    * the packet came to the CPU. Reset to 0 whenever an out of cylce
    * ARP entry sent. */
   word          swFwdCount; 

   t_MACAddr     macAddr;      /* destination MAC address */
   t_IPAddr      ipAddr;       /* destination IP address */
   t_Handle      arpAgeTimer;  /* arp age timer */
   t_Handle      arpRspTimer;  /* arp response timer */
   word          retrNmb;      /* current number of retransmitions */
   word          intfNum;      /* interface number */
   ulng          timeStamp;    /* time stamp (0 means entry not aged) */
   word          entryType;    /* type of ARP entry */
   Bool          permanent;    /* denotes non-aged, cfg entry (local, static) */
   ulng          seqNum;       /* addr entry creation sequence number */
   L7_uint32     interfaceNumber; /* internal interface number */
   L7_uint32     lastHitTime;  /* Last time the entry was hit in hardware (in secs) */
}t_ADR;

#endif
/**********************end of file********************/
