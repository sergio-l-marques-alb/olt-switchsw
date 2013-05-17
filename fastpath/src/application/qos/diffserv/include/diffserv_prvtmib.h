/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   diffserv_prvtmib.h
*
* @purpose    DiffServ component Private MIB structures
*
* @component  DiffServ
*
* @comments   none
*
* @create     03/12/2002
*
* @author     gpaussa
* @end
*
**********************************************************************/
#ifndef INCLUDE_DIFFSERV_PRVTMIB_H
#define INCLUDE_DIFFSERV_PRVTMIB_H

#include "l7_common.h"
#include "diffserv_cfg_limit.h"
#include "usmdb_mib_diffserv_private_api.h"
#include "l7_diffserv_api.h"
#include "avl_api.h"
#include "cnfgr.h"

/* all per-row subordinate index values start at 1 and continually increment
 * (cannot delete and reuse these rows)
 */
#define DSMIB_AUTO_INCR_INDEX_NEXT_START  1

/* local definitions */
#define DSMIB_NO_FLAG                     0

/* maximum number of possible DSCP values */
#define DSMIB_IPDSCP_TOTAL                64

/* external references */
extern  avlTree_t   dsmibAvlTree[];             /* AVL tree anchors           */
extern  void *      dsmibIndexSemId;            /* MIB tables index semaphore */

/* AVL tree control fields */
typedef struct
{
  L7_uint32         entryMaxNum;                /* maximum number of elements */
  L7_uint32         entrySize;                  /* size of each element       */
  L7_uint32         entryKeySize;               /* size of element search key */
  avlTreeTables_t   *pTreeHeap;                 /* tree heap ptr              */
  void              *pDataHeap;                 /* data heap ptr              */
} dsmibAvlCtrl_t;

/* maximum table index definitions */
#define DSMIB_CLASS_INDEX_MAX             L7_DIFFSERV_CLASS_LIM
#define DSMIB_CLASS_RULE_INDEX_MAX        L7_DIFFSERV_RULE_PER_CLASS_LIM
#define DSMIB_POLICY_INDEX_MAX            L7_DIFFSERV_POLICY_LIM
#define DSMIB_POLICY_INST_INDEX_MAX       L7_DIFFSERV_INST_PER_POLICY_LIM
#define DSMIB_POLICY_ATTR_INDEX_MAX       L7_DIFFSERV_ATTR_PER_INST_LIM
#define DSMIB_SERVICE_INTIFNUM_INDEX_MAX  (L7_MAX_INTERFACE_COUNT-1)
#define DSMIB_SERVICE_DIR_INDEX_MAX       2

/* derived maximum table sizes */
#define DSMIB_CLASS_TABLE_SIZE_MAX            L7_DIFFSERV_CLASS_LIM
#define DSMIB_CLASS_RULE_TABLE_SIZE_MAX       L7_DIFFSERV_CLASS_RULE_LIM
#define DSMIB_POLICY_TABLE_SIZE_MAX           L7_DIFFSERV_POLICY_LIM
#define DSMIB_POLICY_INST_TABLE_SIZE_MAX      L7_DIFFSERV_POLICY_INST_LIM
#define DSMIB_POLICY_ATTR_TABLE_SIZE_MAX      L7_DIFFSERV_POLICY_ATTR_LIM
#define DSMIB_POLICY_PERF_IN_TABLE_SIZE_MAX   (L7_DIFFSERV_SERVICE_INTF_LIM * \
                                               L7_DIFFSERV_INST_PER_POLICY_LIM)
#define DSMIB_POLICY_PERF_OUT_TABLE_SIZE_MAX  (L7_DIFFSERV_SERVICE_INTF_LIM * \
                                               L7_DIFFSERV_INST_PER_POLICY_LIM)
#define DSMIB_SERVICE_TABLE_SIZE_MAX          L7_DIFFSERV_SERVICE_INTF_LIM

/* maximum number of rules allowed due to nested class references */
#define DSMIB_CLASS_REF_NESTED_RULE_MAX   L7_DIFFSERV_NESTED_RULE_PER_CLASS_LIM

/* maximum number of chained references is the smaller of the total number of 
 * classes, or the maximum number of nested rules (assumes a worst case where
 * each class contains just a reference rule)
 */
#define DSMIB_CLASS_REF_CHAIN_DEPTH_MAX \
          min(L7_DIFFSERV_CLASS_LIM, DSMIB_CLASS_REF_NESTED_RULE_MAX)


/* configuration default definitions */
#define DSMIB_CFG_DEFAULT_STORAGE_TYPE \
                FD_QOS_DIFFSERV_STORAGE_TYPE
#define DSMIB_CFG_DEFAULT_DEFAULTCLASS_NAME \
                FD_QOS_DIFFSERV_DEFAULTCLASS_NAME
#define DSMIB_CFG_DEFAULT_EXPEDITE_CBURST \
                FD_QOS_DIFFSERV_EXPEDITE_CBURST
#define DSMIB_CFG_DEFAULT_POLICE_CONFORM_ACT \
                FD_QOS_DIFFSERV_POLICE_CONFORM_ACT
#define DSMIB_CFG_DEFAULT_POLICE_EXCEED_ACT \
                FD_QOS_DIFFSERV_POLICE_EXCEED_ACT
#define DSMIB_CFG_DEFAULT_POLICE_NONCONFORM_ACT \
                FD_QOS_DIFFSERV_POLICE_NONCONFORM_ACT
#define DSMIB_CFG_DEFAULT_POLICE_COLOR_CONFORM_INDEX \
                FD_QOS_DIFFSERV_POLICE_COLOR_CONFORM_INDEX
#define DSMIB_CFG_DEFAULT_POLICE_COLOR_CONFORM_MODE \
                FD_QOS_DIFFSERV_POLICE_COLOR_CONFORM_MODE
#define DSMIB_CFG_DEFAULT_POLICE_COLOR_EXCEED_INDEX \
                FD_QOS_DIFFSERV_POLICE_COLOR_EXCEED_INDEX
#define DSMIB_CFG_DEFAULT_POLICE_COLOR_EXCEED_MODE \
                FD_QOS_DIFFSERV_POLICE_COLOR_EXCEED_MODE
#define DSMIB_CFG_DEFAULT_RANDOMDROP_SAMPLING_RATE \
                FD_QOS_DIFFSERV_RANDOMDROP_SAMPLING_RATE
#define DSMIB_CFG_DEFAULT_RANDOMDROP_DECAY_EXPONENT \
                FD_QOS_DIFFSERV_RANDOMDROP_DECAY_EXPONENT
#define DSMIB_CFG_DEFAULT_SERVICE_IF_OPER_STATUS \
                L7_USMDB_MIB_DIFFSERV_IF_OPER_STATUS_DOWN /* not customizable */

/* index parm range checking macros */
#define DSMIB_CLASS_INDEX_OUT_OF_RANGE(_idx) \
          ((L7_BOOL)(((_idx) == 0) || ((_idx) > (L7_uint32)DSMIB_CLASS_INDEX_MAX)))
#define DSMIB_CLASS_RULE_INDEX_OUT_OF_RANGE(_idx) \
          ((L7_BOOL)(((_idx) == 0) || ((_idx) > (L7_uint32)DSMIB_CLASS_RULE_INDEX_MAX)))
#define DSMIB_POLICY_INDEX_OUT_OF_RANGE(_idx) \
          ((L7_BOOL)(((_idx) == 0) || ((_idx) > (L7_uint32)DSMIB_POLICY_INDEX_MAX)))
#define DSMIB_POLICY_INST_INDEX_OUT_OF_RANGE(_idx) \
          ((L7_BOOL)(((_idx) == 0) || ((_idx) > (L7_uint32)DSMIB_POLICY_INST_INDEX_MAX)))
#define DSMIB_POLICY_ATTR_INDEX_OUT_OF_RANGE(_idx) \
          ((L7_BOOL)(((_idx) == 0) || ((_idx) > (L7_uint32)DSMIB_POLICY_ATTR_INDEX_MAX)))
#define DSMIB_SERVICE_INTIFNUM_INDEX_OUT_OF_RANGE(_idx) \
          ((L7_BOOL)(((_idx) == 0) || ((_idx) > (L7_uint32)DSMIB_SERVICE_INTIFNUM_INDEX_MAX)))
#define DSMIB_SERVICE_DIR_INDEX_OUT_OF_RANGE(_idx) \
          ((L7_BOOL)(((_idx) == 0) || ((_idx) > (L7_uint32)DSMIB_SERVICE_DIR_INDEX_MAX)))

/* MIB row invalid flags support macros */
#define DSMIB_ROW_INVALID_FLAG_SET(_p, _f) \
          (((_p)->rowInvalidFlags |= (_f)))
#define DSMIB_ROW_INVALID_FLAG_CLR(_p, _f) \
          (((_p)->rowInvalidFlags &= ~(_f)))
#define DSMIB_ROW_INVALID_FLAG_IS_ON(_p, _f) \
          ((L7_BOOL)(((_p)->rowInvalidFlags & (_f)) != 0))
