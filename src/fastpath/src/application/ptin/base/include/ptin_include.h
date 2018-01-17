/*
 * ptin_include.h
 */

#ifndef PTIN_INCLUDE_H_
#define PTIN_INCLUDE_H_


/*
 ***********************************************************************
 *                           COMMON INCLUDES
 ***********************************************************************
 */

#include "l7_common.h"
#include "l7_resources.h"
#include "osapi.h"
#include "osapi_support.h"
#include "sysapi.h"
#include "nimapi.h"
#include "simapi.h"
#include "dtlapi.h"
#include "trace_api.h"
#include "sysnet_api.h"
#include "log.h"
#include "nvstoreapi.h"
#include "defaultconfig.h"
#include "default_cnfgr.h"
#include "datatypes.h"

/*
 **********************************************************************
 *                           STANDARD LIBRARIES
 **********************************************************************
 */

#ifdef __cplusplus
  #include <stl.h>                        /* Standard Template Library */
using namespace std;
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/*
 ********************************************************************
 *                           APIs
 ********************************************************************
 */

#include "ptin_globaldefs.h"
#include "ptin_structs.h"

//#include "ipc.h"
//#include "fw_shm.h"
//#include "sirerrors.h"
//#include "traces.h"
#include "logger.h"

//#include "ptin_xlate_api.h"
//#include "ptin_xconnect_api.h"
//#include "ptin_cfg.h"
//#include "ptin_cnfgr.h"
//#include "ptin_intf.h"
//#include "ptin_msghandler.h"


#endif /* PTIN_INCLUDE_H_ */
