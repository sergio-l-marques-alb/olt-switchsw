/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename              sppck.h
 *
 * @purpose               Packets and IG format definitions and typedefs
 *                        for OSPF
 *
 * @component             Routing OSPF Component 
 *
 * @comments
 *
 * @create                04/24/2002
 *
 * @author
 *
 * @end
 *
 * ********************************************************************/
#ifndef sppck_h
#define sppck_h

/* Typical length of IP header (bytes) */
#define IP_HDR_LEN 20

/* Length of MD5 authentication data (appended to OSPF packet) */
#define OSPF_MD5_AUTH_DATA_LEN 16

extern const L7_uint32 LSA_HEADER_LEN;
/* ***********************************************************

                     General definitions
  S_ - OSPF SW uniqal prefix
   *********************************************************** */

/*   +------------------------------------+  */
/*   | V | O | DC | EA | N/P | MC | E | Q |  */
/*   +------------------------------------+  */

typedef enum e_OSPF_Options
{
   OSPF_OPT_Q_BIT    =  0x01,
   OSPF_OPT_E_BIT    =  0x02,
   OSPF_OPT_MC_BIT   =  0x04,

  /* NSSA Change: Add N bit to support NSSA */
   OSPF_OPT_N_BIT    =  0x08,
   OSPF_OPT_P_BIT    =  0x08,


   OSPF_OPT_O_BIT    =  0x40,
   VPN_OPTION_BIT    =  0x80
}  e_OSPF_Options;

/* OSPF Packet Types */
typedef enum e_S_PckType
{
	S_ILLEGAL      ,
	S_HELLO        ,
	S_DB_DESCR     ,
	S_LS_REQUEST   ,
	S_LS_UPDATE    ,
	S_LS_ACK       ,
   S_LAST_PACKET_TYPE = S_LS_ACK,
   S_IGMP_QUERY,
   S_IGMP_REPORT
}  e_S_PckType;

extern unsigned char *ospfPckTypeNames[S_LAST_PACKET_TYPE+1];

/* OSPF Packets Header structure */

typedef struct t_S_PckHeader
{
   byte           Version;
	byte           Type;
	byte	         Length[2];
   byte           RouterId[4];
   byte           AreaId[4];
   byte           CheckSum[2];
   byte           AuType[2];
   byte           AuKey[8];
}  t_S_PckHeader;

/* ***********************************************************

       OSPF Packets Format definitions

   *********************************************************** */

/* ------------------------  Hello Packet        ------------ */

typedef struct t_S_Hello
{
   byte           NetworkMask[4];
   byte           HelloInterval[2];
   byte           Options;
   byte           RouterPriority;
   byte           RouterDeadInterval[4];
   byte           DesignatedRouter[4];
   byte           BackupRouter[4];
   /* followed by list of Neighbors */
}  t_S_Hello;

/* -------------------  Database Description Packet  ------------ */

typedef struct t_S_DbDscr
{
   byte           InterfaceMtu[2];
   byte           Options;
   byte           Flags;
   byte           SeqNum[4];
   /* followed by list of LSA headers */
}  t_S_DbDscr;

#define S_INIT_FLAG   4
#define S_MORE_FLAG   2 
#define S_MASTER_FLAG 1

/* -------------------  Link State Request Packet  ------------ */

/* number of following LsReq may be present in this packet */
typedef struct t_S_LsReq
{
   byte           LsType[4];
   byte           LsId[4];
   byte           AdvertisingRouter[4];
}  t_S_LsReq;

/* -------------------  Link State Update Packet  ------------ */

typedef struct t_S_LsUpdate
{
   byte           LsNum[4];
   /* followed by list of LSAs */
}  t_S_LsUpdate;

/* -------------------  Link State Ack Packet  ------------ */

   /* OSPF header followed by list of LSA headers */

/* ***********************************************************

                     LSAs  Format definitions

   *********************************************************** */

/* OSPF Link State (LS) Types Coding */
typedef enum e_S_LScodes
{
   S_ILLEGAL_LSA       ,
   S_ROUTER_LSA        ,
   S_NETWORK_LSA       ,
   S_IPNET_SUMMARY_LSA ,
   S_ASBR_SUMMARY_LSA  ,
   S_AS_EXTERNAL_LSA   ,
   S_GROUP_MEMBER_LSA  ,
   S_NSSA_LSA          ,  /* NSSA Change */
   S_TMP2_LSA          ,
   S_LINK_OPAQUE_LSA   ,
   S_AREA_OPAQUE_LSA   ,
   S_AS_OPAQUE_LSA     ,
   S_LAST_LSA_TYPE
}  e_S_LScodes;

extern unsigned char *lsaTypeNames[S_LAST_LSA_TYPE];

/* LSA Header structure */

typedef struct t_S_LsaHeader
{
   byte           LsAge[2];
	byte           Options;
	byte	         LsType;
   byte           LsId[4];
   byte           AdvertisingRouter[4];
   byte           SeqNum[4];
   byte           CheckSum[2];
   byte           Length[2];
}  t_S_LsaHeader;

/* -------------------  Router-LSA   ------------ */

typedef struct t_S_RouterLsa
{
   byte           Flags;
   byte           Reserved;
   byte           LinkNum[2];
   /* followed by list of LinkDscr */
}  t_S_RouterLsa;

