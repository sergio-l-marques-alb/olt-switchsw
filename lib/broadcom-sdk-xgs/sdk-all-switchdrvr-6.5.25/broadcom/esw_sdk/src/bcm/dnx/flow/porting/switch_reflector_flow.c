/** \file switch_reflector_flow.c
 *  * Reflector APIs for flows APIs conversion.
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm_int/dnx/auto_generated/dnx_flow_dispatch.h>
#include <bcm/flow.h>
#include <bcm/switch.h>
#include <bcm_int/dnx/flow/flow.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <soc/dnx/swstate/auto_generated/access/reflector_access.h>
#include "src/bcm/dnx/flow/porting/switch_reflector_flow.h"
#include "src/bcm/dnx/flow/flow_def.h"
