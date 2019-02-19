
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename   diffserv_migrate.h
*
* @purpose    DiffServ Configuration Migration
*
* @component  DiffServ
*
* @comments   none
*
* @create     08/24/2004
*
* @author     Rama Sasthri, Kristipati
* @end
*
**********************************************************************/

#ifndef DIFFSERV_MIGRATE_H
#define DIFFSERV_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"
#include "l7_diffserv_include.h"


typedef struct
{
  L7_uint32 index;              /* row index (N-A)            */
  L7_uchar8 name[L7_USMDB_MIB_DIFFSERV_CLASS_NAME_MAX + 1];     /* name string */
  dsmibClassType_t type;        /* type of class definition   */
  L7_uint32 aclNum;             /* ACL number (for class type acl) */
  L7_uint32 ruleIndexNextFree;  /* next avail rule index (R-O) */
  dsmibStorageType_t storageType;       /* type of config storage     */
  dsmibRowStatus_t rowStatus;   /* status of this row         */

} dsmibClassMibV1_t;




/*---------------------------------------------------------------------------*/
/*
    dsmibClassFlagsV1_t  - present in RELEASE 4.0.0  (G)
    dsmibClassFlagsV2_t  - introduced in RELEASE 4.3 (I)
*/
/*---------------------------------------------------------------------------*/



/* MIB entry flags */
typedef enum
{
  DSMIB_CLASS_FLAGS_NAME_V1 =                      (1 << 0),
  DSMIB_CLASS_FLAGS_TYPE_V1 =                      (1 << 1),
  DSMIB_CLASS_FLAGS_ACLNUM_V1 =                    (1 << 2)

} dsmibClassFlagsV1_t;

#define DSMIB_CLASS_FLAGS_COMMON_V1 \
                (DSMIB_CLASS_FLAGS_NAME_V1 | \
                 DSMIB_CLASS_FLAGS_TYPE_V1 | \
                 DSMIB_CLASS_FLAGS_ACLNUM_V1)



/* MIB entry flags */
typedef enum
{
  DSMIB_CLASS_FLAGS_NAME_V2 =                      (1 << 0),
  DSMIB_CLASS_FLAGS_TYPE_V2 =                      (1 << 1),
  DSMIB_CLASS_FLAGS_ACLTYPE_V2 =                   (1 << 2),
  DSMIB_CLASS_FLAGS_ACLNUM_V2 =                    (1 << 3)

} dsmibClassFlagsV2_t;

#define DSMIB_CLASS_FLAGS_COMMON_V2 \
                (DSMIB_CLASS_FLAGS_NAME_V2 | \
                 DSMIB_CLASS_FLAGS_TYPE_V2 | \
                 DSMIB_CLASS_FLAGS_ACLTYPE_V2 | \
                 DSMIB_CLASS_FLAGS_ACLNUM_V2)



/*---------------------------------------------------------------------------*/
/*
    L7_diffServClassHdrV1_t  - present in RELEASE 4.0.0  (G)
    L7_diffServClassHdrV2_t  - introduced in RELEASE 4.3 (I)
*/
/*---------------------------------------------------------------------------*/
typedef struct
{
  L7_BOOL inUse;                /* array entry in use (T/F)   */
  L7_uint32 chainIndex;         /* first class rule entry     */
  dsmibClassMibV1_t mib;        /* MIB content                */
  dsmibClassFlagsV1_t   rowInvalidFlags;    /* row invalid object flags   */

} L7_diffServClassHdrV1_t;





/*---------------------------------------------------------------------------*/
/*
    dsmibClassRuleFlagsV1_t  - present in RELEASE 4.0.0  (G)
    dsmibClassRuleFlagsV2_t  - introduced in RELEASE 4.3 (I)
*/
/*---------------------------------------------------------------------------*/



/* MIB entry flags */
typedef enum
{
  DSMIB_CLASS_RULE_FLAGS_MATCH_ENTRYTYPE_V1 =      (1 << 0),
  DSMIB_CLASS_RULE_FLAGS_MATCH_COS_V1 =            (1 << 1),
  DSMIB_CLASS_RULE_FLAGS_MATCH_IPADDR_V1 =         (1 << 2),
  DSMIB_CLASS_RULE_FLAGS_MATCH_IPMASK_V1 =         (1 << 3),
  DSMIB_CLASS_RULE_FLAGS_MATCH_PORTSTART_V1 =      (1 << 4),
  DSMIB_CLASS_RULE_FLAGS_MATCH_PORTEND_V1 =        (1 << 5),
  DSMIB_CLASS_RULE_FLAGS_MATCH_MACADDR_V1 =        (1 << 6),
  DSMIB_CLASS_RULE_FLAGS_MATCH_MACMASK_V1 =        (1 << 7),
  DSMIB_CLASS_RULE_FLAGS_MATCH_IPDSCP_V1 =         (1 << 8),
  DSMIB_CLASS_RULE_FLAGS_MATCH_IPPRECEDENCE_V1 =   (1 << 9),
  DSMIB_CLASS_RULE_FLAGS_MATCH_IPTOSBITS_V1 =      (1 << 10),
  DSMIB_CLASS_RULE_FLAGS_MATCH_IPTOSMASK_V1 =      (1 << 11),
  DSMIB_CLASS_RULE_FLAGS_MATCH_PROTOCOLNUM_V1 =    (1 << 12),
  DSMIB_CLASS_RULE_FLAGS_MATCH_REFCLASSINDEX_V1 =  (1 << 13),
  DSMIB_CLASS_RULE_FLAGS_MATCH_VLANID_V1 =         (1 << 14),
  DSMIB_CLASS_RULE_FLAGS_MATCH_EXCLUDEFLAG_V1 =    (1 << 15)

} dsmibClassRuleFlagsV1_t;


#define DSMIB_CLASS_RULE_FLAGS_ALL_V1 \
          (DSMIB_CLASS_RULE_FLAGS_MATCH_ENTRYTYPE_V1 | \
           DSMIB_CLASS_RULE_FLAGS_MATCH_COS_V1 | \
           DSMIB_CLASS_RULE_FLAGS_MATCH_IPADDR_V1 | \
           DSMIB_CLASS_RULE_FLAGS_MATCH_IPMASK_V1 | \
           DSMIB_CLASS_RULE_FLAGS_MATCH_PORTSTART_V1 | \
           DSMIB_CLASS_RULE_FLAGS_MATCH_PORTEND_V1 | \
           DSMIB_CLASS_RULE_FLAGS_MATCH_MACADDR_V1 | \
           DSMIB_CLASS_RULE_FLAGS_MATCH_MACMASK_V1 | \
           DSMIB_CLASS_RULE_FLAGS_MATCH_IPDSCP_V1 | \
           DSMIB_CLASS_RULE_FLAGS_MATCH_IPPRECEDENCE_V1 | \
           DSMIB_CLASS_RULE_FLAGS_MATCH_IPTOSBITS_V1 | \
           DSMIB_CLASS_RULE_FLAGS_MATCH_IPTOSMASK_V1 | \
           DSMIB_CLASS_RULE_FLAGS_MATCH_PROTOCOLNUM_V1 | \
           DSMIB_CLASS_RULE_FLAGS_MATCH_REFCLASSINDEX_V1 | \
           DSMIB_CLASS_RULE_FLAGS_MATCH_VLANID_V1 | \
           DSMIB_CLASS_RULE_FLAGS_MATCH_EXCLUDEFLAG_V1)



