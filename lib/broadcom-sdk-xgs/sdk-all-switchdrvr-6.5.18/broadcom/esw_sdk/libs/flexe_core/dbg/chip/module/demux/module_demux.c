

/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "flexe_demux.h"
#include "module_demux.h"
#include "rateadpt.h"
#include "sal.h"
#include "module_mux.h"
#include "flexe_oam_rx.h"
/******************************************************************************
*
* FUNCTION
*
*     demux's hardware init.
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
*      liming        2018-02-08         1.0            initial
*
******************************************************************************/
dev_err_t demux_hw_init(UINT_8 unit)
{
    UINT_8 index = 0;
    UINT_8 table_id = 0;
    UINT_8 channel_id = 0;
    UINT_8 ts_id = 0;
    UINT_32 data = 0;
    UINT_32 r_data[3] = {0};
    
    /* check unit whether normal */
    if (unit > (DEV_NUM - 1)) {
        printf("%s: invalid unit = 0x%x...\r\n", __FUNCTION__, unit);
        
        return DEV_PARA_ERR;
    }

    #ifndef SV_TEST
    /* set flexe_group_phy_info to default_value */
    for (index = 0; index < PHY_NUM; index++) {
        flexe_demux_group_info_set(unit, index, 0xf);
    }
    
    /* set w_cfg_ram to default value */
    data = 0xf;
    
    for (table_id = 0; table_id < 10; table_id++) {
        for (channel_id = 0; channel_id < CH_NUM; channel_id++) {
            flexe_demux_wcfgram_set(unit, table_id, channel_id, &data);
            flexe_demux_wcfgram_b_set(unit, table_id, channel_id, &data);
        }
    }

    /* set r_cfg_ram to default value */
    memset(r_data, 0xff, sizeof(r_data));

    for (channel_id = 0; channel_id < CH_NUM; channel_id++) {
        for (ts_id = 0; ts_id < TS_NUM; ts_id++) {
            flexe_demux_rdrule_set(unit, channel_id * TS_NUM + ts_id, r_data);
            flexe_demux_rdrule_b_set(unit, channel_id * TS_NUM + ts_id, r_data);
        }
    }

    /* set ch_property to default_value */
    data = 0xfffff;
    
    for (channel_id = 0; channel_id < CH_NUM; channel_id++) {
        /** coverity[callee_ptr_arith] */
        flexe_demux_property_set(unit, channel_id, &data);
        /** coverity[callee_ptr_arith] */
        flexe_demux_property_b_set(unit, channel_id, &data);
    }

	/*added by mjx according to liufei*/
	flexe_demux_autoset_en_set(unit,0x40000001);
    #endif

    /* set flexe_demux's group_cfg to invalid value */
    for (index = 0; index < PHY_NUM; index++) {
        flexe_demux_group_cfg_set(unit, index, 0xf);

        flexe_demux_reorder_asic_set(unit, index, 0xf0);
    }

    return DEV_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     demux's init.
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
*      liming        2018-01-03         1.0            initial
*
******************************************************************************/
dev_err_t demux_init(UINT_8 unit)
{
    dev_err_t rt = DEV_DEFAULT;

    /* flexe_demux's init */
    rt = flexe_demux_init(unit);

    if (DEV_SUCCESS != rt) {
        printf("%s: unit = 0x%x, flexe_demux init fail...\r\n", __FUNCTION__, unit);

        return rt;
    }

    /* demux's hardware init */
    rt = demux_hw_init(unit);

    if (DEV_SUCCESS != rt) {
        printf("%s: unit = 0x%x, flexe_demux hw_init fail...\r\n", __FUNCTION__, unit);

        return rt;
    }

    flexe_demux_enable_ab_set(unit, 1);

    return DEV_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     demux's uninit.
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
*      liming        2018-01-03         1.0            initial
*
******************************************************************************/
dev_err_t demux_uninit(UINT_8 unit)
{
    dev_err_t rt = DEV_DEFAULT;

    /* demux's uninit */
    rt = flexe_demux_uninit(unit);

    if (DEV_SUCCESS != rt) {
        printf("%s: unit = 0x%x, flexe_demux uninit fail...\r\n", __FUNCTION__, unit);

        return rt;
    }

    return DEV_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     demux debug function.
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
dev_err_t demux_debug(void)
{
    printf("+----------------------demux's initialized/uninit function---------------------------+\r\n");
    printf("demux_init(UINT_8 unit)\r\n");
    printf("demux_uninit(UINT_8 unit)\r\n");
    printf("\r\n");

    printf("parameter description:\r\n");
    printf("unit: dev'index, default start from zero, and increased one by one.\r\n");
    printf("+------------------------------------------------------------------------------------+\r\n");
    printf("\r\n\r\n");

    printf("parameter description:\r\n");
    printf("unit: dev'index, default start from zero, and increased one by one.\r\n");
    printf("phy_mode: bit0~bit7 to phy0~phy7, 0 is 50G, 1 is 100G.\r\n");
    printf("phy_id_l: phy0~phy3's phy_id.\r\n");
    printf("phy_id_h: phy4~phy7's phy_id.\r\n");
    printf("phy_map_l: phy0~phy3's phy_map.\r\n");
    printf("phy_map_h: phy4~phy7's phy_map.\r\n");
    printf("+----------------------------------------------------------------------------------------------------------------------------+\r\n");
    printf("\r\n\r\n");

    printf("+----------------------demux's traffic_cfg function----------------------------------------------------+\r\n");
    printf("demux_traffic_cfg_test(UINT_8 unit, UINT_8 ch, UINT_32 ts_mask_0....ts_mask_7, UINT_8 enable)\r\n");
    printf("\r\n");

    printf("parameter description:\r\n");
    printf("unit: dev'index, default start from zero, and increased one by one.\r\n");
    printf("ch: channel_id, range is 0~79.\r\n");
    printf("ts_mask_0: phy_0's payload ts.\r\n");
    printf("ts_mask_1: phy_1's payload ts.\r\n");
    printf("ts_mask_2: phy_2's payload ts.\r\n");
    printf("ts_mask_3: phy_3's payload ts.\r\n");
    printf("ts_mask_4: phy_4's payload ts.\r\n");
    printf("ts_mask_5: phy_5's payload ts.\r\n");
    printf("ts_mask_6: phy_6's payload ts.\r\n");
    printf("ts_mask_7: phy_7's payload ts.\r\n");
    printf("enable: 0 is remove, 1 is add.\r\n");
    printf("+-----------------------------------------------------------------------------------------------------+\r\n");
    printf("\r\n\r\n");

    printf("+----------------------demux's group_phy_cfg function----------------------------------------------------+\r\n");
    printf("demux_phy_group_cfg_debug(char *string)\r\n");
    printf("\r\n");

    printf("parameter description:\r\n");
    printf("unit: dev'index, default start from zero, and increased one by one.\r\n");
    printf("group_id: phy's group_id.\r\n");
    printf("phy_index: phy's index, phy1->bit0...phy8->bit7.\r\n");
    printf("*phy_id: phy's id pointer.\r\n");
    printf("phy_mode: phy's working state, 0->flexe_50G, 1->flexe_100G.\r\n");
    printf("enable: 0 is remove, 1 is add.\r\n");
    printf("+-----------------------------------------------------------------------------------------------------+\r\n");
    printf("\r\n\r\n");

    printf("+----------------------demux's traffic_flexe_cfg function----------------------------------------------------+\r\n");
    printf("demux_traffic_flexe_cfg_debug(char *string)\r\n");
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

    printf("+----------------------demux's dump function-----------------------------------------------+\r\n");
    printf("flexe_demux_debug(void)\r\n");
    printf("\r\n");

    printf("parameter description: no parameter\r\n");
    printf("+------------------------------------------------------------------------------------------+\r\n");
    printf("\r\n\r\n");

    return DEV_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     demux_group's phy config.
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
*     phy_index: phy's index, phy1->bit0...phy8->bit7
*     phy_id: phy's id pointer
*     phy_mode: each phy's working mode
*     enable: add/remove group's phy
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
*      liming        2018-03-15         1.0            initial
*
******************************************************************************/
dev_err_t demux_phy_group_cfg(UINT_8 unit,
                              UINT_8 group_id,
                              UINT_8 phy_index,
                              UINT_8 *phy_id,
                              flexe_phy_mode phy_mode,
                              UINT_8 enable)
{
    UINT_8 index = 0;
    UINT_8 phy_num = 0;
	UINT_8 phy_num_origin = 0;
	UINT_8 phy_index_record[PHY_NUM] = {0xff};
	UINT_8 phy_index_record_count = 0;
	UINT_8 phy_index_min = DEFAULT_VALUE;
	UINT_32 reg_value = 0;
	flexe_demux_phy_info_t sour_phy[PHY_NUM];
	flexe_demux_phy_info_t des_phy[PHY_NUM];
	flexe_demux_phy_info_t sour_phy_origin[PHY_NUM];
	flexe_demux_phy_info_t des_phy_origin[PHY_NUM];
	flexe_demux_info_t **flexe_demux_chip_info = (flexe_demux_info_t **)malloc(sizeof(flexe_demux_info_t *));
	dev_err_t rt = DEV_DEFAULT;
	UINT_8 division_num = 0;

    /* check phy_index whether normal */
    if (0 == phy_index) {
        printf("%s: unit = 0x%x, invalid phy_index = 0x%x...\r\n", __FUNCTION__, unit, phy_index);
		
        rt = DEV_PARA_ERR;
        goto err;
    }
	/* recording parameter */
	printf("%s,%d:unit=%d,group_id=%d,hard_8bit_phy_index=0x%x,protocal_phy_id[0..7]=[%d,%d,%d,%d,%d,%d,%d,%d],phy_mode=%d,enable=%d\r\n",__FUNCTION__,__LINE__,
	        unit,group_id,phy_index,phy_id[0],phy_id[1],phy_id[2],phy_id[3],phy_id[4],phy_id[5],phy_id[6],phy_id[7],phy_mode,enable);

	/* check pointer whether null */
	if (!flexe_demux_chip_info) {
		printf("%s: unit = 0x%x, pointer is null...\r\n", __FUNCTION__, unit);

		return DEV_POINTER_NULL;
	}
    /* check unit whether normal */
    if (unit > (DEV_NUM - 1)) {
        printf("%s: invalid unit = 0x%x...\r\n", __FUNCTION__, unit);
		
        rt = DEV_PARA_ERR;
        goto err;
    }
    /* check group_id whether normal */
    if (group_id > (PHY_NUM - 1)) {
        printf("%s: unit = 0x%x, invalid group_id = 0x%x...\r\n", __FUNCTION__, unit, group_id);
		
        rt = DEV_PARA_ERR;
        goto err;
    }
	/* check pointer whether normal */
	if (!phy_id) {
        printf("%s: invalid pointer, unit = 0x%x...\r\n", __FUNCTION__, unit);
		
        rt = DEV_POINTER_NULL;
        goto err;
	}
    /* check phy_mode whether normal */
    if ((FLEXE_PHY_50G != phy_mode) && (FLEXE_PHY_100G != phy_mode)) {
        printf("%s: unit = 0x%x, invalid phy_mode = 0x%x...\r\n", __FUNCTION__, unit, phy_mode);
		
        rt = DEV_PARA_ERR;
        goto err;
    }
	/* check add_remove info */
	if ((TRAFFIC_REMOVE != enable) && (TRAFFIC_ADD != enable)) {
		printf("%s: unit = 0x%x, invalid enable = 0x%x...\r\n",
							                      __FUNCTION__,
					                                      unit,
			                                           enable);

		rt = DEV_PARA_ERR;
        goto err;
	}
	
    /* add traffic's group_phy */
	if (TRAFFIC_ADD == enable) {
		rt = flexe_demux_phy_group_cfg(unit, group_id, phy_index, phy_id, phy_mode, enable);

		if (DEV_SUCCESS != rt) {
			printf("%s: unit = 0x%x, config group_id = 0x%x fail...\r\n", __FUNCTION__, unit, group_id);

			goto err;
		}

		rt = flexe_demux_phy_group_out(unit, group_id, &phy_num, sour_phy, des_phy);

		if (DEV_SUCCESS == rt) {
			/* config flexe_group_infosour_phy */
			phy_index_min = DEFAULT_VALUE;
			phy_index_record_count = 0;
			memset(phy_index_record, 0xff, sizeof(phy_index_record));
			for (index = 0; index < PHY_NUM; index++) {
				if (group_id == sour_phy[index].group_id) {
					phy_index_record[phy_index_record_count] = index;
					phy_index_record_count++;
				}
			}
			phy_index_min = min_element(phy_index_record, PHY_NUM);
			flexe_demux_group_info_sync_get(unit, &reg_value);
			for (index = 0; index < phy_index_record_count; index++) {
				reg_value &= (~((UINT_32)(MASK_4_BIT << (SHIFT_BIT_4*phy_index_record[index]))));
				reg_value = reg_value + (UINT_32)((phy_index_min & MASK_4_BIT) << (SHIFT_BIT_4*phy_index_record[index]));
			}
			flexe_demux_group_info_sync_set(unit, reg_value);

			/* config group_cfgdes_phy */
			phy_index_min = DEFAULT_VALUE;
			phy_index_record_count = 0;
			memset(phy_index_record, 0xff, sizeof(phy_index_record));
			for (index = 0; index < PHY_NUM; index++) {
				if (group_id == des_phy[index].group_id) {
					phy_index_record[phy_index_record_count] = index;
					phy_index_record_count++;
				}
			}
			phy_index_min = min_element(phy_index_record, PHY_NUM);
			flexe_demux_group_cfg_sync_get(unit, &reg_value);
			for (index = 0; index < phy_index_record_count; index++) {
				reg_value &= (~((UINT_32)(MASK_4_BIT << (SHIFT_BIT_4*phy_index_record[index]))));
				reg_value = reg_value + (UINT_32)((phy_index_min & MASK_4_BIT) << (SHIFT_BIT_4*phy_index_record[index]));
			}
			flexe_demux_group_cfg_sync_set(unit, reg_value);

			/* config mode_info */
			for (index = 0; index < PHY_NUM; index++) {
				if ((phy_index >> index) & MASK_1_BIT) {
					flexe_demux_mode_info_set(unit, index, phy_mode);
                }
            }

            /* config flexe_demux_reorder_info for 50g_mode */
            if (FLEXE_PHY_50G == phy_mode) {
                for (index = 0; index < PHY_NUM; index++) {
                    if (group_id == des_phy[index].group_id) {
                        flexe_demux_reorder_asic_set(unit, index, des_phy[index].sou_index);
                    }
                }
            }
            /* config flexe_demux_reorder_info for 100g_mode */
            else {
                rt = demux_chip_info_get(unit, flexe_demux_chip_info);

                if (DEV_SUCCESS != rt) {
                    printf("%s: unit = 0x%x, get demux_info fail, rt = %d...\r\n", __FUNCTION__, unit, rt);

                    goto err;
                }
                
                for (index = 0; index < PHY_NUM; index++) {
					if (group_id == sour_phy[index].group_id) {
                        for (division_num = 0; division_num < 2; division_num++) {
                            flexe_demux_reorder_asic_set(unit,
                            sour_phy[index].phy_div_des_index[division_num],
                             (UINT_32)((division_num << SHIFT_BIT_3) | index));
                        }
                    }
                }
            }        
			/* interleave_rst */
			reg_value = 0;
			for (index = 0; index < PHY_NUM; index++) {
				if ((phy_index >> index) & MASK_1_BIT) {
					reg_value = reg_value + (UINT_32)((0x1 & MASK_1_BIT) << (SHIFT_BIT_1*index));
				}
			}
			flexe_demux_interleave_rst_sync_set(unit, reg_value);
			flexe_demux_interleave_rst_sync_set(unit, 0x0);
		}
		else {
			printf("%s: unit = 0x%x, get group_id = 0x%x's phy_info fail...\r\n", __FUNCTION__, unit, group_id);

			goto err;
		}
	}
	/* remove traffic's group_phy */
	else {	
		rt = flexe_demux_phy_group_out(unit, group_id, &phy_num_origin, sour_phy_origin, des_phy_origin);
		if (DEV_SUCCESS != rt) {
			printf("%s: unit = 0x%x, get group_id = 0x%x's origin_phy_info fail...\r\n", __FUNCTION__, unit, group_id);

			goto err;
		}
		
		rt = flexe_demux_phy_group_cfg(unit, group_id, phy_index, phy_id, phy_mode, enable);
		if (DEV_SUCCESS != rt) {
			printf("%s: unit = 0x%x, config group_id = 0x%x fail...\r\n", __FUNCTION__, unit, group_id);

			goto err;
		}

		rt = flexe_demux_phy_group_out(unit, group_id, &phy_num, sour_phy, des_phy);
		if (DEV_SUCCESS != rt) {
			printf("%s: unit = 0x%x, get group_id = 0x%x's phy_info fail...\r\n", __FUNCTION__, unit, group_id);

			goto err;
		}	

        /* set group_cfgdes_phy */
		phy_index_min = DEFAULT_VALUE;
		phy_index_record_count = 0;
		memset(phy_index_record, 0xff, sizeof(phy_index_record));
		for (index = 0; index < PHY_NUM; index++) {
			if (group_id == des_phy[index].group_id) {
				phy_index_record[phy_index_record_count] = index;
				phy_index_record_count++;
			}
		}
		phy_index_min = min_element(phy_index_record,PHY_NUM);
		flexe_demux_group_cfg_sync_get(unit, &reg_value);
		for (index = 0; index < phy_index_record_count; index++) {
			reg_value &= (~((UINT_32)(MASK_4_BIT << (SHIFT_BIT_4*phy_index_record[index]))));
			reg_value = reg_value + (UINT_32)((phy_index_min & MASK_4_BIT) << (SHIFT_BIT_4*phy_index_record[index]));
		}
		flexe_demux_group_cfg_sync_set(unit, reg_value);
		for (index = 0; index < PHY_NUM; index++) {
			if ((phy_index >> index) & MASK_1_BIT) {
				flexe_demux_group_cfg_set(unit, sour_phy_origin[index].des_index, 0xf);
			}
		}

		/* set reorder asicsour_phy*/
		/* config flexe_demux_reorder_info for 50g_mode */
		if (FLEXE_PHY_50G == phy_mode) {
			for (index = 0; index < PHY_NUM; index++) {
				if ((phy_index >> index) & MASK_1_BIT) {
					flexe_demux_reorder_asic_set(unit, sour_phy_origin[index].des_index, 0xf0);
				}
			}
		}
		/* config flexe_demux_reorder_info for 100g_mode */
		else {
			rt = demux_chip_info_get(unit, flexe_demux_chip_info);

			if (DEV_SUCCESS != rt) {
				printf("%s: unit = 0x%x, get demux_info fail, rt = %d...\r\n", __FUNCTION__, unit, rt);

				goto err;
			}

			for (index = 0; index < PHY_NUM; index++) {
				if ((phy_index >> index) & MASK_1_BIT) {
					for (division_num = 0; division_num < 2; division_num++) {
						flexe_demux_reorder_asic_set(unit,
							sour_phy_origin[index].phy_div_des_index[division_num],0xf0);
					}
				}
			}
		}
		/* set flexe_group_infosour_phy */
		phy_index_min = DEFAULT_VALUE;
		phy_index_record_count = 0;
		memset(phy_index_record, 0xff, sizeof(phy_index_record));
		for (index = 0; index < PHY_NUM; index++) {
			if (group_id == sour_phy[index].group_id) {
				phy_index_record[phy_index_record_count] = index;
				phy_index_record_count++;
			}
		}
		phy_index_min = min_element(phy_index_record,PHY_NUM);
		flexe_demux_group_info_sync_get(unit, &reg_value);
		for (index = 0; index < phy_index_record_count; index++) {
			reg_value &= (~((UINT_32)(MASK_4_BIT << (SHIFT_BIT_4*phy_index_record[index]))));
			reg_value = reg_value + (UINT_32)((phy_index_min & MASK_4_BIT) << (SHIFT_BIT_4*phy_index_record[index]));
		}
		flexe_demux_group_info_sync_set(unit, reg_value);
		for (index = 0; index < PHY_NUM; index++) {
			if ((phy_index >> index) & MASK_1_BIT) {
				flexe_demux_group_info_set(unit, index, 0xf);
			}
		}
	}

    free(flexe_demux_chip_info);

    flexe_demux_chip_info = NULL;

    return DEV_SUCCESS;
err:
    free(flexe_demux_chip_info);

    flexe_demux_chip_info = NULL;

    return rt;
}


/******************************************************************************
*
* FUNCTION
*
*     demux's traffic cfg.
*
* DESCRIPTION
*
*     config demux module's traffic
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
*      liming        2018-03-21         1.0            initial
*
******************************************************************************/
dev_err_t demux_traffic_flexe_cfg(UINT_8 unit,
                                  UINT_8 ch,
                                  UINT_8 *phy_index,
                                  UINT_8 phy_num,
                                  UINT_32 *ts_mask,
                                  UINT_8 enable)
{
    UINT_8 index = 0;
    UINT_8 ts_id = 0;
    UINT_8 ts_index = 0;
    UINT_8 sour_phy_index[PHY_NUM] = {0};
    UINT_32 phy_ts_mask[PHY_NUM] = {0};
    UINT_8 phy_ts_num[PHY_NUM] = {0};
    UINT_8 ts_num = 0;
    UINT_8 blk = 0;
    UINT_8 cycle = 0;
    UINT_8 ts_id_info[TS_NUM] = {0};
    UINT_8 table_id = 0;
    UINT_8 ram_depth = 0;
    UINT_32 data = 0;
    UINT_32 r_data[3] = {0};
    UINT_32 own_ts_group[PHY_NUM] = {0};
    flexe_demux_traffic_info_t traffic_info;
    flexe_demux_phy_info_t des_phy[PHY_NUM];
    flexe_demux_ch_info_t flexe_ch_info_out;
    flexe_demux_ts_info_t flexe_ts_info_out[TS_NUM];
    flexe_demux_info_t **flexe_demux_chip_info;
	dev_err_t rt = DEV_DEFAULT;

	/* recording parameter */
	printf("%s,%d:unit=%d,ch=%d,phy_num=%d,enable=%d\r\n",__FUNCTION__,__LINE__,unit,ch,phy_num,enable);
	for (index = 0; index < phy_num; index++) {
		printf("%s,%d:hard_8bit_phy_index[%d]=%d,ts_mask[%d]=0x%x\r\n",__FUNCTION__,__LINE__,index,phy_index[index],index,ts_mask[index]);
	}

    /* check unit whether normal */
    if (unit > (DEV_NUM - 1)) {
        printf("%s: invalid unit = 0x%x...\r\n", __FUNCTION__, unit);
		
        return DEV_PARA_ERR;
    }
    /* check ch whether normal */
    if (ch > (CH_NUM - 1)) {
        printf("%s: unit = 0x%x, invalid ch = 0x%x...\r\n", __FUNCTION__, unit, ch);
		
        return DEV_PARA_ERR;
    }
    /* check phy_num whether normal */
    if (phy_num > PHY_NUM) {
        printf("%s: unit = 0x%x, invalid phy_num = 0x%x...\r\n", __FUNCTION__, unit, phy_num);
		
        return DEV_PARA_ERR;
    }
	/* check pointer whether normal */
    if (!ts_mask || !phy_index) {
        printf("%s: invalid pointer, unit = 0x%x...\r\n", __FUNCTION__, unit);
        
        return DEV_POINTER_NULL;
    }
    /* check add_remove info */
    if ((TRAFFIC_REMOVE != enable) && (TRAFFIC_ADD != enable)) {
        printf("%s: unit = 0x%x, invalid enable = 0x%x...\r\n",
                                                  __FUNCTION__,
                                                          unit,
                                                       enable);

		return DEV_PARA_ERR;
	}

    /* set traffic's ts to zero */
	for (index = 0; index < TS_FIELD; index++) {
		traffic_info.ts[index] = 0;
	}

	traffic_info.unit = unit;
	traffic_info.ch = ch;
	traffic_info.enable = enable;
        flexe_demux_chip_info = (flexe_demux_info_t **)malloc(sizeof(flexe_demux_info_t *));

	/* check pointer whether null */
    if (!flexe_demux_chip_info) {
        printf("%s: unit = 0x%x, pointer is null...\r\n", __FUNCTION__, unit);

        return DEV_POINTER_NULL;
    }
    
    rt = demux_chip_info_get(unit, flexe_demux_chip_info);

    if (DEV_SUCCESS != rt) {
        printf("%s: unit = 0x%x, get demux_info fail, rt = %d...\r\n", __FUNCTION__, unit, rt);

        goto err;
    }

    /* check phy_num whether normal */
    if ((FLEXE_PHY_100G == (*flexe_demux_chip_info)->sour_phy[*phy_index].phy_mode) && (phy_num > (PHY_NUM / 2))) {
        printf("%s: unit = 0x%x, invalid phy_num = 0x%x...\r\n", __FUNCTION__, unit, phy_num);
		rt = DEV_PARA_ERR;
        goto err;
    }

    /* check phy_mode whether normal */
    for (index = 0; index < phy_num; index++) {
        if (0 != *(ts_mask + index)) {
            if ((*flexe_demux_chip_info)->sour_phy[*phy_index].phy_mode !=
                (*flexe_demux_chip_info)->sour_phy[*(phy_index + index)].phy_mode) {
                printf("%s: unit = 0x%x, phy_mode err...\r\n", __FUNCTION__, unit);

                goto err;
            }
        }
        else {
            printf("%s: unit = 0x%x, phy_index = 0x%x, invalid ts_mask = 0x%x...\r\n",
                                                                          __FUNCTION__,
                                                                                  unit,
                                                                  *(phy_index + index),
                                                                   *(ts_mask + index));

            rt = DEV_PARA_ERR;

            goto err;
        }
    }

    /* phy_50g_mode */
    if (FLEXE_PHY_50G == (*flexe_demux_chip_info)->sour_phy[*phy_index].phy_mode) {
        for (index = 0; index < phy_num; index++) {
            ts_num = 0;

            for (ts_index = 0; ts_index < REG_WIDTH; ts_index++) {
                if ((*(ts_mask + index) >> ts_index) & MASK_1_BIT) {
                    ts_num++;
                }
            }

            /* get source_phy's ts_num */
            phy_ts_num[index] = ts_num;
            
            /* get source_phy's phy_index and ts_mask */
            sour_phy_index[index] = *(phy_index + index);
            phy_ts_mask[index] = *(ts_mask + index);
        }
    }
    /* phy_100g_mode */
    else {
        for (index = 0; index < phy_num; index++) {
            ts_num = 0;

            for (ts_index = 0; ts_index < PHY_50G_PL; ts_index++) {
                if ((*(ts_mask + index) >> ts_index) & MASK_1_BIT) {
                    ts_num++;
                }
            }

            /* get source_phy_div_phy_0's ts_num and ts_mask */
            phy_ts_num[2 * index] = ts_num;
            phy_ts_mask[2 * index] = (*(ts_mask + index)) & MASK_10_BIT;

            ts_num = 0;

            for (ts_index = PHY_50G_PL; ts_index < PHY_100G_PL; ts_index++) {
                if ((*(ts_mask + index) >> ts_index) & MASK_1_BIT) {
                    ts_num++;
                }
            }

            /* get source_phy_div_phy_1's ts_num and ts_mask */
            phy_ts_num[2 * index + 1] = ts_num;
            phy_ts_mask[2 * index + 1] = ((*(ts_mask + index)) >> SHIFT_BIT_10) & MASK_10_BIT;
            
            /* get source_phy's phy_index and ts_mask */
            sour_phy_index[index] = *(phy_index + index);
        }
    }


	
    rt = flexe_demux_phy_info_out(unit, sour_phy_index, phy_num, des_phy);

	printf("### phy_num = %d", phy_num);
	for (index = 0; index < PHY_NUM; index++) {
		printf("### sour_phy_index[%d]=%d\r\n", index,sour_phy_index[index]);
		printf("### phy_ts_mask[%d]=0x%x\r\n", index,phy_ts_mask[index]);
		printf("### des_phy[%d].phy_index=%d\r\n", index,des_phy[index].phy_index);
	}

    if (DEV_SUCCESS != rt) {
        printf("%s: unit = 0x%x, rt = %d, get des_phy_info fail...\r\n", __FUNCTION__, unit, rt);

        goto err;
    }

    /* phy_50g_mode */
    if (FLEXE_PHY_50G == (*flexe_demux_chip_info)->sour_phy[*phy_index].phy_mode) {
        /* add traffic and check phy's flow */
        if (TRAFFIC_ADD == enable) {
            for (index = 0; index < phy_num; index++) {
                if (phy_ts_num[index] > (PHY_50G_PL - des_phy[index].phy_div_ts_num)) {
                    printf("%s: unit = 0x%x, phy payload overflow...\r\n",
                                                             __FUNCTION__,
															         unit);
    
                    rt = DEV_FAIL;

                    goto err;
                }
            }
        }
        /* delete traffic and check phy's flow */
        else {
            for (index = 0; index < phy_num; index++) {
                if (phy_ts_num[index] > des_phy[index].phy_div_ts_num) {
                    printf("%s: unit = 0x%x, phy_index = 0x%x payload error...\r\n",
                                                                       __FUNCTION__,
                                                                               unit,
                                                             sour_phy_index[index]);
        
                    rt = DEV_FAIL;

                    goto err;
                }
            }
        }
    }
    /* 100g_mode */
    else {
        /* add traffic and check phy's flow */
        if (TRAFFIC_ADD == enable) {
            for (index = 0; index < phy_num; index++) {
                if ((phy_ts_num[2 * index] > (PHY_50G_PL - des_phy[2 * index].phy_div_ts_num)) ||
                    (phy_ts_num[2 * index + 1] > (PHY_50G_PL - des_phy[2 * index + 1].phy_div_ts_num))) {
                    printf("%s: unit = 0x%x, phy payload overflow...\r\n",
                                                             __FUNCTION__,
															         unit);
    
                    rt = DEV_FAIL;

                    goto err;
                }
            }
        }
        /* delete traffic and check phy's flow */
        else {
            for (index = 0; index < phy_num; index++) {
                if ((phy_ts_num[2 * index] > des_phy[2 * index].phy_div_ts_num) ||
                    (phy_ts_num[2 * index + 1] > des_phy[2 * index + 1].phy_div_ts_num)) {
                    printf("%s: unit = 0x%x, phy_index = 0x%x payload error...\r\n",
                                                                       __FUNCTION__,
                                                                               unit,
                                                             sour_phy_index[index]);
        
                    rt = DEV_FAIL;

                    goto err;
                }
            }
        }
    }

    /* get ts_id */
    if (FLEXE_PHY_50G == (*flexe_demux_chip_info)->sour_phy[*phy_index].phy_mode) {
        for (index = 0; index < phy_num; index++) {
            for (ts_index = 0; ts_index < PHY_50G_PL; ts_index++) {
                if ((phy_ts_mask[index] >> ts_index) & MASK_1_BIT) {
                    ts_id = des_phy[index].phy_index * PHY_50G_PL + ts_index;
                    traffic_info.ts[ts_id / REG_WIDTH] |= ((MASK_1_BIT << (ts_id % REG_WIDTH)) & MASK_32_BIT);
                }
            }
        }
    }
    else {
        for (index = 0; index < phy_num; index++) {
            for (ts_index = 0; ts_index < PHY_50G_PL; ts_index++) {
                if ((phy_ts_mask[2 * index] >> ts_index) & MASK_1_BIT) {
                    ts_id = des_phy[2 * index].phy_index * PHY_50G_PL + ts_index;
                    traffic_info.ts[ts_id / REG_WIDTH] |= ((MASK_1_BIT << (ts_id % REG_WIDTH)) & MASK_32_BIT);
                }

                if ((phy_ts_mask[2 * index + 1] >> ts_index) & MASK_1_BIT) {
                    ts_id = des_phy[2 * index + 1].phy_index * PHY_50G_PL + ts_index;
                    traffic_info.ts[ts_id / REG_WIDTH] |= ((MASK_1_BIT << (ts_id % REG_WIDTH)) & MASK_32_BIT);
                }
            }
        }
    }

    /* get the sort ts_info for asic version */
    ts_num = 0;
    memset(ts_id_info, 0, sizeof(ts_id_info));
    
    for (index = 0; index < TS_FIELD; index++) {
        for (ts_index = 0; ts_index < REG_WIDTH; ts_index++) {
            if ((traffic_info.ts[index] >> ts_index) & MASK_1_BIT) {
                ts_id_info[ts_num] = index * REG_WIDTH + ts_index;
                ts_num++;
            }
        }
    }

    /* cfg traffic */
    rt = flexe_demux_traffic_cfg(traffic_info);

    if (DEV_SUCCESS != rt) {
        printf("%s: unit = 0x%x, traffic cfg fail...\r\n", __FUNCTION__, unit);

        goto err;
    }

    /* add traffic and cfg info */
    if (TRAFFIC_ADD == traffic_info.enable) {
        /* get algorithm result */
        rt = flexe_demux_traffic_out(traffic_info, &flexe_ch_info_out, flexe_ts_info_out);

        if (DEV_SUCCESS != rt) {
            printf("%s: unit = 0x%x, rt = %d, get flexe_demux_algorithm result fail...\r\n", __FUNCTION__, unit, rt);

            goto err;
        }

        /* config calendar */
        for (ts_index = 0; ts_index < ts_num; ts_index++) {
            flexe_demux_calendar_cfg_set(unit, ts_id_info[ts_index], ch + 1);
        }

        /* config channel_own_ts_group */
        memset(own_ts_group, 0, sizeof(own_ts_group));
        
        for (ts_index = 0; ts_index < ts_num; ts_index++) {
            own_ts_group[ts_id_info[ts_index] / 10] = 1;
        }

        for (index = 0; index < PHY_NUM; index++) {
            data |= ((own_ts_group[index] & MASK_1_BIT) << index);
        }
        
        flexe_demux_channel_own_tsgroup_set(unit, ch, data);

        /* config r_cfg_ram */
        for (ts_index = 0; ts_index < ts_num; ts_index++) {
            memset(r_data, 0, sizeof(r_data));
            
            for (cycle = 0; cycle < TCYCLE; cycle++) {
                r_data[(cycle * SHIFT_BIT_8) / REG_WIDTH] |= ((UINT_32)flexe_ts_info_out[ts_index].r_col[cycle]) << ((cycle * SHIFT_BIT_8) % REG_WIDTH);
            }
            
            flexe_demux_rdrule_set(unit, ((ch * TS_NUM) + ts_index), r_data);
        }

        /* config w_cfg_ram */
        for (ts_index = 0; ts_index < ts_num; ts_index++) {
            if (ts_id_info[ts_index] < 40) {
                table_id = ts_id_info[ts_index] % 5;
                ram_depth = (ts_id_info[ts_index] / 5) * 10;

                for (index = 0; index < TCYCLE; index++) {
                    data = flexe_ts_info_out[ts_index].w_col[index];
                    
                    flexe_demux_wcfgram_set(unit, table_id, ram_depth + index, &data);
                }                 
            }
            else if ((ts_id_info[ts_index] >= 40) && (ts_id_info[ts_index] < 80)) {
                table_id = (ts_id_info[ts_index] % 5) + 5;
                ram_depth = ((ts_id_info[ts_index] - 40) / 5) * 10;

                for (index = 0; index < TCYCLE; index++) {
                    data = flexe_ts_info_out[ts_index].w_col[index];
                    
                    flexe_demux_wcfgram_set(unit, table_id, ram_depth + index, &data);
                }
            }
            else {
                printf("%s: unit = 0x%x, asic version's ts_id overstep the boundary...\r\n", __FUNCTION__, unit);

                rt = DEV_TS_UNAVAILABLE;

                goto err;
            }
        }

		/* calc blk */
		rt = flexe_demux_divisor_data_get(ts_num, TCYCLE, &blk);

		if (DEV_SUCCESS != rt) {
            printf("%s: unit = 0x%x, get blk fail...\r\n", __FUNCTION__, unit);

            goto err;
        }
        
        /* config ch_property */
        cycle = (ts_num / blk) - 1;
        blk -= 1;

        data = cycle | ((blk & MASK_4_BIT) << SHIFT_BIT_8) | ((ts_num & MASK_8_BIT) << SHIFT_BIT_12);

        flexe_demux_property_set(unit, ch, &data);

        /* config ready_ts enable */
        for (index = 0; index < ts_num; index++) {
            flexe_demux_ts_en_set(unit, ts_id_info[index], 1);
        }

        /* set sour_phy and des_phy info */
        flexe_demux_phy_info_cfg(unit, sour_phy_index, phy_ts_mask, phy_ts_num, phy_num, traffic_info.enable, 0);

		flexe_oam_rx_ts_add(unit,ch,ts_num*5);
    }
    /* remove traffic and set cfg_info to default value */
    else {
        /* disable ready_ts enable */
        for (index = 0; index < ts_num; index++) {
            flexe_demux_ts_en_set(unit, ts_id_info[index], 0);
        }

        /* set calendar to zero */
        for (ts_index = 0; ts_index < ts_num; ts_index++) {
            flexe_demux_calendar_cfg_set(unit, ts_id_info[ts_index], 0);
        }

        /* set channel_own_tsgroup to zero */
        flexe_demux_channel_own_tsgroup_set(unit, ch, 0);

        /* set r_cfg_ram to default value */
        for (ts_index = 0; ts_index < ts_num; ts_index++) {
            memset(r_data, 0xff, sizeof(r_data));
            
            flexe_demux_rdrule_set(unit, ((ch * TS_NUM) + ts_index), r_data);
        }

        /* set w_cfg_ram to default_value */
        for (ts_index = 0; ts_index < ts_num; ts_index++) {
            if (ts_id_info[ts_index] < 40) {
                table_id = ts_id_info[ts_index] % 5;
                ram_depth = (ts_id_info[ts_index] / 5) * 10;

                for (index = 0; index < TCYCLE; index++) {
                    data = 0xf;
                    
                    flexe_demux_wcfgram_set(unit, table_id, ram_depth + index, &data);
                }                 
            }
            else if ((ts_id_info[ts_index] >= 40) && (ts_id_info[ts_index] < 80)) {
                table_id = (ts_id_info[ts_index] % 5) + 5;
                ram_depth = ((ts_id_info[ts_index] - 40) / 5) * 10;

                for (index = 0; index < TCYCLE; index++) {
                    data = 0xf;
                    
                    flexe_demux_wcfgram_set(unit, table_id, ram_depth + index, &data);
                }
            }
            else {
                printf("%s: unit = 0x%x, asic version's ts_id overstep the boundary...\r\n", __FUNCTION__, unit);

                rt = DEV_TS_UNAVAILABLE;

                goto err;
            }
        }

        /* set property to default value */
        data = 0xfffff;
        
        flexe_demux_property_set(unit, ch, &data);

        /* set sour_phy and des_phy info */
        flexe_demux_phy_info_cfg(unit, sour_phy_index, phy_ts_mask, phy_ts_num, phy_num, traffic_info.enable, 0);

		flexe_oam_rx_ts_del(unit,ch);
    }

    free(flexe_demux_chip_info);

    flexe_demux_chip_info = NULL;

    return DEV_SUCCESS;

err:
    free(flexe_demux_chip_info);

    flexe_demux_chip_info = NULL;

    return rt;
}


/******************************************************************************
*
* FUNCTION
*
*     demux's traffic cfg_b.
*
* DESCRIPTION
*
*     config demux module's traffic
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
dev_err_t demux_traffic_flexe_b_cfg(UINT_8 unit,
								  UINT_8 ch,
								  UINT_8 *phy_index,
								  UINT_8 phy_num,
								  UINT_32 *ts_mask,
								  UINT_8 enable)
{
	UINT_8 index = 0;
	UINT_8 ts_id = 0;
	UINT_8 ts_index = 0;
	UINT_8 sour_phy_index[PHY_NUM] = {0};
	UINT_32 phy_ts_mask[PHY_NUM] = {0};
	UINT_8 phy_ts_num[PHY_NUM] = {0};
	UINT_8 ts_num = 0;
	UINT_8 blk = 0;
	UINT_8 cycle = 0;
	UINT_8 ts_id_info[TS_NUM] = {0};
	UINT_8 table_id = 0;
	UINT_8 ram_depth = 0;
	UINT_32 data = 0;
	UINT_32 r_data[3] = {0};
	UINT_32 own_ts_group[PHY_NUM] = {0};
	flexe_demux_traffic_info_t traffic_info;
	flexe_demux_phy_info_t des_phy[PHY_NUM];
	flexe_demux_ch_info_t flexe_ch_info_out;
	flexe_demux_ts_info_t flexe_ts_info_out[TS_NUM];
        flexe_demux_info_t **flexe_demux_chip_info;
	dev_err_t rt = DEV_DEFAULT;

	/* recording parameter */
	printf("%s,%d:unit=%d,ch=%d,phy_num=%d,enable=%d\r\n",__FUNCTION__,__LINE__,unit,ch,phy_num,enable);
	for (index = 0; index < phy_num; index++) {
		printf("%s,%d:hard_8bit_phy_index[%d]=%d,ts_mask[%d]=0x%x\r\n",__FUNCTION__,__LINE__,index,phy_index[index],index,ts_mask[index]);
	}

	/* check unit whether normal */
	if (unit > (DEV_NUM - 1)) {
		printf("%s: invalid unit = 0x%x...\r\n", __FUNCTION__, unit);
		
		return DEV_PARA_ERR;
	}
	/* check ch whether normal */
	if (ch > (CH_NUM - 1)) {
		printf("%s: unit = 0x%x, invalid ch = 0x%x...\r\n", __FUNCTION__, unit, ch);
		
		return DEV_PARA_ERR;
	}
	/* check phy_num whether normal */
	if (phy_num > PHY_NUM) {
		printf("%s: unit = 0x%x, invalid phy_num = 0x%x...\r\n", __FUNCTION__, unit, phy_num);
		
		return DEV_PARA_ERR;
	}
	/* check pointer whether normal */
	if (!ts_mask || !phy_index) {
		printf("%s: invalid pointer, unit = 0x%x...\r\n", __FUNCTION__, unit);
		
		return DEV_POINTER_NULL;
	}
	/* check add_remove info */
	if ((TRAFFIC_REMOVE != enable) && (TRAFFIC_ADD != enable)) {
		printf("%s: unit = 0x%x, invalid enable = 0x%x...\r\n",
												  __FUNCTION__,
														  unit,
													   enable);

		return DEV_PARA_ERR;
	}

	/* set traffic's ts to zero */
	for (index = 0; index < TS_FIELD; index++) {
		traffic_info.ts[index] = 0;
	}

	traffic_info.unit = unit;
	traffic_info.ch = ch;
	traffic_info.enable = enable;

        flexe_demux_chip_info = (flexe_demux_info_t **)malloc(sizeof(flexe_demux_info_t *));

	/* check pointer whether null */
	if (!flexe_demux_chip_info) {
		printf("%s: unit = 0x%x, pointer is null...\r\n", __FUNCTION__, unit);

		return DEV_POINTER_NULL;
    }
    
    rt = demux_chip_info_get(unit, flexe_demux_chip_info);

    if (DEV_SUCCESS != rt) {
        printf("%s: unit = 0x%x, get demux_info fail, rt = %d...\r\n", __FUNCTION__, unit, rt);

        goto err;
    }

    /* check phy_num whether normal */
    if ((FLEXE_PHY_100G == (*flexe_demux_chip_info)->sour_phy[*phy_index].phy_mode) && (phy_num > (PHY_NUM / 2))) {
        printf("%s: unit = 0x%x, invalid phy_num = 0x%x...\r\n", __FUNCTION__, unit, phy_num);
		rt = DEV_PARA_ERR;
        goto err;
    }

    /* check phy_mode whether normal */
    for (index = 0; index < phy_num; index++) {
        if (0 != *(ts_mask + index)) {
            if ((*flexe_demux_chip_info)->sour_phy[*phy_index].phy_mode !=
                (*flexe_demux_chip_info)->sour_phy[*(phy_index + index)].phy_mode) {
                printf("%s: unit = 0x%x, phy_mode err...\r\n", __FUNCTION__, unit);

                goto err;
            }
        }
        else {
            printf("%s: unit = 0x%x, phy_index = 0x%x, invalid ts_mask = 0x%x...\r\n",
                                                                          __FUNCTION__,
                                                                                  unit,
                                                                  *(phy_index + index),
                                                                   *(ts_mask + index));

            rt = DEV_PARA_ERR;

            goto err;
        }
    }

    /* phy_50g_mode */
    if (FLEXE_PHY_50G == (*flexe_demux_chip_info)->sour_phy[*phy_index].phy_mode) {
        for (index = 0; index < phy_num; index++) {
            ts_num = 0;

            for (ts_index = 0; ts_index < REG_WIDTH; ts_index++) {
                if ((*(ts_mask + index) >> ts_index) & MASK_1_BIT) {
                    ts_num++;
                }
            }

            /* get source_phy's ts_num */
            phy_ts_num[index] = ts_num;
            
            /* get source_phy's phy_index and ts_mask */
            sour_phy_index[index] = *(phy_index + index);
            phy_ts_mask[index] = *(ts_mask + index);
        }
    }
    /* phy_100g_mode */
    else {
        for (index = 0; index < phy_num; index++) {
            ts_num = 0;

            for (ts_index = 0; ts_index < PHY_50G_PL; ts_index++) {
                if ((*(ts_mask + index) >> ts_index) & MASK_1_BIT) {
                    ts_num++;
                }
            }

            /* get source_phy_div_phy_0's ts_num and ts_mask */
            phy_ts_num[2 * index] = ts_num;
            phy_ts_mask[2 * index] = (*(ts_mask + index)) & MASK_10_BIT;

            ts_num = 0;

            for (ts_index = PHY_50G_PL; ts_index < PHY_100G_PL; ts_index++) {
                if ((*(ts_mask + index) >> ts_index) & MASK_1_BIT) {
                    ts_num++;
                }
            }

            /* get source_phy_div_phy_1's ts_num and ts_mask */
            phy_ts_num[2 * index + 1] = ts_num;
            phy_ts_mask[2 * index + 1] = ((*(ts_mask + index)) >> SHIFT_BIT_10) & MASK_10_BIT;
            
            /* get source_phy's phy_index and ts_mask */
            sour_phy_index[index] = *(phy_index + index);
        }
    }

    rt = flexe_demux_phy_info_out(unit, sour_phy_index, phy_num, des_phy);

    if (DEV_SUCCESS != rt) {
        printf("%s: unit = 0x%x, rt = %d, get des_phy_info fail...\r\n", __FUNCTION__, unit, rt);

        goto err;
    }

	   /* phy_50g_mode */
	if (FLEXE_PHY_50G == (*flexe_demux_chip_info)->sour_phy[*phy_index].phy_mode) {
		/* add traffic and check phy's flow */
		if (TRAFFIC_ADD == enable) {
			for (index = 0; index < phy_num; index++) {
				if (phy_ts_num[index] > (PHY_50G_PL - des_phy[index].phy_div_ts_num_b)) {
					printf("%s: unit = 0x%x, phy payload overflow...\r\n",
															 __FUNCTION__,
															         unit);
	
					rt = DEV_FAIL;

					goto err;
				}
			}
		}
		/* delete traffic and check phy's flow */
		else {
			for (index = 0; index < phy_num; index++) {
				if (phy_ts_num[index] > des_phy[index].phy_div_ts_num_b) {
					printf("%s: unit = 0x%x, phy_index = 0x%x payload error...\r\n",
																	   __FUNCTION__,
																			   unit,
															 sour_phy_index[index]);
		
					rt = DEV_FAIL;

					goto err;
				}
			}
		}
	}
	/* 100g_mode */
	else {
		/* add traffic and check phy's flow */
		if (TRAFFIC_ADD == enable) {
			for (index = 0; index < phy_num; index++) {
				if ((phy_ts_num[2 * index] > (PHY_50G_PL - des_phy[2 * index].phy_div_ts_num_b)) ||
					(phy_ts_num[2 * index + 1] > (PHY_50G_PL - des_phy[2 * index + 1].phy_div_ts_num_b))) {
					printf("%s: unit = 0x%x, phy payload overflow...\r\n",
															 __FUNCTION__,
															         unit);
	
					rt = DEV_FAIL;

					goto err;
				}
			}
		}
		/* delete traffic and check phy's flow */
		else {
			for (index = 0; index < phy_num; index++) {
				if ((phy_ts_num[2 * index] > des_phy[2 * index].phy_div_ts_num_b) ||
					(phy_ts_num[2 * index + 1] > des_phy[2 * index + 1].phy_div_ts_num_b)) {
					printf("%s: unit = 0x%x, phy_index = 0x%x payload error...\r\n",
																	   __FUNCTION__,
																			   unit,
															 sour_phy_index[index]);
		
					rt = DEV_FAIL;

					goto err;
				}
			}
		}
	}

    /* get ts_id */
    if (FLEXE_PHY_50G == (*flexe_demux_chip_info)->sour_phy[*phy_index].phy_mode) {
        for (index = 0; index < phy_num; index++) {
            for (ts_index = 0; ts_index < PHY_50G_PL; ts_index++) {
                if ((phy_ts_mask[index] >> ts_index) & MASK_1_BIT) {
                    ts_id = des_phy[index].phy_index * PHY_50G_PL + ts_index;
                    traffic_info.ts[ts_id / REG_WIDTH] |= ((MASK_1_BIT << (ts_id % REG_WIDTH)) & MASK_32_BIT);
                }
            }
        }
    }
    else {
        for (index = 0; index < phy_num; index++) {
            for (ts_index = 0; ts_index < PHY_50G_PL; ts_index++) {
                if ((phy_ts_mask[2 * index] >> ts_index) & MASK_1_BIT) {
                    ts_id = des_phy[2 * index].phy_index * PHY_50G_PL + ts_index;
                    traffic_info.ts[ts_id / REG_WIDTH] |= ((MASK_1_BIT << (ts_id % REG_WIDTH)) & MASK_32_BIT);
                }

                if ((phy_ts_mask[2 * index + 1] >> ts_index) & MASK_1_BIT) {
                    ts_id = des_phy[2 * index + 1].phy_index * PHY_50G_PL + ts_index;
                    traffic_info.ts[ts_id / REG_WIDTH] |= ((MASK_1_BIT << (ts_id % REG_WIDTH)) & MASK_32_BIT);
                }
            }
        }
    }

    /* get the sort ts_info for asic version */
    ts_num = 0;
    memset(ts_id_info, 0, sizeof(ts_id_info));
    
    for (index = 0; index < TS_FIELD; index++) {
        for (ts_index = 0; ts_index < REG_WIDTH; ts_index++) {
            if ((traffic_info.ts[index] >> ts_index) & MASK_1_BIT) {
                ts_id_info[ts_num] = index * REG_WIDTH + ts_index;
                ts_num++;
            }
        }
    }

    /* cfg traffic */
	rt = flexe_demux_traffic_b_cfg(traffic_info);

    if (DEV_SUCCESS != rt) {
        printf("%s: unit = 0x%x, traffic cfg fail...\r\n", __FUNCTION__, unit);

        goto err;
    }

    /* add traffic and cfg info */
    if (TRAFFIC_ADD == traffic_info.enable) {
        /* get algorithm result */
		rt = flexe_demux_traffic_b_out(traffic_info, &flexe_ch_info_out, flexe_ts_info_out);

        if (DEV_SUCCESS != rt) {
            printf("%s: unit = 0x%x, rt = %d, get flexe_demux_algorithm result fail...\r\n", __FUNCTION__, unit, rt);

            goto err;
        }

        /* config calendar */
        for (ts_index = 0; ts_index < ts_num; ts_index++) {
			flexe_demux_calendar_cfg_b_set(unit, ts_id_info[ts_index], ch + 1);
        }

        /* config channel_own_ts_group */
        memset(own_ts_group, 0, sizeof(own_ts_group));
        
        for (ts_index = 0; ts_index < ts_num; ts_index++) {
            own_ts_group[ts_id_info[ts_index] / 10] = 1;
        }

        for (index = 0; index < PHY_NUM; index++) {
            data |= ((own_ts_group[index] & MASK_1_BIT) << index);
        }
        
		flexe_demux_channel_own_tsgroup_b_set(unit, ch, data);

        /* config r_cfg_ram */
        for (ts_index = 0; ts_index < ts_num; ts_index++) {
            memset(r_data, 0, sizeof(r_data));
            
            for (cycle = 0; cycle < TCYCLE; cycle++) {
                r_data[(cycle * SHIFT_BIT_8) / REG_WIDTH] |= ((UINT_32)flexe_ts_info_out[ts_index].r_col[cycle]) << ((cycle * SHIFT_BIT_8) % REG_WIDTH);
            }
            
			flexe_demux_rdrule_b_set(unit, ((ch * TS_NUM) + ts_index), r_data);
        }

        /* config w_cfg_ram */
        for (ts_index = 0; ts_index < ts_num; ts_index++) {
            if (ts_id_info[ts_index] < 40) {
                table_id = ts_id_info[ts_index] % 5;
                ram_depth = (ts_id_info[ts_index] / 5) * 10;

                for (index = 0; index < TCYCLE; index++) {
                    data = flexe_ts_info_out[ts_index].w_col[index];
                    
					flexe_demux_wcfgram_b_set(unit, table_id, ram_depth + index, &data);
                }                 
            }
            else if ((ts_id_info[ts_index] >= 40) && (ts_id_info[ts_index] < 80)) {
                table_id = (ts_id_info[ts_index] % 5) + 5;
                ram_depth = ((ts_id_info[ts_index] - 40) / 5) * 10;

                for (index = 0; index < TCYCLE; index++) {
                    data = flexe_ts_info_out[ts_index].w_col[index];
                    
					flexe_demux_wcfgram_b_set(unit, table_id, ram_depth + index, &data);
                }
            }
            else {
                printf("%s: unit = 0x%x, asic version's ts_id overstep the boundary...\r\n", __FUNCTION__, unit);

                rt = DEV_TS_UNAVAILABLE;

                goto err;
            }
        }

        /* calc blk */
        rt = flexe_demux_divisor_data_get(ts_num, TCYCLE, &blk);

        if (DEV_SUCCESS != rt) {
            printf("%s: unit = 0x%x, get blk fail...\r\n", __FUNCTION__, unit);

            goto err;
        }
        
        /* config ch_property */
        cycle = (ts_num / blk) - 1;
        blk -= 1;

        data = cycle | ((blk & MASK_4_BIT) << SHIFT_BIT_8) | ((ts_num & MASK_8_BIT) << SHIFT_BIT_12);

		flexe_demux_property_b_set(unit, ch, &data);

        /* config ready_ts enable */
        for (index = 0; index < ts_num; index++) {
			flexe_demux_ts_en_b_set(unit, ts_id_info[index], 1);
        }

        /* set sour_phy and des_phy info */
		flexe_demux_phy_info_cfg(unit, sour_phy_index, phy_ts_mask, phy_ts_num, phy_num, traffic_info.enable, 1);
    }
    /* remove traffic and set cfg_info to default value */
    else {
        /* disable ready_ts enable */
        for (index = 0; index < ts_num; index++) {
			flexe_demux_ts_en_b_set(unit, ts_id_info[index], 0);
        }

        /* set calendar to zero */
        for (ts_index = 0; ts_index < ts_num; ts_index++) {
			flexe_demux_calendar_cfg_b_set(unit, ts_id_info[ts_index], 0);
        }

        /* set channel_own_tsgroup to zero */
		flexe_demux_channel_own_tsgroup_b_set(unit, ch, 0);

        /* set r_cfg_ram to default value */
        for (ts_index = 0; ts_index < ts_num; ts_index++) {
            memset(r_data, 0xff, sizeof(r_data));
            
			flexe_demux_rdrule_b_set(unit, ((ch * TS_NUM) + ts_index), r_data);
        }

        /* set w_cfg_ram to default_value */
        for (ts_index = 0; ts_index < ts_num; ts_index++) {
            if (ts_id_info[ts_index] < 40) {
                table_id = ts_id_info[ts_index] % 5;
                ram_depth = (ts_id_info[ts_index] / 5) * 10;

                for (index = 0; index < TCYCLE; index++) {
                    data = 0xf;
                    
					flexe_demux_wcfgram_b_set(unit, table_id, ram_depth + index, &data);
                }                 
            }
            else if ((ts_id_info[ts_index] >= 40) && (ts_id_info[ts_index] < 80)) {
                table_id = (ts_id_info[ts_index] % 5) + 5;
                ram_depth = ((ts_id_info[ts_index] - 40) / 5) * 10;

                for (index = 0; index < TCYCLE; index++) {
                    data = 0xf;
                    
					flexe_demux_wcfgram_b_set(unit, table_id, ram_depth + index, &data);
                }
            }
            else {
                printf("%s: unit = 0x%x, asic version's ts_id overstep the boundary...\r\n", __FUNCTION__, unit);

                rt = DEV_TS_UNAVAILABLE;

                goto err;
            }
        }

        /* set property to default value */
        data = 0xfffff;
        
		flexe_demux_property_b_set(unit, ch, &data);

        /* set sour_phy and des_phy info */
		flexe_demux_phy_info_cfg(unit, sour_phy_index, phy_ts_mask, phy_ts_num, phy_num, traffic_info.enable, 1);
    }

    free(flexe_demux_chip_info);

    flexe_demux_chip_info = NULL;

    return DEV_SUCCESS;

err:
    free(flexe_demux_chip_info);

    flexe_demux_chip_info = NULL;

    return rt;
}


/******************************************************************************
*
* FUNCTION
*
*     get module_demux_chip info.
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
*     flexe_demux_chip_info: chip_info pointer
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
dev_err_t demux_chip_info_get(UINT_8 unit, flexe_demux_info_t **flexe_demux_chip_info)
{
    dev_err_t rt = DEV_DEFAULT;
    
    /* check unit whether normal */
    if (unit > (DEV_NUM - 1)) {
        printf("%s: invalid unit = 0x%x...\r\n", __FUNCTION__, unit);
        
        return DEV_PARA_ERR;
    }

    /* check pointer whether normal */
    if (!flexe_demux_chip_info) {
        printf("%s: invalid pointer, unit = 0x%x...\r\n", __FUNCTION__, unit);
        
        return DEV_POINTER_NULL;
    }

    rt = flexe_demux_chip_info_get(unit, flexe_demux_chip_info);

    if (DEV_SUCCESS != rt) {
        printf("%s: unit = 0x%x, get demux_dev_info fail...\r\n", __FUNCTION__, unit);
    }

    return rt;
}


/******************************************************************************
*
* FUNCTION
*
*     demux_group's phy config debug.
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
*      liming        2018-03-21         1.0            initial
*
******************************************************************************/
dev_err_t demux_phy_group_cfg_debug(char *string)
{
    char *argv[600 + 1];
    UINT_8 argc = 0;
    UINT_8 para_index = 0;
    UINT_32 unit = 0;
    UINT_32 group_id = 0;
    UINT_32 phy_index = 0;
    UINT_32 phy_id[PHY_NUM];
    UINT_8 phy_id_cov[PHY_NUM] = { 0 };
    UINT_32 group_mode = 0;
    flexe_phy_mode phy_mode = FLEXE_PHY_DEFAULT;
    UINT_32 enable = 0;
    dev_err_t rt = DEV_DEFAULT;

    if (!string) {
        printf("%s: pointer is null...\r\n", __FUNCTION__);

        return DEV_POINTER_NULL;
    }

    argc = parse_line(string, argv);

    if (argc > 13) {
        printf("%s: invalid para numbers, para_number = 0x%x...\r\n", __FUNCTION__, argc);

        return DEV_PARA_ERR;
    }

    /* get parameter */
    for (para_index = 0; para_index < argc; para_index++) {
        /* unit para */
        if (0 == para_index) {
            sscanf(argv[para_index], "0x%x", &unit);
        }
        else if (1 == para_index) {
            sscanf(argv[para_index], "0x%x", &group_id);
        }
        else if (2 == para_index) {
            sscanf(argv[para_index], "0x%x", &phy_index);
        }
        else {
            if ((argc - 2) == para_index) {
                sscanf(argv[para_index], "0x%x", &group_mode);
            }
            else if ((argc - 1) == para_index) {
                sscanf(argv[para_index], "0x%x", &enable);
            }
            else {
                sscanf(argv[para_index], "0x%x", &phy_id[para_index - 3]);
                /*coverity[overrun-local : FALSE] */
                phy_id_cov[para_index - 3] = (UINT_8)phy_id[para_index - 3];
            }
        }
    }

    if (0 == group_mode) {
        phy_mode = FLEXE_PHY_50G;
    }
    else if (1 == group_mode) {
        phy_mode = FLEXE_PHY_100G;
    }
    else {
        printf("%s: phy_mode = 0x%x error...\r\n", __FUNCTION__, group_mode);

        return DEV_FAIL;
    }

    /* config group_phy */
    rt = demux_phy_group_cfg((UINT_8)unit, (UINT_8)group_id, (UINT_8)phy_index, phy_id_cov, phy_mode, (UINT_8)enable);

    if (DEV_SUCCESS == rt) {
        printf("%s: unit = 0x%x, group_id = 0x%x's phy cfg success...\r\n", __FUNCTION__, unit, group_id);
    }
    else {
        printf("%s: unit = 0x%x, group_id = 0x%x's phy cfg fail...\r\n", __FUNCTION__, unit, group_id);
    }

    return rt;
}


/******************************************************************************
*
* FUNCTION
*
*     demux's flexe_traffic config debug.
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
*      liming        2018-03-21         1.0            initial
*
******************************************************************************/
dev_err_t demux_traffic_flexe_cfg_debug(char *string)
{
    char *argv[600 + 1];
    UINT_8 argc = 0;
    UINT_8 para_index = 0;
    UINT_32 unit = 0;
    UINT_32 ch = 0;
    UINT_32 phy_index[PHY_NUM] = {0};
    UINT_8 phy_index_cov[PHY_NUM] = {0};
    UINT_32 ts_mask[PHY_NUM] = {0};
    UINT_32 phy_num = 0;
    UINT_32 enable = 0;
    dev_err_t rt = DEV_DEFAULT;

    if (!string) {
        printf("%s: pointer is null...\r\n", __FUNCTION__);

        return DEV_POINTER_NULL;
    }

    argc = parse_line(string, argv);

    if (argc > 20 || (0 != argc % 2)) {
        printf("%s: invalid para numbers, para_number = 0x%x...\r\n", __FUNCTION__, argc);

        return DEV_PARA_ERR;
    }

    /* get parameter */
    for (para_index = 0; para_index < argc; para_index++) {
        /* unit para */
        if (0 == para_index) {
            sscanf(argv[para_index], "0x%x", &unit);
        }
        else if (1 == para_index) {
            sscanf(argv[para_index], "0x%x", &ch);
        }
        else if ((para_index >= 2) && (para_index < ((argc - 4) / 2) + 2)) {
            sscanf(argv[para_index], "0x%x", &phy_index[para_index - 2]);
            /*coverity[overrun-local : FALSE] */
            phy_index_cov[para_index - 2] = (UINT_8)phy_index[para_index - 2];
        }
        else {
            if ((((argc - 4) / 2) + 2) == para_index) {
                sscanf(argv[para_index], "0x%x", &phy_num);
            }
            else if ((argc - 1) == para_index) {
                sscanf(argv[para_index], "0x%x", &enable);
            }
            else {
                sscanf(argv[para_index], "0x%x", &ts_mask[para_index - (((argc - 4) / 2) + 3)]);
            }
        }
    }

    /* config flexe_traffic */
    rt = demux_traffic_flexe_cfg((UINT_8)unit, (UINT_8)ch, phy_index_cov, (UINT_8)phy_num, ts_mask, (UINT_8)enable);

    if (DEV_SUCCESS == rt) {
        printf("%s: unit = 0x%x, ch = 0x%x's traffic cfg success...\r\n", __FUNCTION__, unit, ch);
    }
    else {
        printf("%s: unit = 0x%x, ch = 0x%x's traffic cfg fail...\r\n", __FUNCTION__, unit, ch);
    }

    return rt;
}

dev_err_t demux_traffic_flexe_b_cfg_debug(char *string)
{
    char *argv[600 + 1];
	UINT_8 argc = 0;
	UINT_8 para_index = 0;
	UINT_32 unit = 0;
	UINT_32 ch = 0;
	UINT_32 phy_index[PHY_NUM] = {0};
	UINT_8 phy_index_cov[PHY_NUM] = {0};
	UINT_32 ts_mask[PHY_NUM] = {0};
	UINT_32 phy_num = 0;
    UINT_32 enable = 0;
	dev_err_t rt = DEV_DEFAULT;

	if (!string) {
		printf("%s: pointer is null...\r\n", __FUNCTION__);

		return DEV_POINTER_NULL;
	}

	argc = parse_line(string, argv);

	if (argc > 20 || (0 != argc % 2)) {
		printf("%s: invalid para numbers, para_number = 0x%x...\r\n", __FUNCTION__, argc);

		return DEV_PARA_ERR;
	}

    /* get parameter */
	for (para_index = 0; para_index < argc; para_index++) {
		/* unit para */
		if (0 == para_index) {
			sscanf(argv[para_index], "0x%x", &unit);
		}
		else if (1 == para_index) {
			sscanf(argv[para_index], "0x%x", &ch);
		}
		else if ((para_index >= 2) && (para_index < ((argc - 4) / 2) + 2)) {
			sscanf(argv[para_index], "0x%x", &phy_index[para_index - 2]);
            /*coverity[overrun-local : FALSE] */
			phy_index_cov[para_index - 2] = (UINT_8)phy_index[para_index - 2];
		}
		else {
			if ((((argc - 4) / 2) + 2) == para_index) {
				sscanf(argv[para_index], "0x%x", &phy_num);
			}
			else if ((argc - 1) == para_index) {
				sscanf(argv[para_index], "0x%x", &enable);
			}
			else {
				sscanf(argv[para_index], "0x%x", &ts_mask[para_index - (((argc - 4) / 2) + 3)]);
			}
		}
	}

	/* config flexe_traffic */
	rt = demux_traffic_flexe_b_cfg((UINT_8)unit, (UINT_8)ch, phy_index_cov, (UINT_8)phy_num, ts_mask, (UINT_8)enable);

	if (DEV_SUCCESS == rt) {
		printf("%s: unit = 0x%x, ch = 0x%x's traffic cfg b success...\r\n", __FUNCTION__, unit, ch);
	}
	else {
		printf("%s: unit = 0x%x, ch = 0x%x's traffic cfg b fail...\r\n", __FUNCTION__, unit, ch);
	}

	return rt;
}
