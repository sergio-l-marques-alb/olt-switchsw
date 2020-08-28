

/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "mux_algorithm.h"
#include "flexe_mux.h"
#include "module_mux.h"
#include "math.h"
#include "module_sar.h"
/*#include "module_rateadpt.h"*/
#include "sal.h"
#include "hal.h"
#include "oh_tx.h"
#include "oh_rx.h"
#include "inf_ch_adp_rx.h"
#include "inf_ch_adp_tx.h"
#include "top.h"

extern RET_STATUS rateadp_traffic_cfg(UINT_8 chip_id,UINT_8 add_del_sel,UINT_8 ch_id,UINT_8 ts_num);

/******************************************************************************
*
* FUNCTION
*
*     mux's hardware init.
*
* DESCRIPTION
*
*     RESERVED
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     unit: dev's index, increased one by one
* 
* RETURNS
*
*     0: DEV_SUCCESS
*     1: DEV_FAIL
*     2: DEV_INVALID
*     3: DEV_INITIALIZED
*     4: DEV_DEINITIALIZED
*     5: DEV_POINTER_NULL
*     6: DEV_MM_ALLOC_FAIL
*     7: DEV_PARA_ERR
*     8: DEV_CH_UNAVAILABLE
*     9: DEV_TS_UNAVAILABLE
*    10: DEV_DEFAULT
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
*    <author>        <date>            <CR_ID>         <DESCRIPTION>
*      liming        2018-01-17         1.0            initial
*
******************************************************************************/
dev_err_t mux_hw_env_init(UINT_8 unit)
{
    const UINT_32 flexe_sch_cfg_value[PHY_NUM] = {1, 2, 4, 8, 1, 2, 4, 8};
    UINT_8 cnt = 0;
    UINT_32 env_mode;

    flexe_mux_env_mode_get(unit, &env_mode);
    if (1 == env_mode) {
        for (cnt = 0; cnt < PHY_NUM; cnt++) {
            flexe_mux_group_cfg_set(unit, cnt, 0x1);
            flexe_mux_flexe_enable_set(unit, cnt, 0x1);
            
        }
        for (cnt = 0; cnt < CH_NUM; cnt++) {
            flexe_mux_ch_belong_flexe_set(unit, cnt, 0x1);
        }

        for (cnt = 0; cnt < PHY_NUM; cnt++) {
            flexe_mux_sch_cfg_set(unit, cnt, flexe_sch_cfg_value[cnt]);
        }
    }

    return DEV_SUCCESS;
}

dev_err_t mux_hw_init(UINT_8 unit)
{
    UINT_8 table_id = 0;
    UINT_8 ts_id = 0;
    UINT_32 data = 0;
    UINT_16 w_depth = 0;
    UINT_32 w_data[3] = {0};
    
    /* check unit whether normal */
    if (unit > (DEV_NUM - 1)) {
        printf("%s: invalid unit = 0x%x...\r\n", __FUNCTION__, unit);
        
        return DEV_PARA_ERR;
    }

    #ifndef SV_TEST
	/* set w_ram to default value */
	memset(w_data, 0xff, sizeof(w_data));
	
    for (w_depth = 0; w_depth < CH_NUM * TS_NUM; w_depth++) {
        flexe_mux_wr_cfg_ram_set(unit, w_depth, w_data);
        flexe_mux_wr_cfg_b_ram_set(unit, w_depth, w_data);
    }

    /* set r_ram to default value */
    data = 0xf;
	for (table_id = 0; table_id < 10; table_id++) {
		for (ts_id = 0; ts_id < TS_NUM; ts_id++) {
            /** coverity[ARRAY_VS_SINGLETON:FALSE] */
			flexe_mux_rd_cfg_ram_set(unit, table_id, ts_id, &data);
			flexe_mux_rd_cfg_b_ram_set(unit, table_id, ts_id, &data);
		}
	}
	#endif

    /* set pcs_enable to zero */
	flexe_mux_pcs_enable_set(unit, 0);

	/* set env mode to zero */
	flexe_mux_env_mode_set(unit, 0);

	/* according asic test modify */
	for (table_id = 0; table_id < PHY_NUM; table_id++) {
		flexe_mux_pcs_chid_cfg_set(unit, table_id, 0x7f);
	}

	mux_hw_env_init(unit);
	
	return DEV_SUCCESS;
}