/*---------------------------------------------------------------------------*/
/*
    L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_V1_t  - present in RELEASE 4.0.0  (G)
    L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_V3_t  - introduced in RELEASE 4.3 (I)
*/
/*---------------------------------------------------------------------------*/



/* Values for DiffServ Class Rule types supported as match conditions */
typedef enum
{
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE_V1 = 0,

  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS_V1,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIP_V1,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTL4PORT_V1,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTMAC_V1,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_EVERY_V1,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPDSCP_V1,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPPRECEDENCE_V1,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPTOS_V1,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_PROTOCOL_V1,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_REFCLASS_V1,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIP_V1,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCL4PORT_V1,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCMAC_V1,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLAN_V1,

  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_TOTAL_V1   /* number of enum entries */

} L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_V1_t;


typedef L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_V1_t dsmibClassRuleTypeV1_t;


/*---------------------------------------------------------------------------*/
/*
    L7_diffServClassRuleV1_t  - present in RELEASE 4.0.0  (G)
    L7_diffServClassRuleV3_t  - introduced in RELEASE 4.3 (I)
*/
/*---------------------------------------------------------------------------*/


/* type-specific match fields */
typedef union
{
  /* NOTE:  Only one of the following structures is valid, depending on
   *        the current value of the class rule 'entryType' field.
   */

                                                /* COS type:                  */
  L7_uint32         cos;                        /*   class of service (0-7)   */

  struct 
  {                                             /* Dst IP type:               */
    L7_uint32       addr;                       /*   IP address               */
    L7_uint32       mask;                       /*   IP address mask          */
  } dstIp;

  struct 
  {                                             /* Dst Layer 4 Port type:     */
    L7_uint32       start;                      /*   start of port range      */
    L7_uint32       end;                        /*   end of port range        */
  } dstL4Port;

  struct 
  {                                             /* Dst MAC Address type:      */
    L7_uchar8       addr[L7_MAC_ADDR_LEN];      /*   MAC address              */
    L7_uchar8       mask[L7_MAC_ADDR_LEN];      /*   MAC address mask         */
  } dstMac;

                                                /* Match Every type:          */
  dsmibTruthValue_t matchEvery;                 /*   match every flag (R-O)   */

                                                /* IP DSCP type:              */
  L7_uint32         ipDscp;                     /*   DiffServ Code Point(0-63)*/

                                                /* IP Precedence type:        */
  L7_uint32         ipPrecedence;               /*   precedence value (0-7)   */

  struct                                                                        
  {                                             /* IP TOS type (free form):   */
    L7_uchar8       bits;                       /*   all 8 TOS field bits     */
    L7_uchar8       mask;                       /*   mask for each TOS bit    */
    L7_uchar8       rsvd1[2];                   /* (reserved -- for alignment)*/
  } ipTos; 

                                                /* Protocol type:             */
  L7_uint32         protocolNum;                /*   IANA protocol number     */

                                                /* Reference Class type:      */
  L7_uint32         refClassIndex;              /*   reference class index    */

  struct 
  {                                             /* Src IP type:               */
    L7_uint32       addr;                       /*   IP address               */
    L7_uint32       mask;                       /*   IP address mask          */
  } srcIp;

  struct 
  {                                             /* Src Layer 4 Port type:     */
    L7_uint32       start;                      /*   start of port range      */
    L7_uint32       end;                        /*   end of port range        */
  } srcL4Port;

  struct 
  {                                             /* Src MAC Address type:      */
    L7_uchar8       addr[L7_MAC_ADDR_LEN];      /*   MAC address              */
    L7_uchar8       mask[L7_MAC_ADDR_LEN];      /*   MAC address mask         */
  } srcMac;

                                                /* VLAN type:                 */
  L7_uint32         vlanId;                     /*   VLAN identifier          */

} dsmibClassRuleDataV1_t;


/* MIB entry definition */
typedef struct
{
  L7_uint32             index;                  /* row index (N-A)            */
  dsmibClassRuleTypeV1_t  entryType;              /* defines class rule union   */
  dsmibClassRuleDataV1_t  match;                  /* type-based rule match info */
  dsmibTruthValue_t     excludeFlag;            /* F=include, T=exclude       */
  dsmibStorageType_t    storageType;            /* type of config storage     */
  dsmibRowStatus_t      rowStatus;              /* status of this row         */

} dsmibClassRuleMibV1_t;


typedef struct
{
  L7_uint32               hdrIndexRef;          /* back ptr (non-0 = in use)  */
  L7_uint32               chainIndex;           /* next sequential rule entry */
  dsmibClassRuleMibV1_t   mib;                  /* MIB content                */
  dsmibClassRuleFlagsV1_t rowInvalidFlags;      /* row invalid object flags   */
  L7_uint32               arid;                 /* ACL rule identifier        */

} L7_diffServClassRuleV1_t;



/*---------------------------------------------------------------------------*/
/*
    dsmibPoliceActV1_t  - present in RELEASE 4.0.0  (G)
    dsmibPoliceActV3_t  - introduced in RELEASE 4.3 (I)
*/
/*---------------------------------------------------------------------------*/

/* Values for DiffServ Police Action type */
typedef enum
{
  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_NONE_V1 = 0,

  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_DROP_V1,
  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP_V1,
  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC_V1,
  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_SEND_V1,

  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_TOTAL_V1     /* number of enum entries */

} L7_USMDB_MIB_DIFFSERV_POLICE_ACT_V1_t;

/* Values for DiffServ Bandwidth units type */
typedef enum
{
  L7_USMDB_MIB_DIFFSERV_BW_UNITS_NONE_V1 = 0,
  L7_USMDB_MIB_DIFFSERV_BW_UNITS_KBPS_V1,
  L7_USMDB_MIB_DIFFSERV_BW_UNITS_PERCENT_V1

} L7_USMDB_MIB_DIFFSERV_BW_UNITS_V1_t;

typedef L7_USMDB_MIB_DIFFSERV_POLICE_ACT_V1_t      dsmibPoliceActV1_t;

typedef L7_USMDB_MIB_DIFFSERV_BW_UNITS_V1_t        dsmibBwUnitsV1_t;