typedef struct t_S_LinkDscr
{
   byte           LinkId[4];
   byte           LinkData[4];
   byte           Type;
   byte           TosNum;
   byte           Metric[2];
   /* optionaly followed by list of TOS */
}  t_S_LinkDscr;

typedef struct t_S_Tos
{
   byte           Tos;
   byte           Reserved;
   byte           TosMetric[2];
}  t_S_Tos;

#define S_NSSATRANSLATOR_FLAG  0x10
#define S_WILDCARD_FLAG   0x8
#define S_VIRTUAL_FLAG    0x4
#define S_EXTERNAL_FLAG   0x2 
#define S_BORDER_FLAG     0x1

typedef enum e_LinkTypes
{
   S_POINT_TO_POINT  = 1,
   S_TRANSIT_NET     = 2,
   S_STUB_NET        = 3,
   S_VIRTUAL_LINK    = 4
}  e_LinkTypes;

/* -------------------  Network-LSA   ------------ */

/* number of following NetworkLsa may be present in this LSA 
   byte           NetworkMask[4];
   byte           AttachedRouter[4];
   ...............................
   byte           AttachedRouter[4];
*/
/* -------------------  Summary-LSA   ------------ */

typedef struct t_S_SummaryLsa
{
   byte           NetworkMask[4];
   byte           Reserved;
   byte           Metric[3];
   /* optionaly followed by list of TOS */
}  t_S_SummaryLsa;

/* For each TOS supported, the optional information */
typedef struct t_S_SummaryTos
{
   byte           Tos;
   byte           TosMetric[3];
} t_S_SummaryTos;

/* -------------------  AS-external-LSA   ------------ */

typedef struct t_S_AsExternalLsa
{
   byte           NetworkMask[4];
   byte           Flags;
   byte           Metric[3];
   byte           ForwardingAddress[4];
   byte           ExternalRouteTag[4];
   /* optionaly followed by list of TOS */
}  t_S_AsExternalLsa;

#define S_AS_EXT_E_FLAG 0x80

typedef struct t_S_AsExtTos
{
   byte           Tos;
   byte           TosMetric[3];
   byte           ForwardingAddress[4];
   byte           ExternalRouteTag[4];
}  t_S_AsExtTos;

/* -------------------  Group-membership-LSA   ------------ */

typedef struct t_S_GroupMembLsa
{
   byte           VertexType[4];
   byte           VertexId[4];
}  t_S_GroupMembLsa;

#define ROUTER_VERTEX_TYPE   1
#define TNETWORK_VERTEX_TYPE 2


/* Traffic Engineering extension definitons */

#define TE_OPAQUE_LSA 1
#define TE_LSA_INST_MAX_NUMBER  0x10000     /* 65536  */
#define TE_LSA_ROUTER_INST      0xFFFF
#define TE_LSA_ID_4B            0x01000000  /* 4 bytes of LSA id */
                                           

typedef enum e_TeTlvTypes
{
   TE_TLV_ROUTER  = 1,
   TE_TLV_LINK    = 2
}  e_TeTlvTypes;

typedef enum e_TeLinkSbTlvTypes
{
   TELK_TYPE         = 1, /* Link type                    (1 octet)   */
   TELK_ID           = 2, /* Link ID                      (4 octets)  */
   TELK_LOC_IP_ADR   = 3, /* Local interface IP address   (4 octets)  */
   TELK_REM_IP_ADR   = 4, /* Remote interface IP address  (4 octets)  */
   TELK_METRIC       = 5, /* Traffic engineering metric   (4 octets)  */
   TELK_MX_BDWTH     = 6, /* Maximum bandwidth            (4 octets)  */
   TELK_RSRV_BDWTH   = 7, /* Maximum reservable bandwidth (4 octets)  */
   TELK_UNRSRV_CLS0  = 8, /* Unreserved bandwidth Class 0 (32 octets) */
   TELK_RSRC_COLOR   = 9, /* Resource class/color         (4 octets)  */
   TELK_MUX_CAPABILTY= 10, /* Link Mux Capability */
   TELK_OUT_IF       = 11, /* Outgoing Interface Identifier */
   TELK_INC_IF       = 12, /* Incoming Interface Identifier */
   TELK_MX_LSP_BDWTH = 13, /* Maximum LSP bandwidth         */
   TELK_PROTECTION   = 14, /* Link Protection Type */
   TELK_DESCRIPTOR   = 15, /* Link Descriptor     */
   TELK_SRLG         = 16, /* Shared Risk Link Group (4 * Num of SRLGs in link) */
   TELK_FA_PATH      = 17, /* FA Path */
   TELK_UNRSRV_CLS1  = 18, /* Unreserved bandwidth Class 1 (32 octets) */
   TELK_UNRSRV_CLS2  = 19, /* Unreserved bandwidth Class 2 (32 octets) */
   TELK_UNRSRV_CLS3  = 20  /* Unreserved bandwidth Class 3 (32 octets) */

}  e_TeLinkSbTlvTypes;

#define TE_TLV_LINK_LEN  1500   
#define TE_TLV_ROUTER_LEN (4+4)

#endif /* sppck_h */

/* --- end of file sppck.h --- */