#define DSMIB_ROW_INVALID_FLAG_IS_OFF(_p, _f) \
          ((L7_BOOL)(((_p)->rowInvalidFlags & (_f)) == 0))
#define DSMIB_ROW_IS_COMPLETE(_p) \
          ((L7_BOOL)((_p)->rowInvalidFlags == 0))


/* list of supported private MIB tables
 *
 * NOTE: These are valid only within the private MIB component and must be
 *       translated to/from their corresponding USMDB table ID name when 
 *       working with any table ID at the USMDB layer.
 */
typedef enum
{
  DSMIB_TABLE_ID_NONE = 0,

  DSMIB_TABLE_ID_CLASS,
  DSMIB_TABLE_ID_CLASS_RULE,
  DSMIB_TABLE_ID_POLICY,
  DSMIB_TABLE_ID_POLICY_INST,
  DSMIB_TABLE_ID_POLICY_ATTR,
  DSMIB_TABLE_ID_POLICY_PERF_IN,
  DSMIB_TABLE_ID_POLICY_PERF_OUT,
  DSMIB_TABLE_ID_SERVICE,                       /* contains Service Perf table*/

  DSMIB_TABLE_ID_TOTAL                          /* number of enum entries */

} dsmibTableId_t;

/* general typedefs */
typedef L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t    dsmibStorageType_t;
typedef L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t      dsmibRowStatus_t;
typedef L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_t     dsmibTruthValue_t;
typedef L7_QOS_ETYPE_KEYID_t                    dsmibEtypeKeyid_t;


/****************************************
 *
 * General Status Group
 *
 ****************************************
 */

/* MIB entry definition */
typedef struct
{
  L7_uint32           adminMode;                /* DiffServ admin mode        */
  /* MIB table current size, max values*/
  L7_uint32           tableSize[DSMIB_TABLE_ID_TOTAL];
  L7_uint32           tableMax[DSMIB_TABLE_ID_TOTAL];

} dsmibGenStatusGroup_t;

/* MIB controls */
typedef struct
{
  /* NOTE:  This is a group, not a table, so there are no search keys and 
   *        no need for AVL structural elements.
   */

  /* MIB control fields */

  /* MIB fields */
  dsmibGenStatusGroup_t mib;                      /* MIB object definitions     */

} dsmibGenStatusGroupCtrl_t;


/****************************************
 *
 * Class Table
 *
 ****************************************
 */

typedef L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_t        dsmibClassType_t;
typedef L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_t  dsmibClassL3Protocol_t;
typedef L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_t     dsmibClassAclType_t;

#define DSMIB_CLASS_REF_INDEX_NONE              0   /* class contains no ref  */

/* MIB entry flags */
typedef enum
{
  DSMIB_CLASS_FLAGS_NAME =                      (1 << 0),
  DSMIB_CLASS_FLAGS_TYPE =                      (1 << 1),
  DSMIB_CLASS_FLAGS_L3PROTO =                   (1 << 2),
  DSMIB_CLASS_FLAGS_ACLTYPE =                   (1 << 3),
  DSMIB_CLASS_FLAGS_ACLNUM =                    (1 << 4)

} dsmibClassFlags_t;

/* specific flag combinations */
#define DSMIB_CLASS_FLAGS_COMMON \
                (DSMIB_CLASS_FLAGS_NAME | \
                 DSMIB_CLASS_FLAGS_TYPE | \
                 DSMIB_CLASS_FLAGS_L3PROTO | \
                 DSMIB_CLASS_FLAGS_ACLTYPE | \
                 DSMIB_CLASS_FLAGS_ACLNUM)

/* MIB entry definition */
typedef struct
{
  L7_uint32           index;                    /* row index (N-A)            */
  L7_uchar8           name[L7_USMDB_MIB_DIFFSERV_CLASS_NAME_MAX+1]; /* name string*/
  dsmibClassType_t    type;                     /* type of class definition   */
  dsmibClassL3Protocol_t l3Protocol;            /* layer 3 protocol */
  dsmibClassAclType_t aclType;                  /* ACL type (for class type acl)  */
  L7_uint32           aclNum;                   /* ACL number (for class type acl)*/
  L7_uint32           ruleIndexNextFree;        /* next avail rule index (R-O)*/
  dsmibStorageType_t  storageType;              /* type of config storage     */
  dsmibRowStatus_t    rowStatus;                /* status of this row         */

} dsmibClassMib_t;

/* search keys */
typedef struct
{
  L7_uint32           classIndex;               /* AVL KEY: class index       */
} dsmibClassKey_t;

/* MIB controls */
typedef struct
{
  /* NOTE:  The search keys MUST be the first items in this structure */
  dsmibClassKey_t     key;                      /* AVL search keys            */

  /* MIB control fields */
  dsmibTableId_t      tableId;                  /* table containing this row  */
  dsmibClassFlags_t   rowInvalidFlags;          /* row objects invalid flags  */
  L7_uint32           refCount;                 /*num policy, class references*/
  L7_uint32           ruleCount;                /* num rules comprising class */
  L7_uint32           colorRefCount;            /* num police color class refs*/
  L7_uint32           refClassIndex;            /*if this class has a ref rule*/
  L7_BOOL             rowPending;               /* initial row completion pend*/

  /* MIB fields */
  dsmibClassMib_t     mib;                      /* MIB object definitions     */

  /* NOTE:  The AVL utility REQUIRES a next ptr as the LAST element */
  void                *next;                    /* (RESERVED FOR AVL USE)     */

} dsmibClassEntryCtrl_t;


/****************************************
 *
 * Class Rule Table
 *
 ****************************************
 */

typedef L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t dsmibClassRuleType_t;

/* value indicating no ACL rules being used */
#define DSMIB_CLASS_RULE_ARID_NONE              0

/* mask & shift bit definitions for IP Service Type (a.k.a. IP TOS) octet */
#define DSMIB_CLASS_RULE_IP_DSCP_SHIFT          2
#define DSMIB_CLASS_RULE_IP_DSCP_MASK           0xFC
#define DSMIB_CLASS_RULE_IP_PRECEDENCE_SHIFT    5
#define DSMIB_CLASS_RULE_IP_PRECEDENCE_MASK     0xE0
#define DSMIB_CLASS_RULE_IP_TOS_SHIFT           0
#define DSMIB_CLASS_RULE_IP_TOS_MASK            0xFF

/* general shift amount to convert between DSCP and Precedence
 * (left or right shift accordingly)
 */
#define DSMIB_CLASS_RULE_IP_DSCP_PREC_CONVERT_SHIFT   3

/* Ethertype value and mask  definitions */
#define DSMIB_CLASS_RULE_ETYPE_VALUE_UNUSED     0x00000000

/* MIB entry flags */
typedef enum
{
  DSMIB_CLASS_RULE_FLAGS_MATCH_ENTRYTYPE =      (1 << 0),
  DSMIB_CLASS_RULE_FLAGS_MATCH_COS =            (1 << 1),
  DSMIB_CLASS_RULE_FLAGS_MATCH_ETYPE_KEYID =    (1 << 2),
  DSMIB_CLASS_RULE_FLAGS_MATCH_ETYPE_VALUE =    (1 << 3),
  DSMIB_CLASS_RULE_FLAGS_MATCH_IPADDR =         (1 << 4),
  DSMIB_CLASS_RULE_FLAGS_MATCH_IPMASK =         (1 << 5),
  DSMIB_CLASS_RULE_FLAGS_MATCH_IPV6ADDR =       (1 << 6),
  DSMIB_CLASS_RULE_FLAGS_MATCH_IPV6PLEN =       (1 << 7),
  DSMIB_CLASS_RULE_FLAGS_MATCH_L4PORT_START =   (1 << 8),
  DSMIB_CLASS_RULE_FLAGS_MATCH_L4PORT_END =     (1 << 9),
  DSMIB_CLASS_RULE_FLAGS_MATCH_MACADDR =        (1 << 10),
  DSMIB_CLASS_RULE_FLAGS_MATCH_MACMASK =        (1 << 11),
  DSMIB_CLASS_RULE_FLAGS_MATCH_IPV6FLOWLBL =    (1 << 12),
  DSMIB_CLASS_RULE_FLAGS_MATCH_IPDSCP =         (1 << 13),
  DSMIB_CLASS_RULE_FLAGS_MATCH_IPPRECEDENCE =   (1 << 14),
  DSMIB_CLASS_RULE_FLAGS_MATCH_IPTOSBITS =      (1 << 15),
  DSMIB_CLASS_RULE_FLAGS_MATCH_IPTOSMASK =      (1 << 16),
  DSMIB_CLASS_RULE_FLAGS_MATCH_PROTOCOLNUM =    (1 << 17),
  DSMIB_CLASS_RULE_FLAGS_MATCH_REFCLASSINDEX =  (1 << 18),
  DSMIB_CLASS_RULE_FLAGS_MATCH_VIDSTART =       (1 << 19),
  DSMIB_CLASS_RULE_FLAGS_MATCH_VIDEND =         (1 << 20),
  DSMIB_CLASS_RULE_FLAGS_MATCH_EXCLUDEFLAG =    (1 << 21)

} dsmibClassRuleFlags_t;