/* type-specific attribute fields */
typedef union
{
  /* NOTE:  Only one of the following structures is valid, depending on
   *        the current value of the policy attribute 'entryType' field.
   */

  struct
  {                             /* Bandwidth type:            */
    L7_uint32 crate;            /*   committed rate (CIR)     */
    dsmibBwUnitsV1_t crateUnits;  /*     Kbps, percent          */
  } bandwidth;

  struct
  {                             /* Expedite type:             */
    L7_uint32 crate;            /*   committed rate (CIR)     */
    dsmibBwUnitsV1_t crateUnits;  /*     Kbps, percent          */
    L7_uint32 cburst;           /*   committed burst (CBS)    */
  } expedite;

  struct
  {                             /* Mark COS type:             */
    L7_ushort16 val;            /*   Class of Service (0-7)   */
    L7_uchar8 rsvd1[2];         /* (reserved -- for alignment) */
  } markCos;

  struct
  {                             /* Mark IP DSCP type:         */
    L7_uchar8 val;              /*   DiffServ Code Point(0-63) */
    L7_uchar8 rsvd1[3];         /* (reserved -- for alignment) */
  } markIpDscp;

  struct
  {                             /* Mark IP Precedence type:   */
    L7_uchar8 val;              /*   IP Precedence (0-7)      */
    L7_uchar8 rsvd1[3];         /* (reserved -- for alignment) */
  } markIpPrecedence;

  struct
  {                             /* Police Simple type:        */
    L7_uint32 crate;            /*   committed rate (CIR)     */
    L7_uint32 cburst;           /*   committed burst (CBS)    */
    dsmibPoliceActV1_t conformAct;      /*   conform action           */
    dsmibPoliceActV1_t nonconformAct;   /*   nonconform action        */
    L7_uchar8 conformVal;       /*   conform DSCP mark val    */
    L7_uchar8 nonconformVal;    /*   nonconform DSCP mark val */
    L7_uchar8 rsvd1[2];         /* (reserved -- for alignment) */
  } policeSimple;

  struct
  {                             /* Police Single Rate type:   */
    L7_uint32 crate;            /*   committed rate (CIR)     */
    L7_uint32 cburst;           /*   committed burst (CBS)    */
    L7_uint32 eburst;           /*   excess burst (EBS)       */
    dsmibPoliceActV1_t conformAct;      /*   conform action           */
    dsmibPoliceActV1_t exceedAct;       /*   exceed action            */
    dsmibPoliceActV1_t nonconformAct;   /*   nonconform action        */
    L7_uchar8 conformVal;       /*   conform DSCP mark val    */
    L7_uchar8 exceedVal;        /*   exceed DSCP mark val     */
    L7_uchar8 nonconformVal;    /*   nonconform DSCP mark val */
    L7_uchar8 rsvd1[1];         /* (reserved -- for alignment) */
  } policeSingleRate;

  struct
  {                             /* Police Single Rate type:   */
    L7_uint32 crate;            /*   committed rate (CIR)     */
    L7_uint32 cburst;           /*   committed burst (CBS)    */
    L7_uint32 prate;            /*   peak rate (PIR)          */
    L7_uint32 pburst;           /*   peak burst (PBS)         */
    dsmibPoliceActV1_t conformAct;      /*   conform action           */
    dsmibPoliceActV1_t exceedAct;       /*   exceed action            */
    dsmibPoliceActV1_t nonconformAct;   /*   nonconform action        */
    L7_uchar8 conformVal;       /*   conform DSCP mark val    */
    L7_uchar8 exceedVal;        /*   exceed DSCP mark val     */
    L7_uchar8 nonconformVal;    /*   nonconform DSCP mark val */
    L7_uchar8 rsvd1[1];         /* (reserved -- for alignment) */
  } policeTwoRate;

  struct
  {                             /* Random Drop type:          */
    L7_uint32 minThresh;        /*   begin threshold (bytes)  */
    L7_uint32 maxThresh;        /*   end threshold (bytes)    */
    L7_uint32 maxDropProb;      /*   maximum drop probability */
    L7_uint32 samplingRate;     /*   queue sampling rate      */
    L7_uint32 decayExponent;    /*   decay factor exponent    */
  } randomDrop;

  struct
  {                             /* Shape Average type:        */
    L7_uint32 crate;            /*   committed rate (CIR)     */
  } shapeAverage;

  struct
  {                             /* Shape Peak type:           */
    L7_uint32 crate;            /*   committed rate (CIR)     */
    L7_uint32 prate;            /*   peak rate (PIR)          */
  } shapePeak;

} dsmibPolicyAttrDataV1_t;

/* Values for DiffServ Policy Attribute statement type */
typedef enum
{
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE_V1 = 0,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_BANDWIDTH_V1,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_EXPEDITE_V1,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL_V1,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL_V1,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL_V1,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE_V1,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE_V1,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE_V1,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_RANDOMDROP_V1,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_SHAPE_AVERAGE_V1,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_SHAPE_PEAK_V1,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_TOTAL_V1       /* number of enum entries */
} L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_V1_t;

typedef L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_V1_t dsmibPolicyAttrTypeV1_t;


/*---------------------------------------------------------------------------*/
/*
    dsmibPolicyAttrFlagsV1_t  - present in RELEASE 4.0.0  (G)
    dsmibPolicyAttrFlagsV3_t  - introduced in RELEASE 4.3 (I)
*/
/*---------------------------------------------------------------------------*/


/* MIB entry flags (limited to at most 32 flag values) */
typedef enum
{
  DSMIB_POLICY_ATTR_FLAGS_STMT_ENTRYTYPE_V1 =        (1 << 0),
  DSMIB_POLICY_ATTR_FLAGS_STMT_CRATE_V1 =            (1 << 1),
  DSMIB_POLICY_ATTR_FLAGS_STMT_CRATEUNITS_V1 =       (1 << 2),
  DSMIB_POLICY_ATTR_FLAGS_STMT_CBURST_V1 =           (1 << 3),
  DSMIB_POLICY_ATTR_FLAGS_STMT_EBURST_V1 =           (1 << 4),
  DSMIB_POLICY_ATTR_FLAGS_STMT_PRATE_V1 =            (1 << 5),
  DSMIB_POLICY_ATTR_FLAGS_STMT_PBURST_V1 =           (1 << 6),
  DSMIB_POLICY_ATTR_FLAGS_STMT_COSVAL_V1 =           (1 << 7),
  DSMIB_POLICY_ATTR_FLAGS_STMT_IPDSCPVAL_V1 =        (1 << 8),
  DSMIB_POLICY_ATTR_FLAGS_STMT_IPPRECEDENCEVAL_V1 =  (1 << 9),
  DSMIB_POLICY_ATTR_FLAGS_STMT_CONFORMACT_V1 =       (1 << 10),  /* defaulted */ 
  DSMIB_POLICY_ATTR_FLAGS_STMT_EXCEEDACT_V1 =        (1 << 11),  /* defaulted */ 
  DSMIB_POLICY_ATTR_FLAGS_STMT_NONCONFORMACT_V1 =    (1 << 12),  /* defaulted */ 
  DSMIB_POLICY_ATTR_FLAGS_STMT_CONFORMVAL_V1 =       (1 << 13),                  
  DSMIB_POLICY_ATTR_FLAGS_STMT_EXCEEDVAL_V1 =        (1 << 14),                  
  DSMIB_POLICY_ATTR_FLAGS_STMT_NONCONFORMVAL_V1 =    (1 << 15),                  
  DSMIB_POLICY_ATTR_FLAGS_STMT_MINTHRESH_V1 =        (1 << 16),                  
  DSMIB_POLICY_ATTR_FLAGS_STMT_MAXTHRESH_V1 =        (1 << 17),                  
  DSMIB_POLICY_ATTR_FLAGS_STMT_MAXDROPPROB_V1 =      (1 << 18),                  
  DSMIB_POLICY_ATTR_FLAGS_STMT_SAMPLINGRATE_V1 =     (1 << 19),  /* defaulted */ 
  DSMIB_POLICY_ATTR_FLAGS_STMT_DECAYEXPONENT_V1 =    (1 << 20)   /* defaulted */ 

} dsmibPolicyAttrFlagsV1_t;


