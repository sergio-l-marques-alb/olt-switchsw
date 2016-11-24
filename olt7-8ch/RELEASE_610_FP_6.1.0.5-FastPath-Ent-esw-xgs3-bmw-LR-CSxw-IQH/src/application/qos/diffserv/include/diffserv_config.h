/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   diffserv_config.h
*
* @purpose    DiffServ component structures and config data
*
* @component  DiffServ
*
* @comments   none
*
* @create     02/18/2002
*
* @author     gpaussa
* @end
*
**********************************************************************/
#ifndef INCLUDE_DIFFSERV_CONFIG_H
#define INCLUDE_DIFFSERV_CONFIG_H

#include "l7_common.h"
#include "diffserv_cfg_limit.h"
#include "diffserv_prvtmib.h"

/* DiffServ Configuration Data Structure Array Bounds */
#define L7_DIFFSERV_CLASS_HDR_START     1       /* skip entry 0 */
#define L7_DIFFSERV_CLASS_HDR_MAX       (L7_DIFFSERV_CLASS_HDR_START + L7_DIFFSERV_CLASS_LIM)
#define L7_DIFFSERV_CLASS_RULE_START    1       /* skip entry 0 */
#define L7_DIFFSERV_CLASS_RULE_MAX      (L7_DIFFSERV_CLASS_RULE_START + L7_DIFFSERV_CLASS_RULE_LIM)

#define L7_DIFFSERV_POLICY_HDR_START    1       /* skip entry 0 */
#define L7_DIFFSERV_POLICY_HDR_MAX      (L7_DIFFSERV_POLICY_HDR_START + L7_DIFFSERV_POLICY_LIM)
#define L7_DIFFSERV_POLICY_INST_START   1       /* skip entry 0 */
#define L7_DIFFSERV_POLICY_INST_MAX     (L7_DIFFSERV_POLICY_INST_START + L7_DIFFSERV_POLICY_INST_LIM)
#define L7_DIFFSERV_POLICY_ATTR_START   1       /* skip entry 0 */
#define L7_DIFFSERV_POLICY_ATTR_MAX     (L7_DIFFSERV_POLICY_ATTR_START + L7_DIFFSERV_POLICY_ATTR_LIM)

#define L7_DIFFSERV_CFG_FILENAME  "diffserv.cfg"
#define L7_DIFFSERV_CFG_VER_1       0x1
#define L7_DIFFSERV_CFG_VER_2       0x2
#define L7_DIFFSERV_CFG_VER_3       0x3
#define L7_DIFFSERV_CFG_VER_4       0x4
#define L7_DIFFSERV_CFG_VER_5       0x5
#define L7_DIFFSERV_CFG_VER_CURRENT L7_DIFFSERV_CFG_VER_5

/****************************************
 *
 * Class Configuration Data
 *
 ****************************************
 */

typedef struct
{
  L7_BOOL                 inUse;                /* array entry in use (T/F)   */
  L7_uint32               chainIndex;           /* first class rule entry     */
  dsmibClassMib_t         mib;                  /* MIB content                */
  dsmibClassFlags_t       rowInvalidFlags;      /* row invalid object flags   */

} L7_diffServClassHdr_t;

typedef struct
{
  L7_uint32               hdrIndexRef;          /* back ptr (non-0 = in use)  */
  L7_uint32               chainIndex;           /* next sequential rule entry */
  dsmibClassRuleMib_t     mib;                  /* MIB content                */
  dsmibClassRuleFlags_t   rowInvalidFlags;      /* row invalid object flags   */
  L7_uint32               arid;                 /* ACL rule identifier        */

} L7_diffServClassRule_t;


/****************************************
 *
 * Policy Configuration Data
 *
 ****************************************
 */

typedef struct
{
  L7_BOOL                 inUse;                /* entry in use flag (T/F)    */
  L7_uint32               chainIndex;           /* first policy instance entry*/
  dsmibPolicyMib_t        mib;                  /* MIB content                */
  dsmibPolicyFlags_t      rowInvalidFlags;      /* row invalid object flags   */

} L7_diffServPolicyHdr_t;

