/*
 * ptin_igmp_querytimer.h
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

L7_RC_t snoop_ptin_querytimer_init(void);
L7_RC_t snoop_ptin_querytimer_deinit(void);
L7_RC_t snoop_ptin_querytimer_start(snoopPTinL3Querytimer_t *pTimer, L7_uint16 timeout, snoopPTinL3InfoData_t* groupData, L7_uint32 interfaceIdx);
L7_RC_t snoop_ptin_querytimer_stop(snoopPTinL3Querytimer_t *pTimer);