#define DSMIB_POLICY_ATTR_FLAGS_ALL_V1 \
          (DSMIB_POLICY_ATTR_FLAGS_STMT_ENTRYTYPE_V1 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_CRATE_V1 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_CRATEUNITS_V1 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_CBURST_V1 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_EBURST_V1 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_PRATE_V1 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_PBURST_V1 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_COSVAL_V1 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_IPDSCPVAL_V1 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_IPPRECEDENCEVAL_V1 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_CONFORMACT_V1 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_EXCEEDACT_V1 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_NONCONFORMACT_V1 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_CONFORMVAL_V1 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_EXCEEDVAL_V1 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_NONCONFORMVAL_V1 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_MINTHRESH_V1 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_MAXTHRESH_V1 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_MAXDROPPROB_V1 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_SAMPLINGRATE_V1 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_DECAYEXPONENT_V1)


/* MIB entry definition */
typedef struct
{
  L7_uint32 index;              /* row index (N-A)            */
  dsmibPolicyAttrTypeV1_t entryType;    /* defines policy stmt union  */
  dsmibPolicyAttrDataV1_t stmt; /* type-based policy statement */
  dsmibStorageType_t storageType;       /* type of config storage     */
  dsmibRowStatus_t rowStatus;   /* status of this row         */

} dsmibPolicyAttrMibV1_t;

typedef struct
{
  L7_uint32 instIndexRef;       /* back ptr (non-0 = in use)  */
  L7_uint32 chainIndex;         /* next seq attr entry        */
  dsmibPolicyAttrMibV1_t mib;   /* MIB content                */
  dsmibPolicyAttrFlagsV1_t rowInvalidFlags;       /* row invalid object flags   */
} L7_diffServPolicyAttrV1_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;

  /* DiffServ globals */
  L7_uint32 adminMode;          /* administrative mode        */
  L7_uint32 traceMode;          /* component trace mode       */
  L7_uint32 msgLvl;             /* debug message level        */

  /* Class configuration */
  L7_diffServClassHdrV1_t  classHdr[L7_DIFFSERV_CLASS_HDR_MAX_REL_4_0];
  L7_diffServClassRuleV1_t classRule[L7_DIFFSERV_CLASS_RULE_MAX_REL_4_0];

  /* Policy configuration */
  L7_diffServPolicyHdr_t policyHdr[L7_DIFFSERV_POLICY_HDR_MAX_REL_4_0];
  L7_diffServPolicyInst_t policyInst[L7_DIFFSERV_POLICY_INST_MAX_REL_4_0];
  L7_diffServPolicyAttrV1_t policyAttr[L7_DIFFSERV_POLICY_ATTR_MAX_REL_4_0];

  /* Service configuration */
  L7_diffServService_t serviceIn[L7_MAX_SLOTS_PER_BOX_REL_4_0][L7_MAX_PORTS_PER_SLOT_REL_4_0 + 1];
  L7_diffServService_t serviceOut[L7_MAX_SLOTS_PER_BOX_REL_4_0][L7_MAX_PORTS_PER_SLOT_REL_4_0 + 1];

  L7_uint32 checkSum;

}
L7_diffServCfgV1_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;

  /* DiffServ globals */
  L7_uint32 adminMode;          /* administrative mode        */
  L7_uint32 traceMode;          /* component trace mode       */
  L7_uint32 msgLvl;             /* debug message level        */

  /* Class configuration */
  L7_diffServClassHdrV1_t  classHdr[L7_DIFFSERV_CLASS_HDR_MAX];
  L7_diffServClassRuleV1_t classRule[L7_DIFFSERV_CLASS_RULE_MAX_REL_4_1];

  /* Policy configuration */
  L7_diffServPolicyHdr_t policyHdr[L7_DIFFSERV_POLICY_HDR_MAX];
  L7_diffServPolicyInst_t policyInst[L7_DIFFSERV_POLICY_INST_MAX];
  L7_diffServPolicyAttrV1_t policyAttr[L7_DIFFSERV_POLICY_ATTR_MAX_REL_4_1];
  L7_diffServIntfCfg_t diffServIntfCfg[L7_DIFFSERV_INTF_MAX_COUNT_REL_4_1];
  L7_uint32 checkSum;

} L7_diffServCfgV2_t;


/*---------------------------------------------------------------------------
 * Start V3 definitions (for values introduced in Rel 4.3 (I)
 *
 */

typedef struct
{
  L7_uint32           index;                    /* row index (N-A)            */
  L7_uchar8           name[L7_USMDB_MIB_DIFFSERV_CLASS_NAME_MAX+1]; /* name string*/
  dsmibClassType_t    type;                     /* type of class definition   */
  dsmibClassAclType_t aclType;                  /* ACL type (for class type acl)  */
  L7_uint32           aclNum;                   /* ACL number (for class type acl)*/
  L7_uint32           ruleIndexNextFree;        /* next avail rule index (R-O)*/
  dsmibStorageType_t  storageType;              /* type of config storage     */
  dsmibRowStatus_t    rowStatus;                /* status of this row         */

} dsmibClassMibV3_t;

typedef enum
{
  DSMIB_CLASS_FLAGS_NAME_V3 =                      (1 << 0),
  DSMIB_CLASS_FLAGS_TYPE_V3 =                      (1 << 1),
  DSMIB_CLASS_FLAGS_ACLTYPE_V3 =                   (1 << 3),
  DSMIB_CLASS_FLAGS_ACLNUM_V3 =                    (1 << 4)

} dsmibClassFlagsV3_t;


#define DSMIB_CLASS_FLAGS_COMMON_V3 \
                (DSMIB_CLASS_FLAGS_NAME_V3 | \
                 DSMIB_CLASS_FLAGS_TYPE_V3 | \
                 DSMIB_CLASS_FLAGS_ACLTYPE_V3 | \
                 DSMIB_CLASS_FLAGS_ACLNUM_V3)


