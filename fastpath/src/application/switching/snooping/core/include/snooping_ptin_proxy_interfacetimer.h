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

L7_RC_t   snoop_ptin_proxy_Interfacetimer_init(void);
L7_RC_t   snoop_ptin_proxy_Interfacetimer_deinit(void);
L7_RC_t snoop_ptin_proxy_Interfacetimer_start(snoopPTinProxyInterfacetimer_t* pTimer, L7_uint32 timeout, L7_uchar8 mgmdsQRV, snoopPTinProxyInterfacetimerInfoData_t* groupData);
L7_RC_t   snoop_ptin_proxy_Interfacetimer_stop(snoopPTinProxyInterfacetimer_t *pTimer);
L7_uint32 snoop_ptin_proxy_Interfacetimer_timeleft(snoopPTinProxyInterfacetimer_t *pTimer);
