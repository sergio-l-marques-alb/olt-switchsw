/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   diffserv_cfg_limit.h
*
* @purpose    DiffServ configuration limit definitions
*
* @component  DiffServ
*
* @comments   Contains default table sizes, etc. that can be overridden
* @comments   by platform.h values
*
* @create     08/04/2002
*
* @author     gpaussa
* @end
*
**********************************************************************/
#ifndef INCLUDE_DIFFSERV_CFG_LIMIT_H
#define INCLUDE_DIFFSERV_CFG_LIMIT_H

#include "l7_common.h"
#include "l7_product.h"


/* DiffServ Private MIB Configuration Limits
 *
 *
 * NOTE:  Limits defined here are used unless overridden by platform.h values.
 *
 *        These defaults are intended for platforms that do not support
 *        DiffServ, resulting in a smaller memory footprint.  Any platform
 *        that does support DiffServ should override the appropriate definition
 *        values in their platform.h file.
 */

#ifndef L7_DIFFSERV_RULE_PER_CLASS_LIM
  #define L7_DIFFSERV_RULE_PER_CLASS_LIM    1
#endif

#ifndef L7_DIFFSERV_NESTED_RULE_PER_CLASS_LIM
  #define L7_DIFFSERV_NESTED_RULE_PER_CLASS_LIM   (L7_DIFFSERV_RULE_PER_CLASS_LIM * 2)
#endif

#ifndef L7_DIFFSERV_INST_PER_POLICY_LIM
  #define L7_DIFFSERV_INST_PER_POLICY_LIM   1
#endif

#ifndef L7_DIFFSERV_ATTR_PER_INST_LIM
  #define L7_DIFFSERV_ATTR_PER_INST_LIM     1
#endif

#ifndef L7_DIFFSERV_SERVICE_INTF_LIM
  #define L7_DIFFSERV_SERVICE_INTF_LIM      2   /* any combo of in vs. out */
#endif

#ifndef L7_DIFFSERV_CLASS_LIM
  #define L7_DIFFSERV_CLASS_LIM             2
#endif

#ifndef L7_DIFFSERV_CLASS_RULE_LIM
  #define L7_DIFFSERV_CLASS_RULE_LIM        (L7_DIFFSERV_CLASS_LIM * \
                                             L7_DIFFSERV_RULE_PER_CLASS_LIM)
#endif

#ifndef L7_DIFFSERV_POLICY_LIM
  #define L7_DIFFSERV_POLICY_LIM            L7_DIFFSERV_SERVICE_INTF_LIM
#endif

#ifndef L7_DIFFSERV_POLICY_INST_LIM
  #define L7_DIFFSERV_POLICY_INST_LIM       (L7_DIFFSERV_POLICY_LIM * \
                                             L7_DIFFSERV_INST_PER_POLICY_LIM)
#endif

#ifndef L7_DIFFSERV_POLICY_ATTR_LIM
  #define L7_DIFFSERV_POLICY_ATTR_LIM       (L7_DIFFSERV_POLICY_INST_LIM * \
                                             L7_DIFFSERV_ATTR_PER_INST_LIM)
#endif


/* DiffServ Standard MIB Table Size Limits
 *
 *
 * NOTE:  The Standard MIB limits defined here are NOT intended to be
 *        overridden by platform.h.  They are derived from the appropriate 
 *        private MIB config limits and are located in this file for easy
 *        access to those values.
 */

#define L7_DIFFSERV_DATAPATH_LIM            L7_DIFFSERV_SERVICE_INTF_LIM
 
#define L7_DIFFSERV_CLFR_LIM                L7_DIFFSERV_DATAPATH_LIM

/* A Clfr path is defined as a distinct subpath in the overall datapath.
 * This translates to the number of instances per policy supported in the
 * private MIB, and is the basis for most of the data path element limits.
 */
#define L7_DIFFSERV_CLFR_PATH_LIM           (L7_DIFFSERV_CLFR_LIM * \
                                             L7_DIFFSERV_INST_PER_POLICY_LIM)
 
#define L7_DIFFSERV_CLFR_ELEMENT_LIM        (L7_DIFFSERV_CLFR_PATH_LIM * \
                                             L7_DIFFSERV_NESTED_RULE_PER_CLASS_LIM)
  
#define L7_DIFFSERV_MULTI_FIELD_CLFR_LIM    1 /* not used (Aux MF Clfr used instead) */
  
/* NOTE:  Aux MF Clfr entries are managed by the code to share common
 *        entries among the individual Clfr Elements (helps reduce overall
 *        number of Aux MF Clfr Table entries needed, which is a significant
 *        memory savings).
 */
#define L7_DIFFSERV_AUX_MF_CLFR_LIM         (L7_DIFFSERV_CLASS_LIM * \
                                             L7_DIFFSERV_RULE_PER_CLASS_LIM)

#define L7_DIFFSERV_METER_LIM               (L7_DIFFSERV_CLFR_PATH_LIM*2)

#define L7_DIFFSERV_TBPARAM_LIM             L7_DIFFSERV_METER_LIM
  
#define L7_DIFFSERV_COS_MARK_ACT_LIM        8

#define L7_DIFFSERV_COS2_MARK_ACT_LIM       8

#define L7_DIFFSERV_DSCP_MARK_ACT_LIM       64

#define L7_DIFFSERV_IPPREC_MARK_ACT_LIM     8

#define L7_DIFFSERV_COUNT_ACT_LIM           (L7_DIFFSERV_CLFR_PATH_LIM*2)

#define L7_DIFFSERV_ASSIGN_QUEUE_LIM        L7_DIFFSERV_CLFR_PATH_LIM

#define L7_DIFFSERV_REDIRECT_LIM            L7_DIFFSERV_CLFR_PATH_LIM

#define L7_DIFFSERV_MIRROR_LIM              L7_DIFFSERV_CLFR_PATH_LIM

#define L7_DIFFSERV_ALG_DROP_LIM            L7_DIFFSERV_CLFR_PATH_LIM

#define L7_DIFFSERV_ACTION_LIM              (L7_DIFFSERV_COS_MARK_ACT_LIM + \
                                             L7_DIFFSERV_COS2_MARK_ACT_LIM + \
                                             L7_DIFFSERV_DSCP_MARK_ACT_LIM + \
                                             L7_DIFFSERV_IPPREC_MARK_ACT_LIM + \
                                             L7_DIFFSERV_COUNT_ACT_LIM + \
                                             L7_DIFFSERV_ASSIGN_QUEUE_LIM + \
                                             L7_DIFFSERV_REDIRECT_LIM + \
                                             L7_DIFFSERV_MIRROR_LIM + \
                                             L7_DIFFSERV_ALG_DROP_LIM)
                                             
#define L7_DIFFSERV_RANDOM_DROP_LIM         L7_DIFFSERV_ALG_DROP_LIM
  
#define L7_DIFFSERV_SCHEDULER_LIM           (L7_DIFFSERV_CLFR_LIM*2)
  
#define L7_DIFFSERV_MAXRATE_LIM             L7_DIFFSERV_CLFR_PATH_LIM


#endif /* INCLUDE_DIFFSERV_CFG_LIMIT_H */
