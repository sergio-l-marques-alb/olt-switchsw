
#ifndef MODULE_FLEXE_OH_H
#define MODULE_FLEXE_OH_H

/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include "global_macro.h"
#include "datatype.h"


/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/
typedef enum
{
	flexe_oh_phy_mode_50g = 0,
	flexe_oh_phy_mode_100g = 1,
	flexe_oh_phy_mode_200g = 2,
	flexe_oh_phy_mode_400g = 3,
	flexe_oh_phy_mode_last
}flexe_oh_phy_mode_t;

#define INSTANCE_NUM (8)

#ifdef __cplusplus
extern "C"
{
#endif
	
extern RET_STATUS flexe_oh_init(UINT_8 chip_id);
extern RET_STATUS flexe_oh_port_mode_set(UINT_8 chip_id,UINT_8 instance_id,UINT_8 mode);
extern RET_STATUS flexe_oh_port_mf16t32_set(UINT_8 chip_id,UINT_8 instance_id,UINT_8 mf16t32);
extern RET_STATUS flexe_oh_instance_alm_get(UINT_8 chip_id,UINT_8 instance_id,UINT_16* status);
extern RET_STATUS flexe_oh_ccm_alm_get(UINT_8 chip_id,UINT_8 instance_id,UINT_32* slot,UINT_16* client);
extern RET_STATUS flexe_oh_exp_gid_set(UINT_8 chip_id,UINT_8 instance_id,UINT_32 exp_gid); 
extern RET_STATUS flexe_oh_tx_gid_set(UINT_8 chip_id,UINT_8 instance_id,UINT_32 tx_grp_id); 
extern RET_STATUS flexe_oh_rx_gid_get (UINT_8 chip_id,UINT_8 instance_id,UINT_32* group_id);   
extern RET_STATUS flexe_oh_exp_pid_set(UINT_8 chip_id,UINT_8 instance_id,UINT_8 phy_id); 
extern RET_STATUS flexe_oh_tx_pid_set(UINT_8 chip_id,UINT_8 instance_id,UINT_8 phy_id); 
extern RET_STATUS flexe_oh_rx_pid_get (UINT_8 chip_id,UINT_8 instance_id,UINT_32* phy_id);   
extern RET_STATUS flexe_oh_exp_pm_set(UINT_8 chip_id,UINT_8 instance_id,UINT_32* value); 
extern RET_STATUS flexe_oh_tx_pm_set(UINT_8 chip_id,UINT_8 instance_id,UINT_32* value); 
extern RET_STATUS flexe_oh_rx_pm_get (UINT_8 chip_id,UINT_8 instance_id,UINT_32* value);   
extern RET_STATUS flexe_oh_exp_ccab_set(UINT_8 chip_id,UINT_8 field_id,UINT_8 instance_id,UINT_32* value); 
extern RET_STATUS flexe_oh_tx_ccab_set(UINT_8 chip_id,UINT_8 field_id,UINT_8 instance_id,UINT_32* value); 
extern RET_STATUS flexe_oh_rx_ccab_get (UINT_8 chip_id,UINT_8 field_id,UINT_8 instance_id,UINT_32* value);   
extern RET_STATUS flexe_oh_res_set(UINT_8 chip_id,UINT_8 instance_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS flexe_oh_res_get(UINT_8 chip_id,UINT_8 instance_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS flexe_oh_res_src_set(UINT_8 chip_id,UINT_8 instance_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS flexe_oh_smc_src_set(UINT_8 chip_id,UINT_8 instance_id,UINT_32 parameter);
extern void module_flexe_oh_debug_set(unsigned long int parameter);


#ifdef __cplusplus
}
#endif
#endif

