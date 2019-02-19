/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_filter_xgs2.h
*
* @component hapi
*
* @create    4/18/2006
*
* @author    sdoke
*
* @end
*
**********************************************************************/

#ifndef BROAD_FILTER_XGS2_H
#define BROAD_FILTER_XGS2_H

#include "broad_policy_common.h"

typedef int BROAD_ENTRY_t;

#define POLICY_TABLE_NONE     0
#define POLICY_TABLE_USED     1

typedef struct
{
    BROAD_ENTRY_t     entry;
    int               meter_flags;  
    int               ffpmtr_id;     /* ffp requires meter id */
    bcm_port_t        mtr_port;      /* ffp requires port */
    int               cntr_flags;  
    int               prio;
    int               ffpcntr_id;    /* ffp requires counter id */
    bcm_port_t        cntr_port;     /* ffp requires port */
    L7_BOOL           mirror_rule;  /* True if action is to mirror*/
}BROAD_FFP_RULE_INFO_t;

int policy_filter_add_rule(int                        unit, 
                           BROAD_POLICY_RULE_ENTRY_t *ruleInfo, 
                           BROAD_ENTRY_t             *entry,
                           int                        prio);                           

int policy_filter_delete(int           unit, 
                         BROAD_ENTRY_t entry);


int policy_filter_delete_rule(int                        unit,
                              BROAD_FFP_RULE_INFO_t      *ruleInfo);

int policy_filter_set_ipbm(int           unit, 
                           BROAD_ENTRY_t entry, 
                           bcm_pbmp_t    ipbm, 
                           bcm_pbmp_t    ipbmMask);


int policy_filter_get_stats(int                        unit,
                            BROAD_FFP_RULE_INFO_t     *ruleInfo,
                            uint64                    *val1,
                            uint64                    *val2);

#endif /* BROAD_FILTER_XGS2_H */