typedef struct
{
  L7_BOOL                 inUse;                /* array entry in use (T/F)   */
  L7_uint32               chainIndex;           /* first class rule entry     */
  dsmibClassMibV3_t       mib;                  /* MIB content                */
  dsmibClassFlagsV3_t     rowInvalidFlags;      /* row invalid object flags   */

} L7_diffServClassHdrV3_t;

typedef enum
{
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE_V3 = 0,

  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS_V3,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS2_V3,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIP_V3,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTL4PORT_V3,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTMAC_V3,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_ETYPE_V3,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_EVERY_V3,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPDSCP_V3,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPPRECEDENCE_V3,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPTOS_V3,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_PROTOCOL_V3,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_REFCLASS_V3,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIP_V3,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCL4PORT_V3,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCMAC_V3,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID_V3,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID2_V3,

  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_TOTAL_V3   /* number of enum entries */

} L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_V3_t;

typedef L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_V3_t dsmibClassRuleTypeV3_t;

typedef union
{
  /* NOTE:  Only one of the following structures is valid, depending on
   *        the current value of the class rule 'entryType' field.
   */

                                                /* COS type:                  */
  L7_uint32           cos;                      /*   class of service (0-7)   */
                                            
                                                /* Secondary COS (COS2) type: */
  L7_uint32           cos2;                     /*   class of service (0-7)   */
                                            
  struct                                    
  {                                             /* Dst IP type:               */
    L7_uint32         addr;                     /*   IP address               */
    L7_uint32         mask;                     /*   IP address mask          */
  } dstIp;                                  
                                            
  struct                                    
  {                                             /* Dst Layer 4 Port type:     */
    L7_uint32         start;                    /*   start of port range      */
    L7_uint32         end;                      /*   end of port range        */
  } dstL4Port;                              
                                            
  struct                                    
  {                                             /* Dst MAC Address type:      */
    L7_uchar8         addr[L7_MAC_ADDR_LEN];    /*   MAC address              */
    L7_uchar8         mask[L7_MAC_ADDR_LEN];    /*   MAC address mask         */
  } dstMac;

                                                /* Match Every type:          */
  struct 
  {                                             /* Ethertype type:            */
    dsmibEtypeKeyid_t keyid;                    /*   keyword identifier       */
    L7_uint32         value;                    /*   Ethertype custom value   */
  } etype;    

  dsmibTruthValue_t   matchEvery;               /*   match every flag (R-O)   */
                                             
  L7_uint32           ipDscp;                   /*   DiffServ Code Point(0-63)*/
                                             
                                                /* IP Precedence type:        */
  L7_uint32           ipPrecedence;             /*   precedence value (0-7)   */
                                             
  struct                                                                        
  {                                             /* IP TOS type (free form):   */
    L7_uchar8         bits;                     /*   all 8 TOS field bits     */
    L7_uchar8         mask;                     /*   mask for each TOS bit    */
    L7_uchar8         rsvd1[2];                 /*   (rsvd -- for alignment)  */
  } ipTos;                                   
                                             
                                                /* Protocol type:             */
  L7_uint32           protocolNum;              /*   IANA protocol number     */
                                             
                                                /* Reference Class type:      */
  L7_uint32           refClassIndex;            /*   reference class index    */
                                             
  struct                                     
  {                                             /* Src IP type:               */
    L7_uint32         addr;                     /*   IP address               */
    L7_uint32         mask;                     /*   IP address mask          */
  } srcIp;                                   
                                             
  struct                                     
  {                                             /* Src Layer 4 Port type:     */
    L7_uint32         start;                    /*   start of port range      */
    L7_uint32         end;                      /*   end of port range        */
  } srcL4Port;                               
                                             
  struct                                     
  {                                             /* Src MAC Address type:      */
    L7_uchar8         addr[L7_MAC_ADDR_LEN];    /*   MAC address              */
    L7_uchar8         mask[L7_MAC_ADDR_LEN];    /*   MAC address mask         */
  } srcMac;                                  
                                             
  struct                                     
  {                                             /* VLAN type:                 */
    L7_uint32         start;                    /*   start of VLAN ID range   */
    L7_uint32         end;                      /*   end of VLAN ID range     */
  } vlanId;   

  struct                                    
  {                                             /* Secondary VLAN ID type:    */
    L7_uint32         start;                    /*   start of VLAN ID2 range  */
    L7_uint32         end;                      /*   end of VLAN ID2 range    */
  } vlanId2;                                
                                            
} dsmibClassRuleDataV3_t;

typedef struct
{
  L7_uint32              index;                  /* row index (N-A)            */
  dsmibClassRuleTypeV3_t entryType;              /* defines class rule union   */
  dsmibClassRuleDataV3_t match;                  /* type-based rule match info */
  dsmibTruthValue_t      excludeFlag;            /* F=include, T=exclude       */
  dsmibStorageType_t     storageType;            /* type of config storage     */
  dsmibRowStatus_t       rowStatus;              /* status of this row         */

} dsmibClassRuleMibV3_t;

typedef enum
{
  DSMIB_CLASS_RULE_FLAGS_MATCH_ENTRYTYPE_V3 =      (1 << 0),
  DSMIB_CLASS_RULE_FLAGS_MATCH_COS_V3 =            (1 << 1),
  DSMIB_CLASS_RULE_FLAGS_MATCH_ETYPE_KEYID_V3 =    (1 << 2),
  DSMIB_CLASS_RULE_FLAGS_MATCH_ETYPE_VALUE_V3 =    (1 << 3),
  DSMIB_CLASS_RULE_FLAGS_MATCH_IPADDR_V3 =         (1 << 4),
  DSMIB_CLASS_RULE_FLAGS_MATCH_IPMASK_V3 =         (1 << 5),
  DSMIB_CLASS_RULE_FLAGS_MATCH_L4PORT_START_V3 =   (1 << 6),
  DSMIB_CLASS_RULE_FLAGS_MATCH_L4PORT_END_V3 =     (1 << 7),
  DSMIB_CLASS_RULE_FLAGS_MATCH_MACADDR_V3 =        (1 << 8),
  DSMIB_CLASS_RULE_FLAGS_MATCH_MACMASK_V3 =        (1 << 9),
  DSMIB_CLASS_RULE_FLAGS_MATCH_IPDSCP_V3 =         (1 << 10),
  DSMIB_CLASS_RULE_FLAGS_MATCH_IPPRECEDENCE_V3 =   (1 << 11),
  DSMIB_CLASS_RULE_FLAGS_MATCH_IPTOSBITS_V3 =      (1 << 12),
  DSMIB_CLASS_RULE_FLAGS_MATCH_IPTOSMASK_V3 =      (1 << 13),
  DSMIB_CLASS_RULE_FLAGS_MATCH_PROTOCOLNUM_V3 =    (1 << 14),
  DSMIB_CLASS_RULE_FLAGS_MATCH_REFCLASSINDEX_V3 =  (1 << 15),
  DSMIB_CLASS_RULE_FLAGS_MATCH_VIDSTART_V3 =       (1 << 16),
  DSMIB_CLASS_RULE_FLAGS_MATCH_VIDEND_V3 =         (1 << 17),
  DSMIB_CLASS_RULE_FLAGS_MATCH_EXCLUDEFLAG_V3 =    (1 << 18)

} dsmibClassRuleFlagsV3_t;

