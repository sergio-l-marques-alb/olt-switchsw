/*********************************************************************
* <pre>
* LL   VV  VV LL   7777777  (C) Copyright LVL7 Systems 2006-2007
* LL   VV  VV LL   7   77   All Rights Reserved.
* LL   VV  VV LL      77
* LL    VVVV  LL     77
* LLLLL  VV   LLLLL 77      Code classified LVL7 Confidential
* </pre>
**********************************************************************
*
* @filename link_dependency_api.h
*
* @purpose application data structures
*
* @component link dependency
*
* @comments none
*
* @create 12/06/2006
*
* @author dflint
* @end
*
**********************************************************************/

#ifndef LINK_DEPENDENCY_DATA_H
#define LINK_DEPENDENCY_DATA_H

#include "datatypes.h"
#include "commdefs.h"
#include "comm_mask.h"
#include "link_dependency_exports.h"

typedef enum
{
  L7_LINK_DEPENDENCY_SEMAPHORE_WAIT = 1000,
  L7_LINK_DEPENDENCY_LAST_PRIVATE_CONSTANT
} link_dependency_private_constants_t;

typedef struct
{
  L7_BOOL        inUse;
  L7_BOOL        allLinksDown;
  L7_INTF_MASK_t members;
  L7_INTF_MASK_t dependencies;
  link_dependency_action_constants_t        link_action;
} link_dependency_group_t;

typedef struct
{
  L7_mask_values_t member    [L7_MASK_LEN(L7_LINK_DEPENDENCY_MAX_GROUPS)];
  L7_mask_values_t dependency[L7_MASK_LEN(L7_LINK_DEPENDENCY_MAX_GROUPS)];
} link_dependency_membership_t;

typedef struct
{
  L7_BOOL                      configChanged;
  L7_INTF_MASK_t               intfStatus;
  link_dependency_group_t      groups[L7_LINK_DEPENDENCY_MAX_GROUPS];
  link_dependency_membership_t membership[L7_MAX_INTERFACE_COUNT];
  L7_BOOL                      warmRestart;
} link_dependency_global_data_t;

/* Entire LinkDependencyData configuration */
typedef struct
{
  L7_fileHdr_t                 hdr;
  L7_BOOL                      configChanged;
  link_dependency_group_t      groups[L7_LINK_DEPENDENCY_MAX_GROUPS];
  L7_uint32                    checkSum;
} link_dependency_cfg_data_t;

extern link_dependency_global_data_t LinkDependencyData_g;
extern link_dependency_cfg_data_t LinkDependencyData_cfg;
extern void                         *LinkDependencySemaphore;

#define LINK_DEPENDENCY_FILENAME "link_dependency.cfg"
#define LINK_DEPENDENCY_CFG_VER_CURRENT 1

#endif  /* LINK_DEPENDENCY_DATA_H */