typedef struct
{
  L7_uint32               hdrIndexRef;          /* back ptr (non-0 = in use)  */
  L7_uint32               attrChainIndex;       /* first attr entry           */
  L7_uint32               chainIndex;           /* next seq instance entry    */
  dsmibPolicyInstMib_t    mib;                  /* MIB content                */
  dsmibPolicyInstFlags_t  rowInvalidFlags;      /* row invalid object flags   */

} L7_diffServPolicyInst_t;

typedef struct
{
  L7_uint32               instIndexRef;         /* back ptr (non-0 = in use)  */
  L7_uint32               chainIndex;           /* next seq attr entry        */
  dsmibPolicyAttrMib_t    mib;                  /* MIB content                */
  dsmibPolicyAttrFlags_t  rowInvalidFlags;      /* row invalid object flags   */

} L7_diffServPolicyAttr_t;


/****************************************
 *
 * Service Configuration Data
 *
 ****************************************
 */

typedef struct
{
  L7_BOOL                 inUse;                /* entry in use flag (T/F)    */
  dsmibServiceMib_t       mib;                  /* MIB content                */
  dsmibServiceFlags_t     rowInvalidFlags;      /* row invalid object flags   */

} L7_diffServService_t;

/*********************************************
 *
 * DiffServ User Interface Configuration File                
 *
 *********************************************
 */

typedef struct 
{
  nimConfigID_t           configId;             /* the config ID must the first */

  /* Service configuration */
  L7_diffServService_t    serviceIn;
  L7_diffServService_t    serviceOut;

} L7_diffServIntfCfg_t;

/****************************************
 *
 * DiffServ User Configuration File                
 *
 ****************************************
 */

typedef struct
{
  L7_fileHdr_t            cfgHdr;

  /* DiffServ globals */
  L7_uint32               adminMode;            /* administrative mode        */
  L7_uint32               traceMode;            /* component trace mode       */
  L7_uint32               msgLvl;               /* debug message level        */

  /* Class configuration */
  L7_diffServClassHdr_t   classHdr[L7_DIFFSERV_CLASS_HDR_MAX];
  L7_diffServClassRule_t  classRule[L7_DIFFSERV_CLASS_RULE_MAX];

  /* Policy configuration */
  L7_diffServPolicyHdr_t  policyHdr[L7_DIFFSERV_POLICY_HDR_MAX];
  L7_diffServPolicyInst_t policyInst[L7_DIFFSERV_POLICY_INST_MAX];
  L7_diffServPolicyAttr_t policyAttr[L7_DIFFSERV_POLICY_ATTR_MAX];
  L7_diffServIntfCfg_t    diffServIntfCfg[L7_DIFFSERV_INTF_MAX_COUNT];
  L7_uint32               checkSum;

} L7_diffServCfg_t;

extern L7_diffServCfg_t *pDiffServCfgData_g;

#define L7_DIFFSERV_CFG_DATA_SIZE       sizeof(L7_diffServCfg_t)


/* external reference for configuration mapping table */
extern L7_uint32 *diffServMapTbl_g;


/*****************************************
 *
 *  DiffServ deregistered functions
 *
 *  This struct will be deprecated when
 *  component deregistration is supported
 *
 *****************************************
 */

typedef struct 
{
  L7_BOOL       diffServSave;
  L7_BOOL       diffServRestore;
  L7_BOOL       diffServHasDataChanged;
  L7_BOOL       diffServIntfChangeCallback;
} diffServDeregister_t;

extern diffServDeregister_t   diffServDeregister_g;


/* external references to debug display strings for config data fields */
extern char *dsmibTruthValueStr[];
extern char *dsmibStgTypeStr[]; 
extern char *dsmibRowPendingStr[];
extern char *dsmibRowStatusStr[];
extern char *dsmibClassTypeStr[];
extern char *dsmibClassL3ProtoStr[];
extern char *dsmibClassAclTypeStr[];
extern char *dsmibClassRuleTypeStr[];
extern char *dsmibEtypeKeyidStr[];
extern char *dsmibPolicyTypeStr[];
extern char *dsmibPolicyAttrTypeStr[];
extern char *dsmibBwUnitsStr[];
extern char *dsmibPoliceActStr[];
extern char *dsmibPoliceColorStr[];
extern char *dsmibNimIntfTypeStr[];
extern char *dsmibServiceIfDir[];
extern char *dsmibServiceIfOperStatus[];


#endif /* INCLUDE_DIFFSERV_CONFIG_H */