#define DSMIB_CLASS_RULE_FLAGS_ALL_V3 \
          (DSMIB_CLASS_RULE_FLAGS_MATCH_ENTRYTYPE_V3 | \
           DSMIB_CLASS_RULE_FLAGS_MATCH_COS_V3 | \
           DSMIB_CLASS_RULE_FLAGS_MATCH_ETYPE_KEYID_V3 | \
           DSMIB_CLASS_RULE_FLAGS_MATCH_ETYPE_VALUE_V3 | \
           DSMIB_CLASS_RULE_FLAGS_MATCH_IPADDR_V3 | \
           DSMIB_CLASS_RULE_FLAGS_MATCH_IPMASK_V3 | \
           DSMIB_CLASS_RULE_FLAGS_MATCH_L4PORT_START_V3 | \
           DSMIB_CLASS_RULE_FLAGS_MATCH_L4PORT_END_V3 | \
           DSMIB_CLASS_RULE_FLAGS_MATCH_MACADDR_V3 | \
           DSMIB_CLASS_RULE_FLAGS_MATCH_MACMASK_V3 | \
           DSMIB_CLASS_RULE_FLAGS_MATCH_IPDSCP_V3 | \
           DSMIB_CLASS_RULE_FLAGS_MATCH_IPPRECEDENCE_V3 | \
           DSMIB_CLASS_RULE_FLAGS_MATCH_IPTOSBITS_V3 | \
           DSMIB_CLASS_RULE_FLAGS_MATCH_IPTOSMASK_V3 | \
           DSMIB_CLASS_RULE_FLAGS_MATCH_PROTOCOLNUM_V3 | \
           DSMIB_CLASS_RULE_FLAGS_MATCH_REFCLASSINDEX_V3 | \
           DSMIB_CLASS_RULE_FLAGS_MATCH_VIDSTART_V3 | \
           DSMIB_CLASS_RULE_FLAGS_MATCH_VIDEND_V3 | \
           DSMIB_CLASS_RULE_FLAGS_MATCH_EXCLUDEFLAG_V3)



typedef struct
{
  L7_uint32               hdrIndexRef;          /* back ptr (non-0 = in use)  */
  L7_uint32               chainIndex;           /* next sequential rule entry */
  dsmibClassRuleMibV3_t   mib;                  /* MIB content                */
  dsmibClassRuleFlagsV3_t rowInvalidFlags;      /* row invalid object flags   */
  L7_uint32               arid;                 /* ACL rule identifier        */

} L7_diffServClassRuleV3_t;

/* Values for DiffServ Policy Attribute statement type */
typedef enum
{
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE_V3 = 0,

  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE_V3,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_BANDWIDTH_V3,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP_V3,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_EXPEDITE_V3,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL_V3,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL_V3,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL_V3,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL_V3,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE_V3,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE_V3,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE_V3,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_RANDOMDROP_V3,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT_V3,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_SHAPE_AVERAGE_V3,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_SHAPE_PEAK_V3,

  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_TOTAL_V3  /* number of enum entries */

} L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_V3_t;

/* Values for DiffServ Police Action type */
typedef enum
{
  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_NONE_V3 = 0,

  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_DROP_V3,
  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_V3,
  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2_V3,
  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP_V3,
  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC_V3,
  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_SEND_V3,

  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_TOTAL_V3      /* number of enum entries */

} L7_USMDB_MIB_DIFFSERV_POLICE_ACT_V3_t;

/* Values for DiffServ Police Color Aware type */
typedef enum
{
  L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_NONE_V3 = 0,

  L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_BLIND_V3,     /* color-blind mode           */
  L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS_V3,       /* aware COS (802.1p)         */
  L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS2_V3,      /* aware COS2 (802.1p)        */
  L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPDSCP_V3,    /* aware IP DSCP              */
  L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPPREC_V3,    /* aware IP Prec              */
  L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_UNUSED_V3,    /* exceed color not specified */

  L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_TOTAL_V3      /* number of enum entries */

} L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_V3_t;

typedef L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_V3_t   dsmibPolicyAttrTypeV3_t;
typedef L7_USMDB_MIB_DIFFSERV_POLICE_ACT_V3_t         dsmibPoliceActV3_t;
typedef L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_V3_t       dsmibPoliceColorV3_t;

