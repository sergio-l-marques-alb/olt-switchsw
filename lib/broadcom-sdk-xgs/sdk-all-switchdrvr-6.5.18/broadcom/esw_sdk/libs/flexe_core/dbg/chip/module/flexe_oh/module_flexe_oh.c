

/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "sal.h"
#include "reg_operate.h"
#include "oh_rx.h"
#include "oh_tx.h"
#include "flexe_multi_deskew.h"
#include "oam_rx.h"
#include "oam_tx.h"
#include "flexe_oam_rx.h"
#include "flexe_oam_tx.h"
#include "ohoam_ef.h"
#include "module_sar.h"
#include "module_flexe_oh.h"

/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/
unsigned long int module_flexe_oh_debug = 0;
/*unsigned int g_flexe_oh_alm_enable[DEV_NUM][INSTANCE_NUM][flexe_oh_alm_last] = {0};*/

/*extern traffic_group_info_t g_group_info[DEV_NUM][PHY_NUM];*/
/*extern busa_info_t busa_rx_info[DEV_NUM][8];*/
/*extern busa_info_t busa_tx_info[DEV_NUM][8];*/

/******************************************************************************
*
* FUNCTION flexe_oh_init
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
*	  chip_id:chip number used
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    jxma    	2018-1-2    1.0           initial
*
******************************************************************************/
RET_STATUS flexe_oh_init(UINT_8 chip_id)
{
	UINT_8 i = 0;

	/*rdi insert cfg init*/
	for(i = 0;i < PHY_NUM; i++)
	{
		flexe_multi_deskew_grp_cfg_set(chip_id, i, 0);
		
		oh_tx_mode_cfg_set(chip_id,i,PHY_INST_TX_RI_SW_CFG,i);

		oh_tx_oh_ins_src_cfg_set(chip_id,i,SECTION_SRC_CFG,0x1);  
		oh_tx_oh_ins_src_cfg_set(chip_id,i,SHIM2SHIM_SRC_CFG,0x1);  
		oh_tx_oh_ins_src_cfg_set(chip_id,i,SMC_SRC_CFG,0x1);  

		oh_tx_oh_ins_src_cfg_set(chip_id,i,GID_SRC_CFG,0);
		oh_tx_oh_ins_cfg_set(chip_id,i,GID_INS,0x1);
		oh_rx_exgid_expid_set(chip_id,i,EXGID,0x1);
		flexe_multi_deskew_ssf_cfg_set(chip_id,i,0);
	}

	/*add by taos 1225 init 200ppm*/
	flexe_multi_deskew_rate_set(chip_id,0,804591503);
	flexe_multi_deskew_rate_set(chip_id,1,825337743);
	flexe_multi_deskew_rate_set(chip_id,2,1609183006);
	flexe_multi_deskew_rate_set(chip_id,3,1629929246);

	/*init 1588 sc insert*/
	for(i = 0;i < PHY_NUM;i++)
	{
		oh_tx_oh_ins_src_cfg_set(chip_id,i,SMC_SRC_CFG,2);
		oh_tx_res_ins_cfg_set(chip_id,i,SC_INS,1);
	}
	
	return RET_SUCCESS;
}
#if 0
/******************************************************************************
*
* FUNCTION flexe_oh_instance_num_of_port_get
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
*	  chip_id:chip number used
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    jxma    	2018-1-2    1.0           initial
*
******************************************************************************/
RET_STATUS flexe_oh_instance_num_of_port_get(UINT_8 chip_id,UINT_8 instance_id,UINT_8* instance_num)
{
	UINT_8 num = 0;
	UINT_8 i = 0;
	if(busa_rx_info[chip_id][port_id].rate != busa_tx_info[chip_id][port_id].rate)
	{
		printf("[%s]ERROR!rate not equaled!",__FUNCTION__);
		return RET_FAIL;
	}

	switch(busa_rx_info[chip_id][port_id].rate)
	{
		case flexe_oh_phy_mode_50g:
		case flexe_oh_phy_mode_100g:
			num = 1;
			break;
		case flexe_oh_phy_mode_200g:
			num = 2;
			break;
		case flexe_oh_phy_mode_400g:
			num = 4;
			break;
		default:
			printf("[%s]ERROR!rate out of range!",__FUNCTION__);
			return RET_FAIL;
	}

	for(i = 0; i < num; i++)
	{
		if((0xf == busa_rx_info[chip_id][port_id].instance[i]) ||
			(0xf == busa_tx_info[chip_id][port_id].instance[i]))
		{
			printf("[%s]ERROR!instance not configed!",__FUNCTION__);
			return RET_FAIL;
		}
		else if(busa_rx_info[chip_id][port_id].instance[i] != busa_tx_info[chip_id][port_id].instance[i])
		{
			printf("[%s]ERROR!rx tx instance not equaled!",__FUNCTION__);
			return RET_FAIL;
		}
	}

	*instance_num = num;
	return RET_SUCCESS;
}
#endif
/******************************************************************************
*
* FUNCTION flexe_oh_phy_mode_set
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
*	  chip_id:chip number used
*     instance_id:
*	  mode:0:flexe_oh_phy_mode_50g 1:flexe_oh_phy_mode_100g 2:flexe_oh_phy_mode_200g 3:flexe_oh_phy_mode_400g
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    jxma    	2018-1-2    1.0           initial
*
******************************************************************************/
RET_STATUS flexe_oh_port_mode_set(UINT_8 chip_id,UINT_8 instance_id,UINT_8 mode)
{
	if(chip_id > MAX_DEV || instance_id > (PHY_NUM-1))
	{
		if (module_flexe_oh_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,instance_id=%d\r\n",__FUNCTION__,chip_id,instance_id);
		}

		return RET_PARAERR;
	}
	
	oh_tx_mode_cfg_set(chip_id,instance_id,FLEXE_TX_MODE,mode);
	oh_rx_flexe_rx_mode_set(chip_id,instance_id,FLEXE_RX_MODE,mode);	

	/*flexe_oh_phy_mode_100g/flexe_oh_phy_mode_200g/flexe_oh_phy_mode_400g mf16t32 set 1(default 32)*/
	if(flexe_oh_phy_mode_50g != mode)
	{
		oh_tx_mode_cfg_set(chip_id,instance_id,MF16T32_TX_CFG,1);
		oh_rx_flexe_rx_mode_set(chip_id,instance_id,MF16T32_RX_CFG,1);
		
		ohoam_ef_soh_inst_mode_set(chip_id,instance_id,1);
	}
	else
	{
		oh_tx_mode_cfg_set(chip_id,instance_id,MF16T32_TX_CFG,0);
		oh_rx_flexe_rx_mode_set(chip_id,instance_id,MF16T32_RX_CFG,0);
		
		ohoam_ef_soh_inst_mode_set(chip_id,instance_id,0);
	}

	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION flexe_oh_phy_mf16t32_set
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
*	  chip_id:chip number used
*     instance_id:
*	  mf16t32:0:16 1:32
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    jxma    	2018-1-2    1.0           initial
*
******************************************************************************/
RET_STATUS flexe_oh_port_mf16t32_set(UINT_8 chip_id,UINT_8 instance_id,UINT_8 mf16t32)
{	
	UINT_32 tx_mode = 0;
	UINT_32 rx_mode = 0;
	
	if(chip_id > MAX_DEV || instance_id > (PHY_NUM-1))
	{
		if (module_flexe_oh_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,instance_id=%d\r\n",__FUNCTION__,chip_id,instance_id);
		}

		return RET_PARAERR;
	}
	
	oh_tx_mode_cfg_get(chip_id,instance_id,FLEXE_TX_MODE,&tx_mode);
	oh_rx_flexe_rx_mode_get(chip_id,instance_id,FLEXE_RX_MODE,&rx_mode);

	if(flexe_oh_phy_mode_50g == tx_mode && flexe_oh_phy_mode_50g == rx_mode)
	{
		oh_tx_mode_cfg_set(chip_id,instance_id,MF16T32_TX_CFG,mf16t32);
		oh_rx_flexe_rx_mode_set(chip_id,instance_id,MF16T32_RX_CFG,mf16t32);
		
		ohoam_ef_soh_inst_mode_set(chip_id,instance_id,mf16t32);
	}
	else
	{
		printf("[%s]ERROR! tx_mode=%d,rx_mode=%d NOT in 50G mode.",__FUNCTION__,tx_mode,rx_mode);
		return RET_FAIL;
	}			
 
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*     flexe_oh_instance_alm_get
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
*	  instance_id:
*	  *status:bit0:GIDM_ALM;bit1:phy_num mismatch;bit2:LOF;bit3:LOM;bit4:RPF;bit5:flexe_ob_err;bit6:flexe_cb_err;
*				 bit7:PMM;bit8:CCAM;bit9:CCBM;bit10:flexe_loga
* 
* RETURNS
* 
*    RET_SUCCESS: success
*    RET_FAIL: fail
*    RET_PARAERR:parameter error
******************************************************************************/
RET_STATUS flexe_oh_instance_alm_get(UINT_8 chip_id,UINT_8 instance_id,UINT_16* status)
{
	UINT_32 alm_status = 0;
	UINT_32 reg_data = 0;
	UINT_8 lof_flag = 0;
	UINT_8 lom_flag = 0;
	UINT_32 ccmab_sel = 0;
	
	if(chip_id > MAX_DEV || instance_id > (INSTANCE_NUM - 1))
	{
		if (module_flexe_oh_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,instance_id=%d\r\n",__FUNCTION__,chip_id,instance_id);
		}

		return RET_PARAERR;
	}
	
	if (NULL == status)
	{
		if (module_flexe_oh_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	*status = 0;

	/*get oh_rx_alm*/
	regp_read(chip_id,FLEXE_OH_RX_BASE_ADDR,0x20+instance_id*FLEXE_OFFSET,&reg_data);

	/*lof alm restrain gidm/pidm*/
	lof_flag = (reg_data >> SHIFT_BIT_1) & MASK_1_BIT; /*lof bit1*/
	lom_flag |= (reg_data >> SHIFT_BIT_3) & MASK_1_BIT; /*lom bit3*/
	

	/*get gidm alm*/
	alm_status = (reg_data >> SHIFT_BIT_5) & MASK_1_BIT;/*gidm bit5*/
	*status |= (alm_status & MASK_1_BIT);
	
	/*get pnm alm*/
	alm_status = (reg_data >> SHIFT_BIT_6) & MASK_1_BIT;/*pidm bit6*/
	*status |= ((alm_status & MASK_1_BIT)<<SHIFT_BIT_1); 

	/*get lof alm*/
	alm_status = lof_flag;	
	*status |= ((alm_status & MASK_1_BIT)<<SHIFT_BIT_2); 

	
	/*get lom alm*/
	alm_status = (reg_data >> SHIFT_BIT_3) & MASK_1_BIT; /*lom bit3*/
	*status |= ((alm_status & MASK_1_BIT)<<SHIFT_BIT_3); 

	/*get rpf alm*/
	alm_status = (reg_data >> SHIFT_BIT_8) & MASK_1_BIT; /*rpg bit8*/
	*status |= ((alm_status & MASK_1_BIT)<<SHIFT_BIT_4); 

	/*get flexe_ob_err alm*/
	alm_status = (reg_data >> SHIFT_BIT_12) & MASK_1_BIT; /*flexe_ob_err bit12*/
	*status |= ((alm_status & MASK_1_BIT)<<SHIFT_BIT_5); 

	/*get flexe_cb_err alm*/
	alm_status = (reg_data >> SHIFT_BIT_13) & MASK_1_BIT; /*flexe_cb_err bit13*/
	*status |= ((alm_status & MASK_1_BIT)<<SHIFT_BIT_6); 

	/*get pmm alm*/
	alm_status = (reg_data >> SHIFT_BIT_7) & MASK_1_BIT; /*pmm bit7 		*/
	*status |= ((alm_status & MASK_1_BIT)<<SHIFT_BIT_7);

	/*get ccam/ccbm alm*/
	oh_rx_cc_rx_value_get(chip_id,instance_id,ACCCC,&ccmab_sel);
	oh_rx_ccm_rx_alm_get(chip_id,instance_id,&reg_data);
	
	if(0 == ccmab_sel)
	{
		if (reg_data)
		{
			*status |= (1<<SHIFT_BIT_8);
		}			
	}
	else if(1 == ccmab_sel)
	{
		if (reg_data)
		{
			*status |= (1<<SHIFT_BIT_9);
		}			
	}
	else
	{

	}

	/*get flexe_loga*/
	flexe_multi_deskew_dsw_alm_get(chip_id,GRP_DSW_ALM,&reg_data);
	if(reg_data)
	{
		*status |= (1<<SHIFT_BIT_10);
	}


	/*lof alm restrain gidm/pidm cca ccb rpf*/
	if(1 == lof_flag)
	{
		*status &= ~((MASK_1_BIT)<<SHIFT_BIT_0);
		*status &= ~((MASK_1_BIT)<<SHIFT_BIT_1);
		*status &= ~((MASK_1_BIT)<<SHIFT_BIT_4);
		*status &= ~((MASK_1_BIT)<<SHIFT_BIT_8);
		*status &= ~((MASK_1_BIT)<<SHIFT_BIT_9);
	}

	/*lom alm restrain cca ccb pmm*/
	if(1 == lom_flag)
	{
		*status &= ~((MASK_1_BIT)<<SHIFT_BIT_7);
		*status &= ~((MASK_1_BIT)<<SHIFT_BIT_8);
		*status &= ~((MASK_1_BIT)<<SHIFT_BIT_9);
	}

	if (module_flexe_oh_debug)
	{
		printf("[%s] chip_id=%d,instance_id=%d,status = 0x%x\r\n",__FUNCTION__,chip_id,instance_id,*status);
	}

	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*     flexe_oh_ccm_alm_get
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
*	  instance_id:
*	  *slot:20bit---slot
*	  *client:20 array client_num
* 
* RETURNS
* 
*    RET_SUCCESS: success
*    RET_FAIL: fail
*    RET_PARAERR:parameter error
******************************************************************************/
RET_STATUS flexe_oh_ccm_alm_get(UINT_8 chip_id,UINT_8 instance_id,UINT_32* slot,UINT_16* client)
{
    UINT_32 ccmab_sel = 0;
	UINT_32 regp_data = 0;
	UINT_8 i = 0;
	UINT_32 client_num = 0;

	if(chip_id > MAX_DEV || instance_id > (INSTANCE_NUM - 1))
	{
		if (module_flexe_oh_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,instance_id=%d\r\n",__FUNCTION__,chip_id,instance_id);
		}

		return RET_PARAERR;
	}
	
	if ((NULL == slot) || (NULL == client))
	{
		if (module_flexe_oh_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	oh_rx_cc_rx_value_get(chip_id,instance_id,ACCCC,&ccmab_sel);
	oh_rx_ccm_rx_alm_get(chip_id,instance_id,&regp_data);
    
	for(i = 0; i < 20 ; i++)
	{	
		if(((regp_data >> i) & MASK_1_BIT) == 1)
		{
			oh_rx_ccab_rx_exp_get(chip_id,instance_id,ccmab_sel,i,&client_num);
            client[i] = (UINT_16)(client_num & 0xffff);
		}			
	}

    *slot = regp_data;

	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*     flexe_oh_exp_gid_set
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
*	  instance_id:
*	  exp_gid:
* 
* RETURNS
* 
*    RET_SUCCESS: success
*    RET_FAIL: fail
*    RET_PARAERR:parameter error
******************************************************************************/
RET_STATUS flexe_oh_exp_gid_set(UINT_8 chip_id,UINT_8 instance_id,UINT_32 exp_gid) 
{	
	if(chip_id > MAX_DEV || instance_id > (INSTANCE_NUM - 1))
	{
		if (module_flexe_oh_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,instance_id=%d\r\n",__FUNCTION__,chip_id,instance_id);
		}

		return RET_PARAERR;
	}

	oh_rx_exgid_expid_set(chip_id,instance_id ,EXGID,exp_gid);

	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*     flexe_oh_tx_gid_set
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
*	  instance_id:
*	  tx_grp_id:
* 
* RETURNS
* 
*    RET_SUCCESS: success
*    RET_FAIL: fail
*    RET_PARAERR:parameter error
******************************************************************************/
RET_STATUS flexe_oh_tx_gid_set(UINT_8 chip_id,UINT_8 instance_id,UINT_32 tx_grp_id) 
{		
	if(chip_id > MAX_DEV || instance_id > (INSTANCE_NUM - 1))
	{
		if (module_flexe_oh_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d\r\n",__FUNCTION__,chip_id);
		}

		return RET_PARAERR;
	}

	oh_tx_oh_ins_cfg_set(chip_id,instance_id,GID_INS,tx_grp_id);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*     flexe_oh_rx_gid_get
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
*	  instance_id:
*	  *group_id:
* 
* RETURNS
* 
*    RET_SUCCESS: success
*    RET_FAIL: fail
*    RET_PARAERR:parameter error
******************************************************************************/
RET_STATUS flexe_oh_rx_gid_get (UINT_8 chip_id,UINT_8 instance_id,UINT_32* group_id)   
{
	if(chip_id > MAX_DEV || instance_id > (INSTANCE_NUM-1) )
	{
		if (module_flexe_oh_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,instance_id=%d\r\n",__FUNCTION__,chip_id,instance_id);
		}

		return RET_PARAERR;
	}

	if (NULL == group_id)
	{
		if (module_flexe_oh_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	oh_rx_id_rx_value_get( chip_id,instance_id,ACGID,group_id);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*     flexe_oh_exp_pid_set
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
*	  tunnel_id:
*	  phy_id:
* 
* RETURNS
* 
*    RET_SUCCESS: success
*    RET_FAIL: fail
*    RET_PARAERR:parameter error
******************************************************************************/
RET_STATUS flexe_oh_exp_pid_set(UINT_8 chip_id,UINT_8 instance_id,UINT_8 phy_id) 
{
	if(chip_id > MAX_DEV || instance_id > (PHY_NUM-1))
	{
		if (module_flexe_oh_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,instance_id=%d\r\n",__FUNCTION__,chip_id,instance_id);
		}

		return RET_PARAERR;
	}

	oh_rx_exgid_expid_set(chip_id, instance_id, EXPID, phy_id);

	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*     flexe_oh_tx_pid_set
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
*	  tunnel_id:
*	  phy_id:
* 
* RETURNS
* 
*    RET_SUCCESS: success
*    RET_FAIL: fail
*    RET_PARAERR:parameter error
******************************************************************************/
RET_STATUS flexe_oh_tx_pid_set(UINT_8 chip_id,UINT_8 instance_id,UINT_8 phy_id) 
{
	if(chip_id > MAX_DEV || instance_id > (PHY_NUM-1))
	{
		if (module_flexe_oh_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,instance_id=%d\r\n",__FUNCTION__,chip_id,instance_id);
		}

		return RET_PARAERR;
	}

    oh_tx_oh_ins_cfg_set(chip_id, instance_id, PID_INS, phy_id);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*     flexe_oh_rx_pid_get
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
*	  instance_id:
*	  *group_id:
* 
* RETURNS
* 
*    RET_SUCCESS: success
*    RET_FAIL: fail
*    RET_PARAERR:parameter error
******************************************************************************/
RET_STATUS flexe_oh_rx_pid_get (UINT_8 chip_id,UINT_8 instance_id,UINT_32* phy_id)   
{
	if(chip_id > MAX_DEV || instance_id > (INSTANCE_NUM-1) )
	{
		if (module_flexe_oh_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,instance_id=%d\r\n",__FUNCTION__,chip_id,instance_id);
		}

		return RET_PARAERR;
	}

	if (NULL == phy_id)
	{
		if (module_flexe_oh_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	oh_rx_id_rx_value_get( chip_id,instance_id,ACPID,phy_id);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*     flexe_oh_exp_pmm_set
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
*	  instance_id:
*	  value:array[32]
* 
* RETURNS
* 
*    RET_SUCCESS: success
*    RET_FAIL: fail
*    RET_PARAERR:parameter error
******************************************************************************/
RET_STATUS flexe_oh_exp_pm_set(UINT_8 chip_id,UINT_8 instance_id,UINT_32* value) 
{
	UINT_8 offset = 0;

	if(chip_id > MAX_DEV || instance_id > (PHY_NUM-1))
	{
		if (module_flexe_oh_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,instance_id=%d\r\n",__FUNCTION__,chip_id,instance_id);
		}

		return RET_PARAERR;
	}

	if (NULL == value)
	{
		if (module_flexe_oh_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	for(offset = 0; offset < 32;offset++)
	{
		oh_rx_phymap_rx_exp_set(chip_id, instance_id,offset, &value[offset]);
	}
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*     flexe_oh_tx_pmm_set
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
*	  instance_id:
*	  value:array[32]
* 
* RETURNS
* 
*    RET_SUCCESS: success
*    RET_FAIL: fail
*    RET_PARAERR:parameter error
******************************************************************************/
RET_STATUS flexe_oh_tx_pm_set(UINT_8 chip_id,UINT_8 instance_id,UINT_32* value) 
{
	UINT_8 offset = 0;

	if(chip_id > MAX_DEV || instance_id > (PHY_NUM-1))
	{
		if (module_flexe_oh_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,instance_id=%d\r\n",__FUNCTION__,chip_id,instance_id);
		}

		return RET_PARAERR;
	}

	if (NULL == value)
	{
		if (module_flexe_oh_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	for(offset = 0; offset < 32;offset++)
	{
		oh_tx_phymap_cca_ccb_ins_cfg_set(chip_id, instance_id, 0,offset, &value[offset]);
    }
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*     flexe_oh_rx_pmm_get
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
*	  instance_id:
*	  value:array[32]
* 
* RETURNS
* 
*    RET_SUCCESS: success
*    RET_FAIL: fail
*    RET_PARAERR:parameter error
******************************************************************************/
RET_STATUS flexe_oh_rx_pm_get (UINT_8 chip_id,UINT_8 instance_id,UINT_32* value)   
{
	UINT_8 offset = 0;
    
	if(chip_id > MAX_DEV || instance_id > (INSTANCE_NUM-1) )
	{
		if (module_flexe_oh_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,instance_id=%d\r\n",__FUNCTION__,chip_id,instance_id);
		}

		return RET_PARAERR;
	}

	if (NULL == value)
	{
		if (module_flexe_oh_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	for(offset = 0; offset < 32;offset++)
	{
		oh_rx_phymap_rx_get(chip_id, instance_id,offset, &value[offset]);
	}
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*     flexe_oh_exp_ccab_set
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
*	  field_id:0:cca 1:ccb
*	  instance_id:
*	  value:array[20]
* 
* RETURNS
* 
*    RET_SUCCESS: success
*    RET_FAIL: fail
*    RET_PARAERR:parameter error
******************************************************************************/
RET_STATUS flexe_oh_exp_ccab_set(UINT_8 chip_id,UINT_8 field_id,UINT_8 instance_id,UINT_32* value) 
{
	UINT_8 offset = 0;
    
	if(chip_id > MAX_DEV || instance_id > (PHY_NUM-1))
	{
		if (module_flexe_oh_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,instance_id=%d\r\n",__FUNCTION__,chip_id,instance_id);
		}

		return RET_PARAERR;
	}

	if (NULL == value)
	{
		if (module_flexe_oh_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	for(offset = 0; offset < 20;offset++)
	{
		oh_rx_ccab_rx_exp_set(chip_id, instance_id,field_id,offset, &value[offset]);
	}
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*     flexe_oh_tx_ccab_set
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
*	  field_id:0:cca 1:ccb
*	  instance_id:
*	  value:array[20]
* 
* RETURNS
* 
*    RET_SUCCESS: success
*    RET_FAIL: fail
*    RET_PARAERR:parameter error
******************************************************************************/
RET_STATUS flexe_oh_tx_ccab_set(UINT_8 chip_id,UINT_8 field_id,UINT_8 instance_id,UINT_32* value) 
{
	UINT_8 offset = 0;
		
	if(chip_id > MAX_DEV || instance_id > (PHY_NUM-1))
	{
		if (module_flexe_oh_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,instance_id=%d\r\n",__FUNCTION__,chip_id,instance_id);
		}

		return RET_PARAERR;
	}
	
	if (NULL == value)
	{
		if (module_flexe_oh_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	for(offset = 0; offset < 32;offset++)
	{
		oh_tx_phymap_cca_ccb_ins_cfg_set(chip_id, instance_id, field_id+1,offset, &value[offset]);
	}

	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*     flexe_oh_rx_ccab_get
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
*	  field_id:0:cca 1:ccb
*	  instance_id:
*	  value:array[20]
* 
* RETURNS
* 
*    RET_SUCCESS: success
*    RET_FAIL: fail
*    RET_PARAERR:parameter error
******************************************************************************/
RET_STATUS flexe_oh_rx_ccab_get (UINT_8 chip_id,UINT_8 field_id,UINT_8 instance_id,UINT_32* value)   
{
	UINT_8 offset = 0;
    
	if(chip_id > MAX_DEV || instance_id > (INSTANCE_NUM-1) )
	{
		if (module_flexe_oh_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,instance_id=%d\r\n",__FUNCTION__,chip_id,instance_id);
		}

		return RET_PARAERR;
	}

	if (NULL == value)
	{
		if (module_flexe_oh_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	for(offset = 0; offset < 32;offset++)
	{
		oh_rx_calendar_rx_get(chip_id, instance_id,field_id,offset, &value[offset]);
	}
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION flexe_oh_reserved_set
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
*	  chip_id:chip number used
*     instance_id:0~INSTANCE_NUM-1
*	  field_id:0:1-1 bit0-31 1:1-2 bit32-46 1:2
*	  parameter:
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    jxma    	2018-1-2    1.0           initial
*
******************************************************************************/
RET_STATUS flexe_oh_res_set(UINT_8 chip_id,UINT_8 instance_id,UINT_8 field_id,UINT_32 parameter)
{	
	if(chip_id > MAX_DEV || instance_id > (PHY_NUM-1))
	{
		if (module_flexe_oh_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,instance_id=%d\r\n",__FUNCTION__,chip_id,instance_id);
		}

		return RET_PARAERR;
	}
	
	switch(field_id)
	{
		case 0:
			regp_bit_write(chip_id,FLEXE_OH_TX_BASE_ADDR,0x10 + FLEXE_OFFSET*instance_id,31,4,parameter&0xfffffff);
			regp_bit_write(chip_id,FLEXE_OH_TX_BASE_ADDR,0x11 + FLEXE_OFFSET*instance_id,3,0,(parameter>>28)&0xf);
			break;
		case 1:
			regp_bit_write(chip_id,FLEXE_OH_TX_BASE_ADDR,0x11 + FLEXE_OFFSET*instance_id,18,4,parameter&0x7fff);
			break;
		case 2:
			regp_bit_write(chip_id,FLEXE_OH_TX_BASE_ADDR,0x11 + FLEXE_OFFSET*instance_id,31,19,parameter&0x1fff);
			break;
		default:
			return RET_PARAERR;
	}
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION flexe_oh_reserved_get
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
*	  chip_id:chip number used
*     instance_id:0~INSTANCE_NUM-1
*	  field_id:0:1 1:2
*	  *parameter:
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    jxma    	2018-1-2    1.0           initial
*
******************************************************************************/
RET_STATUS flexe_oh_res_get(UINT_8 chip_id,UINT_8 instance_id,UINT_8 field_id,UINT_32* parameter)
{	
	UINT_32 reg_data = 0;

	if(chip_id > MAX_DEV || instance_id > (PHY_NUM-1))
	{
		if (module_flexe_oh_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,instance_id=%d\r\n",__FUNCTION__,chip_id,instance_id);
		}

		return RET_PARAERR;
	}
	
	switch(field_id)
	{
		case 0:
			regp_bit_read(chip_id,FLEXE_OH_TX_BASE_ADDR,0x10 + FLEXE_OFFSET*instance_id,31,4,parameter);
			regp_bit_read(chip_id,FLEXE_OH_TX_BASE_ADDR,0x11 + FLEXE_OFFSET*instance_id,3,0,&reg_data);
			*parameter |= ((reg_data & 0xf) << 28);
			break;
		case 1:
			regp_bit_read(chip_id,FLEXE_OH_TX_BASE_ADDR,0x11 + FLEXE_OFFSET*instance_id,18,4,parameter);
			break;
		case 2:
			regp_bit_read(chip_id,FLEXE_OH_TX_BASE_ADDR,0x11 + FLEXE_OFFSET*instance_id,31,19,parameter);
			break;
		default:
			return RET_PARAERR;
	}
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION flexe_oh_reserved_src_set
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
*	  chip_id:chip number used
*     instance_id:0~INSTANCE_NUM-1
*	  field_id:0:reserved1 1:reserved2
*	  parameter:0:upi 1:
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    jxma    	2018-1-2    1.0           initial
*
******************************************************************************/
RET_STATUS flexe_oh_res_src_set(UINT_8 chip_id,UINT_8 instance_id,UINT_8 field_id,UINT_32 parameter)
{	
	if(chip_id > MAX_DEV || instance_id > (PHY_NUM-1))
	{
		if (module_flexe_oh_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,instance_id=%d\r\n",__FUNCTION__,chip_id,instance_id);
		}

		return RET_PARAERR;
	}
	
	oh_tx_res_ins_src_cfg_set(chip_id,instance_id,field_id,parameter);

	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION flexe_oh_smc_src_set
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
*	  chip_id:chip number used
*     instance_id:0~INSTANCE_NUM-1
*	  field_id:0:1 1:2
*	  parameter:0:upi 1:
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    jxma    	2018-1-2    1.0           initial
*
******************************************************************************/
RET_STATUS flexe_oh_smc_src_set(UINT_8 chip_id,UINT_8 instance_id,UINT_32 parameter)
{	
	if(chip_id > MAX_DEV || instance_id > (PHY_NUM-1))
	{
		if (module_flexe_oh_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,instance_id=%d\r\n",__FUNCTION__,chip_id,instance_id);
		}

		return RET_PARAERR;
	}
		
	oh_tx_oh_ins_src_cfg_set(chip_id,instance_id,SMC_SRC_CFG,parameter);

	return RET_SUCCESS;
}

void module_flexe_oh_debug_set(unsigned long int parameter)
{
	module_flexe_oh_debug = parameter;
	printf("module_flexe_oh_debug = %ld\r\n",module_flexe_oh_debug);
}