/******************************************************************************
*
* FUNCTION
*
*     mux's init.
*
* DESCRIPTION
*
*     RESERVED
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     unit: dev's index, increased one by one
* 
* RETURNS
*
*     0: DEV_SUCCESS
*     1: DEV_FAIL
*     2: DEV_INVALID
*     3: DEV_INITIALIZED
*     4: DEV_DEINITIALIZED
*     5: DEV_POINTER_NULL
*     6: DEV_MM_ALLOC_FAIL
*     7: DEV_PARA_ERR
*     8: DEV_CH_UNAVAILABLE
*     9: DEV_TS_UNAVAILABLE
*    10: DEV_DEFAULT
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
*    <author>        <date>            <CR_ID>         <DESCRIPTION>
*      liming        2018-01-17         1.0            initial
*
******************************************************************************/
dev_err_t mux_init(UINT_8 unit)
{
    dev_err_t rt = DEV_DEFAULT;

    /* flexe_mux's init */
    rt = flexe_mux_init(unit);

    if (DEV_SUCCESS != rt) {
        printf("%s: unit = 0x%x, flexe_mux sw_init fail...\r\n", __FUNCTION__, unit);

        return rt;
    }

    /* mux's hardware init */
    rt = mux_hw_init(unit);

    if (DEV_SUCCESS != rt) {
        printf("%s: unit = 0x%x, flexe_mux hw_init fail...\r\n", __FUNCTION__, unit);

        return rt;
    }

    return DEV_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     mux's uninit.
*
* DESCRIPTION
*
*     RESERVED
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     unit: dev's index, increased one by one
* 
* RETURNS
*
*     0: DEV_SUCCESS
*     1: DEV_FAIL
*     2: DEV_INVALID
*     3: DEV_INITIALIZED
*     4: DEV_DEINITIALIZED
*     5: DEV_POINTER_NULL
*     6: DEV_MM_ALLOC_FAIL
*     7: DEV_PARA_ERR
*     8: DEV_CH_UNAVAILABLE
*     9: DEV_TS_UNAVAILABLE
*    10: DEV_DEFAULT
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
*    <author>        <date>            <CR_ID>         <DESCRIPTION>
*      liming        2018-01-17         1.0            initial
*
******************************************************************************/
dev_err_t mux_uninit(UINT_8 unit)
{
    dev_err_t rt = DEV_DEFAULT;

    /* mux's uninit */
    rt = flexe_mux_uninit(unit);

    if (DEV_SUCCESS != rt) {
        printf("%s: unit = 0x%x, flexe_mux uninit fail...\r\n", __FUNCTION__, unit);

        return rt;
    }

    return DEV_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     mux's pcs cfg.
*
* DESCRIPTION
*
*     config mux module's pcs traffic
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     unit: dev's index, increased one by one
*     ch: pcs's ch
*     phy_index: phy's index
*     phy_mode: pcs's phy_mode
*     enable: add or remove traffic
* 
* RETURNS
*
*     0: DEV_SUCCESS
*     1: DEV_FAIL
*     2: DEV_INVALID
*     3: DEV_INITIALIZED
*     4: DEV_DEINITIALIZED
*     5: DEV_POINTER_NULL
*     6: DEV_MM_ALLOC_FAIL
*     7: DEV_PARA_ERR
*     8: DEV_CH_UNAVAILABLE
*     9: DEV_TS_UNAVAILABLE
*    10: DEV_DEFAULT
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
*    <author>        <date>            <CR_ID>         <DESCRIPTION>
*      liming        2018-01-17         1.0            initial
*      liming        2018-06-22         2.0            add asic_version
*
******************************************************************************/
dev_err_t mux_traffic_pcs_cfg(UINT_8 unit,
                              UINT_8 ch,
                              UINT_8 phy_index,
                              flexe_phy_mode phy_mode,
                              UINT_8 enable)
{
    UINT_8 index = 0;
    UINT_8 pcs_num = 0;
    UINT_32 pcs_num_val = 0;
    UINT_32 pcs_enable = 0;
    UINT_32 data = 0;
	UINT_32 pcs_sch_cfg_value = 0;
    flexe_mux_phy_info_t phy_pcs_info[PHY_NUM];
    dev_err_t rt = DEV_DEFAULT;
    
    /* check unit whether normal */
    if (unit > (DEV_NUM - 1)) 
	{
        printf("%s: invalid unit = 0x%x...\r\n", __FUNCTION__, unit);
        return DEV_PARA_ERR;
    }
    /* check ch whether normal */
    if (ch > (CH_NUM - 1)) 
	{
        printf("%s: unit = 0x%x, invalid ch = 0x%x...\r\n", __FUNCTION__, unit, ch);
        return DEV_CH_UNAVAILABLE;
    }
    /* check phy_index whether normal for asic */
    if (phy_index > (PHY_NUM - 1)) 
	{
        printf("%s: unit = 0x%x, invalid phy_index = 0x%x...\r\n", __FUNCTION__, unit, phy_index);
        return DEV_PARA_ERR;
    }

	/* check phy_mode whether normal */
	if ((PCS_PHY_50G != phy_mode) && (PCS_PHY_100G != phy_mode) && (PCS_PHY_200G != phy_mode)) 
	{
		printf("%s: unit = 0x%x, invalid phy_mode = 0x%x...\r\n", __FUNCTION__, unit, phy_mode);
		return DEV_PARA_ERR;
	}
	/* check add_remove info */
	if ((TRAFFIC_REMOVE != enable) && (TRAFFIC_ADD != enable)) 
	{
		printf("%s: unit = 0x%x, invalid enable = 0x%x...\r\n", __FUNCTION__, unit, enable);
		return DEV_PARA_ERR;
	}

	/* config phy_pcs */
	rt = flexe_mux_phy_pcs_cfg(unit, ch, phy_index, phy_mode, enable);
	if (DEV_SUCCESS == rt) 
	{
		/* get pcs_num and pcs's phy_info */
		flexe_mux_phy_pcs_out(unit, &pcs_num, phy_pcs_info);

		/* get pcs enable */
		flexe_mux_pcs_enable_get(unit, &pcs_enable);

		/* add pcs */
		if (TRAFFIC_ADD == enable) 
		{
			for (index = 0; index < pcs_num; index++) 
			{
				data = (UINT_32)(phy_pcs_info[index].phy_index + 1);
				
				if (PCS_PHY_50G == phy_pcs_info[index].phy_mode) /* 50G mode */
				{

					pcs_sch_cfg_value = pcs_sch_cfg_value + (UINT_32)((data & MASK_4_BIT) << (SHIFT_BIT_4 * (pcs_num_val)));
					
					pcs_num_val++;
				}
				else if (PCS_PHY_100G == phy_pcs_info[index].phy_mode) /* 100G mode */
				{

					pcs_sch_cfg_value = pcs_sch_cfg_value + (UINT_32)((data & MASK_4_BIT) << (SHIFT_BIT_4 * (pcs_num_val)));
					pcs_sch_cfg_value = pcs_sch_cfg_value + (UINT_32)((data & MASK_4_BIT) << (SHIFT_BIT_4 * (pcs_num_val + 1)));
					
					pcs_num_val += 2;
				}
				else if (PCS_PHY_200G == phy_pcs_info[index].phy_mode) /* 200G mode */
				{

					pcs_sch_cfg_value = pcs_sch_cfg_value + (UINT_32)((data & MASK_4_BIT) << (SHIFT_BIT_4 * (pcs_num_val)));
					pcs_sch_cfg_value = pcs_sch_cfg_value + (UINT_32)((data & MASK_4_BIT) << (SHIFT_BIT_4 * (pcs_num_val + 1)));
					pcs_sch_cfg_value = pcs_sch_cfg_value + (UINT_32)((data & MASK_4_BIT) << (SHIFT_BIT_4 * (pcs_num_val + 2)));
					pcs_sch_cfg_value = pcs_sch_cfg_value + (UINT_32)((data & MASK_4_BIT) << (SHIFT_BIT_4 * (pcs_num_val + 3)));
					
					pcs_num_val += 4;
				}
				else 
				{
					printf("%s: unit = 0x%x, pcs_phy_mode error...\r\n", __FUNCTION__, unit);
					return DEV_FAIL;
				}
			}

			/* config pcs_num */
			flexe_mux_pcs_num_set(unit, pcs_num_val - 1);

			pcs_enable |= (MASK_1_BIT << phy_index);
			
            /* config pcs_channel_id */
			flexe_mux_pcs_chid_cfg_set(unit, phy_index, (UINT_32)ch);
		}
		else /* delete pcs */
		{
			if (0 == pcs_num) 
			{


				/* config pcs_num */
				flexe_mux_pcs_num_set(unit, 0);
			}
			else 
			{
				for (index = 0; index < pcs_num; index++) 
				{
					data = (UINT_32)(phy_pcs_info[index].phy_index + 1);
					
					if (PCS_PHY_50G == phy_pcs_info[index].phy_mode) /* 50G mode */
					{
						pcs_sch_cfg_value = pcs_sch_cfg_value + (UINT_32)((data & MASK_4_BIT) << (SHIFT_BIT_4 * (pcs_num_val)));
						
						pcs_num_val++;
					}
					else if (PCS_PHY_100G == phy_pcs_info[index].phy_mode) /* 100G mode */
					{

						pcs_sch_cfg_value = pcs_sch_cfg_value + (UINT_32)((data & MASK_4_BIT) << (SHIFT_BIT_4 * (pcs_num_val)));
						pcs_sch_cfg_value = pcs_sch_cfg_value + (UINT_32)((data & MASK_4_BIT) << (SHIFT_BIT_4 * (pcs_num_val + 1)));
						
						pcs_num_val += 2;
					}
					else if (PCS_PHY_200G == phy_pcs_info[index].phy_mode) /* 200G mode */
					{

						pcs_sch_cfg_value = pcs_sch_cfg_value + (UINT_32)((data & MASK_4_BIT) << (SHIFT_BIT_4 * (pcs_num_val)));
						pcs_sch_cfg_value = pcs_sch_cfg_value + (UINT_32)((data & MASK_4_BIT) << (SHIFT_BIT_4 * (pcs_num_val + 1)));
						pcs_sch_cfg_value = pcs_sch_cfg_value + (UINT_32)((data & MASK_4_BIT) << (SHIFT_BIT_4 * (pcs_num_val + 2)));
						pcs_sch_cfg_value = pcs_sch_cfg_value + (UINT_32)((data & MASK_4_BIT) << (SHIFT_BIT_4 * (pcs_num_val + 3)));
						
						pcs_num_val += 4;
					}
					else 
					{
						printf("%s: unit = 0x%x, pcs_phy_mode error...\r\n", __FUNCTION__, unit);
						return DEV_FAIL;
					}
				}

				/* config pcs_num */
				flexe_mux_pcs_num_set(unit, pcs_num_val - 1);
			}

			pcs_enable &= ~(MASK_1_BIT << phy_index);

			/* config pcs_channel_id */
			/* according asic test modify */
			flexe_mux_pcs_chid_cfg_set(unit, phy_index, 0x7f);
		}

		/* config pcs_sch_cfg */
		for (index = 0; index < PHY_NUM; index++) 
		{
			data = (pcs_sch_cfg_value >> (SHIFT_BIT_4 * index)) & MASK_4_BIT;
			flexe_mux_pcs_sch_cfg_set(unit, index, data);
		}

		/* config pcs_enable */
		flexe_mux_pcs_enable_set(unit, pcs_enable);

		/* config mux_pls */
		flexe_mux_cfg_pls_set(unit);	
	}
	else 
	{
        printf("%s: unit = 0x%x, pcs cfg fail...\r\n", __FUNCTION__, unit);
	}

	return rt;
}


/******************************************************************************
*
* FUNCTION
*
*     mux debug function.
*
* DESCRIPTION
*
*     RESERVED
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     RESERVED
* 
* RETURNS
*
*     0: DEV_SUCCESS
*     1: DEV_FAIL
*     2: DEV_INVALID
*     3: DEV_INITIALIZED
*     4: DEV_DEINITIALIZED
*     5: DEV_POINTER_NULL
*     6: DEV_MM_ALLOC_FAIL
*     7: DEV_PARA_ERR
*     8: DEV_CH_UNAVAILABLE
*     9: DEV_TS_UNAVAILABLE
*    10: DEV_DEFAULT
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
*    <author>        <date>            <CR_ID>         <DESCRIPTION>
*      liming        2018-01-12         1.0            initial
*
******************************************************************************/
dev_err_t mux_debug(void)
{
    printf("+----------------------mux's initialized/uninit function--------------------------------------+\r\n");
    printf("mux_init(UINT_8 unit)\r\n");
    printf("mux_uninit(UINT_8 unit)\r\n");
    printf("\r\n");

    printf("parameter description:\r\n");
    printf("unit: dev'index, default start from zero, and increased one by one.\r\n");
    printf("+---------------------------------------------------------------------------------------------+\r\n");
    printf("\r\n\r\n");

    printf("parameter description:\r\n");
    printf("unit: dev'index, default start from zero, and increased one by one.\r\n");
    printf("phy_mode: 4bits for one phy, 0->flexe_50G, 1->flexe_100G, 2->pcs_50G, 3->pcs_100G, 4->default.\r\n");
    printf("+---------------------------------------------------------------------------------------------+\r\n");
    printf("\r\n\r\n");

    printf("+----------------------traffic_pcs function----------------------------------------------------------+\r\n");
    printf("mux_traffic_pcs_cfg(UINT_8 unit, UINT_8 ch, UINT_8 phy_index, flexe_phy_mode phy_mode, UINT_8 enable)\r\n");
    printf("\r\n");

    printf("parameter description:\r\n");
    printf("unit: dev'index, default start from zero, and increased one by one.\r\n");
    printf("ch: channel_id, range is 0~79.\r\n");
    printf("phy_index: pcs's phy_index, range is 0~7.\r\n");
    printf("phy_mode: 4bits for one phy, 0->flexe_50G, 1->flexe_100G, 2->pcs_50G, 3->pcs_100G, 4->default.\r\n");
    printf("enable: 0 is remove, 1 is add.\r\n");
    printf("+---------------------------------------------------------------------------------------------------+\r\n");
    printf("\r\n\r\n");

    printf("+----------------------traffic_flexe function----------------------------------------------------------+\r\n");
    printf("mux_traffic_flexe_cfg_test(UINT_8 unit, UINT_8 ch, UINT_32 ts_mask_l, UINT_32 ts_mask_m, UINT_32 ts_mask_h, UINT_8 enable)\r\n");
    printf("\r\n");

    printf("parameter description:\r\n");
    printf("unit: dev'index, default start from zero, and increased one by one.\r\n");
    printf("ch: channel_id, range is 0~79.\r\n");
    printf("ts_mask_l: ts payload bit, ts0~31.\r\n");
    printf("ts_mask_m: ts payload bit, ts32~63.\r\n");
    printf("ts_mask_h: ts payload bit, ts64~79.\r\n");
    printf("enable: 0 is remove, 1 is add.\r\n");
    printf("+----------------------------------------------------------------------------------------------------+\r\n");
    printf("\r\n\r\n");

    printf("+----------------------mux's group_phy_cfg function----------------------------------------------------+\r\n");
    printf("mux_phy_group_cfg(UINT_8 unit, UINT_8 group_id, UINT_8 phy_index, flexe_phy_mode phy_mode, UINT_8 enable)\r\n");
    printf("\r\n");

    printf("parameter description:\r\n");
    printf("unit: dev'index, default start from zero, and increased one by one.\r\n");
    printf("group_id: phy's group_id.\r\n");
    printf("phy_index: phy's index, phy1->bit0...phy8->bit7.\r\n");
    printf("phy_mode: phy's working state, 0->flexe_50G, 1->flexe_100G, 2->pcs_50G, 3->pcs_100G.\r\n");
    printf("enable: 0 is remove, 1 is add.\r\n");
    printf("+-----------------------------------------------------------------------------------------------------+\r\n");
    printf("\r\n\r\n");

    printf("+----------------------mux's traffic_flexe_cfg function----------------------------------------------------+\r\n");
    printf("mux_traffic_flexe_cfg_debug(char *string)\r\n");
    printf("\r\n");

    printf("parameter description:\r\n");
    printf("unit: dev'index, default start from zero, and increased one by one.\r\n");
    printf("ch: traffic's ch_id, range is from 0...79.\r\n");
    printf("*phy_index: traffic's phy's index pointer.\r\n");
    printf("phy_num: traffic's payload phy_num.\r\n");
    printf("*ts_mask: traffic_phy's payload ts pointer.\r\n");
    printf("enable: 0 is remove, 1 is add.\r\n");

    printf("+-----------------------------------------------------------------------------------------------------+\r\n");
    printf("\r\n\r\n");

    printf("+----------------------mux's traffic_cfg function----------------------------------------------------+\r\n");
    printf("mux_traffic_cfg_debug(char *string)\r\n");
    printf("\r\n");

    printf("parameter description:\r\n");
    printf("unit: dev'index, default start from zero, and increased one by one.\r\n");
    printf("ch: traffic's ch_id, range is from 0...79.\r\n");
    printf("*ts_mask: traffic_phy's payload ts pointer.\r\n");
    printf("enable: 0 is remove, 1 is add.\r\n");
    printf("+-----------------------------------------------------------------------------------------------------+\r\n");
    printf("\r\n\r\n");    

    printf("+----------------------mux's dump function----------------------------------------------------+\r\n");
    printf("flexe_mux_debug(void)\r\n");
    printf("\r\n");

    printf("parameter description: no parameter\r\n");
    printf("+---------------------------------------------------------------------------------------------+\r\n");
    printf("\r\n\r\n");

    return DEV_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     get module_mux_chip info.
*
* DESCRIPTION
*
*     RESERVED
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     unit: dev's index, increased one by one
*     flexe_mux_chip_info: chip_info pointer
* 
* RETURNS
*
*     0: DEV_SUCCESS
*     1: DEV_FAIL
*     2: DEV_INVALID
*     3: DEV_INITIALIZED
*     4: DEV_DEINITIALIZED
*     5: DEV_POINTER_NULL
*     6: DEV_MM_ALLOC_FAIL
*     7: DEV_PARA_ERR
*     8: DEV_CH_UNAVAILABLE
*     9: DEV_TS_UNAVAILABLE
*    10: DEV_DEFAULT
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
*    <author>        <date>            <CR_ID>         <DESCRIPTION>
*      liming        2018-03-30         1.0            initial
*
******************************************************************************/
dev_err_t mux_chip_info_get(UINT_8 unit, flexe_mux_info_t **flexe_mux_chip_info)
{
    dev_err_t rt = DEV_DEFAULT;
    
    /* check unit whether normal */
    if (unit > (DEV_NUM - 1)) {
        printf("%s: invalid unit = 0x%x...\r\n", __FUNCTION__, unit);
        
        return DEV_PARA_ERR;
    }

    /* check pointer whether normal */
    if (!flexe_mux_chip_info) {
        printf("%s: invalid pointer, unit = 0x%x...\r\n", __FUNCTION__, unit);
        
        return DEV_POINTER_NULL;
    }

    rt = flexe_mux_chip_info_get(unit, flexe_mux_chip_info);

    if (DEV_SUCCESS != rt) {
        printf("%s: unit = 0x%x, get mux_dev_info fail...\r\n", __FUNCTION__, unit);
    }

    return rt;
}

dev_err_t mux_phy_group_c_cfg(UINT_8 unit, UINT_8 group_id, UINT_8 c_value)
{
    UINT_8 index = 0;
    dev_err_t rt = DEV_DEFAULT;
    flexe_mux_group_info_t group_info;

    /* check unit whether normal */
    if (unit > (DEV_NUM - 1)) {
        printf("%s: invalid unit = 0x%x...\r\n", __FUNCTION__, unit);
        
        return DEV_PARA_ERR;
    }

    /* check group_id whether normal */
    if (group_id > (PHY_NUM - 1)) {
        printf("%s: unit = 0x%x, invalid group_id = 0x%x...\r\n", __FUNCTION__, unit, group_id);
        
        return DEV_PARA_ERR;
    }

    rt = flexe_mux_phy_group_info_get(unit, group_id, &group_info);
    if (DEV_SUCCESS != rt) {
        printf("%s: unit = 0x%x, get group_id = 0x%x's info fail, rt = %d...\r\n",
                                                                     __FUNCTION__,
                                                                             unit,
                                                                         group_id,
                                                                               rt);

        return rt;
    }

    for (index = 0; index < group_info.phy_num; index++) {
    
        flexe_mux_c_cfg_set(unit, group_info.phy_index[index], (UINT_32)c_value);
    }

    flexe_mux_cfg_pls_set(unit);

    return rt;
}

/******************************************************************************
*
* FUNCTION
*
*     mux's group_phy config.
*
* DESCRIPTION
*
*     RESERVED
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     unit: dev's index, increased one by one
*     group_id: phy's group_id
*     phy_index: phy's enable state,bit0->phy0....bit7->phy7
*     phy_mode: phy's working mode
*     enable: 0->remove, 1->add
* 
* RETURNS
*
*     0: DEV_SUCCESS
*     1: DEV_FAIL
*     2: DEV_INVALID
*     3: DEV_INITIALIZED
*     4: DEV_DEINITIALIZED
*     5: DEV_POINTER_NULL
*     6: DEV_MM_ALLOC_FAIL
*     7: DEV_PARA_ERR
*     8: DEV_CH_UNAVAILABLE
*     9: DEV_TS_UNAVAILABLE
*    10: DEV_DEFAULT
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
*    <author>        <date>            <CR_ID>         <DESCRIPTION>
*      liming        2018-03-23         1.0            initial
*      liming        2018-06-12         2.0        add asic_version
*
******************************************************************************/
dev_err_t mux_phy_group_cfg(UINT_8 unit,
                            UINT_8 group_id,
                            UINT_8 phy_index,
                            flexe_phy_mode phy_mode,
                            UINT_8 enable)
{
    UINT_8 index = 0;
	UINT_8 channel = 0;
    UINT_8 phy_num = 0;
	UINT_8 channel_belong_status = false;
    UINT_8 flexe_group_cfg = DEFAULT_VALUE;
    UINT_8 flexe_group_phy_cfg[PHY_NUM];
    UINT_8 group_phy_index[PHY_NUM];
	flexe_mux_group_info_t group_info;
	flexe_mux_group_info_t group_info_after_delete;
    dev_err_t rt = DEV_DEFAULT;
        UINT_32 env_mode = 0;/*used for ASIC mux mode sel*/
	/* recording parameter */
	printf("%s,%d:unit=%d,group_id=%d,hard_8bit_phy_index=0x%x,phy_mode=%d,enable=%d\r\n",__FUNCTION__,__LINE__, unit, group_id, phy_index, phy_mode, enable);

    /* check unit whether normal */
    if (unit > (DEV_NUM - 1)) 
	{
        printf("%s: invalid unit = 0x%x...\r\n", __FUNCTION__, unit);
        return DEV_PARA_ERR;
    }
    /* check group_id whether normal */
    if (group_id > (PHY_NUM - 1)) 
	{
        printf("%s: unit = 0x%x, invalid group_id = 0x%x...\r\n", __FUNCTION__, unit, group_id);
        return DEV_PARA_ERR;
    }
	/* check add_remove info */
	if ((TRAFFIC_REMOVE != enable) && (TRAFFIC_ADD != enable)) 
	{
		printf("%s: unit = 0x%x, invalid enable = 0x%x...\r\n", __FUNCTION__, unit, enable);
		return DEV_PARA_ERR;
	}

	flexe_mux_env_mode_get(unit, &env_mode);
	/* add group's phy_info */
	if (TRAFFIC_ADD == enable) 
	{
	    /* check phy_index whether normal */
	    if (0 == phy_index) {
	        printf("%s: unit = 0x%x, enable = 0x%x, invalid phy_index = 0x%x...\r\n", __FUNCTION__, unit, enable, phy_index);
	        return DEV_PARA_ERR;
	    }

	    /* check phy_mode whether normal */
	    if ((FLEXE_PHY_50G != phy_mode) && (FLEXE_PHY_100G != phy_mode) 
			&& (PCS_PHY_50G != phy_mode) && (PCS_PHY_100G != phy_mode) && (PCS_PHY_200G != phy_mode))
		{
	        printf("%s: unit = 0x%x, enable = 0x%x, invalid phy_mode = 0x%x...\r\n", __FUNCTION__, unit, enable, phy_mode);
	        return DEV_PARA_ERR;
	    }
		
	    /* set group_phy_index and group_phy_cfg state to default value */
		memset(group_phy_index, DEFAULT_VALUE, sizeof(group_phy_index));
		memset(flexe_group_phy_cfg, 0, sizeof(flexe_group_phy_cfg));
		
		/* config group's phy_info */
	    rt = flexe_mux_phy_group_cfg(unit, group_id, phy_index, phy_mode, enable);
		if (DEV_SUCCESS != rt)
		{
			printf("%s: unit = 0x%x, group_id = 0x%x, enable = 0x%x, group_phy cfg fail...\r\n", __FUNCTION__, unit, group_id, enable);
			return rt;
		}

		rt = flexe_mux_phy_group_info_get(unit, group_id, &group_info);
		if (DEV_SUCCESS != rt) 
		{
			printf("%s: unit = 0x%x, enable = 0x%x, get group_id = 0x%x's info fail, rt = %d...\r\n", __FUNCTION__, unit, enable, group_id, rt);
			return rt;
		}

		if ((FLEXE_PHY_100G == phy_mode) || (FLEXE_PHY_50G == phy_mode)) 
		{
			if (FLEXE_PHY_100G == phy_mode) 
			{
				for (index = 0; index < group_info.phy_num; index += 2) 
				{
					/* config flexe_100g_mode */
					flexe_mux_mode_set(unit, group_info.phy_index[index], 1);
					flexe_mux_mode_set(unit, group_info.phy_index[index + 1], 2);

					/* config flexe_100g_cphy2 */
					flexe_mux_cphy2_cfg_set(unit, group_info.phy_index[index], group_info.phy_index[index + 1]);
				}
			}
			else /* config flexe_50g_mode */
			{
				for (index = 0; index < group_info.phy_num; index++) 
				{
					flexe_mux_mode_set(unit, group_info.phy_index[index], 0);
				}
			}

			/* flexe's group_phy_index is 1->5->2->6->3->7->4->8 sort for asic_version */
			/* get one group's phy_index */
			for (index = 0; index < group_info.phy_num; index++) 
			{
				flexe_group_phy_cfg[group_info.phy_index[index]] = 1;
			}

			if (1 == flexe_group_phy_cfg[0]) 
			{
				flexe_group_cfg = 1;
			}
			else if (1 == flexe_group_phy_cfg[4]) 
			{
				flexe_group_cfg = 5;
			}
			else if (1 == flexe_group_phy_cfg[1]) 
			{
				flexe_group_cfg = 2;
			}
			else if (1 == flexe_group_phy_cfg[5]) 
			{
				flexe_group_cfg = 6;
			}
			else if (1 == flexe_group_phy_cfg[2]) 
			{
				flexe_group_cfg = 3;
			}
			else if (1 == flexe_group_phy_cfg[6]) 
			{
				flexe_group_cfg = 7;
			}
			else if (1 == flexe_group_phy_cfg[3]) 
			{
				flexe_group_cfg = 4;
			}
			else if (1 == flexe_group_phy_cfg[7]) 
			{
				flexe_group_cfg = 8;
			}
			else 
			{
				printf("%s: unit = 0x%x, group_id = 0x%x, enable = 0x%x, there is no phy config...\r\n", __FUNCTION__, unit, group_id, enable);
				return DEV_FAIL;
			}
			
			for(channel = 0; channel < CH_NUM; channel++)
			{
				channel_belong_status = false;
				
				rt = flexe_mux_ch_belong_group(unit, channel, group_id, &channel_belong_status);
				
				if(rt != DEV_SUCCESS)
				{
					printf("%s: unit=0x%x,group_id = 0x%x execute fail!\r\n",__FUNCTION__,unit,group_id);
					return DEV_FAIL;
				}
				
				if(true == channel_belong_status)
				{
					flexe_mux_ch_belong_flexe_set(unit, channel, flexe_group_cfg);
				}
			}

            /* config flexe_mux_group info */
			for (index = 0; index < group_info.phy_num; index++) 
			{
				/* config flexe_mux_group info */
				flexe_mux_group_cfg_set(unit, group_info.phy_index[index], flexe_group_cfg);
				top_psy_sel_cfg_set(unit, group_info.phy_index[index], flexe_group_cfg);
				/* config flexe_enable */
				if (0 == env_mode) 
				{
					flexe_mux_flexe_enable_set(unit, group_info.phy_index[index], 1);
				}					
			}

			for (index = 0; index < PHY_NUM; index++) 
			{
				if (1 == flexe_group_phy_cfg[index]) 
				{
					group_phy_index[phy_num] = index;
					phy_num++;
				}
			}

			/* config the RR4 for flexe */
			if (0 == env_mode) 
			{
				for (index = 0; index < phy_num; index++) 
				{
					if (group_phy_index[index] > 3) 
					{
						flexe_mux_sch_cfg_set(unit, group_phy_index[index], 1 << (group_phy_index[index] - 4));
					}
					else 
					{
						flexe_mux_sch_cfg_set(unit, group_phy_index[index], 1 << group_phy_index[index]);
					}
				}	
			}
		}
	}
	else /* delete group_info */
	{
		rt = flexe_mux_phy_group_info_get(unit, group_id, &group_info);
		if (DEV_SUCCESS != rt) 
		{
			printf("%s: unit = 0x%x, enable = 0x%x, get group_id = 0x%x's info fail, rt = %d...\r\n", __FUNCTION__, unit, enable, group_id, rt);
			return rt;
		}

	    rt = flexe_mux_phy_group_cfg(unit, group_id, phy_index, phy_mode, enable);
		if (DEV_SUCCESS != rt)
		{
			printf("%s: unit = 0x%x, group_id = 0x%x, enable = 0x%x, group_phy remove fail...\r\n", __FUNCTION__, unit, group_id, enable);
			return rt;
		}

		rt = flexe_mux_phy_group_info_get(unit,group_id,&group_info_after_delete);
		if (DEV_SUCCESS != rt)
		{
			printf("%s unit = 0x%x, group_id = 0x%x,get group info after delete fail...\r\n",__FUNCTION__,unit,group_id);
			return rt;
		}
		
		if (group_info_after_delete.phy_num != 0)
		{
			/* flexe's group_phy_index is 1->5->2->6->3->7->4->8 sort for asic_version */
			/* get one group's phy_index */
			for (index = 0; index < PHY_NUM; index++) 
			{
				if (DEFAULT_VALUE != group_info_after_delete.phy_index[index])
				{
					flexe_group_phy_cfg[group_info_after_delete.phy_index[index]] = 1;
				}			
			}

			if (1 == flexe_group_phy_cfg[0]) 
			{
				flexe_group_cfg = 1;
			}
			else if (1 == flexe_group_phy_cfg[4]) 
			{
				flexe_group_cfg = 5;
			}
			else if (1 == flexe_group_phy_cfg[1]) 
			{
				flexe_group_cfg = 2;
			}
			else if (1 == flexe_group_phy_cfg[5]) 
			{
				flexe_group_cfg = 6;
			}
			else if (1 == flexe_group_phy_cfg[2]) 
			{
				flexe_group_cfg = 3;
			}
			else if (1 == flexe_group_phy_cfg[6]) 
			{
				flexe_group_cfg = 7;
			}
			else if (1 == flexe_group_phy_cfg[3]) 
			{
				flexe_group_cfg = 4;
			}
			else if (1 == flexe_group_phy_cfg[7]) 
			{
				flexe_group_cfg = 8;
			}
			else 
			{
				printf("%s: unit = 0x%x, group_id = 0x%x, enable = 0x%x, there is no phy config...\r\n", __FUNCTION__, unit, group_id, enable);
				return DEV_FAIL;
			}

			for (index = 0; index < PHY_NUM; index++) 
			{
				if (0 == env_mode) 
				{
					/* set flexe_mux_group info to default_value */
					if(DEFAULT_VALUE != group_info_after_delete.phy_index[index])
					{
						flexe_mux_group_cfg_set(unit, group_info_after_delete.phy_index[index], flexe_group_cfg); 
						top_psy_sel_cfg_set(unit,group_info_after_delete.phy_index[index],flexe_group_cfg);
					}
				}
			}
			for(channel = 0; channel < CH_NUM; channel++)
			{
				channel_belong_status = false;
				
				rt = flexe_mux_ch_belong_group(unit, channel, group_id, &channel_belong_status);
				
				if(rt != DEV_SUCCESS)
				{
					printf("%s: unit=0x%x,group_id = 0x%x execute fail!\r\n",__FUNCTION__,unit,group_id);
					return DEV_FAIL;
				}
				
				if(true == channel_belong_status)
				{
					flexe_mux_ch_belong_flexe_set(unit, channel, flexe_group_cfg);
				}
			}
		}
		else
		{
			 flexe_mux_group_ch_delete(unit,group_id);
		}

		#if 0
		if ((FLEXE_PHY_100G == phy_mode) || (FLEXE_PHY_50G == phy_mode)) 
		{
			if (FLEXE_PHY_100G == phy_mode) 
			{
				for (index = 0; index < group_info.phy_num; index += 2) 
				{
					if((DEFAULT_VALUE == group_info_after_delete.phy_index[index])&&
					(DEFAULT_VALUE != group_info.phy_index[index]))
					{
						if (0 == env_mode) 
						{
							/* set flexe_100g mux_phy_mode for default_value */
							flexe_mux_mode_set(unit, group_info.phy_index[index], 0);
							flexe_mux_mode_set(unit, group_info.phy_index[index + 1], 0);

							/* set flexe_100g_cphy2 to default value */
							flexe_mux_cphy2_cfg_set(unit, group_info.phy_index[index], group_info.phy_index[index]);
						}
					}
				}
			}
			else /* set flexe_50g mux_phy_mode for default_value */
			{
				for (index = 0; index < group_info.phy_num; index++) 
				{
					if((DEFAULT_VALUE == group_info_after_delete.phy_index[index])&&
					(DEFAULT_VALUE != group_info.phy_index[index]))
					{
						if (0 == env_mode) 
						{
							flexe_mux_mode_set(unit, group_info.phy_index[index], 0); 
						}
					}
				}
			}
		
			/* flexe's group_phy_index is 1->5->2->6->3->7->4->8 for asic_version */
			for (index = 0; index < group_info.phy_num; index++) 
			{
				if((DEFAULT_VALUE == group_info_after_delete.phy_index[index])&&
					(DEFAULT_VALUE != group_info.phy_index[index]))
				{
					if (0 == env_mode) 
					{
						/* set flexe_mux_group info to default_value */
						flexe_mux_group_cfg_set(unit, group_info.phy_index[index], 0);
						/* set flexe_enable to default value */
						flexe_mux_flexe_enable_set(unit, group_info.phy_index[index], 0);
						/* set the RR4 for flexe to default value*/
						flexe_mux_sch_cfg_set(unit, group_info.phy_index[index], 0);
					}
				}
			}
		}
		#endif
	}

	/* config mux_pls */
	flexe_mux_cfg_pls_set(unit);

	return DEV_SUCCESS;
}

UINT_8 min_element(UINT_8 *start, UINT_8 length)
{
	UINT_8 i = 0;
	UINT_8 min = start[0];
	
	for(i = 1;i < length;i++)
	{
		if(min > start[i])
		{
			min = start[i];
		}
	}
    return min;
}

/******************************************************************************
*
* FUNCTION
*
*     mux's traffic cfg function.
*
* DESCRIPTION
*
*     config mux module's flexe_traffic
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     unit: dev's index, increased one by one
*     ch: traffic's ch
*     phy_index: payload traffic's phy_index pointer
*     phy_num: payload traffic's phy_num
*     ts_mask: each phy's payload ts
*     enable: add or remove traffic
* 
* RETURNS
*
*     0: DEV_SUCCESS
*     1: DEV_FAIL
*     2: DEV_INVALID
*     3: DEV_INITIALIZED
*     4: DEV_DEINITIALIZED
*     5: DEV_POINTER_NULL
*     6: DEV_MM_ALLOC_FAIL
*     7: DEV_PARA_ERR
*     8: DEV_CH_UNAVAILABLE
*     9: DEV_TS_UNAVAILABLE
*    10: DEV_DEFAULT
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
*    <author>        <date>            <CR_ID>         <DESCRIPTION>
*      liming        2018-01-17         1.0            initial
*
******************************************************************************/
dev_err_t mux_traffic_flexe_cfg(UINT_8 unit,
                                UINT_8 ch,
                                UINT_8 *phy_index,
                                UINT_8 phy_num,
                                UINT_32 *ts_mask,
                                UINT_8 enable)
{
    UINT_8 index = 0;
    UINT_8 ts_index = 0;
    UINT_8 ts_num = 0;
    UINT_8 blk = 0;
    UINT_8 cycle = 0;
    UINT_8 ts_id[TS_NUM] = {0};
    UINT_8 table_id = 0;
    UINT_8 ram_depth = 0;
    UINT_32 w_data[3] = {0};    
	UINT_32 data = 0;
	flexe_mux_traffic_info_t traffic_info;
	flexe_mux_ch_info_t mux_ch_info;
	flexe_mux_ts_info_t mux_ts_info[TS_NUM];
    dev_err_t rt = DEV_DEFAULT;
	flexe_mux_info_t *flexe_mux_chip_info;
        UINT_8 ts_sort_index[TS_NUM];
        UINT_8 ts_pos = 0;
        flexe_mux_logic_phy_info_t group_phy_info[PHY_NUM];
        UINT_32 env_mode = 0;

	/* recording parameter */
	printf("%s,%d:unit=%d,ch=%d,phy_num=%d,enable=%d\r\n",__FUNCTION__,__LINE__,unit,ch,phy_num,enable);
	for (index = 0; index < phy_num; index++) 
	{
		printf("%s,%d:hard_8bit_phy_index=[%d]=%d,ts_mask[%d]=0x%x\r\n",__FUNCTION__,__LINE__,index,phy_index[index],index,ts_mask[index]);
	}

    /* check unit whether normal */
    if (unit > (DEV_NUM - 1)) 
	{
        printf("%s: invalid unit = 0x%x...\r\n", __FUNCTION__, unit);
        return DEV_PARA_ERR;
    }

	rt = mux_chip_info_get(unit, &flexe_mux_chip_info);
	if (DEV_SUCCESS != rt) 
	{
		printf("%s: unit = 0x%x, get mux chip info fail...\r\n", __FUNCTION__, unit);
		return DEV_POINTER_NULL;
	}

    /* check ch whether normal */
    if (ch > (CH_NUM - 1)) 
	{
        printf("%s: unit = 0x%x, invalid ch = 0x%x...\r\n", __FUNCTION__, unit, ch);
        return DEV_PARA_ERR;
    }
    /* check phy_num whether normal */
    if (phy_num > PHY_NUM) 
	{
        printf("%s: unit = 0x%x, invalid phy_num = 0x%x...\r\n", __FUNCTION__, unit, phy_num);
        return DEV_PARA_ERR;
    }
	/* check pointer whether normal */
	if (!ts_mask || !phy_index) 
	{
        printf("%s: invalid pointer, unit = 0x%x...\r\n", __FUNCTION__, unit);
        return DEV_POINTER_NULL;
	}
	/* check add_remove info */
	if ((TRAFFIC_REMOVE != enable) && (TRAFFIC_ADD != enable)) 
	{
		printf("%s: unit = 0x%x, invalid enable = 0x%x...\r\n", __FUNCTION__, unit, enable);
		return DEV_PARA_ERR;
	}

	flexe_mux_env_mode_get(unit, &env_mode);
	
	/* set ts_num to zero */
	ts_num = 0;
	memset(ts_sort_index, 0, sizeof(ts_sort_index));

    /* get group_logic_phy's info */
	rt = flexe_mux_logic_phy_group_info_get(unit, phy_index, phy_num, group_phy_info);
	if (DEV_SUCCESS != rt) 
	{
		printf("%s: unit = 0x%x, get group_phy_info fail, rt = %d...\r\n", __FUNCTION__, unit, rt);
		return rt;
	}

    /* flexe_100g_mode */
	if (1 == group_phy_info[0].phy_master) 
	{
		for (index = 0; index < phy_num; index++) 
		{
			if (0 == *(ts_mask + index)) 
			{
				printf("%s: unit = 0x%x, ts_mask error for flexe_100_mdoe...\r\n", __FUNCTION__, unit);
				return DEV_PARA_ERR;
			}

			for (ts_index = 0; ts_index < PHY_100G_PL; ts_index++)
			{
				if ((*(ts_mask + index) >> ts_index) & MASK_1_BIT)
				{
					if (ts_index < 5) 
					{
						ts_id[ts_num] = group_phy_info[index].phy_comb_index[0] * PHY_50G_PL + ts_index;
					}
					else if ((ts_index >= 5) && (ts_index < 10)) 
					{
						ts_id[ts_num] = group_phy_info[index].phy_comb_index[1] * PHY_50G_PL + (ts_index - 5);
					}
					else if ((ts_index >= 10) && (ts_index < 15)) 
					{
						ts_id[ts_num] = group_phy_info[index].phy_comb_index[0] * PHY_50G_PL + (ts_index - 5);
					}
					else 
					{
						ts_id[ts_num] = group_phy_info[index].phy_comb_index[1] * PHY_50G_PL + (ts_index - 10);
					}
					
					ts_num++;
				}
			}

			/* get ts_mask info */
			group_phy_info[index].phy_comb_ts_mask[0] = (*(ts_mask + index)) & MASK_5_BIT;
			group_phy_info[index].phy_comb_ts_mask[1] = (*(ts_mask + index) >> SHIFT_BIT_10) & MASK_5_BIT;
			group_phy_info[index].phy_comb_ts_mask[2] = (*(ts_mask + index) >> SHIFT_BIT_5) & MASK_5_BIT;
			group_phy_info[index].phy_comb_ts_mask[3] = (*(ts_mask + index) >> SHIFT_BIT_15) & MASK_5_BIT;
			group_phy_info[index].phy_comb_ts_num = ts_num;
		}
	}
	else if (0 == group_phy_info[0].phy_master) /* flexe_50g_mode */
	{
		for (index = 0; index < phy_num; index++) 
		{
			if (0 == *(ts_mask + index)) 
			{
				printf("%s: unit = 0x%x, ts_mask error for flexe_50_mdoe...\r\n", __FUNCTION__, unit);
				return DEV_PARA_ERR;
			}

			for (ts_index = 0; ts_index < PHY_50G_PL; ts_index++) 
			{
				if ((*(ts_mask + index) >> ts_index) & MASK_1_BIT) 
				{
					ts_id[ts_num] = group_phy_info[index].phy_index * PHY_50G_PL + ts_index;
					ts_num++;
				}
			}

			/* get ts_mask info */
			group_phy_info[index].phy_comb_ts_mask[0] = (*(ts_mask + index)) & MASK_5_BIT;
			group_phy_info[index].phy_comb_ts_mask[1] = (*(ts_mask + index) >> SHIFT_BIT_5) & MASK_5_BIT;
			group_phy_info[index].phy_comb_ts_num = ts_num;
		}
	}
	else /* phy_mode error */
	{
		printf("%s: unit = 0x%x, phy_mode error...\r\n", __FUNCTION__, unit);

		return DEV_FAIL;
	}

    /* set traffic's ts to zero */
	for (index = 0; index < TS_FIELD; index++)
	{
		traffic_info.ts[index] = 0;
	}

	traffic_info.unit = unit;
	traffic_info.ch = ch;
	traffic_info.enable = enable;
	traffic_info.group_id = group_phy_info[0].group_id;
    /* get the traffic's ts_info */
	for (index = 0; index < ts_num; index++) 
	{
		traffic_info.ts[ts_id[index] / REG_WIDTH] |= ((MASK_1_BIT << (ts_id[index] % REG_WIDTH)) & MASK_32_BIT);
		traffic_info.ts_sort[index] = ts_id[index];
	}

	/* get ts_sort index */
	for (index = 0; index < ts_num; index++) 
	{
		ts_pos = 0;
		
		for (ts_index = 0; ts_index <= ts_id[index]; ts_index++) 
		{
			if ((traffic_info.ts[ts_index / REG_WIDTH] >> (ts_index % REG_WIDTH)) & MASK_1_BIT) 
			{
				ts_pos++;
			}
		}

		if (0 == ts_pos) 
		{
			printf("%s: unit = 0x%x, ch = 0x%x, ts error...\r\n", __FUNCTION__, unit, ch);
			return DEV_PARA_ERR;
		}

		ts_sort_index[index] = ts_pos - 1;
	}

	/* cfg traffic */
	rt = flexe_mux_traffic_cfg(traffic_info);
	if (DEV_SUCCESS != rt) 
	{
		printf("%s: unit = 0x%x, traffic cfg fail...\r\n", __FUNCTION__, unit);
		return rt;
	}

    /* add traffic and cfg info */
	if (TRAFFIC_ADD == traffic_info.enable) 
	{
                UINT_8 phy_idx = 0;
                UINT_8 group_phy_record_count = 0;
                UINT_8 group_phy_record[PHY_NUM] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
                UINT_8 group_phy_index_record[PHY_NUM] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
                UINT_32 group_phy_index_sort_min = 0;
                const UINT_8 group_phy_index_sort[PHY_NUM] = {0, 4, 1, 5, 2, 6, 3, 7};
		/* get algorithm result */
		rt = flexe_mux_traffic_out(traffic_info, &mux_ch_info, mux_ts_info);
		if (DEV_SUCCESS != rt) 
		{
			printf("%s: unit = 0x%x, rt = %d, get flexe_mux_algorithm result fail...\r\n", __FUNCTION__, unit, rt);
			return rt;
		}

		/* config ts's msi */
		for (index = 0; index < ts_num; index++) 
		{
			data = (UINT_32)ch;
			flexe_mux_msi_cfg_set(unit, ts_id[index], data);
		}

		/* config ch's ts_num */
		flexe_mux_tsnum_cfg_set(unit, ch, (UINT_32)ts_num);

		/* calc blk */
		rt = flexe_mux_divisor_data_get(ts_num, TCYCLE, &blk);
		if (DEV_SUCCESS != rt) 
		{
			printf("%s: unit = 0x%x, get blk fail...\r\n", __FUNCTION__, unit);
			return rt;
		}

		/* config cycle */
		flexe_mux_cycle_cfg_set(unit, ch, (UINT_32)((ts_num / blk) - 1));

		/* config blk */
		flexe_mux_block_cfg_set(unit, ch, (UINT_32)(blk - 1));

		/* config ch_belong_flexe */
		/*flexe_mux_ch_belong_flexe_set(unit, ch, ch_belong_flexe);*/

		/*modify*/

		for (phy_idx = 0; phy_idx < PHY_NUM; phy_idx++) 
		{
			if (flexe_mux_chip_info->logic_phy[phy_idx].group_id == group_phy_info[0].group_id) 
			{
				group_phy_record[group_phy_record_count] = phy_idx;
				group_phy_record_count++;
			}
		}

		for (index = 0; index < group_phy_record_count; index++)
		{
			for (phy_idx = 0; phy_idx < PHY_NUM; phy_idx++) 
			{
				if (group_phy_index_sort[phy_idx] == group_phy_record[index]) 
				{
					group_phy_index_record[index] = phy_idx;
				}
			}
		}

		group_phy_index_sort_min = min_element(group_phy_index_record, PHY_NUM);

		/* config ch_belong_flexe */

		flexe_mux_ch_belong_flexe_set(unit, ch, group_phy_index_sort[group_phy_index_sort_min] + 1);

		
		/* config w_ram result */
		for (ts_index = 0; ts_index < ts_num; ts_index++) 
		{
			memset(w_data, 0, sizeof(w_data));
			
			for (cycle = 0; cycle < TCYCLE; cycle++) 
			{
				w_data[(cycle * SHIFT_BIT_8) / REG_WIDTH] |=
				((UINT_32)mux_ts_info[ts_sort_index[ts_index]].w_col[cycle]) << ((cycle * SHIFT_BIT_8) % REG_WIDTH);
			}

			flexe_mux_wr_cfg_ram_set(unit, (UINT_16)((ch * CH_NUM) + ts_index), w_data);
		}

		/* config r_ram result */
		for (ts_index = 0; ts_index < ts_num; ts_index++) 
		{
			if (ts_id[ts_index] < (TS_NUM / 2)) 
			{
				table_id = ts_id[ts_index] % 5;
				ram_depth = (ts_id[ts_index] / 5) * 10;

				for (index = 0; index < TCYCLE; index++) 
				{
					data = mux_ts_info[ts_sort_index[ts_index]].r_col[index];
					flexe_mux_rd_cfg_ram_set(unit, table_id, ram_depth + index, &data);
				}				 
			}
			else if ((ts_id[ts_index] >= (TS_NUM / 2)) && (ts_id[ts_index] < TS_NUM)) 
			{
				table_id = (ts_id[ts_index] % 5) + 5;
				ram_depth = ((ts_id[ts_index] - (TS_NUM / 2)) / 5) * 10;

				for (index = 0; index < TCYCLE; index++) 
				{
					data = mux_ts_info[ts_sort_index[ts_index]].r_col[index];
					flexe_mux_rd_cfg_ram_set(unit, table_id, ram_depth + index, &data);
				}
			}
			else 
			{
				printf("%s: unit = 0x%x, asic version's ts_id overstep the boundary...\r\n", __FUNCTION__, unit);
				return DEV_TS_UNAVAILABLE;
			}
		}

		/* config ts_enable */
		for (index = 0; index < ts_num; index++) 
		{
			flexe_mux_ts_enable_set(unit, ts_id[index], 1);
		}

        /* storage the flexe_100g_info */
		for (index = 0; index < phy_num; index++) 
		{
			group_phy_info[index].enable = 1;
			flexe_mux_logic_phy_info_cfg(unit, group_phy_info[index]);
		}
	}
	else /* remove traffic and cfg info */
	{
		/* config ts's msi to default value */
		for (index = 0; index < ts_num; index++) 
		{
			flexe_mux_msi_cfg_set(unit, ts_id[index], 0x7f);
		}

		/* config ch's ts_num to default value */
		flexe_mux_tsnum_cfg_set(unit, ch, 0);

		/* config cycle to default value */
		flexe_mux_cycle_cfg_set(unit, ch, 0);

		/* config blk to default value */
		flexe_mux_block_cfg_set(unit, ch, 0);




		/* config w_ram result */
		for (ts_index = 0; ts_index < ts_num; ts_index++) 
		{
			memset(w_data, 0xff, sizeof(w_data));
			flexe_mux_wr_cfg_ram_set(unit, (UINT_16)((ch * CH_NUM) + ts_index), w_data);
		}

		/* config r_ram result */
		for (ts_index = 0; ts_index < ts_num; ts_index++) 
		{
			if (ts_id[ts_index] < (TS_NUM / 2)) 
			{
				table_id = ts_id[ts_index] % 5;
				ram_depth = (ts_id[ts_index] / 5) * 10;
                data = 0xf;
				
				for (index = 0; index < TCYCLE; index++) 
				{
					flexe_mux_rd_cfg_ram_set(unit, table_id, ram_depth + index, &data);
				}				 
			}
			else if ((ts_id[ts_index] >= (TS_NUM / 2)) && (ts_id[ts_index] < TS_NUM)) 
			{
				table_id = (ts_id[ts_index] % 5) + 5;
				ram_depth = ((ts_id[ts_index] - (TS_NUM / 2)) / 5) * 10;
                data = 0xf;
				
				for (index = 0; index < TCYCLE; index++) 
				{
					flexe_mux_rd_cfg_ram_set(unit, table_id, ram_depth + index, &data);
				}
			}
			else 
			{
				printf("%s: unit = 0x%x, asic version's ts_id overstep the boundary...\r\n", __FUNCTION__, unit);
				return DEV_TS_UNAVAILABLE;
			}
		}

		/* config ts_enable */
		for (index = 0; index < ts_num; index++) 
		{
			flexe_mux_ts_enable_set(unit, ts_id[index], 0);
		}

        /* storage the flexe_100g_info */
		for (index = 0; index < phy_num; index++) 
		{
			group_phy_info[index].enable = 0;
			flexe_mux_logic_phy_info_cfg(unit, group_phy_info[index]);
		}
	}

	/* config mux_pls */
	flexe_mux_cfg_pls_set(unit);

	return DEV_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     mux's traffic_b cfg function.
*
* DESCRIPTION
*
*     config mux module's flexe_traffic
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     unit: dev's index, increased one by one
*     ch: traffic's ch
*     phy_index: payload traffic's phy_index pointer
*     phy_num: payload traffic's phy_num
*     ts_mask: each phy's payload ts
*     enable: add or remove traffic
* 
* RETURNS
*
*     0: DEV_SUCCESS
*     1: DEV_FAIL
*     2: DEV_INVALID
*     3: DEV_INITIALIZED
*     4: DEV_DEINITIALIZED
*     5: DEV_POINTER_NULL
*     6: DEV_MM_ALLOC_FAIL
*     7: DEV_PARA_ERR
*     8: DEV_CH_UNAVAILABLE
*     9: DEV_TS_UNAVAILABLE
*    10: DEV_DEFAULT
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
*	<author>		<date>			<CR_ID>		 <DESCRIPTION>
*  	liming  	  2018-07-04		 1.0        	initial
*
******************************************************************************/
dev_err_t mux_traffic_flexe_b_cfg(UINT_8 unit,
                                  UINT_8 ch,
                                  UINT_8 *phy_index,
                                  UINT_8 phy_num,
                                  UINT_32 *ts_mask,
                                  UINT_8 enable)
{
    UINT_8 index = 0;
	UINT_8 ts_index = 0;
	UINT_8 ts_num = 0;
	UINT_8 blk = 0;
	UINT_8 cycle = 0;
	UINT_8 ts_id[TS_NUM] = {0};
	UINT_8 table_id = 0;
	UINT_8 ram_depth = 0;
	UINT_32 w_data[3] = {0};
	UINT_32 data = 0;
	flexe_mux_traffic_info_t traffic_info;
	flexe_mux_ch_info_t mux_ch_info;
	flexe_mux_ts_info_t mux_ts_info[TS_NUM];
    dev_err_t rt = DEV_DEFAULT;
        UINT_8 ts_sort_index[TS_NUM];
        UINT_8 ts_pos = 0;
        flexe_mux_logic_phy_info_t group_phy_info[PHY_NUM];
        UINT_32 env_mode = 0;
	flexe_mux_info_t *flexe_mux_chip_info;

	/* recording parameter */
	printf("%s,%d:unit=%d,ch=%d,phy_num=%d,enable=%d\r\n",__FUNCTION__,__LINE__,unit,ch,phy_num,enable);
	for (index = 0; index < phy_num; index++) 
	{
		printf("%s,%d:hard_8bit_phy_index=[%d]=%d,ts_mask[%d]=0x%x\r\n",__FUNCTION__,__LINE__,index,phy_index[index],index,ts_mask[index]);
	}

    /* check unit whether normal */
    if (unit > (DEV_NUM - 1)) 
	{
        printf("%s: invalid unit = 0x%x...\r\n", __FUNCTION__, unit);
        return DEV_PARA_ERR;
    }
	rt = mux_chip_info_get(unit, &flexe_mux_chip_info);
	if (DEV_SUCCESS != rt) 
	{
		printf("%s: unit = 0x%x, get mux chip info fail...\r\n", __FUNCTION__, unit);
		return DEV_POINTER_NULL;
	}
    /* check ch whether normal */
    if (ch > (CH_NUM - 1)) 
	{
        printf("%s: unit = 0x%x, invalid ch = 0x%x...\r\n", __FUNCTION__, unit, ch);
        return DEV_PARA_ERR;
    }

    /* check phy_num whether normal */
    if (phy_num > PHY_NUM) 
	{
        printf("%s: unit = 0x%x, invalid phy_num = 0x%x...\r\n", __FUNCTION__, unit, phy_num);
        return DEV_PARA_ERR;
    }
	/* check pointer whether normal */
	if (!ts_mask || !phy_index) 
	{
        printf("%s: invalid pointer, unit = 0x%x...\r\n", __FUNCTION__, unit);
        return DEV_POINTER_NULL;
	}
	/* check add_remove info */
	if ((TRAFFIC_REMOVE != enable) && (TRAFFIC_ADD != enable)) 
	{
		printf("%s: unit = 0x%x, invalid enable = 0x%x...\r\n", __FUNCTION__, unit, enable);
		return DEV_PARA_ERR;
	}

    flexe_mux_env_mode_get(unit, &env_mode);
    
    /* set ts_num to zero */
    ts_num = 0;
    memset(ts_sort_index, 0, sizeof(ts_sort_index));

    /* get group_logic_phy's info */
    rt = flexe_mux_logic_phy_group_info_get(unit, phy_index, phy_num, group_phy_info);

    if (DEV_SUCCESS != rt) {
        printf("%s: unit = 0x%x, get group_phy_info fail, rt = %d...\r\n", __FUNCTION__, unit, rt);

        return rt;
    }

    /* flexe_100g_mode */
    if (1 == group_phy_info[0].phy_master) {
        for (index = 0; index < phy_num; index++) {
            if (0 == *(ts_mask + index)) {
                printf("%s: unit = 0x%x, ts_mask error for flexe_100_mdoe...\r\n", __FUNCTION__, unit);

                return DEV_PARA_ERR;
            }

            for (ts_index = 0; ts_index < PHY_100G_PL; ts_index++) {
                if ((*(ts_mask + index) >> ts_index) & MASK_1_BIT) {
                    if (ts_index < 5) {
                        ts_id[ts_num] = group_phy_info[index].phy_comb_index[0] * PHY_50G_PL + ts_index;
                    }
                    else if ((ts_index >= 5) && (ts_index < 10)) {
                        ts_id[ts_num] = group_phy_info[index].phy_comb_index[1] * PHY_50G_PL + (ts_index - 5);
                    }
                    else if ((ts_index >= 10) && (ts_index < 15)) {
                        ts_id[ts_num] = group_phy_info[index].phy_comb_index[0] * PHY_50G_PL + (ts_index - 5);
                    }
                    else {
                        ts_id[ts_num] = group_phy_info[index].phy_comb_index[1] * PHY_50G_PL + (ts_index - 10);
                    }
                    
                    ts_num++;
                }
            }

            /* get ts_mask info */
            group_phy_info[index].phy_comb_ts_mask[0] = (*(ts_mask + index)) & MASK_5_BIT;
            group_phy_info[index].phy_comb_ts_mask[1] = (*(ts_mask + index) >> SHIFT_BIT_10) & MASK_5_BIT;
            group_phy_info[index].phy_comb_ts_mask[2] = (*(ts_mask + index) >> SHIFT_BIT_5) & MASK_5_BIT;
            group_phy_info[index].phy_comb_ts_mask[3] = (*(ts_mask + index) >> SHIFT_BIT_15) & MASK_5_BIT;
            group_phy_info[index].phy_comb_ts_num = ts_num;
        }
    }
    /* flexe_50g_mode */
    else if (0 == group_phy_info[0].phy_master) {
        for (index = 0; index < phy_num; index++) {
            if (0 == *(ts_mask + index)) {
                printf("%s: unit = 0x%x, ts_mask error for flexe_50_mdoe...\r\n", __FUNCTION__, unit);

                return DEV_PARA_ERR;
            }

            for (ts_index = 0; ts_index < PHY_50G_PL; ts_index++) {
                if ((*(ts_mask + index) >> ts_index) & MASK_1_BIT) {
                    ts_id[ts_num] = group_phy_info[index].phy_index * PHY_50G_PL + ts_index;
                    ts_num++;
                }
            }

            /* get ts_mask info */
            group_phy_info[index].phy_comb_ts_mask[0] = (*(ts_mask + index)) & MASK_5_BIT;
            group_phy_info[index].phy_comb_ts_mask[1] = (*(ts_mask + index) >> SHIFT_BIT_5) & MASK_5_BIT;
            group_phy_info[index].phy_comb_ts_num = ts_num;
		}
	}
	/* phy_mode error */
	else {
		printf("%s: unit = 0x%x, phy_mode error...\r\n", __FUNCTION__, unit);
		return DEV_FAIL;
	}

    /* set traffic's ts to zero */
	for (index = 0; index < TS_FIELD; index++)
		traffic_info.ts[index] = 0;

	traffic_info.unit = unit;
	traffic_info.ch = ch;
	traffic_info.enable = enable;

    /* get the traffic's ts_info */
	for (index = 0; index < ts_num; index++) 
	{
		traffic_info.ts[ts_id[index] / REG_WIDTH] |= ((MASK_1_BIT << (ts_id[index] % REG_WIDTH)) & MASK_32_BIT);
		traffic_info.ts_sort[index] = ts_id[index];
	}

	/* get ts_sort index */
	for (index = 0; index < ts_num; index++) 
	{
		ts_pos = 0;
		
		for (ts_index = 0; ts_index <= ts_id[index]; ts_index++) 
		{
			if ((traffic_info.ts[ts_index / REG_WIDTH] >> (ts_index % REG_WIDTH)) & MASK_1_BIT) 
			{
				ts_pos++;
			}
		}

		if (0 == ts_pos) 
		{
			printf("%s: unit = 0x%x, ch = 0x%x, ts error...\r\n", __FUNCTION__, unit, ch);
			return DEV_PARA_ERR;
		}

		ts_sort_index[index] = ts_pos - 1;
	}

	/* cfg traffic */
        /* coverity[uninit_use_in_call] */
	rt = flexe_mux_traffic_b_cfg(traffic_info);
	if (DEV_SUCCESS != rt) 
	{
		printf("%s: unit = 0x%x, traffic cfg fail...\r\n", __FUNCTION__, unit);
		return rt;
	}

    /* add traffic and cfg info */
	if (TRAFFIC_ADD == traffic_info.enable) 
	{
                UINT_8 phy_idx = 0;
                UINT_8 group_phy_record_count = 0;
                UINT_8 group_phy_record[PHY_NUM] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
                UINT_8 group_phy_index_record[PHY_NUM] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
                UINT_32 group_phy_index_sort_min = 0;
                const UINT_8 group_phy_index_sort[PHY_NUM] = {0, 4, 1, 5, 2, 6, 3, 7};
		/* get algorithm result */
		rt = flexe_mux_traffic_b_out(traffic_info, &mux_ch_info, mux_ts_info);
		if (DEV_SUCCESS != rt) 
		{
			printf("%s: unit = 0x%x, rt = %d, get flexe_mux_algorithm result fail...\r\n", __FUNCTION__, unit, rt);
			return rt;
		}

		/* config ts's msi */
		for (index = 0; index < ts_num; index++) 
		{
			data = (UINT_32)ch;
			flexe_mux_msi_cfg_b_set(unit, ts_id[index], data);
		}

		/* config ch's ts_num */
		flexe_mux_tsnum_cfg_b_set(unit, ch, (UINT_32)ts_num);

		/* calc blk */
		rt = flexe_mux_divisor_data_get(ts_num, TCYCLE, &blk);
		if (DEV_SUCCESS != rt) 
		{
			printf("%s: unit = 0x%x, get blk fail...\r\n", __FUNCTION__, unit);
			return rt;
		}

		/* config cycle */
		flexe_mux_cycle_cfg_b_set(unit, ch, (UINT_32)((ts_num / blk) - 1));

		/* config blk */
		flexe_mux_block_cfg_b_set(unit, ch, (UINT_32)(blk - 1));

		/* config ch_belong_flexe */
		/*flexe_mux_ch_belong_flexe_set(unit, ch, ch_belong_flexe);*/

		/*modify*/

		for (phy_idx = 0; phy_idx < PHY_NUM; phy_idx++) 
		{
			if (flexe_mux_chip_info->logic_phy[phy_idx].group_id == group_phy_info[0].group_id) 
			{
				group_phy_record[group_phy_record_count] = phy_idx;
				group_phy_record_count++;
			}
		}

		for (index = 0; index < group_phy_record_count; index++)
		{
			for (phy_idx = 0; phy_idx < PHY_NUM; phy_idx++) 
			{
				if (group_phy_index_sort[phy_idx] == group_phy_record[index]) 
				{
					group_phy_index_record[index] = phy_idx;
				}
			}
		}

		group_phy_index_sort_min = min_element(group_phy_index_record, PHY_NUM);

		/* config ch_belong_flexe */

		flexe_mux_ch_belong_flexe_set(unit, ch, group_phy_index_sort[group_phy_index_sort_min] + 1);


		/* config w_ram result */
		for (ts_index = 0; ts_index < ts_num; ts_index++) 
		{
			memset(w_data, 0, sizeof(w_data));
			
			for (cycle = 0; cycle < TCYCLE; cycle++) 
			{
				w_data[(cycle * SHIFT_BIT_8) / REG_WIDTH] |=
				((UINT_32)mux_ts_info[ts_sort_index[ts_index]].w_col[cycle]) << ((cycle * SHIFT_BIT_8) % REG_WIDTH);
			}

			flexe_mux_wr_cfg_b_ram_set(unit, (UINT_16)((ch * CH_NUM) + ts_index), w_data);
		}

		/* config r_ram result */
		for (ts_index = 0; ts_index < ts_num; ts_index++) 
		{
			if (ts_id[ts_index] < (TS_NUM / 2)) 
			{
				table_id = ts_id[ts_index] % 5;
				ram_depth = (ts_id[ts_index] / 5) * 10;

				for (index = 0; index < TCYCLE; index++) 
				{
					data = mux_ts_info[ts_sort_index[ts_index]].r_col[index];

					flexe_mux_rd_cfg_b_ram_set(unit, table_id, ram_depth + index, &data);
				}				 
			}
			else if ((ts_id[ts_index] >= (TS_NUM / 2)) && (ts_id[ts_index] < TS_NUM)) 
			{
				table_id = (ts_id[ts_index] % 5) + 5;
				ram_depth = ((ts_id[ts_index] - (TS_NUM / 2)) / 5) * 10;

				for (index = 0; index < TCYCLE; index++) 
				{
					data = mux_ts_info[ts_sort_index[ts_index]].r_col[index];
					flexe_mux_rd_cfg_b_ram_set(unit, table_id, ram_depth + index, &data);
				}
			}
			else 
			{
				printf("%s: unit = 0x%x, asic version's ts_id overstep the boundary...\r\n", __FUNCTION__, unit);
				return DEV_TS_UNAVAILABLE;
			}
		}

		/* config ts_enable */
		for (index = 0; index < ts_num; index++) 
		{
			flexe_mux_ts_enable_b_set(unit, ts_id[index], 1);
		}

        #if 0
        /* storage the flexe_100g_info */
		for (index = 0; index < phy_num; index++) {
			group_phy_info[index].enable = 1;
			
			flexe_mux_logic_phy_info_cfg(unit, group_phy_info[index]);
		}
		#endif
	}
	else /* remove traffic and cfg info */
	{
		/* config ts's msi to default value */
		for (index = 0; index < ts_num; index++) 
		{
			flexe_mux_msi_cfg_b_set(unit, ts_id[index], 0x7f);
		}

		/* config ch's ts_num to default value */
		flexe_mux_tsnum_cfg_b_set(unit, ch, 0);

		/* config cycle to default value */
		flexe_mux_cycle_cfg_b_set(unit, ch, 0);

		/* config blk to default value */
		flexe_mux_block_cfg_b_set(unit, ch, 0);

		/* config ch_belong_flexe to default value */
	
		
		/* config w_ram result */
		for (ts_index = 0; ts_index < ts_num; ts_index++) 
		{
			memset(w_data, 0xff, sizeof(w_data));
			flexe_mux_wr_cfg_b_ram_set(unit, (UINT_16)((ch * CH_NUM) + ts_index), w_data);
		}

		/* config r_ram result */
		for (ts_index = 0; ts_index < ts_num; ts_index++) 
		{
			if (ts_id[ts_index] < (TS_NUM / 2)) 
			{
				table_id = ts_id[ts_index] % 5;
				ram_depth = (ts_id[ts_index] / 5) * 10;
                data = 0xf;
				
				for (index = 0; index < TCYCLE; index++) 
				{
					flexe_mux_rd_cfg_b_ram_set(unit, table_id, ram_depth + index, &data);
				}				 
			}
			else if ((ts_id[ts_index] >= (TS_NUM / 2)) && (ts_id[ts_index] < TS_NUM)) 
			{
				table_id = (ts_id[ts_index] % 5) + 5;
				ram_depth = ((ts_id[ts_index] - (TS_NUM / 2)) / 5) * 10;
                data = 0xf;
				
				for (index = 0; index < TCYCLE; index++) 
				{
					flexe_mux_rd_cfg_b_ram_set(unit, table_id, ram_depth + index, &data);
				}
			}
			else 
			{
				printf("%s: unit = 0x%x, asic version's ts_id overstep the boundary...\r\n", __FUNCTION__, unit);
				return DEV_TS_UNAVAILABLE;
			}
		}

		/* config ts_enable */
		for (index = 0; index < ts_num; index++) 
		{
			flexe_mux_ts_enable_b_set(unit, ts_id[index], 0);
		}

        #if 0
        /* storage the flexe_100g_info */
		for (index = 0; index < phy_num; index++) {
			group_phy_info[index].enable = 0;
			
			flexe_mux_logic_phy_info_cfg(unit, group_phy_info[index]);
		}
		#endif
	}

	/* config mux_pls */
	flexe_mux_cfg_pls_set(unit);


	return DEV_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     mux's flexe_traffic config debug.
*
* DESCRIPTION
*
*     RESERVED
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     RESERVED
* 
* RETURNS
*
*     0: DEV_SUCCESS
*     1: DEV_FAIL
*     2: DEV_INVALID
*     3: DEV_INITIALIZED
*     4: DEV_DEINITIALIZED
*     5: DEV_POINTER_NULL
*     6: DEV_MM_ALLOC_FAIL
*     7: DEV_PARA_ERR
*     8: DEV_CH_UNAVAILABLE
*     9: DEV_TS_UNAVAILABLE
*    10: DEV_DEFAULT
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
*    <author>        <date>            <CR_ID>         <DESCRIPTION>
*      liming        2018-03-24         1.0            initial
*
******************************************************************************/
dev_err_t mux_traffic_flexe_cfg_debug(char *string)
{
	char *argv[600 + 1];
	UINT_8 argc = 0;
	UINT_8 para_index = 0;
	unsigned int unit = 0;
	unsigned int ch = 0;
	UINT_32 phy_index[PHY_NUM] = {0};
	UINT_8 phy_index_cov[PHY_NUM] = {0};
	UINT_32 ts_mask[PHY_NUM] = {0};
	unsigned int phy_num = 0;
	unsigned int enable = 0;
	dev_err_t rt = DEV_DEFAULT;

	if (!string) 
	{
		printf("%s: pointer is null...\r\n", __FUNCTION__);
		return DEV_POINTER_NULL;
	}

	argc = parse_line(string, argv);

	if (argc > 20 || (0 != argc % 2)) 
	{
		printf("%s: invalid para numbers, para_number = 0x%x...\r\n", __FUNCTION__, argc);
		return DEV_PARA_ERR;
	}

	/* get parameter */
	for (para_index = 0; para_index < argc; para_index++) 
	{
		
		if (0 == para_index) /* unit */
		{
			sscanf(argv[para_index], "0x%x", &unit);
		}
		else if (1 == para_index) /* ch */
		{
			sscanf(argv[para_index], "0x%x", &ch);
		}
		else if ((para_index >= 2) && (para_index < ((argc - 4) / 2) + 2)) /* phy_index */
		{
			sscanf(argv[para_index], "0x%x", &phy_index[para_index - 2]);

            /* coverity[overrun-local : FALSE] */
			phy_index_cov[para_index - 2] = (UINT_8)phy_index[para_index - 2];
		}
		else 
		{
			if ((((argc - 4) / 2) + 2) == para_index) /* phy_num */
			{
				sscanf(argv[para_index], "0x%x", &phy_num);
			}
			else if ((argc - 1) == para_index) /* enable */
			{
				sscanf(argv[para_index], "0x%x", &enable);
			}
			else /* ts_mask */
			{
				sscanf(argv[para_index], "0x%x", &ts_mask[para_index - (((argc - 4) / 2) + 3)]);
			}
		}
	}

	/* config flexe_traffic */
	rt = mux_traffic_flexe_cfg(unit, ch, phy_index_cov, phy_num, ts_mask, enable);

	if (DEV_SUCCESS == rt) {
		printf("%s: unit = 0x%x, ch = 0x%x's traffic cfg success...\r\n", __FUNCTION__, unit, ch);
	}
	else {
		printf("%s: unit = 0x%x, ch = 0x%x's traffic cfg fail...\r\n", __FUNCTION__, unit, ch);
	}

	return rt;
}

dev_err_t mux_traffic_flexe_b_cfg_debug(char *string) /* "unit/ch/phy_index.../phy_num/ts_mask.../enable", 1610*/
{
	char *argv[600 + 1];
	UINT_8 argc = 0;
	UINT_8 para_index = 0;
	unsigned int unit = 0;
	unsigned int ch = 0;
	UINT_32 phy_index[PHY_NUM] = {0};
	UINT_8 phy_index_cov[PHY_NUM] = {0};
	UINT_32 ts_mask[PHY_NUM] = {0};
	unsigned int phy_num = 0;
	unsigned int enable = 0;
	dev_err_t rt = DEV_DEFAULT;

	if (!string) {
		printf("%s: pointer is null...\r\n", __FUNCTION__);
		return DEV_POINTER_NULL;
	}

	argc = parse_line(string, argv);

	if (argc > 20 || (0 != argc % 2)) 
	{
		printf("%s: invalid para numbers, para_number = 0x%x...\r\n", __FUNCTION__, argc);
		return DEV_PARA_ERR;
	}

	/* get parameter */
	for (para_index = 0; para_index < argc; para_index++) 
	{
		
		if (0 == para_index) /* unit */
		{
			sscanf(argv[para_index], "0x%x", &unit);
		}
		else if (1 == para_index) /* ch */
		{
			sscanf(argv[para_index], "0x%x", &ch);
		}
		else if ((para_index >= 2) && (para_index < ((argc - 4) / 2) + 2)) /* phy_index */
		{
			sscanf(argv[para_index], "0x%x", &phy_index[para_index - 2]);
            /* coverity[overrun-local : FALSE] */
			phy_index_cov[para_index - 2] = (UINT_8)phy_index[para_index - 2];
		}
		else 
		{
			if ((((argc - 4) / 2) + 2) == para_index) /* phy_num */
			{
				sscanf(argv[para_index], "0x%x", &phy_num);
			}
			else if ((argc - 1) == para_index) /* enable */
			{
				sscanf(argv[para_index], "0x%x", &enable);
			}
			else  /* ts_mask */
			{
				sscanf(argv[para_index], "0x%x", &ts_mask[para_index - (((argc - 4) / 2) + 3)]);
			}
		}
	}

	/* config flexe_traffic */
	rt = mux_traffic_flexe_b_cfg((UINT_8)unit, (UINT_8)ch, phy_index_cov, (UINT_8)phy_num, ts_mask, (UINT_8)enable);

	if (DEV_SUCCESS == rt) {
		printf("%s: unit = 0x%x, ch = 0x%x's traffic cfg b success...\r\n", __FUNCTION__, unit, ch);
	}
	else {
		printf("%s: unit = 0x%x, ch = 0x%x's traffic cfg b fail...\r\n", __FUNCTION__, unit, ch);
	}

	return rt;
}

dev_err_t mux_phy_xc_cfg(UINT_8 unit,
                         UINT_8 phy_index, /*flexe phy inst process index*/
                         UINT_8 phy_out_index) /*pcs index*/
{
	dev_err_t rt = DEV_DEFAULT;

	return rt;
}

dev_err_t flexe_default_config(UINT_8 unit,UINT_8 instance_id,UINT_8 mode)
{
	regp_write(unit,0xc202,0x0,0x84218421);
	if (3 == mode) 
	{
		regp_write(unit,0x22,0x0,0x1111);
		
		flexe_mux_flexe_enable_set(unit,0,1);
		flexe_mux_flexe_enable_set(unit,1,1);
		flexe_mux_flexe_enable_set(unit,2,1);
		flexe_mux_flexe_enable_set(unit,3,1);
		flexe_mux_flexe_enable_set(unit,4,1);
		flexe_mux_flexe_enable_set(unit,5,1);
		flexe_mux_flexe_enable_set(unit,6,1);
		flexe_mux_flexe_enable_set(unit,7,1);
		regp_write(unit,0xc200,0,0xaa55);
		regp_write(unit,0xc201,0x0,0x11111111);
		regp_write(unit,0xc2c1,0x0,0x76547654);
						
		regp_bit_write(unit,0x18000,0,3+instance_id*4,instance_id*4,instance_id);
		regp_bit_write(unit,0x18080,0xe,3+instance_id*4,instance_id*4,instance_id);
		regp_bit_write(unit,0x18000,0xa,2+instance_id*3,instance_id*3,instance_id);
		regp_bit_write(unit,0x18000,0xa,2+(instance_id+1)*3,(instance_id+1)*3,instance_id);
		regp_bit_write(unit,0x18000,0xa,2+(instance_id+2)*3,(instance_id+2)*3,instance_id);
		regp_bit_write(unit,0x18000,0xa,2+(instance_id+3)*3,(instance_id+3)*3,instance_id);
		/* config flexe env cfg */
		flexe_env_cfg(unit,1,instance_id,3);

		/* config flexe tx mode */
		oh_tx_mode_cfg_set(unit,instance_id,0,3);
		oh_tx_mode_cfg_set(unit,instance_id+1,0,3);
		oh_tx_mode_cfg_set(unit,instance_id+2,0,3);
		oh_tx_mode_cfg_set(unit,instance_id+3,0,3);

		/* config flexe rx mode */
		oh_rx_flexe_rx_mode_set(unit,instance_id,0,3);
		oh_rx_flexe_rx_mode_set(unit,instance_id+1,0,3);
		oh_rx_flexe_rx_mode_set(unit,instance_id+2,0,3);
		oh_rx_flexe_rx_mode_set(unit,instance_id+3,0,3);
		
		regp_write(unit,0xc26a,0x0,0x1);
		
		inf_ch_adp_rx_mode_cfg_400g_set(unit,1);
		inf_ch_adp_tx_mode_cfg_set(unit,0,1);
		inf_ch_adp_rx_ts_delta_set(unit,0,instance_id,2);
        inf_ch_adp_rx_ts_delta_set(unit,1,instance_id,2);
	}
	else if (2 == mode) 
	{
		regp_write(unit,0x22,0x0,0x550011);
		
		flexe_mux_flexe_enable_set(unit,instance_id,1);
		flexe_mux_flexe_enable_set(unit,instance_id+1,1);
		flexe_mux_flexe_enable_set(unit,instance_id+2,1);
		flexe_mux_flexe_enable_set(unit,instance_id+3,1);
		
		if(instance_id == 0)
		{
			regp_bit_write(unit,0xc200,0x0,7,0,0xa5);
			regp_bit_write(unit,0xc201,0x0,15,0,0x1111);
			regp_bit_write(unit,0xc2c1,0x0,7,0,0x32);
			
			inf_ch_adp_rx_mode_cfg_400g_set(unit,0);
			inf_ch_adp_rx_mode_cfg_200g_set(unit,0,1);

			inf_ch_adp_tx_mode_cfg_set(unit,0,0);
			inf_ch_adp_tx_mode_cfg_bypass_set(unit,0,0);
			inf_ch_adp_tx_mode_cfg_200g_set(unit,0,1);
		}
		else if(instance_id == 4)
		{
			regp_bit_write(unit,0xc200,0x0,15,8,0xa5);
			regp_bit_write(unit,0xc201,0x0,31,16,0x5555);
			regp_bit_write(unit,0xc2c1,0x0,23,16,0x76);
			
			inf_ch_adp_rx_mode_cfg_400g_set(unit,0);
			inf_ch_adp_rx_mode_cfg_200g_set(unit,2,1);
			inf_ch_adp_rx_mode_cfg_set(unit,3,0);

			inf_ch_adp_tx_mode_cfg_set(unit,0,0);
			inf_ch_adp_tx_mode_cfg_bypass_set(unit,4,0);
			inf_ch_adp_tx_mode_cfg_200g_set(unit,2,1);
			inf_ch_adp_tx_mode_cfg_set(unit,4,0);
		}
		
						
		regp_bit_write(unit,0x18000,0,3+instance_id*4,instance_id*4,instance_id);
		regp_bit_write(unit,0x18080,0xe,3+instance_id*4,instance_id*4,instance_id);
		regp_bit_write(unit,0x18000,0xa,2+instance_id*3,instance_id*3,instance_id);
		regp_bit_write(unit,0x18000,0xa,2+(instance_id+1)*3,(instance_id+1)*3,instance_id);
		/* config flexe env cfg */
		flexe_env_cfg(unit,1,instance_id,2);

		/* config flexe tx mode */
		oh_tx_mode_cfg_set(unit,instance_id,0,2);
		oh_tx_mode_cfg_set(unit,instance_id+1,0,2);

		/* config flexe rx mode */
		oh_rx_flexe_rx_mode_set(unit,instance_id,0,2);
		oh_rx_flexe_rx_mode_set(unit,instance_id+1,0,2);
		
		regp_write(unit,0xc26a,0x0,0x1);
		inf_ch_adp_rx_ts_delta_set(unit,0,instance_id,5);
        inf_ch_adp_rx_ts_delta_set(unit,1,instance_id,5);
	}
	else if (1 == mode) 
	{
		flexe_mux_flexe_enable_set(unit,instance_id,1);
		flexe_mux_flexe_enable_set(unit,instance_id+1,1);
		regp_bit_write(unit,0xc200,0,3+instance_id*2,instance_id*2,9);
		regp_bit_write(unit,0xc201,0x0,3+instance_id*4,instance_id*4,instance_id+1);
		regp_bit_write(unit,0xc201,0x0,7+instance_id*4,instance_id*4+4,instance_id+1);
		regp_bit_write(unit,0xc2c1,0x0,3+instance_id*4,instance_id*4,instance_id+1);
		regp_bit_write(unit,0xc2c1,0x0,7+instance_id*4,instance_id*4+4,instance_id+1);
						
		regp_bit_write(unit,0x18000,0,3+instance_id*4,instance_id*4,instance_id);
		regp_bit_write(unit,0x18080,0xe,3+instance_id*4,instance_id*4,instance_id);
		regp_bit_write(unit,0x18000,0xa,2+instance_id*3,instance_id*3,instance_id);
		/* config flexe env cfg */
		flexe_env_cfg(unit,1,instance_id,1);

		/* config flexe tx mode */
		oh_tx_mode_cfg_set(unit,instance_id,0,1);

		/* config flexe rx mode */
		oh_rx_flexe_rx_mode_set(unit,instance_id,0,1);
		
		regp_write(unit,0xc26a,0x0,0x1);
		inf_ch_adp_rx_ts_delta_set(unit,0,instance_id,10);
        inf_ch_adp_rx_ts_delta_set(unit,1,instance_id,10);
	}
	else
	{
		flexe_mux_flexe_enable_set(unit,instance_id,1);
		regp_write(unit,0x22,0x0,0x87654321);
		regp_bit_write(unit,0xc200,0x0,1+instance_id*2,instance_id*2,0);
		regp_bit_write(unit,0xc201,0x0,3+instance_id*4,instance_id*4,instance_id+1);
		regp_bit_write(unit,0xc2c1,0x0,3+instance_id*4,instance_id*4,instance_id);
		
		regp_bit_write(unit,0x18000,0,3+instance_id*4,instance_id*4,instance_id);
		regp_bit_write(unit,0x18080,0xe,3+instance_id*4,instance_id*4,instance_id);
		regp_bit_write(unit,0x18000,0xa,2+instance_id*3,instance_id*3,instance_id);
		
		
		/* config flexe env cfg */
		flexe_env_cfg(unit,1,instance_id,0);
			
		/* config flexe tx mode */
		oh_tx_mode_cfg_set(unit,instance_id,0,0);
			
		/* config flexe rx mode */
		oh_rx_flexe_rx_mode_set(unit,instance_id,0,0);
		

		regp_write(unit,0xc26a,0x0,0x1);
		inf_ch_adp_rx_ts_delta_set(unit,0,instance_id,20);
        inf_ch_adp_rx_ts_delta_set(unit,1,instance_id,20);
	}	
	
	return DEV_SUCCESS;
}