/* type-specific attribute fields */
typedef union
{
  /* NOTE:  Only one of the following structures is valid, depending on
   *        the current value of the policy attribute 'entryType' field.
   */

  struct 
  {                                             /* Assign Queue type:         */
    L7_uint32           qid;                    /*   queue ID                 */
  } assignQueue;

  struct 
  {                                             /* Bandwidth type:            */
    L7_uint32           crate;                  /*   committed rate (CIR)     */
    dsmibBwUnitsV1_t      crateUnits;             /*     Kbps, percent          */
  } bandwidth;

  dsmibTruthValue_t     drop;                   /* Drop attribute flag (R-O)  */
                                             
  struct 
  {                                             /* Expedite type:             */
    L7_uint32           crate;                  /*   committed rate (CIR)     */
    dsmibBwUnitsV1_t      crateUnits;             /*     Kbps, percent          */
    L7_uint32           cburst;                 /*   committed burst (CBS)    */
  } expedite;  

  struct 
  {                                             /* Mark COS type:             */
    L7_uchar8           val;                    /*   Class of Service (0-7)   */
    L7_uchar8           rsvd1[3];               /*   (rsvd -- for alignment)  */
  } markCos;   

  struct 
  {                                             /* Mark Secondary COS type:   */
    L7_uchar8           val;                    /*   Class of Service (0-7)   */
    L7_uchar8           rsvd1[3];               /*   (rsvd -- for alignment)  */
  } markCos2;   

  struct 
  {                                             /* Mark IP DSCP type:         */
    L7_uchar8           val;                    /*   DiffServ Code Point(0-63)*/
    L7_uchar8           rsvd1[3];               /*   (rsvd -- for alignment)  */
  } markIpDscp;

  struct 
  {                                             /* Mark IP Precedence type:   */
    L7_uchar8           val;                    /*   IP Precedence (0-7)      */
    L7_uchar8           rsvd1[3];               /*   (rsvd -- for alignment)  */
  } markIpPrecedence;

  struct 
  {                                             /* Police Simple type:        */
    L7_uint32           crate;                  /*   committed rate (CIR)     */
    L7_uint32           cburst;                 /*   committed burst (CBS)    */
    dsmibPoliceActV3_t  conformAct;             /*   conform action           */
    dsmibPoliceActV3_t  nonconformAct;          /*   nonconform action        */
    L7_uchar8           conformVal;             /*   conform DSCP mark val    */
    L7_uchar8           nonconformVal;          /*   nonconform DSCP mark val */
    L7_uchar8           rsvd1[2];               /*   (rsvd -- for alignment)  */
    L7_uint32           colorConformIndex;      /*   color conform index      */
    dsmibPoliceColorV3_t  colorConformMode;     /*   color conform mode       */
    L7_uchar8           colorConformVal;        /*   color-aware conform val  */
    L7_uchar8           rsvd2[3];               /*   (rsvd -- for alignment)  */
  } policeSimple;

  struct 
  {                                             /* Police Single Rate type:   */
    L7_uint32           crate;                  /*   committed rate (CIR)     */
    L7_uint32           cburst;                 /*   committed burst (CBS)    */
    L7_uint32           eburst;                 /*   excess burst (EBS)       */
    dsmibPoliceActV3_t  conformAct;             /*   conform action           */
    dsmibPoliceActV3_t  exceedAct;              /*   exceed action            */
    dsmibPoliceActV3_t  nonconformAct;          /*   nonconform action        */
    L7_uchar8           conformVal;             /*   conform DSCP mark val    */
    L7_uchar8           exceedVal;              /*   exceed DSCP mark val     */
    L7_uchar8           nonconformVal;          /*   nonconform DSCP mark val */
    L7_uchar8           rsvd1[1];               /*   (rsvd -- for alignment)  */
    L7_uint32           colorConformIndex;      /*   color conform index      */
    L7_uint32           colorExceedIndex;       /*   color exceed index       */
    dsmibPoliceColorV3_t  colorConformMode;     /*   color conform mode       */
    dsmibPoliceColorV3_t  colorExceedMode;      /*   color exceed mode        */
    L7_uchar8           colorConformVal;        /*   color-aware conform val  */
    L7_uchar8           colorExceedVal;         /*   color-aware exceed val   */
    L7_uchar8           rsvd2[2];               /*   (rsvd -- for alignment)  */
  } policeSingleRate;

  struct 
  {                                             /* Police Single Rate type:   */
    L7_uint32           crate;                  /*   committed rate (CIR)     */
    L7_uint32           cburst;                 /*   committed burst (CBS)    */
    L7_uint32           prate;                  /*   peak rate (PIR)          */
    L7_uint32           pburst;                 /*   peak burst (PBS)         */
    dsmibPoliceActV3_t  conformAct;             /*   conform action           */
    dsmibPoliceActV3_t  exceedAct;              /*   exceed action            */
    dsmibPoliceActV3_t  nonconformAct;          /*   nonconform action        */
    L7_uchar8           conformVal;             /*   conform DSCP mark val    */
    L7_uchar8           exceedVal;              /*   exceed DSCP mark val     */
    L7_uchar8           nonconformVal;          /*   nonconform DSCP mark val */
    L7_uchar8           rsvd1[1];               /*   (rsvd -- for alignment)  */
    L7_uint32           colorConformIndex;      /*   color conform index      */
    L7_uint32           colorExceedIndex;       /*   color exceed index       */
    dsmibPoliceColorV3_t  colorConformMode;     /*   color conform mode       */
    dsmibPoliceColorV3_t  colorExceedMode;      /*   color exceed mode        */
    L7_uchar8           colorConformVal;        /*   color-aware conform val  */
    L7_uchar8           colorExceedVal;         /*   color-aware exceed val   */
    L7_uchar8           rsvd2[2];               /*   (rsvd -- for alignment)  */
  } policeTwoRate;

  struct 
  {                                             /* Random Drop type:          */
    L7_uint32           minThresh;              /*   begin threshold (bytes)  */
    L7_uint32           maxThresh;              /*   end threshold (bytes)    */
    L7_uint32           maxDropProb;            /*   maximum drop probability */
    L7_uint32           samplingRate;           /*   queue sampling rate      */
    L7_uint32           decayExponent;          /*   decay factor exponent    */
  } randomDrop;

  struct 
  {                                             /* Redirect type:             */
    nimConfigID_t       configId;               /*   intf config ID           */
  } redirect;      

  struct 
  {                                             /* Shape Average type:        */
    L7_uint32           crate;                  /*   committed rate (CIR)     */
  } shapeAverage;

  struct 
  {                                             /* Shape Peak type:           */
    L7_uint32           crate;                  /*   committed rate (CIR)     */
    L7_uint32           prate;                  /*   peak rate (PIR)          */
  } shapePeak;

} dsmibPolicyAttrDataV3_t;

/* MIB entry flags (limited to at most 32 flag values) */
typedef enum
{
  DSMIB_POLICY_ATTR_FLAGS_STMT_ENTRYTYPE_V3 =         (1 << 0), 
  DSMIB_POLICY_ATTR_FLAGS_STMT_QUEUEID_V3 =           (1 << 1), 
  DSMIB_POLICY_ATTR_FLAGS_STMT_CRATE_V3 =             (1 << 2), 
  DSMIB_POLICY_ATTR_FLAGS_STMT_CRATEUNITS_V3 =        (1 << 3), 
  DSMIB_POLICY_ATTR_FLAGS_STMT_CBURST_V3 =            (1 << 4), 
  DSMIB_POLICY_ATTR_FLAGS_STMT_EBURST_V3 =            (1 << 5), 
  DSMIB_POLICY_ATTR_FLAGS_STMT_PRATE_V3 =             (1 << 6), 
  DSMIB_POLICY_ATTR_FLAGS_STMT_PBURST_V3 =            (1 << 7), 
  DSMIB_POLICY_ATTR_FLAGS_STMT_COSVAL_V3 =            (1 << 8), 
  DSMIB_POLICY_ATTR_FLAGS_STMT_COS2VAL_V3 =           (1 << 9), 
  DSMIB_POLICY_ATTR_FLAGS_STMT_IPDSCPVAL_V3 =         (1 << 10),
  DSMIB_POLICY_ATTR_FLAGS_STMT_IPPRECEDENCEVAL_V3 =   (1 << 11),
  DSMIB_POLICY_ATTR_FLAGS_STMT_CONFORMACT_V3 =        (1 << 12), /* defaulted */ 
  DSMIB_POLICY_ATTR_FLAGS_STMT_EXCEEDACT_V3 =         (1 << 13), /* defaulted */ 
  DSMIB_POLICY_ATTR_FLAGS_STMT_NONCONFORMACT_V3 =     (1 << 14), /* defaulted */ 
  DSMIB_POLICY_ATTR_FLAGS_STMT_CONFORMVAL_V3 =        (1 << 15),                 
  DSMIB_POLICY_ATTR_FLAGS_STMT_EXCEEDVAL_V3 =         (1 << 16),                 
  DSMIB_POLICY_ATTR_FLAGS_STMT_NONCONFORMVAL_V3 =     (1 << 17),                 
  DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMINDEX_V3 = (1 << 18), /* defaulted */
  DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMMODE_V3 =  (1 << 19), /* defaulted */
  DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMVAL_V3 =   (1 << 20),                 
  DSMIB_POLICY_ATTR_FLAGS_STMT_COLOREXCEEDINDEX_V3 =  (1 << 21), /* defaulted */ 
  DSMIB_POLICY_ATTR_FLAGS_STMT_COLOREXCEEDMODE_V3 =   (1 << 22), /* defaulted */ 
  DSMIB_POLICY_ATTR_FLAGS_STMT_COLOREXCEEDVAL_V3 =    (1 << 23),                 
  DSMIB_POLICY_ATTR_FLAGS_STMT_MINTHRESH_V3 =         (1 << 24),                 
  DSMIB_POLICY_ATTR_FLAGS_STMT_MAXTHRESH_V3 =         (1 << 25),                 
  DSMIB_POLICY_ATTR_FLAGS_STMT_MAXDROPPROB_V3 =       (1 << 26),                 
  DSMIB_POLICY_ATTR_FLAGS_STMT_SAMPLINGRATE_V3 =      (1 << 27), /* defaulted */ 
  DSMIB_POLICY_ATTR_FLAGS_STMT_DECAYEXPONENT_V3 =     (1 << 28), /* defaulted */ 
  DSMIB_POLICY_ATTR_FLAGS_STMT_REDIRECTINTF_V3 =      (1 << 29)                  

} dsmibPolicyAttrFlagsV3_t;