/* specific flag combinations */
#define DSMIB_CLASS_RULE_FLAGS_COMMON \
                (DSMIB_CLASS_RULE_FLAGS_MATCH_ENTRYTYPE)

#define DSMIB_CLASS_RULE_FLAGS_TYPE_COS \
                (DSMIB_CLASS_RULE_FLAGS_MATCH_COS | \
                 DSMIB_CLASS_RULE_FLAGS_MATCH_EXCLUDEFLAG)

#define DSMIB_CLASS_RULE_FLAGS_TYPE_COS2 \
                (DSMIB_CLASS_RULE_FLAGS_MATCH_COS | \
                 DSMIB_CLASS_RULE_FLAGS_MATCH_EXCLUDEFLAG)

#define DSMIB_CLASS_RULE_FLAGS_TYPE_DSTIP \
                (DSMIB_CLASS_RULE_FLAGS_MATCH_IPADDR | \
                 DSMIB_CLASS_RULE_FLAGS_MATCH_IPMASK | \
                 DSMIB_CLASS_RULE_FLAGS_MATCH_EXCLUDEFLAG)

#define DSMIB_CLASS_RULE_FLAGS_TYPE_DSTIPV6 \
                (DSMIB_CLASS_RULE_FLAGS_MATCH_IPV6ADDR | \
                 DSMIB_CLASS_RULE_FLAGS_MATCH_IPV6PLEN | \
                 DSMIB_CLASS_RULE_FLAGS_MATCH_EXCLUDEFLAG)

#define DSMIB_CLASS_RULE_FLAGS_TYPE_DSTL4PORT \
                (DSMIB_CLASS_RULE_FLAGS_MATCH_L4PORT_START | \
                 DSMIB_CLASS_RULE_FLAGS_MATCH_L4PORT_END | \
                 DSMIB_CLASS_RULE_FLAGS_MATCH_EXCLUDEFLAG)

#define DSMIB_CLASS_RULE_FLAGS_TYPE_DSTMAC \
                (DSMIB_CLASS_RULE_FLAGS_MATCH_MACADDR | \
                 DSMIB_CLASS_RULE_FLAGS_MATCH_MACMASK | \
                 DSMIB_CLASS_RULE_FLAGS_MATCH_EXCLUDEFLAG)

#define DSMIB_CLASS_RULE_FLAGS_TYPE_ETYPE \
                (DSMIB_CLASS_RULE_FLAGS_MATCH_ETYPE_KEYID | \
                 DSMIB_CLASS_RULE_FLAGS_MATCH_ETYPE_VALUE | \
                 DSMIB_CLASS_RULE_FLAGS_MATCH_EXCLUDEFLAG)

#define DSMIB_CLASS_RULE_FLAGS_TYPE_EVERY \
                (DSMIB_CLASS_RULE_FLAGS_MATCH_EXCLUDEFLAG)

#define DSMIB_CLASS_RULE_FLAGS_TYPE_IP6VFLOWLBL \
                (DSMIB_CLASS_RULE_FLAGS_MATCH_IPV6FLOWLBL | \
                 DSMIB_CLASS_RULE_FLAGS_MATCH_EXCLUDEFLAG)

#define DSMIB_CLASS_RULE_FLAGS_TYPE_IPDSCP \
                (DSMIB_CLASS_RULE_FLAGS_MATCH_IPDSCP | \
                 DSMIB_CLASS_RULE_FLAGS_MATCH_EXCLUDEFLAG)

#define DSMIB_CLASS_RULE_FLAGS_TYPE_IPPRECEDENCE \
                (DSMIB_CLASS_RULE_FLAGS_MATCH_IPPRECEDENCE | \
                 DSMIB_CLASS_RULE_FLAGS_MATCH_EXCLUDEFLAG)

#define DSMIB_CLASS_RULE_FLAGS_TYPE_IPTOS \
                (DSMIB_CLASS_RULE_FLAGS_MATCH_IPTOSBITS | \
                 DSMIB_CLASS_RULE_FLAGS_MATCH_IPTOSMASK | \
                 DSMIB_CLASS_RULE_FLAGS_MATCH_EXCLUDEFLAG)

#define DSMIB_CLASS_RULE_FLAGS_TYPE_PROTOCOL \
                (DSMIB_CLASS_RULE_FLAGS_MATCH_PROTOCOLNUM | \
                 DSMIB_CLASS_RULE_FLAGS_MATCH_EXCLUDEFLAG)

#define DSMIB_CLASS_RULE_FLAGS_TYPE_REFCLASS \
                (DSMIB_CLASS_RULE_FLAGS_MATCH_REFCLASSINDEX | \
                 DSMIB_CLASS_RULE_FLAGS_MATCH_EXCLUDEFLAG)

#define DSMIB_CLASS_RULE_FLAGS_TYPE_SRCIP \
                (DSMIB_CLASS_RULE_FLAGS_MATCH_IPADDR | \
                 DSMIB_CLASS_RULE_FLAGS_MATCH_IPMASK | \
                 DSMIB_CLASS_RULE_FLAGS_MATCH_EXCLUDEFLAG)

#define DSMIB_CLASS_RULE_FLAGS_TYPE_SRCIPV6 \
                (DSMIB_CLASS_RULE_FLAGS_MATCH_IPV6ADDR | \
                 DSMIB_CLASS_RULE_FLAGS_MATCH_IPV6PLEN | \
                 DSMIB_CLASS_RULE_FLAGS_MATCH_EXCLUDEFLAG)

#define DSMIB_CLASS_RULE_FLAGS_TYPE_SRCL4PORT \
                (DSMIB_CLASS_RULE_FLAGS_MATCH_L4PORT_START | \
                 DSMIB_CLASS_RULE_FLAGS_MATCH_L4PORT_END | \
                 DSMIB_CLASS_RULE_FLAGS_MATCH_EXCLUDEFLAG)

#define DSMIB_CLASS_RULE_FLAGS_TYPE_SRCMAC \
                (DSMIB_CLASS_RULE_FLAGS_MATCH_MACADDR | \
                 DSMIB_CLASS_RULE_FLAGS_MATCH_MACMASK | \
                 DSMIB_CLASS_RULE_FLAGS_MATCH_EXCLUDEFLAG)

#define DSMIB_CLASS_RULE_FLAGS_TYPE_VLANID \
                (DSMIB_CLASS_RULE_FLAGS_MATCH_VIDSTART | \
                 DSMIB_CLASS_RULE_FLAGS_MATCH_VIDEND | \
                 DSMIB_CLASS_RULE_FLAGS_MATCH_EXCLUDEFLAG)

#define DSMIB_CLASS_RULE_FLAGS_TYPE_VLANID2 \
                (DSMIB_CLASS_RULE_FLAGS_MATCH_VIDSTART | \
                 DSMIB_CLASS_RULE_FLAGS_MATCH_VIDEND | \
                 DSMIB_CLASS_RULE_FLAGS_MATCH_EXCLUDEFLAG)

/* type-specific match fields */
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
  {                                             /* Dst IPv6 type:             */
    L7_in6_prefix_t   ip6prefix;                /*   IPv6 address and prefix length */
  } dstIpv6;                                  
                                            
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
                                             
                                                /* IPv6 Flow Label type:      */
  L7_uint32           ipv6FlowLabel;            /*   IPv6 Flow Label          */

                                                /* IP DSCP type:              */
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
  {                                             /* Src IPv6 type:             */
    L7_in6_prefix_t   ip6prefix;                /*   IPv6 address and prefix length */
  } srcIpv6;                                  
                                            
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
                                            
} dsmibClassRuleData_t;

/* MIB entry definition */
typedef struct
{
  L7_uint32             index;                  /* row index (N-A)            */
  dsmibClassRuleType_t  entryType;              /* defines class rule union   */
  dsmibClassRuleData_t  match;                  /* type-based rule match info */
  dsmibTruthValue_t     excludeFlag;            /* F=include, T=exclude       */
  dsmibStorageType_t    storageType;            /* type of config storage     */
  dsmibRowStatus_t      rowStatus;              /* status of this row         */

} dsmibClassRuleMib_t;

/* search keys */
typedef struct
{
  L7_uint32             classIndex;             /* AVL KEY: class index       */
  L7_uint32             classRuleIndex;         /* AVL KEY: class rule index  */
} dsmibClassRuleKey_t;

/* MIB controls */
typedef struct
{
  /* NOTE:  The search keys MUST be the first items in this structure */
  dsmibClassRuleKey_t   key;                    /* AVL search key             */

  /* MIB control fields */
  dsmibTableId_t        tableId;                /* table containing this row  */
  dsmibClassRuleFlags_t rowInvalidFlags;        /* row objects invalid flags  */
  dsmibClassEntryCtrl_t *pClass;                /* back ptr (for convenience) */
  L7_BOOL               rowPending;             /* initial row completion pend*/
  L7_uint32             arid;                   /* ACL rule identifier        */

  /* MIB fields */
  dsmibClassRuleMib_t   mib;                    /* MIB object definitions     */

  /* NOTE:  The AVL utility REQUIRES a next ptr as the LAST element */
  void                  *next;                  /* (RESERVED FOR AVL USE)     */

} dsmibClassRuleEntryCtrl_t;


