/*
 * snooping_ptin_interfacetimer.h
 *
 *  Created on: 30 of May 2013
 *      Author: Márcio Daniel Melo
 */

#include "ptin_xlate_api.h"
#include "ptin_intf.h"
#include "ptin_utils.h"
#include "ptin_evc.h"
#include "usmdb_snooping_api.h"
#include "snooping_api.h"
#include "avl_api.h"
#include "buff_api.h"
#include "l7apptimer_api.h"
#include "l7handle_api.h"
#include "snooping.h"

L7_RC_t   snoop_ptin_proxy_Grouptimer_init(void);
L7_RC_t   snoop_ptin_proxy_Grouptimer_deinit(void);
L7_RC_t   snoop_ptin_proxy_Grouptimer_start(snoopPTinProxyGrouptimer_t *pTimer, L7_uint32 timeout, L7_uchar8 mgmdsQRV,snoopPTinProxyGrouptimerInfoData_t* groupData);
L7_RC_t   snoop_ptin_proxy_Grouptimer_stop(snoopPTinProxyGrouptimer_t *pTimer);
L7_uint32 snoop_ptin_proxy_Grouptimer_timeleft(snoopPTinProxyGrouptimer_t *pTimer);
L7_BOOL snoop_ptin_proxy_Grouptimer_isRunning(snoopPTinProxyGrouptimer_t *pTimer);