#define DSMIB_POLICY_ATTR_FLAGS_ALL_V3 \
          (DSMIB_POLICY_ATTR_FLAGS_STMT_ENTRYTYPE_V3 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_QUEUEID_V3 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_CRATE_V3 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_CRATEUNITS_V3 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_CBURST_V3 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_EBURST_V3 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_PRATE_V3 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_PBURST_V3 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_COSVAL_V3 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_COS2VAL_V3 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_IPDSCPVAL_V3 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_IPPRECEDENCEVAL_V3 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_CONFORMACT_V3 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_EXCEEDACT_V3 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_NONCONFORMACT_V3 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_CONFORMVAL_V3 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_EXCEEDVAL_V3 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_NONCONFORMVAL_V3 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMINDEX_V3 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMMODE_V3 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMVAL_V3 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_COLOREXCEEDINDEX_V3 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_COLOREXCEEDMODE_V3 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_COLOREXCEEDVAL_V3 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_MINTHRESH_V3 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_MAXTHRESH_V3 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_MAXDROPPROB_V3 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_SAMPLINGRATE_V3 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_DECAYEXPONENT_V3 | \
           DSMIB_POLICY_ATTR_FLAGS_STMT_REDIRECTINTF_V3)

/* MIB entry definition */
typedef struct
{
  L7_uint32               index;                /* row index (N-A)            */
  dsmibPolicyAttrTypeV3_t entryType;            /* defines policy stmt union  */
  dsmibPolicyAttrDataV3_t stmt;                 /* type-based policy statement*/
  dsmibStorageType_t      storageType;          /* type of config storage     */
  dsmibRowStatus_t        rowStatus;            /* status of this row         */

} dsmibPolicyAttrMibV3_t;

typedef struct
{
  L7_uint32                 instIndexRef;       /* back ptr (non-0 = in use)  */
  L7_uint32                 chainIndex;         /* next seq attr entry        */
  dsmibPolicyAttrMibV3_t    mib;                /* MIB content                */
  dsmibPolicyAttrFlagsV3_t  rowInvalidFlags;    /* row invalid object flags   */

} L7_diffServPolicyAttrV3_t;

typedef struct
{
  L7_fileHdr_t            cfgHdr;

  /* DiffServ globals */
  L7_uint32               adminMode;            /* administrative mode        */
  L7_uint32               traceMode;            /* component trace mode       */
  L7_uint32               msgLvl;               /* debug message level        */

  /* Class configuration */
  L7_diffServClassHdrV3_t   classHdr[L7_DIFFSERV_CLASS_HDR_MAX_REL_4_3];
  L7_diffServClassRuleV3_t  classRule[L7_DIFFSERV_CLASS_RULE_MAX_REL_4_3];

  /* Policy configuration */
  L7_diffServPolicyHdr_t  policyHdr[L7_DIFFSERV_POLICY_HDR_MAX_REL_4_3];
  L7_diffServPolicyInst_t policyInst[L7_DIFFSERV_POLICY_INST_MAX_REL_4_3];
  L7_diffServPolicyAttrV3_t policyAttr[L7_DIFFSERV_POLICY_ATTR_MAX_REL_4_3];
  L7_diffServIntfCfg_t    diffServIntfCfg[L7_DIFFSERV_INTF_MAX_COUNT_REL_4_3];
  L7_uint32               checkSum;

} L7_diffServCfgV3_t;

/*
 * End V3 definitions
 *
 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
 * Start V4 definitions (for values introduced in Rel 4.5 (L)
 *
 */

typedef struct
{
  L7_fileHdr_t            cfgHdr;

  /* DiffServ globals */
  L7_uint32               adminMode;            /* administrative mode        */
  L7_uint32               traceMode;            /* component trace mode       */
  L7_uint32               msgLvl;               /* debug message level        */

  /* Class configuration */
  L7_diffServClassHdrV3_t   classHdr[L7_DIFFSERV_CLASS_HDR_MAX_REL_4_4];
  L7_diffServClassRuleV3_t  classRule[L7_DIFFSERV_CLASS_RULE_MAX_REL_4_4];

  /* Policy configuration */
  L7_diffServPolicyHdr_t  policyHdr[L7_DIFFSERV_POLICY_HDR_MAX_REL_4_4];
  L7_diffServPolicyInst_t policyInst[L7_DIFFSERV_POLICY_INST_MAX_REL_4_4];
  L7_diffServPolicyAttr_t policyAttr[L7_DIFFSERV_POLICY_ATTR_MAX_REL_4_4];
  L7_diffServIntfCfg_t    diffServIntfCfg[L7_DIFFSERV_INTF_MAX_COUNT_REL_4_4];
  L7_uint32               checkSum;

} L7_diffServCfgV4_t;
/*
 * End V4 definitions
 *
 *---------------------------------------------------------------------------*/
typedef L7_diffServCfg_t L7_diffServCfgV5_t;


/*---------------------------------------------------------------------------*/
/*
             ENUMS FOR DIFFSERV MIGRATION 

*/
/*---------------------------------------------------------------------------*/





#endif /* DIFFSERV_MIGRATE_H */
