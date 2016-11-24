/*
 * ptinHapi_include.h
 *
 *  Created on: 2010/04/08
 *      Author: Andre Brizido
 */

#ifndef PTINHAPI_INCLUDE_H_
#define PTINHAPI_INCLUDE_H_


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



#include "ptin/globaldefs.h"
#include "sirerrors.h"
#include "traces.h"
#include "ptinHapi_Structs.h"

#include "ptinHapi_cfg.h"
#include "ptinHapi_cnfgr.h"

#include "ptinHapi_control.h"
#include "ptinHapi_debug.h"


#endif /* PTINHAPI_INCLUDE_H_ */