/****************************************
 *
 * Policy Table
 *
 ****************************************
 */

typedef L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_t     dsmibPolicyType_t;

/* MIB entry flags */
typedef enum
{
  DSMIB_POLICY_FLAGS_NAME =                     (1 << 0),
  DSMIB_POLICY_FLAGS_TYPE =                     (1 << 1)

} dsmibPolicyFlags_t;

/* specific flag combinations */
#define DSMIB_POLICY_FLAGS_COMMON \
                (DSMIB_POLICY_FLAGS_NAME | \
                 DSMIB_POLICY_FLAGS_TYPE)

/* MIB entry definition */
typedef struct
{
  L7_uint32           index;                    /* row index (N-A)            */
  L7_uchar8           name[L7_USMDB_MIB_DIFFSERV_POLICY_NAME_MAX+1]; /*name string*/
  dsmibPolicyType_t   type;                     /* type of policy definition  */
  L7_uint32           instIndexNextFree;        /* next avail inst idx (R-O)  */
  dsmibStorageType_t  storageType;              /* type of config storage     */
  dsmibRowStatus_t    rowStatus;                /* status of this row         */

} dsmibPolicyMib_t;

/* search keys */
typedef struct
{
  L7_uint32           policyIndex;              /* AVL KEY: policy index      */
} dsmibPolicyKey_t;

/* MIB controls */
typedef struct
{
  /* NOTE:  The search keys MUST be the first items in this structure */
  dsmibPolicyKey_t    key;                      /* AVL search keys            */

  /* MIB control fields */
  dsmibTableId_t      tableId;                  /* table containing this row  */
  dsmibPolicyFlags_t  rowInvalidFlags;          /* row objects invalid flags  */
  L7_uint32           refCount;                 /* num service references     */
  L7_uint32           instCount;                /* num policy class instances */
  L7_BOOL             rowPending;               /* initial row completion pend*/

  /* MIB fields */
  dsmibPolicyMib_t    mib;                      /* MIB object definitions     */

  /* NOTE:  The AVL utility REQUIRES a next ptr as the LAST element */
  void                *next;                    /* (RESERVED FOR AVL USE)     */

} dsmibPolicyEntryCtrl_t;


/****************************************
 *
 * Policy-Class Instance Table
 *
 ****************************************
 */

/* MIB entry flags */
typedef enum
{
  DSMIB_POLICY_INST_FLAGS_CLASSINDEX =          (1 << 0)

} dsmibPolicyInstFlags_t;

/* specific flag combinations */
#define DSMIB_POLICY_INST_FLAGS_COMMON \
                (DSMIB_POLICY_INST_FLAGS_CLASSINDEX)

/* MIB entry definition */
typedef struct
{
  L7_uint32               index;                /* row index (N-A)            */
  L7_uint32               classIndex;           /* class instance definition  */
  L7_uint32               attrIndexNextFree;    /* next avail attrib idx (R-O)*/
  dsmibStorageType_t      storageType;          /* type of config storage     */
  dsmibRowStatus_t        rowStatus;            /* status of this row         */

} dsmibPolicyInstMib_t;

/* search keys */
typedef struct
{
  L7_uint32               policyIndex;          /* AVL KEY: policy index      */
  L7_uint32               policyInstIndex;      /* AVL KEY: policy inst index */
} dsmibPolicyInstKey_t;

/* MIB controls */
typedef struct
{
  /* NOTE:  The search keys MUST be the first items in this structure */
  dsmibPolicyInstKey_t    key;                  /* AVL search keys            */

  /* MIB control fields */
  dsmibTableId_t          tableId;              /* table containing this row  */
  dsmibPolicyInstFlags_t  rowInvalidFlags;      /* row objects invalid flags  */
  dsmibPolicyEntryCtrl_t  *pPolicy;             /* back ptr (for convenience) */
  L7_uint32               attrCount;            /* num policy attr instances  */
  L7_BOOL                 rowPending;           /* initial row completion pend*/

  /* MIB fields */
  dsmibPolicyInstMib_t    mib;                  /* MIB object definitions     */

  /* NOTE:  The AVL utility REQUIRES a next ptr as the LAST element */
  void                    *next;                /* (RESERVED FOR AVL USE)     */

} dsmibPolicyInstEntryCtrl_t;


/****************************************
 *
 * Policy Attribute Table
 *
 ****************************************
 */

typedef L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t    dsmibPolicyAttrType_t;
typedef L7_USMDB_MIB_DIFFSERV_BW_UNITS_t            dsmibBwUnits_t;
typedef L7_USMDB_MIB_DIFFSERV_POLICE_ACT_t          dsmibPoliceAct_t;
typedef L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_LEVEL_t  dsmibPoliceColorLevel_t;
typedef L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_t        dsmibPoliceColor_t;

/* MIB entry flags (limited to at most 32 flag values) */
typedef enum
{
  DSMIB_POLICY_ATTR_FLAGS_STMT_ENTRYTYPE =         (1 << 0), 
  DSMIB_POLICY_ATTR_FLAGS_STMT_QUEUEID =           (1 << 1), 
  DSMIB_POLICY_ATTR_FLAGS_STMT_CRATE =             (1 << 2), 
  DSMIB_POLICY_ATTR_FLAGS_STMT_CRATEUNITS =        (1 << 3), 
  DSMIB_POLICY_ATTR_FLAGS_STMT_CBURST =            (1 << 4), 
  DSMIB_POLICY_ATTR_FLAGS_STMT_EBURST =            (1 << 5), 
  DSMIB_POLICY_ATTR_FLAGS_STMT_PRATE =             (1 << 6), 
  DSMIB_POLICY_ATTR_FLAGS_STMT_PBURST =            (1 << 7), 
  DSMIB_POLICY_ATTR_FLAGS_STMT_COSVAL =            (1 << 8), 
  DSMIB_POLICY_ATTR_FLAGS_STMT_COS2VAL =           (1 << 9), 
  DSMIB_POLICY_ATTR_FLAGS_STMT_IPDSCPVAL =         (1 << 10),
  DSMIB_POLICY_ATTR_FLAGS_STMT_IPPRECEDENCEVAL =   (1 << 11),
  DSMIB_POLICY_ATTR_FLAGS_STMT_CONFORMACT =        (1 << 12), /* defaulted */ 
  DSMIB_POLICY_ATTR_FLAGS_STMT_EXCEEDACT =         (1 << 13), /* defaulted */ 
  DSMIB_POLICY_ATTR_FLAGS_STMT_NONCONFORMACT =     (1 << 14), /* defaulted */ 
  DSMIB_POLICY_ATTR_FLAGS_STMT_CONFORMVAL =        (1 << 15),                 
  DSMIB_POLICY_ATTR_FLAGS_STMT_EXCEEDVAL =         (1 << 16),                 
  DSMIB_POLICY_ATTR_FLAGS_STMT_NONCONFORMVAL =     (1 << 17),                 
  DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMINDEX = (1 << 18), /* defaulted */
  DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMMODE =  (1 << 19), /* defaulted */
  DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMVAL =   (1 << 20),                 
  DSMIB_POLICY_ATTR_FLAGS_STMT_COLOREXCEEDINDEX =  (1 << 21), /* defaulted */ 
  DSMIB_POLICY_ATTR_FLAGS_STMT_COLOREXCEEDMODE =   (1 << 22), /* defaulted */ 
  DSMIB_POLICY_ATTR_FLAGS_STMT_COLOREXCEEDVAL =    (1 << 23),                 
  DSMIB_POLICY_ATTR_FLAGS_STMT_MINTHRESH =         (1 << 24),                 
  DSMIB_POLICY_ATTR_FLAGS_STMT_MAXTHRESH =         (1 << 25),                 
  DSMIB_POLICY_ATTR_FLAGS_STMT_MAXDROPPROB =       (1 << 26),                 
  DSMIB_POLICY_ATTR_FLAGS_STMT_SAMPLINGRATE =      (1 << 27), /* defaulted */ 
  DSMIB_POLICY_ATTR_FLAGS_STMT_DECAYEXPONENT =     (1 << 28), /* defaulted */ 
  DSMIB_POLICY_ATTR_FLAGS_STMT_REDIRECTINTF =      (1 << 29),
  DSMIB_POLICY_ATTR_FLAGS_STMT_MIRRORINTF =        (1 << 30),
  DSMIB_POLICY_ATTR_FLAGS_STMT_COS_AS_COS2 =        (1 << 31)

} dsmibPolicyAttrFlags_t;

/* specific flag combinations (does not include defaulted objects) */
#define DSMIB_POLICY_ATTR_FLAGS_COMMON \
                (DSMIB_POLICY_ATTR_FLAGS_STMT_ENTRYTYPE)

#define DSMIB_POLICY_ATTR_FLAGS_TYPE_ASSIGN_QUEUE \
                (DSMIB_POLICY_ATTR_FLAGS_STMT_QUEUEID)

#define DSMIB_POLICY_ATTR_FLAGS_TYPE_DROP \
                (DSMIB_NO_FLAG)

