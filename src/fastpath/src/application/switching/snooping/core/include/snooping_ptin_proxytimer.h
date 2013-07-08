/*
 * snooping_ptin_grouptimer.h
 *
 *  Created on: 22 de Abr de 2013
 *      Author: Daniel Figueira
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

L7_RC_t   snoop_ptin_proxytimer_init(void);
L7_RC_t   snoop_ptin_proxytimer_deinit(void);
L7_RC_t snoop_ptin_proxytimer_start(snoopPTinProxyTimer_t* pTimer, L7_uint32 timeout,L7_uint8 reportType, L7_BOOL isInterface,L7_uint32 noOfRecords, snoopPTinProxyGroup_t* groupData);
L7_RC_t snoop_ptin_proxytimer_stop(snoopPTinProxyTimer_t *pTimer);
L7_uint32 snoop_ptin_proxytimer_timeleft(snoopPTinProxyTimer_t *pTimer);
L7_BOOL snoop_ptin_proxytimer_isRunning(snoopPTinProxyTimer_t *pTimer);
