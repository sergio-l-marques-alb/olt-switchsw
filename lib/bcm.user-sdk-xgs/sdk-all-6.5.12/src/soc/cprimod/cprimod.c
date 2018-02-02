/*
 *         
 * $Id:$
 * 
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *         
 *     
 */

#include <shared/bsl.h>
#include <soc/error.h>
#include <soc/esw/port.h>

#include <soc/types.h>
#include <soc/error.h>
#include <soc/cprimod/cprimod.h>
#include <soc/cprimod/cprimod_dispatch.h>


        
#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

int cprimod_port_cpm_type_get(int unit, int port,  cprimod_dispatch_type_t* type){

    SOC_INIT_FUNC_DEFS;
#ifdef CPRIMOD_CPRI_FALCON_SUPPORT
    *type = cprimodDispatchTypeCprif; 
#else
    *type = cprimodDispatchTypeCount;
#endif
    SOC_FUNC_RETURN;
}

int cprimod_test_api_non_dispatch(int unit, int port, int* value)
{        
    SOC_INIT_FUNC_DEFS;
    
    if(value == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("value NULL parameter"));
    }
    LOG_CLI((" cpri_test_api_non_dispatch call u=%d port=%d .\n",unit,port));
        
exit:
    SOC_FUNC_RETURN; 
    
}




#undef _ERR_MSG_MODULE_NAME