#define DSMIB_POLICY_ATTR_FLAGS_TYPE_MARK_COSVAL \
                (DSMIB_POLICY_ATTR_FLAGS_STMT_COSVAL)

#define DSMIB_POLICY_ATTR_FLAGS_TYPE_MARK_COS_AS_COS2 \
                (DSMIB_POLICY_ATTR_FLAGS_STMT_COS_AS_COS2)

#define DSMIB_POLICY_ATTR_FLAGS_TYPE_MARK_COS2VAL \
                (DSMIB_POLICY_ATTR_FLAGS_STMT_COS2VAL)

#define DSMIB_POLICY_ATTR_FLAGS_TYPE_MARK_IPDSCPVAL \
                (DSMIB_POLICY_ATTR_FLAGS_STMT_IPDSCPVAL)

#define DSMIB_POLICY_ATTR_FLAGS_TYPE_MARK_IPPRECEDENCEVAL \
                (DSMIB_POLICY_ATTR_FLAGS_STMT_IPPRECEDENCEVAL)

#define DSMIB_POLICY_ATTR_FLAGS_TYPE_MIRROR \
                (DSMIB_POLICY_ATTR_FLAGS_STMT_MIRRORINTF)

#define DSMIB_POLICY_ATTR_FLAGS_TYPE_POLICE_CONFORM_MARKCOS \
                 (DSMIB_POLICY_ATTR_FLAGS_STMT_CONFORMVAL)

#define DSMIB_POLICY_ATTR_FLAGS_TYPE_POLICE_EXCEED_MARKCOS \
                 (DSMIB_POLICY_ATTR_FLAGS_STMT_EXCEEDVAL)

#define DSMIB_POLICY_ATTR_FLAGS_TYPE_POLICE_NONCONFORM_MARKCOS \
                 (DSMIB_POLICY_ATTR_FLAGS_STMT_NONCONFORMVAL)

#define DSMIB_POLICY_ATTR_FLAGS_TYPE_POLICE_CONFORM_MARKCOS2 \
                 (DSMIB_POLICY_ATTR_FLAGS_STMT_CONFORMVAL)

#define DSMIB_POLICY_ATTR_FLAGS_TYPE_POLICE_EXCEED_MARKCOS2 \
                 (DSMIB_POLICY_ATTR_FLAGS_STMT_EXCEEDVAL)

#define DSMIB_POLICY_ATTR_FLAGS_TYPE_POLICE_NONCONFORM_MARKCOS2 \
                 (DSMIB_POLICY_ATTR_FLAGS_STMT_NONCONFORMVAL)

#define DSMIB_POLICY_ATTR_FLAGS_TYPE_POLICE_CONFORM_MARKDSCP \
                 (DSMIB_POLICY_ATTR_FLAGS_STMT_CONFORMVAL)

#define DSMIB_POLICY_ATTR_FLAGS_TYPE_POLICE_EXCEED_MARKDSCP \
                 (DSMIB_POLICY_ATTR_FLAGS_STMT_EXCEEDVAL)

#define DSMIB_POLICY_ATTR_FLAGS_TYPE_POLICE_NONCONFORM_MARKDSCP \
                 (DSMIB_POLICY_ATTR_FLAGS_STMT_NONCONFORMVAL)

#define DSMIB_POLICY_ATTR_FLAGS_TYPE_POLICE_CONFORM_MARKPREC \
                 (DSMIB_POLICY_ATTR_FLAGS_STMT_CONFORMVAL)

#define DSMIB_POLICY_ATTR_FLAGS_TYPE_POLICE_EXCEED_MARKPREC \
                 (DSMIB_POLICY_ATTR_FLAGS_STMT_EXCEEDVAL)

#define DSMIB_POLICY_ATTR_FLAGS_TYPE_POLICE_NONCONFORM_MARKPREC \
                 (DSMIB_POLICY_ATTR_FLAGS_STMT_NONCONFORMVAL)

#define DSMIB_POLICY_ATTR_FLAGS_TYPE_POLICE_COLORAWARE \
                 (DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMMODE | \
                  DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMVAL)

#define DSMIB_POLICY_ATTR_FLAGS_TYPE_POLICE_SIMPLE \
                (DSMIB_POLICY_ATTR_FLAGS_STMT_CRATE | \
                 DSMIB_POLICY_ATTR_FLAGS_STMT_CBURST)

#define DSMIB_POLICY_ATTR_FLAGS_TYPE_POLICE_SINGLERATE \
                (DSMIB_POLICY_ATTR_FLAGS_STMT_CRATE | \
                 DSMIB_POLICY_ATTR_FLAGS_STMT_CBURST | \
                 DSMIB_POLICY_ATTR_FLAGS_STMT_EBURST)

#define DSMIB_POLICY_ATTR_FLAGS_TYPE_POLICE_TWORATE \
                (DSMIB_POLICY_ATTR_FLAGS_STMT_CRATE | \
                 DSMIB_POLICY_ATTR_FLAGS_STMT_CBURST | \
                 DSMIB_POLICY_ATTR_FLAGS_STMT_PRATE | \
                 DSMIB_POLICY_ATTR_FLAGS_STMT_PBURST)

#define DSMIB_POLICY_ATTR_FLAGS_TYPE_REDIRECT \
                (DSMIB_POLICY_ATTR_FLAGS_STMT_REDIRECTINTF)

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

  dsmibTruthValue_t     drop;                   /* Drop attribute flag (R-O)  */
                                             
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
  {                                             /* Mark COS as Secondary COS (Cos2) type  */
    L7_BOOL             val;                    /*   TRUE or FALSE               */
  } markCosAsCos2;   

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
  {                                             /* Mirror type:               */
    nimConfigID_t       configId;               /*   intf config ID           */
  } mirror;

  struct 
  {                                             /* Police Simple type:        */
    L7_uint32           crate;                  /*   committed rate (CIR)     */
    L7_uint32           cburst;                 /*   committed burst (CBS)    */
    dsmibPoliceAct_t    conformAct;             /*   conform action           */
    dsmibPoliceAct_t    nonconformAct;          /*   nonconform action        */
    L7_uchar8           conformVal;             /*   conform DSCP mark val    */
    L7_uchar8           nonconformVal;          /*   nonconform DSCP mark val */
    L7_uchar8           rsvd1[2];               /*   (rsvd -- for alignment)  */
    L7_uint32           colorConformIndex;      /*   color conform index      */
    dsmibPoliceColor_t  colorConformMode;       /*   color conform mode       */
    L7_uchar8           colorConformVal;        /*   color-aware conform val  */
    L7_uchar8           rsvd2[3];               /*   (rsvd -- for alignment)  */
  } policeSimple;

  struct 
  {                                             /* Police Single Rate type:   */
    L7_uint32           crate;                  /*   committed rate (CIR)     */
    L7_uint32           cburst;                 /*   committed burst (CBS)    */
    L7_uint32           eburst;                 /*   excess burst (EBS)       */
    dsmibPoliceAct_t    conformAct;             /*   conform action           */
    dsmibPoliceAct_t    exceedAct;              /*   exceed action            */
    dsmibPoliceAct_t    nonconformAct;          /*   nonconform action        */
    L7_uchar8           conformVal;             /*   conform DSCP mark val    */
    L7_uchar8           exceedVal;              /*   exceed DSCP mark val     */
    L7_uchar8           nonconformVal;          /*   nonconform DSCP mark val */
    L7_uchar8           rsvd1[1];               /*   (rsvd -- for alignment)  */
    L7_uint32           colorConformIndex;      /*   color conform index      */
    L7_uint32           colorExceedIndex;       /*   color exceed index       */
    dsmibPoliceColor_t  colorConformMode;       /*   color conform mode       */
    dsmibPoliceColor_t  colorExceedMode;        /*   color exceed mode        */
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
    dsmibPoliceAct_t    conformAct;             /*   conform action           */
    dsmibPoliceAct_t    exceedAct;              /*   exceed action            */
    dsmibPoliceAct_t    nonconformAct;          /*   nonconform action        */
    L7_uchar8           conformVal;             /*   conform DSCP mark val    */
    L7_uchar8           exceedVal;              /*   exceed DSCP mark val     */
    L7_uchar8           nonconformVal;          /*   nonconform DSCP mark val */
    L7_uchar8           rsvd1[1];               /*   (rsvd -- for alignment)  */
    L7_uint32           colorConformIndex;      /*   color conform index      */
    L7_uint32           colorExceedIndex;       /*   color exceed index       */
    dsmibPoliceColor_t  colorConformMode;       /*   color conform mode       */
    dsmibPoliceColor_t  colorExceedMode;        /*   color exceed mode        */
    L7_uchar8           colorConformVal;        /*   color-aware conform val  */
    L7_uchar8           colorExceedVal;         /*   color-aware exceed val   */
    L7_uchar8           rsvd2[2];               /*   (rsvd -- for alignment)  */
  } policeTwoRate;

  struct 
  {                                             /* Redirect type:             */
    nimConfigID_t       configId;               /*   intf config ID           */
  } redirect;      

} dsmibPolicyAttrData_t;

