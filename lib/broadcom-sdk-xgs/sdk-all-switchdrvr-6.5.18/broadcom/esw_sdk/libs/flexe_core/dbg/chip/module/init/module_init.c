

/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include "reg_operate.h"
#include "hal.h"
#include <stdio.h>
#include "module_mux.h"
#include "module_demux.h"
#include "module_66bswitch.h"
#include "module_mcmac.h"
#include "module_sar.h"
#include "module_cpb.h"
#include "module_oam.h"
#include "module_rateadpt.h"
#include "sal.h"
#include "module_init.h"
#include "module_flexe_oh.h"
#include "module_1588.h"

/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/
unsigned long int init_para_debug = 0;

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
*
* FUNCTION
*
*     
*
* DESCRIPTION
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*      
* 
* RETURNS
* 
*    RET_SUCCESS: success
*    RET_FAIL: fail
*    RET_PARAERR:parameter error
******************************************************************************/
RET_STATUS module_init(UINT_8 chip_id)
{
    mux_init(chip_id);
    demux_init(chip_id);
    b66switch_init(chip_id);
    mcmac_init(chip_id);
    cpb_egress_init(chip_id);
    sar_rx_init(chip_id);
    sar_tx_init(chip_id);
    inf_ch_adp_rx_init(chip_id);
    inf_ch_adp_tx_init(chip_id);
    sar_oam_init(chip_id);
    flexe_oam_init(chip_id);
    flexe_macrx_init(chip_id);
    rateadp_init(chip_id);
    mac_rateadp_init(chip_id);
    client_env_init(chip_id);
    flexe_oh_init(chip_id);
    ieee1588_init(chip_id);

    return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     
*
* DESCRIPTION
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*      
* 
* RETURNS
* 
*    RET_SUCCESS: success
*    RET_FAIL: fail
*    RET_PARAERR:parameter error
******************************************************************************/
RET_STATUS module_uninit(UINT_8 chip_id)
{
    mux_uninit(chip_id);
    demux_uninit(chip_id);
    
    return RET_SUCCESS;
}

void print_software_ver()
{
    printf("ver:20190430\r\n");
}
#ifdef __cplusplus
}
#endif