/* MIB entry definition */
typedef struct
{
  L7_uint32             index;                  /* row index (N-A)            */
  dsmibPolicyAttrType_t entryType;              /* defines policy stmt union  */
  dsmibPolicyAttrData_t stmt;                   /* type-based policy statement*/
  dsmibStorageType_t    storageType;            /* type of config storage     */
  dsmibRowStatus_t      rowStatus;              /* status of this row         */

} dsmibPolicyAttrMib_t;

/* search keys */
typedef struct
{
  L7_uint32             policyIndex;            /* AVL KEY: policy index      */
  L7_uint32             policyInstIndex;        /* AVL KEY: policy inst index */
  L7_uint32             policyAttrIndex;        /* AVL KEY: policy attrib idx */
} dsmibPolicyAttrKey_t;

/* MIB controls */
typedef struct
{
  /* NOTE:  The search keys MUST be the first items in this structure */
  dsmibPolicyAttrKey_t        key;              /* AVL search keys            */

  /* MIB control fields */
  dsmibTableId_t              tableId;          /* table containing this row  */
  dsmibPolicyAttrFlags_t      rowInvalidFlags;  /* row objects invalid flags  */
  dsmibPolicyInstEntryCtrl_t  *pPolicyInst;     /* back ptr (for convenience) */
  L7_BOOL                     rowPending;       /* initial row completion pend*/

  /* MIB fields */
  dsmibPolicyAttrMib_t        mib;              /* MIB object definitions     */

  /* NOTE:  The AVL utility REQUIRES a next ptr as the LAST element */
  void                        *next;            /* (RESERVED FOR AVL USE)     */

} dsmibPolicyAttrEntryCtrl_t;


/****************************************
 *
 * Policy-Class Inbound Performance Table
 *
 ****************************************
 */

/* MIB entry flags */
typedef enum
{
  DSMIB_POLICY_PERF_IN_FLAGS_COMMON =           0   /* all objects R-O */

} dsmibPolicyPerfInFlags_t;

/* MIB entry definition */
typedef struct
{
  /* NOTE:  Only the 64-bit version of each counter is maintained in this
   *        structure (the MIB code obtains 32-bit versions from this).
   */

  L7_ulong64          HCOfferedOctets;          /* offered octets count       */
  L7_ulong64          HCOfferedPackets;         /* offered packets count      */
  L7_ulong64          HCDiscardedOctets;        /* discarded octets count     */
  L7_ulong64          HCDiscardedPackets;       /* discarded packets count    */
  dsmibStorageType_t  storageType;              /* type of config storage     */
  dsmibRowStatus_t    rowStatus;                /* status of this row         */

} dsmibPolicyPerfInMib_t;

/* search keys */
typedef struct
{
  L7_uint32           policyIndex;              /* AVL KEY: policy index      */
  L7_uint32           policyInstIndex;          /* AVL KEY: policy inst index */
  L7_uint32           ifIndex;                  /* AVL KEY: interface index   */
} dsmibPolicyPerfInKey_t;

/* MIB controls */
typedef struct
{
  /* NOTE:  The search keys MUST be the first items in this structure */
  dsmibPolicyPerfInKey_t    key;                /* AVL search keys            */

  /* MIB control fields */
  dsmibTableId_t            tableId;            /* table containing this row  */
  dsmibPolicyPerfInFlags_t  rowInvalidFlags;    /* row objects invalid flags  */
  dsmibPolicyInstEntryCtrl_t  *pPolicyInst;     /* back ptr (for convenience) */
  L7_BOOL                   rowPending;         /* initial row completion pend*/

  /* MIB fields */
  dsmibPolicyPerfInMib_t    mib;                /* MIB inbound perf counters  */

  /* NOTE:  The AVL utility REQUIRES a next ptr as the LAST element */
  void                      *next;              /* (RESERVED FOR AVL USE)     */

} dsmibPolicyPerfInCtrl_t;


/*****************************************
 *
 * Policy-Class Outbound Performance Table
 *
 *****************************************
 */

/* MIB entry flags */
typedef enum
{
  DSMIB_POLICY_PERF_OUT_FLAGS_COMMON =          0   /* all objects R-O */

} dsmibPolicyPerfOutFlags_t;

/* MIB entry definition */
typedef struct
{
  /* NOTE:  Only the 64-bit version of each counter is maintained in this
   *        structure (the MIB code obtains 32-bit versions from this).
   */

  L7_ulong64          HCOfferedOctets;          /* offered octets count       */
  L7_ulong64          HCOfferedPackets;         /* offered packets count      */
  L7_ulong64          HCDiscardedOctets;        /* discarded octets count     */
  L7_ulong64          HCDiscardedPackets;       /* discarded packets count    */
  dsmibStorageType_t  storageType;              /* type of config storage     */
  dsmibRowStatus_t    rowStatus;                /* status of this row         */

} dsmibPolicyPerfOutMib_t;

/* search keys */
typedef struct
{
  L7_uint32           policyIndex;              /* AVL KEY: policy index      */
  L7_uint32           policyInstIndex;          /* AVL KEY: policy inst index */
  L7_uint32           ifIndex;                  /* AVL KEY: interface index   */
} dsmibPolicyPerfOutKey_t;


/* MIB controls */
typedef struct
{
  /* NOTE:  The search keys MUST be the first items in this structure */
  dsmibPolicyPerfOutKey_t     key;              /* AVL search keys            */

  /* MIB control fields */
  dsmibTableId_t              tableId;          /* table containing this row  */
  dsmibPolicyPerfOutFlags_t   rowInvalidFlags;  /* row objects invalid flags  */
  dsmibPolicyInstEntryCtrl_t  *pPolicyInst;     /* back ptr (for convenience) */
  L7_BOOL                     rowPending;       /* initial row completion pend*/

  /* MIB fields */
  dsmibPolicyPerfOutMib_t     mib;              /* MIB outbound perf counters */

  /* NOTE:  The AVL utility REQUIRES a next ptr as the LAST element */
  void                        *next;            /* (RESERVED FOR AVL USE)     */

} dsmibPolicyPerfOutCtrl_t;


/****************************************
 *
 * Service Table
 * Service Performance Table (Augment)
 *
 ****************************************
 */

typedef L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t    dsmibServiceIfDir_t;
typedef L7_USMDB_MIB_DIFFSERV_IF_OPER_STATUS_t  dsmibServiceIfOper_t;

#define DSMIB_SERVICE_PERF_CTR_UPDATE_LIMIT     3     /* seconds */

/* MIB entry flags */
typedef enum
{
  DSMIB_SERVICE_FLAGS_POLICYINDEX =             (1 << 0)

} dsmibServiceFlags_t;

/* specific flag combinations */
#define DSMIB_SERVICE_FLAGS_COMMON \
                (DSMIB_SERVICE_FLAGS_POLICYINDEX)

/* service performance counters */
typedef struct
{
  /* NOTE:  Only the 64-bit version of each counter is maintained in this
   *        structure (the MIB code obtains 32-bit versions from this).
   */

  L7_ulong64          HCOfferedOctets;          /* offered octets count       */
  L7_ulong64          HCOfferedPackets;         /* offered packets count      */
  L7_ulong64          HCDiscardedOctets;        /* discarded octets count     */
  L7_ulong64          HCDiscardedPackets;       /* discarded packets count    */
  L7_ulong64          HCSentOctets;             /* sent octets count          */
  L7_ulong64          HCSentPackets;            /* sent packets count         */

} dsmibServicePerf_t;

/* MIB entry definition */
typedef struct
{
  L7_uint32           index;                    /* intf index (N-A)           */
  dsmibServiceIfDir_t direction;                /* intf direction (N-A)       */
  L7_uint32           policyIndex;              /* index of attached policy   */
  dsmibServiceIfOper_t  ifOperStatus;           /* intf oper status (R-O)     */
  dsmibStorageType_t  storageType;              /* type of config storage     */
  dsmibRowStatus_t    rowStatus;                /* status of this row         */

} dsmibServiceMib_t;

/* search keys */
typedef struct
{
  L7_uint32           ifIndex;                  /* AVL KEY: intf index        */
  dsmibServiceIfDir_t ifDirection;              /* AVL KEY: intf direction    */
} dsmibServiceKey_t;

/* MIB controls */
typedef struct
{
  /* NOTE:  The search keys MUST be the first items in this structure */
  dsmibServiceKey_t   key;                      /* AVL search keys            */

  /* MIB control fields */
  dsmibTableId_t      tableId;                  /* table containing this row  */
  dsmibServiceFlags_t rowInvalidFlags;          /* row objects invalid flags  */
  L7_uint32           intIfNum;                 /* intf number (for easy ref.)*/
  L7_uint32           lastUpdateTime;           /* sys up time of last refresh*/
  L7_BOOL             rowPending;               /* initial row completion pend*/

  /* MIB fields */
  dsmibServiceMib_t   mib;                      /* MIB object definitions     */
  dsmibServicePerf_t  mibPerf;                  /* MIB service perf counters  */

  /* NOTE:  The AVL utility REQUIRES a next ptr as the LAST element */
  void                *next;                    /* (RESERVED FOR AVL USE)     */

} dsmibServiceEntryCtrl_t;


/* reference to MIB instantiations */
extern dsmibGenStatusGroup_t   genStatus;      /* General Status Group */


/*********************************
 *
 * Internal Function Prototypes
 *
 *********************************
 */

/* diffserv_prvtmib.c */
L7_RC_t diffServPrivateMibInit(void);
void diffServPrivateMibFini(void);
L7_RC_t diffServPrivateMibReset(void);
L7_BOOL dsmibSpecialConfigModeGet(void);
void dsmibSpecialConfigModeOn(void);
void dsmibSpecialConfigModeOff(void);
L7_RC_t dsmibMibTableSizeCheck(dsmibTableId_t tableId);
L7_RC_t dsmibRowInsert(avlTree_t *pTree, void *pElem);
L7_RC_t dsmibRowDelete(avlTree_t *pTree, void *pElem);
void *dsmibRowFind(avlTree_t *pTree, void *pKeys, L7_uint32 matchMode);
L7_BOOL dsmibStorageTypeIsValid(dsmibStorageType_t val);
L7_BOOL dsmibRowStatusIsValid(dsmibRowStatus_t val);
L7_RC_t dsmibRowStatusObservable(dsmibRowStatus_t val, 
                                 L7_uint32 rowInvalidFlags, 
                                 dsmibRowStatus_t *pOut);
void    dsmibRowCompleteCheck(dsmibRowStatus_t oldVal, L7_uint32 flags,
                              dsmibRowStatus_t *pNewVal, L7_BOOL *pJustCompleted);
L7_RC_t dsmibRowStatusUpdate(dsmibRowStatus_t oldVal, dsmibRowStatus_t reqVal,
                             L7_uint32 flags, dsmibRowStatus_t *pRowStatus);
L7_RC_t dsmibRowEventCheck(dsmibTableId_t tableId, void *pRow,
                           dsmibRowStatus_t oldStatus,
                           dsmibRowStatus_t newStaus);
void dsmibAvlCtrlShow(void);
void dsmibPrvtShowAll(void);

/* diffserv_prvtmib_api.c */
void dsmibGenStatusShow(void);

/* diffserv_class_api.c */
L7_RC_t dsmibClassSetTest(L7_uint32 classIndex, 
                          L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_TYPE_t oid, 
                          void *pValue,
                          dsmibClassEntryCtrl_t **ppRow,
                          size_t *pValueLen);
L7_RC_t dsmibClassObjectSet(L7_uint32 classIndex,
                            L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_TYPE_t oid, 
                            void *pValue,
                            dsmibClassEntryCtrl_t **ppRow,
                            size_t *pValueLen);
void    dsmibClassIndexNextInit(void);
void    dsmibClassIndexNextRead(L7_uint32 *pNextFree);
void    dsmibClassIndexNextUpdate(void);
L7_RC_t dsmibClassIndexNextVerify(L7_uint32 classIndex);
dsmibClassEntryCtrl_t *dsmibClassRowFind(L7_uint32 classIndex);
L7_BOOL dsmibClassIsInRefChain(L7_uint32 classIndex,
                               L7_uint32 refClassIndex);
L7_RC_t dsmibClassObjectGetCheck(dsmibClassEntryCtrl_t *pRow,
                                 L7_DIFFSERV_CLASS_TABLE_OBJECT_t oid);
L7_RC_t dsmibClassAclSetup(dsmibClassEntryCtrl_t *pRow, L7_uint32 aclNum);
void    dsmibClassConvertOidToType(L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_TYPE_t oid,
                                   L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_t *pClassType);
L7_RC_t dsmibClassFeaturePresentCheck(L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_t classType);
L7_RC_t dsmibClassProtoFeaturePresentCheck(L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_t classL3Proto);
void    dsmibClassTableShow(void);

/* diffserv_classrule_api.c */
L7_RC_t dsmibClassRuleDelete(L7_uint32 classIndex, 
                             L7_uint32 classRuleIndex,
                             L7_BOOL   bypassEval);
L7_RC_t dsmibClassRuleSetTest(L7_uint32 classIndex,
                              L7_uint32 classRuleIndex,
                              L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_TYPE_t oid,
                              void *pValue,
                              dsmibClassRuleEntryCtrl_t **ppRow,
                              size_t *pValueLen);
L7_RC_t dsmibClassRuleObjectSet(L7_uint32 classIndex, 
                                L7_uint32 classRuleIndex,
                                L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_TYPE_t oid,
                                void *pValue,
                                dsmibClassRuleEntryCtrl_t **ppRow,
                                size_t *pValueLen);
L7_RC_t dsmibClassRuleRefChainGetNext(L7_uint32 prevClassIndex, 
                                      L7_uint32 prevClassRuleIndex,
                                      L7_uint32 *pClassIndex,
                                      L7_uint32 *pClassRuleIndex);
L7_RC_t dsmibClassRuleIndexNextRead(L7_uint32 classIndex, L7_uint32 *pNextFree);
void    dsmibClassRuleIndexNextUpdate(L7_uint32 classIndex);
void    dsmibClassRuleIndexNextUndo(L7_uint32 classIndex);
L7_RC_t dsmibClassRuleIndexNextVerify(L7_uint32 classIndex, 
                                      L7_uint32 classRuleIndex);
dsmibClassRuleEntryCtrl_t *dsmibClassRuleRowFind(L7_uint32 classIndex, 
                                                 L7_uint32 classRuleIndex);
L7_RC_t dsmibClassRuleAclCreate(L7_uint32 classIndex, 
                                dsmibClassRuleEntryCtrl_t **pRowPtr);
L7_RC_t dsmibClassRuleAclDelete(L7_uint32 classIndex);
L7_RC_t dsmibClassRuleInstanceValidate(dsmibClassRuleEntryCtrl_t *pRow);
void    dsmibClassRulePolicyChangeNotify(L7_uint32 classIndex);
L7_RC_t dsmibClassRuleObjectGetCheck(dsmibClassRuleEntryCtrl_t *pRow,
                                     L7_DIFFSERV_CLASS_RULE_TABLE_OBJECT_t oid);
L7_RC_t dsmibClassRuleRefLimitCheck(void);
L7_BOOL dsmibClassRuleIsRuleAllowed(L7_uint32 classIndex, 
                                    L7_uint32 classRuleIndex);
L7_BOOL dsmibClassRuleIsValueAllowed(L7_uint32 classIndex, 
                                     L7_uint32 classRuleIndex);
L7_RC_t dsmibClassRuleRowFlagsReplace(dsmibClassRuleEntryCtrl_t *pRow,
                                      dsmibClassRuleType_t entryType);
void    dsmibClassRuleConvertOidToType(L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_TYPE_t oid,
                                       L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t *pClassRuleType);
L7_RC_t dsmibClassRuleEtypeCompatCheck(dsmibClassRuleEntryCtrl_t *pRow1,
                                       dsmibClassRuleEntryCtrl_t *pRow2);
L7_RC_t dsmibEtypeKeyidTranslate(L7_uint32 classIndex, 
                                 L7_uint32 classRuleIndex,
                                 L7_uint32 *pValue1,
                                 L7_uint32 *pValue2);
L7_RC_t dsmibClassRuleFeaturePresentCheck(L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t classRuleType);
L7_RC_t diffServClassOrderedListGet(L7_uint32 *orderedClassList, L7_uint32 *count);
void    dsmibClassRuleTableShow(void);

/* diffserv_policy_api.c */
L7_RC_t dsmibPolicySetTest(L7_uint32 policyIndex, 
                           L7_USMDB_MIB_DIFFSERV_POLICY_SETTEST_OID_TYPE_t oid, 
                           void *pValue,
                           dsmibPolicyEntryCtrl_t **ppRow,
                           size_t *pValueLen);
L7_RC_t dsmibPolicyObjectSet(L7_uint32 policyIndex, 
                             L7_USMDB_MIB_DIFFSERV_POLICY_SETTEST_OID_TYPE_t oid, 
                             void *pValue,
                             dsmibPolicyEntryCtrl_t **ppRow,
                             size_t *pValueLen);
void    dsmibPolicyIndexNextInit(void);
void    dsmibPolicyIndexNextRead(L7_uint32 *pNextFree);
void    dsmibPolicyIndexNextUpdate(void);
L7_RC_t dsmibPolicyIndexNextVerify(L7_uint32 policyIndex);
dsmibPolicyEntryCtrl_t *dsmibPolicyRowFind(L7_uint32 policyIndex);
L7_RC_t dsmibPolicyObjectGetCheck(dsmibPolicyEntryCtrl_t *pRow, 
                                  L7_DIFFSERV_POLICY_TABLE_OBJECT_t oid);
L7_RC_t dsmibPolicyAttachConstraintCheck(L7_uint32 policyIndex, 
                                         L7_uint32 intIfNum,
                                         L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection,
                                         L7_BOOL *pIsValid);
void    dsmibPolicyTableShow(void);

/* diffserv_policyinst_api.c */
L7_RC_t dsmibPolicyInstDelete(L7_uint32 policyIndex, 
                              L7_uint32 policyInstIndex,
                              L7_BOOL   bypassEval);
L7_RC_t dsmibPolicyInstSetTest(L7_uint32 policyIndex, 
                               L7_uint32 policyInstIndex, 
                               L7_USMDB_MIB_DIFFSERV_POLICY_INST_SETTEST_OID_TYPE_t oid,
                               void *pValue,
                               dsmibPolicyInstEntryCtrl_t **ppRow,
                               size_t *pValueLen);
L7_RC_t dsmibPolicyInstObjectSet(L7_uint32 policyIndex,
                                 L7_uint32 policyInstIndex,
                                 L7_USMDB_MIB_DIFFSERV_POLICY_INST_SETTEST_OID_TYPE_t oid,
                                 void *pValue,
                                 dsmibPolicyInstEntryCtrl_t **ppRow,
                                 size_t *pValueLen);
L7_RC_t dsmibPolicyInstIndexNextRead(L7_uint32 policyIndex, L7_uint32 *pNextFree);
void    dsmibPolicyInstIndexNextUpdate(L7_uint32 policyIndex);
void    dsmibPolicyInstIndexNextUndo(L7_uint32 policyIndex);
L7_RC_t dsmibPolicyInstIndexNextVerify(L7_uint32 policyIndex, 
                                       L7_uint32 policyInstIndex);
dsmibPolicyInstEntryCtrl_t *dsmibPolicyInstRowFind(L7_uint32 policyIndex, 
                                                   L7_uint32 policyInstIndex);
L7_RC_t dsmibPolicyInstClassValidate(L7_uint32 policyIndex, 
                                     L7_uint32 classIndex);
L7_RC_t dsmibPolicyInstObjectGetCheck(dsmibPolicyInstEntryCtrl_t *pRow, 
                                      L7_DIFFSERV_POLICY_INST_TABLE_OBJECT_t oid);
L7_RC_t dsmibPolicyInstPerfAttach(L7_uint32 policyIndex, 
                                  L7_uint32 policyInstIndex);
L7_RC_t dsmibPolicyInstPerfDetach(L7_uint32 policyIndex, 
                                  L7_uint32 policyInstIndex);
void    dsmibPolicyInstTableShow(void);

/* diffserv_policyattr_api.c */
L7_RC_t dsmibPolicyAttrDelete(L7_uint32 policyIndex,
                              L7_uint32 policyInstIndex, 
                              L7_uint32 policyAttrIndex,
                              L7_BOOL   bypassEval);
L7_RC_t dsmibPolicyAttrSetTest(L7_uint32 policyIndex, 
                               L7_uint32 policyInstIndex, 
                               L7_uint32 policyAttrIndex, 
                               L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_TYPE_t oid,
                               void *pValue,
                               dsmibPolicyAttrEntryCtrl_t **ppRow,
                               size_t *pValueLen);
L7_RC_t dsmibPolicyAttrObjectSet(L7_uint32 policyIndex, 
                                 L7_uint32 policyInstIndex, 
                                 L7_uint32 policyAttrIndex, 
                                 L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_TYPE_t oid, 
                                 void *pValue,
                                 dsmibPolicyAttrEntryCtrl_t **ppRow,
                                 size_t *pValueLen);
L7_RC_t dsmibPolicyAttrIndexNextRead(L7_uint32 policyIndex, 
                                     L7_uint32 policyInstIndex, L7_uint32 *pNextFree);
void    dsmibPolicyAttrIndexNextUpdate(L7_uint32 policyIndex, L7_uint32 policyInstIndex);
void    dsmibPolicyAttrIndexNextUndo(L7_uint32 policyIndex, 
                                     L7_uint32 policyInstIndex);
L7_RC_t dsmibPolicyAttrIndexNextVerify(L7_uint32 policyIndex, 
                                       L7_uint32 policyInstIndex, L7_uint32 policyAttrIndex);
dsmibPolicyAttrEntryCtrl_t *dsmibPolicyAttrRowFind(L7_uint32 policyIndex, 
                                                   L7_uint32 policyInstIndex,
                                                   L7_uint32 policyAttrIndex);
L7_RC_t dsmibPolicyAttrObjectGetCheck(dsmibPolicyAttrEntryCtrl_t *pRow, 
                                      L7_DIFFSERV_POLICY_ATTR_TABLE_OBJECT_t oid);
L7_RC_t dsmibPolicyAttrTypeDefaultSet(dsmibPolicyAttrEntryCtrl_t *pRow, 
                                      dsmibPolicyAttrType_t entryType);
L7_RC_t dsmibPolicyAttrRowFlagsReplace(dsmibPolicyAttrEntryCtrl_t *pRow, 
                                       dsmibPolicyAttrType_t entryType);
L7_RC_t dsmibPolicyAttrTypeCheck(L7_uint32 policyIndex,
                                 L7_uint32 policyInstIndex,
                                 L7_uint32 policyAttrIndex,
                                 dsmibPolicyAttrType_t type);
L7_RC_t dsmibPolicyAttrColorModeConflictCheck(L7_uint32 policyIndex,
                                              L7_uint32 policyInstIndex,
                                              dsmibPoliceColor_t colorMode);
L7_RC_t dsmibPolicyAttrColorClassRefCountUpdate(L7_uint32 classIndexOld,
                                                L7_uint32 classIndexNew);
void    dsmibPolicyAttrConvertOidToType(L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_TYPE_t oid,
                                        L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t *pPolicyAttrType);
L7_RC_t dsmibPolicyAttrFeaturePresentCheck(L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t policyAttrType);
void    dsmibPolicyAttrIpDscpSupportInit(void);
void    dsmibPolicyAttrTableShow(void);

/* diffserv_policyperf_api.c */
L7_RC_t dsmibPolicyPerfInCreate(L7_uint32 policyIndex,
                                L7_uint32 policyInstIndex,
                                L7_uint32 intIfNum,
                                L7_BOOL activateRow);
L7_RC_t dsmibPolicyPerfInDelete(L7_uint32 policyIndex,
                                L7_uint32 policyInstIndex,
                                L7_uint32 intIfNum);
dsmibPolicyPerfInCtrl_t *dsmibPolicyPerfInRowFind(L7_uint32 policyIndex,
                                                  L7_uint32 policyInstIndex,
                                                  L7_uint32 intIfNum);
L7_RC_t dsmibPolicyPerfInObjectGetCheck(dsmibPolicyPerfInCtrl_t *pRow, 
                                        L7_DIFFSERV_POLICY_PERF_IN_TABLE_OBJECT_t oid);
L7_RC_t dsmibPolicyPerfOutCreate(L7_uint32 policyIndex,
                                 L7_uint32 policyInstIndex,
                                 L7_uint32 intIfNum,
                                 L7_BOOL activateRow);
L7_RC_t dsmibPolicyPerfOutDelete(L7_uint32 policyIndex,
                                 L7_uint32 policyInstIndex,
                                 L7_uint32 intIfNum);
dsmibPolicyPerfOutCtrl_t *dsmibPolicyPerfOutRowFind(L7_uint32 policyIndex,
                                                    L7_uint32 policyInstIndex,
                                                    L7_uint32 intIfNum);
L7_RC_t dsmibPolicyPerfOutObjectGetCheck(dsmibPolicyPerfOutCtrl_t *pRow, 
                                         L7_DIFFSERV_POLICY_PERF_OUT_TABLE_OBJECT_t oid);
L7_RC_t dsmibPolicyPerfAttach(L7_uint32 policyIndex, 
                              L7_uint32 intIfNum,
                              L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection);
L7_RC_t dsmibPolicyPerfDetach(L7_uint32 policyIndex, 
                              L7_uint32 intIfNum,
                              L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection);
void    dsmibPolicyPerfInTableShow(void);
void    dsmibPolicyPerfOutTableShow(void);

/* diffserv_service_api.c */
L7_RC_t dsmibServiceSetTest(L7_uint32 intIfNum, 
                            L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection,
                            L7_USMDB_MIB_DIFFSERV_SERVICE_SETTEST_OID_TYPE_t oid, 
                            void *pValue,
                            dsmibServiceEntryCtrl_t **ppRow,
                            size_t *pValueLen);
L7_RC_t dsmibServiceObjectSet(L7_uint32 intIfNum,
                              L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection,
                              L7_USMDB_MIB_DIFFSERV_SERVICE_SETTEST_OID_TYPE_t oid, 
                              void *pValue,
                              dsmibServiceEntryCtrl_t **ppRow,
                              size_t *pValueLen);
dsmibServiceEntryCtrl_t *dsmibServiceRowFind(L7_uint32 intIfNum,
                              L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection);
L7_RC_t dsmibServiceObjectGetCheck(dsmibServiceEntryCtrl_t *pRow, 
                                   L7_DIFFSERV_SERVICE_TABLE_OBJECT_t oid);
L7_RC_t dsmibServicePerfCtrUpdate(dsmibServiceEntryCtrl_t *pRow);
L7_RC_t dsmibServiceIfOperStatusUp(L7_uint32 intIfNum,
                            L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection);
L7_RC_t dsmibServiceIfOperStatusDown(L7_uint32 intIfNum,
                            L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection);
void    dsmibServiceTableShow(void);


#endif /* INCLUDE_DIFFSERV_PRVTMIB_H */
