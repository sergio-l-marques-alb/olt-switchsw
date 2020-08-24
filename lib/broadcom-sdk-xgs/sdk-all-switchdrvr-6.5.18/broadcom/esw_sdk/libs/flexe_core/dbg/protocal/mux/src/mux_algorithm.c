#include "mux_algorithm.h"
#include "hal.h"
#include <stdio.h>
#include <string.h>
#include <malloc.h>


flexe_mux_info_t *flexe_mux_info[DEV_NUM] = {NULL};  /* dev's mux pointer array */


/******************************************************************************
*
* FUNCTION
*
*     init dev's mux module.
*
* DESCRIPTION
*
*     malloc memory for mux module and init variable or structure.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     unit: dev's index
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
dev_err_t flexe_mux_init(UINT_8 unit)
{
	UINT_8 index = 0;
	dev_err_t rt = DEV_DEFAULT;
	
	/* check unit whether normal */
	if (unit > (DEV_NUM - 1)) 
	{
		printf("%s: invalid unit = 0x%x...\r\n", __FUNCTION__, unit);
		return DEV_PARA_ERR;
	}

	if (NULL != flexe_mux_info[unit])
	{
		printf("%s: unit = 0x%x have already initialized...\r\n", __FUNCTION__, unit);
		return DEV_INITIALIZED;
	}

	/* malloc memory for flexe_mux_info structure */
	flexe_mux_info[unit] = (flexe_mux_info_t *)malloc(sizeof(flexe_mux_info_t));
	if (NULL == flexe_mux_info[unit])
	{
		printf("%s: malloc memory fail for flexe_mux_info, unit = 0x%x...\r\n", __FUNCTION__, unit);
		return DEV_MM_ALLOC_FAIL;
	}

	/* storage the unit info */
	flexe_mux_info[unit]->unit = unit;

	/* set the ch and ts field to zero */
	memset(flexe_mux_info[unit]->ch_mask, 0, sizeof(flexe_mux_info[unit]->ch_mask));
	memset(flexe_mux_info[unit]->ts_mask, 0, sizeof(flexe_mux_info[unit]->ts_mask));
	memset(flexe_mux_info[unit]->ch_mask_b, 0, sizeof(flexe_mux_info[unit]->ch_mask_b));
	memset(flexe_mux_info[unit]->ts_mask_b, 0, sizeof(flexe_mux_info[unit]->ts_mask_b));
	flexe_mux_info[unit]->phy_mask = 0;
	flexe_mux_info[unit]->group_num = 0;

	/* set pcs_info to default_vaule */
	flexe_mux_info[unit]->pcs_info.unit = unit;
	flexe_mux_info[unit]->pcs_info.pcs_num = 0;

	for (index = 0; index < PHY_NUM; index++) 
	{
		flexe_mux_info[unit]->pcs_info.ch[index] = DEFAULT_VALUE;
		flexe_mux_info[unit]->pcs_info.phy_index[index] = DEFAULT_VALUE;
		flexe_mux_info[unit]->pcs_info.phy_mode[index] = FLEXE_PHY_DEFAULT;
	}

	/* set phy's group and phy_info to default value */
	for (index = 0; index < PHY_NUM; index++) 
	{
		flexe_mux_info[unit]->group_info[index].unit = unit;
		flexe_mux_info[unit]->group_info[index].group_id = DEFAULT_VALUE;
		flexe_mux_info[unit]->group_info[index].group_phy_mode = FLEXE_PHY_DEFAULT;
		flexe_mux_info[unit]->group_info[index].phy_num = 0;

		memset(flexe_mux_info[unit]->group_info[index].phy_index, DEFAULT_VALUE, sizeof(flexe_mux_info[unit]->group_info[index].phy_index));

        /* set flexe_phy_info to default value */
		flexe_mux_info[unit]->phy[index].unit = unit;
		flexe_mux_info[unit]->phy[index].group_id = DEFAULT_VALUE;
		flexe_mux_info[unit]->phy[index].phy_index = index;
		flexe_mux_info[unit]->phy[index].phy_mode = FLEXE_PHY_DEFAULT;
		flexe_mux_info[unit]->phy[index].ts_mask = 0;
		flexe_mux_info[unit]->phy[index].ts_num = 0;
		flexe_mux_info[unit]->phy[index].enable = TRAFFIC_REMOVE;
		flexe_mux_info[unit]->phy[index].available = 0;

		/* set flexe_logic_phy_info to default value */
		memset(flexe_mux_info[unit]->logic_phy[index].phy_comb_index, DEFAULT_VALUE, sizeof(flexe_mux_info[unit]->logic_phy[index].phy_comb_index));
		memset(flexe_mux_info[unit]->logic_phy[index].phy_comb_ts_mask, 0, sizeof(flexe_mux_info[unit]->logic_phy[index].phy_comb_ts_mask));
		
		flexe_mux_info[unit]->logic_phy[index].unit = unit;
		flexe_mux_info[unit]->logic_phy[index].group_id = DEFAULT_VALUE;
		flexe_mux_info[unit]->logic_phy[index].phy_index = index;
		flexe_mux_info[unit]->logic_phy[index].phy_master = DEFAULT_VALUE;
		flexe_mux_info[unit]->logic_phy[index].phy_comb_ts_num = 0;
		flexe_mux_info[unit]->logic_phy[index].enable = TRAFFIC_REMOVE;
		flexe_mux_info[unit]->logic_phy[index].available = 0;
	}

	/* set flexe_ch pointer to default null state */
	for (index = 0; index < CH_NUM; index++) 
	{
		flexe_mux_info[unit]->flexe_ch_info[index] = NULL;
		flexe_mux_info[unit]->flexe_ch_info_b[index] = NULL;
	}

	/* set flexe_ts pointer to default null state */
	for (index = 0; index < TS_NUM; index++) 
	{
		flexe_mux_info[unit]->flexe_ts_info[index] = NULL;
		flexe_mux_info[unit]->flexe_ts_info_b[index] = NULL;
	}

	/* funcptr init */
	flexe_mux_info[unit]->mm_alloc = flexe_mux_mm_alloc;

	/* malloc memory */
	rt = flexe_mux_info[unit]->mm_alloc(unit, FLEXE_MM_CH, CH_NUM);
	if (DEV_SUCCESS != rt) 
	{
		printf("%s: unit = 0x%x, ch memory alloc fail...\r\n", __FUNCTION__, unit);
		return rt;
	}

	rt = flexe_mux_info[unit]->mm_alloc(unit, FLEXE_MM_CH_B, CH_NUM);
	if (DEV_SUCCESS != rt) 
	{
		printf("%s: unit = 0x%x, ch_b memory alloc fail...\r\n", __FUNCTION__, unit);
		return rt;
	}
	
	rt = flexe_mux_info[unit]->mm_alloc(unit, FLEXE_MM_TS, TS_NUM);
	if (DEV_SUCCESS != rt) 
	{
		printf("%s: unit = 0x%x, ts memory alloc fail...\r\n", __FUNCTION__, unit);
		return rt;
	}

	rt = flexe_mux_info[unit]->mm_alloc(unit, FLEXE_MM_TS_B, TS_NUM);
	if (DEV_SUCCESS != rt) 
	{
		printf("%s: unit = 0x%x, ts memory alloc fail...\r\n", __FUNCTION__, unit);
		return rt;
	}

	printf("%s: unit = 0x%x, mux module malloc memory success...\r\n", __FUNCTION__, unit);

	/* set structure to default value */
	for (index = 0; index < CH_NUM; index++) 
	{
		flexe_mux_info[unit]->flexe_ch_info[index]->unit = unit;
		flexe_mux_info[unit]->flexe_ch_info[index]->ch_id = DEFAULT_VALUE;

		memset(flexe_mux_info[unit]->flexe_ch_info[index]->ts, 0, sizeof(flexe_mux_info[unit]->flexe_ch_info[index]->ts));

		flexe_mux_info[unit]->flexe_ch_info[index]->ts_num = 0;
		flexe_mux_info[unit]->flexe_ch_info[index]->ts_min = DEFAULT_VALUE;
		flexe_mux_info[unit]->flexe_ch_info[index]->enable = 0;
		flexe_mux_info[unit]->flexe_ch_info[index]->group_id = DEFAULT_VALUE;

		flexe_mux_info[unit]->flexe_ch_info_b[index]->unit = unit;
		flexe_mux_info[unit]->flexe_ch_info_b[index]->ch_id = DEFAULT_VALUE;

		memset(flexe_mux_info[unit]->flexe_ch_info_b[index]->ts, 0, sizeof(flexe_mux_info[unit]->flexe_ch_info_b[index]->ts));

		flexe_mux_info[unit]->flexe_ch_info_b[index]->ts_num = 0;
		flexe_mux_info[unit]->flexe_ch_info_b[index]->ts_min = DEFAULT_VALUE;
		flexe_mux_info[unit]->flexe_ch_info_b[index]->enable = 0;
	}

	for (index = 0; index < TS_NUM; index++) 
	{
		flexe_mux_info[unit]->flexe_ts_info[index]->unit = unit;
		flexe_mux_info[unit]->flexe_ts_info[index]->ch_id = DEFAULT_VALUE;
		flexe_mux_info[unit]->flexe_ts_info[index]->ts_id = DEFAULT_VALUE;

		memset(flexe_mux_info[unit]->flexe_ts_info[index]->w_col, DEFAULT_VALUE, sizeof(flexe_mux_info[unit]->flexe_ts_info[index]->w_col));
		
		memset(flexe_mux_info[unit]->flexe_ts_info[index]->r_col, DEFAULT_VALUE, sizeof(flexe_mux_info[unit]->flexe_ts_info[index]->r_col));

		flexe_mux_info[unit]->flexe_ts_info_b[index]->unit = unit;
		flexe_mux_info[unit]->flexe_ts_info_b[index]->ch_id = DEFAULT_VALUE;
		flexe_mux_info[unit]->flexe_ts_info_b[index]->ts_id = DEFAULT_VALUE;

		memset(flexe_mux_info[unit]->flexe_ts_info_b[index]->w_col, DEFAULT_VALUE, sizeof(flexe_mux_info[unit]->flexe_ts_info_b[index]->w_col));
		
		memset(flexe_mux_info[unit]->flexe_ts_info_b[index]->r_col, DEFAULT_VALUE, sizeof(flexe_mux_info[unit]->flexe_ts_info_b[index]->r_col));
	}

	printf("%s: unit = 0x%x, mux module initialized success...\r\n", __FUNCTION__, unit);

	return DEV_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     deinitial dev's mux module.
*
* DESCRIPTION
*
*     free memory for mux module and it's info.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     unit: dev's index
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
dev_err_t flexe_mux_uninit(UINT_8 unit)
{
    UINT_8 index = 0;
    
    /* check unit whether normal */
    if (unit > (DEV_NUM - 1)) 
	{
        printf("%s: invalid unit = 0x%x...\r\n", __FUNCTION__, unit);
        return DEV_PARA_ERR;
    }

	if (NULL == flexe_mux_info[unit])
	{
        printf("%s: unit = 0x%x not initialized...\r\n", __FUNCTION__, unit);
        return DEV_DEINITIALIZED;
	}

	/* free mux_ch pointer */
	for (index = 0; index < CH_NUM; index++) 
	{
		free(flexe_mux_info[unit]->flexe_ch_info[index]);
		free(flexe_mux_info[unit]->flexe_ch_info_b[index]);

		flexe_mux_info[unit]->flexe_ch_info[index] = NULL;
		flexe_mux_info[unit]->flexe_ch_info_b[index] = NULL;
	}

	/* free mux_ts pointer */
	for (index = 0; index < TS_NUM; index++) 
	{
		free(flexe_mux_info[unit]->flexe_ts_info[index]);
		free(flexe_mux_info[unit]->flexe_ts_info_b[index]);

		flexe_mux_info[unit]->flexe_ts_info[index] = NULL;
		flexe_mux_info[unit]->flexe_ts_info_b[index] = NULL;
	}

	/* free mux global pointer and set it to null */
	free(flexe_mux_info[unit]);

	flexe_mux_info[unit] = NULL;

	printf("%s: unit = 0x%x, mux module uninit success...\r\n", __FUNCTION__, unit);

	return DEV_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     mux's global_phy_flexe_pcs config.
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
*     flexe_phy_mode: phy's working mode
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
dev_err_t flexe_mux_phy_mode_cfg(UINT_8 unit, flexe_phy_mode *phy_mode)
{
    UINT_8 index = 0;
    UINT_8 payload = 0;

    /* check unit whether normal */
    if (unit > (DEV_NUM - 1)) {
        printf("%s: invalid unit = 0x%x...\r\n", __FUNCTION__, unit);
        
        return DEV_PARA_ERR;
    }

    /* check pointer whether null */
    if (!phy_mode) {
        printf("%s: unit = 0x%x, pointer null...\r\n", __FUNCTION__, unit);

        return DEV_POINTER_NULL;
    }

    /* check unit whether initialized */
    if (flexe_mux_info[unit]) {
        for (index = 0; index < PHY_NUM; index++) {
            if (0 == index % 2) {
                payload = 0;
            }

            /* 50G mode */
            if ((FLEXE_PHY_50G == *(phy_mode + index)) || (PCS_PHY_50G == *(phy_mode + index))) {
                payload += PHY_50G_PL * 5;
            }
            /* 100G mode */
            else if ((FLEXE_PHY_100G == *(phy_mode + index)) || (PCS_PHY_100G == *(phy_mode + index))) {
                payload += PHY_100G_PL * 5;
            }
            /* default mode */
            else {
                *(phy_mode + index) = FLEXE_PHY_DEFAULT;
            }

            if (0 == (index + 1) % 2) {
                if (payload > (PHY_100G_PL * 5)) {
                    printf("%s: unit = 0x%x, phy_mode cfg over payload...\r\n", __FUNCTION__, unit);

                    return DEV_FAIL;
                }
            }
        }

        /* storage the phy_mode */
        for (index = 0; index < PHY_NUM; index++) {
            flexe_mux_info[unit]->phy[index].phy_mode = *(phy_mode + index);
        }
    }
    /* unit's mux not initialized */
    else {
        printf("%s: unit = 0x%x not initialized...\r\n", __FUNCTION__, unit);

        return DEV_DEINITIALIZED;
    }
    
    return DEV_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*    get mux's global_phy_flexe mode.
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
*     flexe_phy_mode: phy's working mode
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
dev_err_t flexe_mux_phy_mode_out(UINT_8 unit, flexe_phy_mode *phy_mode)
{
    UINT_8 index = 0;

    /* check unit whether normal */
    if (unit > (DEV_NUM - 1)) {
        printf("%s: invalid unit = 0x%x...\r\n", __FUNCTION__, unit);
        
        return DEV_PARA_ERR;
    }

    /* check pointer whether null */
    if (!phy_mode) {
        printf("%s: unit = 0x%x, pointer null...\r\n", __FUNCTION__, unit);

        return DEV_POINTER_NULL;
    }

    /* check unit whether initialized */
    if (flexe_mux_info[unit]) {
        /* get the phy_mode */
        for (index = 0; index < PHY_NUM; index++) {
            *(phy_mode + index) = flexe_mux_info[unit]->phy[index].phy_mode;
        }
    }
    /* unit's mux not initialized */
    else {
        printf("%s: unit = 0x%x not initialized...\r\n", __FUNCTION__, unit);

        return DEV_DEINITIALIZED;
    }
    
    return DEV_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     mux's phy_pcs config.
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
*     flexe_phy_mode: phy's working mode
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
dev_err_t flexe_mux_phy_pcs_cfg(UINT_8 unit,
                                UINT_8 ch,
                                UINT_8 phy_index,
                                flexe_phy_mode phy_mode,
                                UINT_8 enable)
{
    UINT_8 index = 0;
    UINT_8 shift_barrel = 0;
    bool flag = false;
    
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
        return DEV_PARA_ERR;
    }

    /* check phy_index whether normal */
    if (phy_index > (PHY_NUM - 1)) 
	{
        printf("%s: unit = 0x%x, invalid phy_index = 0x%x...\r\n", __FUNCTION__, unit, phy_index);
        return DEV_PARA_ERR;
    }

    /* check phy_index whether normal */
    if ((PCS_PHY_50G != phy_mode) && (PCS_PHY_100G != phy_mode) && (PCS_PHY_200G != phy_mode)) 
	{
        printf("%s: unit = 0x%x, invalid phy_mode = %d...\r\n", __FUNCTION__, unit, phy_mode);
        return DEV_PARA_ERR;
    }

	/* check add_remove info */
	if ((TRAFFIC_REMOVE != enable) && (TRAFFIC_ADD != enable)) 
	{
		printf("%s: unit = 0x%x, invalid enable = 0x%x...\r\n", __FUNCTION__, unit, enable);
		return DEV_PARA_ERR;
	}

	/* check unit whether initialized */
	if (NULL == flexe_mux_info[unit]) 
	{
        printf("%s: unit = 0x%x not initialized...\r\n", __FUNCTION__, unit);
        return DEV_DEINITIALIZED;
	}
	
	/* check pcs mode whether normal */
	if (flexe_mux_info[unit]->phy[phy_index].phy_mode != phy_mode) 
	{
		printf("%s: unit = 0x%x, pcs phy_mode = %d not match the config phy_mode...\r\n", __FUNCTION__, unit, phy_mode);
		return DEV_FAIL;
	}

    /* check phy's working state */
	if (enable == flexe_mux_info[unit]->phy[phy_index].enable) 
	{
		printf("%s: unit = 0x%x, pcs_phy = 0x%x's state error...\r\n", __FUNCTION__, unit, phy_index);
		return DEV_FAIL;
	}

	/* add pcs traffic */
	if (TRAFFIC_ADD == enable) 
	{
		if ((flexe_mux_info[unit]->ch_mask[ch / REG_WIDTH] >> (ch % REG_WIDTH)) & MASK_1_BIT) 
		{
			printf("%s: unit = 0x%x, ch = 0x%x have already cfg...\r\n", __FUNCTION__, unit, ch);
			return DEV_CH_UNAVAILABLE;
		}
		else /* ch, phy_index and phy_mode available */
		{
			flexe_mux_info[unit]->phy[phy_index].enable = enable;
			flexe_mux_info[unit]->ch_mask[ch / REG_WIDTH] |= ((MASK_1_BIT << (ch % REG_WIDTH)) & MASK_32_BIT);
			flexe_mux_info[unit]->flexe_ch_info[ch]->ch_id = ch;
			flexe_mux_info[unit]->flexe_ch_info[ch]->mode = phy_mode;
			flexe_mux_info[unit]->flexe_ch_info[ch]->enable = TRAFFIC_ADD;
			flexe_mux_info[unit]->pcs_info.ch[flexe_mux_info[unit]->pcs_info.pcs_num] = ch;
			flexe_mux_info[unit]->pcs_info.phy_index[flexe_mux_info[unit]->pcs_info.pcs_num] = phy_index;
			flexe_mux_info[unit]->pcs_info.phy_mode[flexe_mux_info[unit]->pcs_info.pcs_num] = phy_mode;
			flexe_mux_info[unit]->pcs_info.pcs_num++;	
		}
	}
	else /* remove traffic */
	{
		if (!((flexe_mux_info[unit]->ch_mask[ch / REG_WIDTH] >> (ch % REG_WIDTH)) & MASK_1_BIT)) 
		{
			printf("%s: unit = 0x%x, ch = 0x%x not cfg...\r\n", __FUNCTION__, unit, ch);
			return DEV_CH_UNAVAILABLE;
		}
		else /* delete traffic_info */
		{
			for (index = 0; index < flexe_mux_info[unit]->pcs_info.pcs_num; index++) 
			{
				if ((phy_index == flexe_mux_info[unit]->pcs_info.phy_index[index]) && (ch == flexe_mux_info[unit]->pcs_info.ch[index])) 
				{
					if (1 == flexe_mux_info[unit]->pcs_info.pcs_num) 
					{
						flexe_mux_info[unit]->pcs_info.ch[index] = DEFAULT_VALUE;
						flexe_mux_info[unit]->pcs_info.phy_index[index] = DEFAULT_VALUE;
						flexe_mux_info[unit]->pcs_info.phy_mode[index] = FLEXE_PHY_DEFAULT;
					}
					else 
					{
						for (shift_barrel = index; shift_barrel < flexe_mux_info[unit]->pcs_info.pcs_num - 1; shift_barrel++) 
						{
							flexe_mux_info[unit]->pcs_info.ch[shift_barrel] = flexe_mux_info[unit]->pcs_info.ch[shift_barrel + 1];
							flexe_mux_info[unit]->pcs_info.phy_index[shift_barrel] = flexe_mux_info[unit]->pcs_info.phy_index[shift_barrel + 1];
							flexe_mux_info[unit]->pcs_info.phy_mode[shift_barrel] = flexe_mux_info[unit]->pcs_info.phy_mode[shift_barrel + 1];

							if (shift_barrel == flexe_mux_info[unit]->pcs_info.pcs_num - 2) 
							{
								flexe_mux_info[unit]->pcs_info.ch[shift_barrel + 1] = DEFAULT_VALUE;
								flexe_mux_info[unit]->pcs_info.phy_index[shift_barrel + 1] = DEFAULT_VALUE;
								flexe_mux_info[unit]->pcs_info.phy_mode[shift_barrel + 1] = FLEXE_PHY_DEFAULT;
							}
						}
					}

					flag = true;

					break;	
				}
			}

            /* parameter error */
			if (false == flag) 
			{
				printf("%s: unit = 0x%x, ch = 0x%x not matched phy = 0x%x...\r\n", __FUNCTION__, unit, ch, phy_index);
				return DEV_FAIL;
			}

			flexe_mux_info[unit]->phy[phy_index].enable = enable;
			flexe_mux_info[unit]->ch_mask[ch / REG_WIDTH] &= ~((MASK_1_BIT << (ch % REG_WIDTH)) & MASK_32_BIT);
			flexe_mux_info[unit]->flexe_ch_info[ch]->ch_id = DEFAULT_VALUE;
			flexe_mux_info[unit]->flexe_ch_info[ch]->mode = FLEXE_PHY_DEFAULT;
			flexe_mux_info[unit]->flexe_ch_info[ch]->enable = TRAFFIC_REMOVE;
			flexe_mux_info[unit]->pcs_info.pcs_num--;
		}
	}
	
	return DEV_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     get mux's phy_pcs.
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
*     pcs_num: current pcs_num pointer
*     phy_pcs_info: current phy's pcs info
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
dev_err_t flexe_mux_phy_pcs_out(UINT_8 unit,
                                UINT_8 *pcs_num,
                                flexe_mux_phy_info_t *phy_pcs_info)
{
    UINT_8 index = 0;
    
    /* check unit whether normal */
    if (unit > (DEV_NUM - 1)) 
	{
        printf("%s: invalid unit = 0x%x...\r\n", __FUNCTION__, unit);
        return DEV_PARA_ERR;
    }
    /* check pointer whether null */
    if (!pcs_num || !phy_pcs_info) 
	{
        printf("%s: unit = 0x%x, pointer is null...\r\n", __FUNCTION__, unit);
        return DEV_POINTER_NULL;
    }

	if (flexe_mux_info[unit]) /* check unit whether initialized */
	{
        for (index = 0; index < flexe_mux_info[unit]->pcs_info.pcs_num; index++) 
		{
			 (*(phy_pcs_info + index)).unit = flexe_mux_info[unit]->pcs_info.unit;
			 (*(phy_pcs_info + index)).phy_index = flexe_mux_info[unit]->pcs_info.phy_index[index];
			 (*(phy_pcs_info + index)).phy_mode = flexe_mux_info[unit]->pcs_info.phy_mode[index];
    	}

		*pcs_num = flexe_mux_info[unit]->pcs_info.pcs_num;
	}
	else /* unit's mux not initialized */
	{
        printf("%s: unit = 0x%x not initialized...\r\n", __FUNCTION__, unit);
        return DEV_DEINITIALIZED;
    }
    
    return DEV_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     calculation the greatest common divisor of input data.
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
*    input_data_1: input data
*    input_data_2: input data
*    result_out: output data
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
dev_err_t flexe_mux_divisor_data_get(UINT_8 input_data_1, UINT_8 input_data_2, UINT_8 *result_out)
{
    UINT_8 data_max = 0;
    UINT_8 data_min = 0;
    UINT_8 remainder = 0;
    
    /* check pointer whether is null */
    if (!result_out) {
        printf("%s: pointer is null...\r\n", __FUNCTION__);

        return DEV_POINTER_NULL;
    }

    /* check input data value */
    if (0 == input_data_1 || 0 == input_data_2) {
        printf("%s: input para invalid, data_1 = 0x%x, data_2 = 0x%x...\r\n",
                                                                __FUNCTION__,
                                                                input_data_1,
                                                               input_data_2);

        return DEV_PARA_ERR;
    }

    /* get the input max and min value */
    if (input_data_1 >= input_data_2) {
        data_max = input_data_1;
        data_min = input_data_2;
    }
    else {
        data_max = input_data_2;
        data_min = input_data_1;
    }

    /* get the greatest common divisor between input_data_1 and input_data_2 */
    remainder = data_max % data_min;

    /* calculation the greatest common divisor */
    if (remainder) {
        data_max = data_min;
        data_min = remainder;

        while (remainder) {
            remainder = data_max % data_min;

            if (remainder) {
                data_max = data_min;
                data_min = remainder;
            }
            else {
                *result_out = data_min;
            }
        }
    }
    /* the data_min is the greatest common divisor */
    else {
        *result_out = data_min;
    }

    return DEV_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     malloc memory for flexe_mux's structure.
*
* DESCRIPTION
*
*     malloc memory for structure's variable
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     unit: dev's index, increase one by one
*     mm_type: variable type
*     size: memory size
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
dev_err_t flexe_mux_mm_alloc(UINT_8 unit, flexe_mm_type mm_type, UINT_8 size)
{
    dev_err_t rt = DEV_DEFAULT;
    
    /* check unit whether normal */
    if (unit > (DEV_NUM - 1)) {
        printf("%s: invalid unit = 0x%x...\r\n", __FUNCTION__, unit);
        
        return DEV_PARA_ERR;
    }

    /* make sure unit only initialized one time */
    if (flexe_mux_info[unit]) {
        switch (mm_type) {
            case FLEXE_MM_CH:
                rt = flexe_mux_ch_mm_alloc(unit, size);
                
                break;

            case FLEXE_MM_TS:
                rt = flexe_mux_ts_mm_alloc(unit, size);
                
                break;

            case FLEXE_MM_CH_B:
                rt = flexe_mux_ch_b_mm_alloc(unit, size);
                
                break;

            case FLEXE_MM_TS_B:
                rt = flexe_mux_ts_b_mm_alloc(unit, size);
                
                break;

            default:
                
                break;
        }

        if (DEV_SUCCESS != rt) {
            printf("%s: unit = 0x%x, flexe_mm_type = %d malloc memory fail...\r\n", __FUNCTION__, unit, mm_type);
        }

        return rt;
    }
    /* unit's flexe_mux not initialized */
    else {
        printf("%s: unit = 0x%x not initialized...\r\n", __FUNCTION__, unit);

        return DEV_DEINITIALIZED;
    }
}


/******************************************************************************
*
* FUNCTION
*
*     malloc memory for flexe_mux's ch structure.
*
* DESCRIPTION
*
*     malloc memory for structure's variable
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     unit: dev's index, increase one by one
*     size: memory size
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
dev_err_t flexe_mux_ch_mm_alloc(UINT_8 unit, UINT_8 size)
{
    UINT_8 index = 0;
    UINT_8 ch_idx = 0;
    
    /* check unit whether normal */
    if (unit > (DEV_NUM - 1)) {
        printf("%s: invalid unit = 0x%x...\r\n", __FUNCTION__, unit);
        
        return DEV_PARA_ERR;
    }

    /* make sure unit pointer not null */
    if (flexe_mux_info[unit]) {
        /* check ch whether malloc memory */
        for (index = 0; index < CH_NUM; index++) {
            if (flexe_mux_info[unit]->flexe_ch_info[index]) {
                printf("%s: unit = 0x%x, ch's memory have already malloc...\r\n", __FUNCTION__, unit);

                return DEV_MM_ALLOC_FAIL;
            }
        }

        /* malloc memory for flexe's ch structure */
        for (ch_idx = 0; ch_idx < size; ch_idx++) {
            flexe_mux_info[unit]->flexe_ch_info[ch_idx] = (flexe_mux_ch_info_t *)malloc(sizeof(flexe_mux_ch_info_t));

            /* check malloc whether success */
            if (!flexe_mux_info[unit]->flexe_ch_info[ch_idx]) {
                if (0 != ch_idx) {
                    for (index = 0; index < ch_idx; index++) {
                        free(flexe_mux_info[unit]->flexe_ch_info[index]);

                        flexe_mux_info[unit]->flexe_ch_info[index] = NULL;
                    }
                }

                printf("%s: unit = 0x%x, ch's memory malloc fail...\r\n", __FUNCTION__, unit);

                return DEV_MM_ALLOC_FAIL;
            }
        }

        printf("%s: unit = 0x%x, malloc ch's memory size = 0x%x success...\r\n", __FUNCTION__, unit, size);

        return DEV_SUCCESS;
    }
    /* unit's mux not initialized */
    else {
        printf("%s: unit = 0x%x not initialized...\r\n", __FUNCTION__, unit);

        return DEV_DEINITIALIZED;
    }
}


/******************************************************************************
*
* FUNCTION
*
*     malloc memory for flexe_mux's ch_b structure.
*
* DESCRIPTION
*
*     malloc memory for structure's variable
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     unit: dev's index, increase one by one
*     size: memory size
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
*      liming        2018-07-03         1.0            initial
*
******************************************************************************/
dev_err_t flexe_mux_ch_b_mm_alloc(UINT_8 unit, UINT_8 size)
{
    UINT_8 index = 0;
    UINT_8 ch_idx = 0;
    
    /* check unit whether normal */
    if (unit > (DEV_NUM - 1)) {
        printf("%s: invalid unit = 0x%x...\r\n", __FUNCTION__, unit);
        
        return DEV_PARA_ERR;
    }

    /* make sure unit pointer not null */
    if (flexe_mux_info[unit]) {
        /* check ch whether malloc memory */
        for (index = 0; index < CH_NUM; index++) {
            if (flexe_mux_info[unit]->flexe_ch_info_b[index]) {
                printf("%s: unit = 0x%x, ch's memory have already malloc...\r\n", __FUNCTION__, unit);

                return DEV_MM_ALLOC_FAIL;
            }
        }

        /* malloc memory for flexe's ch structure */
        for (ch_idx = 0; ch_idx < size; ch_idx++) {
            flexe_mux_info[unit]->flexe_ch_info_b[ch_idx] = (flexe_mux_ch_info_t *)malloc(sizeof(flexe_mux_ch_info_t));

            /* check malloc whether success */
            if (!flexe_mux_info[unit]->flexe_ch_info_b[ch_idx]) {
                if (0 != ch_idx) {
                    for (index = 0; index < ch_idx; index++) {
                        free(flexe_mux_info[unit]->flexe_ch_info_b[index]);

                        flexe_mux_info[unit]->flexe_ch_info_b[index] = NULL;
                    }
                }

                printf("%s: unit = 0x%x, ch's memory malloc fail...\r\n", __FUNCTION__, unit);

                return DEV_MM_ALLOC_FAIL;
            }
        }

        printf("%s: unit = 0x%x, malloc ch's memory size = 0x%x success...\r\n", __FUNCTION__, unit, size);

        return DEV_SUCCESS;
    }
    /* unit's mux not initialized */
    else {
        printf("%s: unit = 0x%x not initialized...\r\n", __FUNCTION__, unit);

        return DEV_DEINITIALIZED;
    }
}


/******************************************************************************
*
* FUNCTION
*
*     malloc memory for flexe_mux's ts structure.
*
* DESCRIPTION
*
*     malloc memory for structure's variable
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     unit: dev's index, increase one by one
*     size: memory size
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
dev_err_t flexe_mux_ts_mm_alloc(UINT_8 unit, UINT_8 size)
{
    UINT_8 index = 0;
    UINT_8 ts_idx = 0;
    
    /* check unit whether normal */
    if (unit > (DEV_NUM - 1)) {
        printf("%s: invalid unit = 0x%x...\r\n", __FUNCTION__, unit);
        
        return DEV_PARA_ERR;
    }

    /* make sure unit pointer not null */
    if (flexe_mux_info[unit]) {
        /* check ts whether malloc memory */
        for (index = 0; index < TS_NUM; index++) {
            if (flexe_mux_info[unit]->flexe_ts_info[index]) {
                printf("%s: unit = 0x%x, ts's memory have already malloc...\r\n", __FUNCTION__, unit);

                return DEV_MM_ALLOC_FAIL;
            }
        }

        /* malloc memory for flexe's ts structure */
        for (ts_idx = 0; ts_idx < size; ts_idx++) {
            flexe_mux_info[unit]->flexe_ts_info[ts_idx] = (flexe_mux_ts_info_t *)malloc(sizeof(flexe_mux_ts_info_t));

            /* check malloc whether success */
            if (!flexe_mux_info[unit]->flexe_ts_info[ts_idx]) {
                if (0 != ts_idx) {
                    for (index = 0; index < ts_idx; index++) {
                        free(flexe_mux_info[unit]->flexe_ts_info[index]);

                        flexe_mux_info[unit]->flexe_ts_info[index] = NULL;
                    }
                }

                printf("%s: unit = 0x%x, ts's memory malloc fail...\r\n", __FUNCTION__, unit);

                return DEV_MM_ALLOC_FAIL;
            }
        }

        printf("%s: unit = 0x%x, malloc ts's memory size = 0x%x success...\r\n", __FUNCTION__, unit, size);

        return DEV_SUCCESS;
    }
    /* unit's mux not initialized */
    else {
        printf("%s: unit = 0x%x not initialized...\r\n", __FUNCTION__, unit);

        return DEV_DEINITIALIZED;
    }
}


/******************************************************************************
*
* FUNCTION
*
*     malloc memory for flexe_mux's ts_b structure.
*
* DESCRIPTION
*
*     malloc memory for structure's variable
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     unit: dev's index, increase one by one
*     size: memory size
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
*      liming        2018-07-03         1.0            initial
*
******************************************************************************/
dev_err_t flexe_mux_ts_b_mm_alloc(UINT_8 unit, UINT_8 size)
{
    UINT_8 index = 0;
    UINT_8 ts_idx = 0;
    
    /* check unit whether normal */
    if (unit > (DEV_NUM - 1)) {
        printf("%s: invalid unit = 0x%x...\r\n", __FUNCTION__, unit);
        
        return DEV_PARA_ERR;
    }

    /* make sure unit pointer not null */
    if (flexe_mux_info[unit]) {
        /* check ts whether malloc memory */
        for (index = 0; index < TS_NUM; index++) {
            if (flexe_mux_info[unit]->flexe_ts_info_b[index]) {
                printf("%s: unit = 0x%x, ts's memory have already malloc...\r\n", __FUNCTION__, unit);

                return DEV_MM_ALLOC_FAIL;
            }
        }

        /* malloc memory for flexe's ts structure */
        for (ts_idx = 0; ts_idx < size; ts_idx++) {
            flexe_mux_info[unit]->flexe_ts_info_b[ts_idx] = (flexe_mux_ts_info_t *)malloc(sizeof(flexe_mux_ts_info_t));

            /* check malloc whether success */
            if (!flexe_mux_info[unit]->flexe_ts_info_b[ts_idx]) {
                if (0 != ts_idx) {
                    for (index = 0; index < ts_idx; index++) {
                        free(flexe_mux_info[unit]->flexe_ts_info_b[index]);

                        flexe_mux_info[unit]->flexe_ts_info_b[index] = NULL;
                    }
                }

                printf("%s: unit = 0x%x, ts's memory malloc fail...\r\n", __FUNCTION__, unit);

                return DEV_MM_ALLOC_FAIL;
            }
        }

        printf("%s: unit = 0x%x, malloc ts's memory size = 0x%x success...\r\n", __FUNCTION__, unit, size);

        return DEV_SUCCESS;
    }
    /* unit's mux not initialized */
    else {
        printf("%s: unit = 0x%x not initialized...\r\n", __FUNCTION__, unit);

        return DEV_DEINITIALIZED;
    }
}


/******************************************************************************
*
* FUNCTION
*
*     dev's mux module algorithm input.
*
* DESCRIPTION
*
*     algorithm implementation.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     traffic_info: mux's input info
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
dev_err_t flexe_mux_algorithm_cfg(flexe_mux_traffic_info_t traffic_info)
{
    UINT_8 ts[TS_NUM] = {0};
    UINT_8 ts_num = 0;
    UINT_8 index = 0;
    UINT_8 t_cycle = 0;
    UINT_8 r_column = 1;
    UINT_8 r_col_offset = 1;
    UINT_8 divisor = 0;
    UINT_32 ts_data_cnt = 0;
    UINT_8 w_row = 0;
    UINT_8 w_row_offset = 0;
    UINT_8 w_col_offset = 0;
    UINT_8 pos_flag[TS_NUM][TCYCLE] = {{0}};

    /* check unit whether normal */
    if (traffic_info.unit > (DEV_NUM - 1)) {
        printf("%s: invalid unit = 0x%x...\r\n", __FUNCTION__, traffic_info.unit);
        
        return DEV_PARA_ERR;
    }

    /* check ch info */
    if (traffic_info.ch > (CH_NUM - 1)) {
        printf("%s: unit = 0x%x, invalid traffic_ch_id = 0x%x...\r\n",
                                                         __FUNCTION__,
                                                    traffic_info.unit,
                                                     traffic_info.ch);

        return DEV_PARA_ERR;
    }

    /* check ts info */
    if (traffic_info.ts[TS_FIELD - 1] > 0xffff) {
        printf("%s: unit = 0x%x, invalid traffic_ts[%d] = 0x%x...\r\n",
                                                           __FUNCTION__,
                                                      traffic_info.unit,
                                                           TS_FIELD - 1,
                                         traffic_info.ts[TS_FIELD - 1]);

        return DEV_PARA_ERR;
    }

    /* check add_remove info */
    if ((TRAFFIC_REMOVE != traffic_info.enable) && (TRAFFIC_ADD != traffic_info.enable)) {
        printf("%s: unit = 0x%x, invalid traffic_enable = 0x%x...\r\n",
                                                          __FUNCTION__,
                                                     traffic_info.unit,
                                                  traffic_info.enable);

        return DEV_PARA_ERR;
    }

    /* get the ts info */
    for (index = 0; index < TS_NUM; index++) {
        if ((traffic_info.ts[index / REG_WIDTH] >> (index % REG_WIDTH)) & MASK_1_BIT) {
            ts[ts_num] = index;
            ts_num++;
        }
    }

    memset(ts, DEFAULT_VALUE, sizeof(ts));

    for (index = 0; index < ts_num; index++) {
        ts[index] = traffic_info.ts_sort[index];
    }

    /* get the ts_num and tcycle's divisor */
    flexe_mux_divisor_data_get(ts_num, TCYCLE, &divisor);

    printf("%s: ts_num = 0x%x, tcyle = 0x%x, divisor = 0x%x...\r\n", __FUNCTION__, ts_num, TCYCLE, divisor);

    /* add traffic, calculate the mux algorithm result */
    if (TRAFFIC_ADD == traffic_info.enable) {
        memset(pos_flag, 0, sizeof(pos_flag));
        
        /* mux_ram_write_rule */
        for (index = 0; index < ts_num; index++) {
            /* row and col's bit_start position */
            if (0 == index) {
                w_row_offset = ts[index];
                w_row = index;
                w_col_offset = index;
            }

            for (t_cycle = 0; t_cycle < TCYCLE; t_cycle++) {
                /* find the col position */
                while (1 == pos_flag[w_row_offset][w_col_offset]) {
                    w_col_offset++;

                    if (w_col_offset > (TCYCLE - 1)) {
                        w_col_offset -= TCYCLE;
                    }
                }
                
                flexe_mux_info[traffic_info.unit]->flexe_ts_info[ts[index]]->w_col[w_col_offset] = ts[w_row];
                pos_flag[w_row_offset][w_col_offset] = 1;
                w_row++;
                w_col_offset++;

                if (w_row > (ts_num - 1)) {
                    w_row -= ts_num;    
                }

                /* row shift to next row */
                w_row_offset = ts[w_row];

                if (w_col_offset > (TCYCLE - 1)) {
                    w_col_offset -= TCYCLE;
                }
            }
        }

        /* mux_ram_read_rule */
        for (t_cycle = 0; t_cycle < TCYCLE; t_cycle++) {
            /* one sub_cycle is bit_end, so offset add one */
            if (ts_data_cnt == ((ts_num * TCYCLE) / divisor)) {
                ts_data_cnt = 0;
                r_col_offset++;
                r_column = r_col_offset;
            }
            
            for (index = 0; index < ts_num; index++) {
                ts_data_cnt++;

                if (r_column > TCYCLE)
                    r_column %= TCYCLE;

                /* storage the write operation result */
                flexe_mux_info[traffic_info.unit]->flexe_ts_info[ts[index]]->r_col[t_cycle] = r_column - 1;
                r_column++;
            }
        }

        /* storage the ts's info */
        for (index = 0; index < ts_num; index++) {
            flexe_mux_info[traffic_info.unit]->flexe_ts_info[ts[index]]->unit = traffic_info.unit;
            flexe_mux_info[traffic_info.unit]->flexe_ts_info[ts[index]]->ch_id = traffic_info.ch;
            flexe_mux_info[traffic_info.unit]->flexe_ts_info[ts[index]]->ts_id = ts[index];
        }

        /* storage the ch's info */
        flexe_mux_info[traffic_info.unit]->flexe_ch_info[traffic_info.ch]->unit = traffic_info.unit;
        flexe_mux_info[traffic_info.unit]->flexe_ch_info[traffic_info.ch]->ch_id = traffic_info.ch;

        for (index = 0; index < TS_FIELD; index++)
            flexe_mux_info[traffic_info.unit]->flexe_ch_info[traffic_info.ch]->ts[index] = traffic_info.ts[index];
        
        flexe_mux_info[traffic_info.unit]->flexe_ch_info[traffic_info.ch]->ts_num = ts_num;
        flexe_mux_info[traffic_info.unit]->flexe_ch_info[traffic_info.ch]->ts_min = ts[0];
        flexe_mux_info[traffic_info.unit]->flexe_ch_info[traffic_info.ch]->enable = TRAFFIC_ADD;
		flexe_mux_info[traffic_info.unit]->flexe_ch_info[traffic_info.ch]->group_id = traffic_info.group_id;
        /* storage the dev's ch_mask and ts_mask info */
        flexe_mux_info[traffic_info.unit]->ch_mask[traffic_info.ch / REG_WIDTH] |=
                    ((MASK_1_BIT << (traffic_info.ch % REG_WIDTH)) & MASK_32_BIT);

        for (index = 0; index < TS_FIELD; index++)
            flexe_mux_info[traffic_info.unit]->ts_mask[index] |= traffic_info.ts[index];
    }
    /* remove traffic, set structure to default value */
    else {
        /* set the write operation result to default value */
        for (index = 0; index < ts_num; index++) {
            for (t_cycle = 0; t_cycle < TCYCLE; t_cycle++) {
                flexe_mux_info[traffic_info.unit]->flexe_ts_info[ts[index]]->w_col[t_cycle] = DEFAULT_VALUE;
                flexe_mux_info[traffic_info.unit]->flexe_ts_info[ts[index]]->r_col[t_cycle] = DEFAULT_VALUE;
            }
        }

        /* set the ts's info to default value */
        for (index = 0; index < ts_num; index++) {
            flexe_mux_info[traffic_info.unit]->flexe_ts_info[ts[index]]->unit = traffic_info.unit;
            flexe_mux_info[traffic_info.unit]->flexe_ts_info[ts[index]]->ch_id = DEFAULT_VALUE;
            flexe_mux_info[traffic_info.unit]->flexe_ts_info[ts[index]]->ts_id = DEFAULT_VALUE;
        }

        /* storage the ch's info */
        flexe_mux_info[traffic_info.unit]->flexe_ch_info[traffic_info.ch]->unit = traffic_info.unit;
        flexe_mux_info[traffic_info.unit]->flexe_ch_info[traffic_info.ch]->ch_id = DEFAULT_VALUE;

        for (index = 0; index < TS_FIELD; index++)
            flexe_mux_info[traffic_info.unit]->flexe_ch_info[traffic_info.ch]->ts[index] = 0;
        
        flexe_mux_info[traffic_info.unit]->flexe_ch_info[traffic_info.ch]->ts_num = 0;
        flexe_mux_info[traffic_info.unit]->flexe_ch_info[traffic_info.ch]->ts_min = DEFAULT_VALUE;
        flexe_mux_info[traffic_info.unit]->flexe_ch_info[traffic_info.ch]->enable = TRAFFIC_REMOVE;

        /* storage the dev's info */
        flexe_mux_info[traffic_info.unit]->ch_mask[traffic_info.ch / REG_WIDTH] &=
                   ~((MASK_1_BIT << (traffic_info.ch % REG_WIDTH)) & MASK_32_BIT);

        for (index = 0; index < TS_FIELD; index++)
            flexe_mux_info[traffic_info.unit]->ts_mask[index] &= ~traffic_info.ts[index];
    }

    return DEV_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     dev's mux module algorithm input_b.
*
* DESCRIPTION
*
*     algorithm implementation.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     traffic_info: mux's input info
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
*      liming        2018-07-03         1.0            initial
*
******************************************************************************/
dev_err_t flexe_mux_algorithm_b_cfg(flexe_mux_traffic_info_t traffic_info)
{
    UINT_8 ts[TS_NUM] = {0};
    UINT_8 ts_num = 0;
    UINT_8 index = 0;
    UINT_8 t_cycle = 0;
    UINT_8 r_column = 1;
    UINT_8 r_col_offset = 1;
    UINT_8 divisor = 0;
    UINT_32 ts_data_cnt = 0;
    UINT_8 w_row = 0;
    UINT_8 w_row_offset = 0;
    UINT_8 w_col_offset = 0;
    UINT_8 pos_flag[TS_NUM][TCYCLE] = {{0}};

    /* check unit whether normal */
    if (traffic_info.unit > (DEV_NUM - 1)) {
        printf("%s: invalid unit = 0x%x...\r\n", __FUNCTION__, traffic_info.unit);
        
        return DEV_PARA_ERR;
    }

    /* check ch info */
    if (traffic_info.ch > (CH_NUM - 1)) {
        printf("%s: unit = 0x%x, invalid traffic_ch_id = 0x%x...\r\n",
                                                         __FUNCTION__,
                                                    traffic_info.unit,
                                                     traffic_info.ch);

        return DEV_PARA_ERR;
    }

    /* check ts info */
    if (traffic_info.ts[TS_FIELD - 1] > 0xffff) {
        printf("%s: unit = 0x%x, invalid traffic_ts[%d] = 0x%x...\r\n",
                                                           __FUNCTION__,
                                                      traffic_info.unit,
                                                           TS_FIELD - 1,
                                         traffic_info.ts[TS_FIELD - 1]);

        return DEV_PARA_ERR;
    }

    /* check add_remove info */
    if ((TRAFFIC_REMOVE != traffic_info.enable) && (TRAFFIC_ADD != traffic_info.enable)) {
        printf("%s: unit = 0x%x, invalid traffic_enable = 0x%x...\r\n",
                                                          __FUNCTION__,
                                                     traffic_info.unit,
                                                  traffic_info.enable);

        return DEV_PARA_ERR;
    }

    /* get the ts info */
    for (index = 0; index < TS_NUM; index++) {
        if ((traffic_info.ts[index / REG_WIDTH] >> (index % REG_WIDTH)) & MASK_1_BIT) {
            ts[ts_num] = index;
            ts_num++;
        }
    }

    memset(ts, DEFAULT_VALUE, sizeof(ts));

    for (index = 0; index < ts_num; index++) {
        ts[index] = traffic_info.ts_sort[index];
    }

    /* get the ts_num and tcycle's divisor */
    flexe_mux_divisor_data_get(ts_num, TCYCLE, &divisor);

    printf("%s: ts_num = 0x%x, tcyle = 0x%x, divisor = 0x%x...\r\n", __FUNCTION__, ts_num, TCYCLE, divisor);

    /* add traffic, calculate the mux algorithm result */
    if (TRAFFIC_ADD == traffic_info.enable) {
        memset(pos_flag, 0, sizeof(pos_flag));
        
        /* mux_ram_write_rule */
        for (index = 0; index < ts_num; index++) {
            /* row and col's bit_start position */
            if (0 == index) {
                w_row_offset = ts[index];
                w_row = index;
                w_col_offset = index;
            }

            for (t_cycle = 0; t_cycle < TCYCLE; t_cycle++) {
                /* find the col position */
                while (1 == pos_flag[w_row_offset][w_col_offset]) {
                    w_col_offset++;

                    if (w_col_offset > (TCYCLE - 1)) {
                        w_col_offset -= TCYCLE;
                    }
                }
                
                flexe_mux_info[traffic_info.unit]->flexe_ts_info_b[ts[index]]->w_col[w_col_offset] = ts[w_row];
                pos_flag[w_row_offset][w_col_offset] = 1;
                w_row++;
                w_col_offset++;

                if (w_row > (ts_num - 1)) {
                    w_row -= ts_num;    
                }

                /* row shift to next row */
                w_row_offset = ts[w_row];

                if (w_col_offset > (TCYCLE - 1)) {
                    w_col_offset -= TCYCLE;
                }
            }
        }

        /* mux_ram_read_rule */
        for (t_cycle = 0; t_cycle < TCYCLE; t_cycle++) {
            /* one sub_cycle is bit_end, so offset add one */
            if (ts_data_cnt == ((ts_num * TCYCLE) / divisor)) {
                ts_data_cnt = 0;
                r_col_offset++;
                r_column = r_col_offset;
            }
            
            for (index = 0; index < ts_num; index++) {
                ts_data_cnt++;

                if (r_column > TCYCLE)
                    r_column %= TCYCLE;

                /* storage the write operation result */
                flexe_mux_info[traffic_info.unit]->flexe_ts_info_b[ts[index]]->r_col[t_cycle] = r_column - 1;
                r_column++;
            }
        }

        /* storage the ts's info */
        for (index = 0; index < ts_num; index++) {
            flexe_mux_info[traffic_info.unit]->flexe_ts_info_b[ts[index]]->unit = traffic_info.unit;
            flexe_mux_info[traffic_info.unit]->flexe_ts_info_b[ts[index]]->ch_id = traffic_info.ch;
            flexe_mux_info[traffic_info.unit]->flexe_ts_info_b[ts[index]]->ts_id = ts[index];
        }

        /* storage the ch's info */
        flexe_mux_info[traffic_info.unit]->flexe_ch_info_b[traffic_info.ch]->unit = traffic_info.unit;
        flexe_mux_info[traffic_info.unit]->flexe_ch_info_b[traffic_info.ch]->ch_id = traffic_info.ch;

        for (index = 0; index < TS_FIELD; index++)
            flexe_mux_info[traffic_info.unit]->flexe_ch_info_b[traffic_info.ch]->ts[index] = traffic_info.ts[index];
        
        flexe_mux_info[traffic_info.unit]->flexe_ch_info_b[traffic_info.ch]->ts_num = ts_num;
        flexe_mux_info[traffic_info.unit]->flexe_ch_info_b[traffic_info.ch]->ts_min = ts[0];
        flexe_mux_info[traffic_info.unit]->flexe_ch_info_b[traffic_info.ch]->enable = TRAFFIC_ADD;

        /* storage the dev's ch_mask and ts_mask info */
        flexe_mux_info[traffic_info.unit]->ch_mask_b[traffic_info.ch / REG_WIDTH] |=
                      ((MASK_1_BIT << (traffic_info.ch % REG_WIDTH)) & MASK_32_BIT);

        for (index = 0; index < TS_FIELD; index++)
            flexe_mux_info[traffic_info.unit]->ts_mask_b[index] |= traffic_info.ts[index];
    }
    /* remove traffic, set structure to default value */
    else {
        /* set the write operation result to default value */
        for (index = 0; index < ts_num; index++) {
            for (t_cycle = 0; t_cycle < TCYCLE; t_cycle++) {
                flexe_mux_info[traffic_info.unit]->flexe_ts_info_b[ts[index]]->w_col[t_cycle] = DEFAULT_VALUE;
                flexe_mux_info[traffic_info.unit]->flexe_ts_info_b[ts[index]]->r_col[t_cycle] = DEFAULT_VALUE;
            }
        }

        /* set the ts's info to default value */
        for (index = 0; index < ts_num; index++) {
            flexe_mux_info[traffic_info.unit]->flexe_ts_info_b[ts[index]]->unit = traffic_info.unit;
            flexe_mux_info[traffic_info.unit]->flexe_ts_info_b[ts[index]]->ch_id = DEFAULT_VALUE;
            flexe_mux_info[traffic_info.unit]->flexe_ts_info_b[ts[index]]->ts_id = DEFAULT_VALUE;
        }

        /* storage the ch's info */
        flexe_mux_info[traffic_info.unit]->flexe_ch_info_b[traffic_info.ch]->unit = traffic_info.unit;
        flexe_mux_info[traffic_info.unit]->flexe_ch_info_b[traffic_info.ch]->ch_id = DEFAULT_VALUE;

        for (index = 0; index < TS_FIELD; index++)
            flexe_mux_info[traffic_info.unit]->flexe_ch_info_b[traffic_info.ch]->ts[index] = 0;
        
        flexe_mux_info[traffic_info.unit]->flexe_ch_info_b[traffic_info.ch]->ts_num = 0;
        flexe_mux_info[traffic_info.unit]->flexe_ch_info_b[traffic_info.ch]->ts_min = DEFAULT_VALUE;
        flexe_mux_info[traffic_info.unit]->flexe_ch_info_b[traffic_info.ch]->enable = TRAFFIC_REMOVE;

        /* storage the dev's info */
        flexe_mux_info[traffic_info.unit]->ch_mask_b[traffic_info.ch / REG_WIDTH] &=
                     ~((MASK_1_BIT << (traffic_info.ch % REG_WIDTH)) & MASK_32_BIT);

        for (index = 0; index < TS_FIELD; index++)
            flexe_mux_info[traffic_info.unit]->ts_mask_b[index] &= ~traffic_info.ts[index];
    }

    return DEV_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     config dev's mux traffic.
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
*    flexe_mux_traffic_info_t: dev_mux's traffic input info.
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
dev_err_t flexe_mux_traffic_cfg(flexe_mux_traffic_info_t traffic_info)
{
    UINT_8 index = 0;
    dev_err_t rt = DEV_DEFAULT;

    /* check unit whether normal */
	if (traffic_info.unit > (DEV_NUM - 1)) 
	{
		printf("%s: invalid unit = 0x%x...\r\n", __FUNCTION__, traffic_info.unit);
		
		return DEV_PARA_ERR;
	}
	/* check traffic_enable info */
	if ((TRAFFIC_REMOVE != traffic_info.enable) && (TRAFFIC_ADD != traffic_info.enable)) 
	{
		printf("%s: unit = 0x%x, invalid traffic_enable = 0x%x...\r\n",
														  __FUNCTION__,
													 traffic_info.unit,
												  traffic_info.enable);

		return DEV_PARA_ERR;
	}
	/* check ch info */
	if (traffic_info.ch > (CH_NUM - 1)) 
	{
        printf("%s: unit = 0x%x, invalid traffic_ch_id = 0x%x...\r\n",
                                                         __FUNCTION__,
                                                    traffic_info.unit,
                                                     traffic_info.ch);

        return DEV_PARA_ERR;
    }

    /* check ts info */
    if (traffic_info.ts[TS_FIELD - 1] > 0xffff) {
        printf("%s: unit = 0x%x, invalid traffic_ts[%d] = 0x%x...\r\n",
                                                           __FUNCTION__,
                                                      traffic_info.unit,
                                                           TS_FIELD - 1,
                                         traffic_info.ts[TS_FIELD - 1]);

        return DEV_PARA_ERR;
    }

	/* check flexe_mux_module whether initialized */
	if(!flexe_mux_info[traffic_info.unit]) 
	{
		printf("%s: unit = 0x%x not initialized...\r\n", __FUNCTION__, traffic_info.unit);

		return DEV_DEINITIALIZED;
	}

	/* add traffic */
	if (TRAFFIC_ADD == traffic_info.enable) 
	{
		/* check ch whether used */
		if (!((flexe_mux_info[traffic_info.unit]->ch_mask[traffic_info.ch / REG_WIDTH] >> (traffic_info.ch % REG_WIDTH)) & MASK_1_BIT)) 
		{
			/* check ch pointer whether initialized */
			if (flexe_mux_info[traffic_info.unit]->flexe_ch_info[traffic_info.ch]) 
			{
				for (index = 0; index < TS_NUM; index++) 
				{
					if ((traffic_info.ts[index / REG_WIDTH] >> (index % REG_WIDTH)) & MASK_1_BIT) {
						/* check ts whether used */
						if ((flexe_mux_info[traffic_info.unit]->ts_mask[index / REG_WIDTH] >> (index % REG_WIDTH)) & MASK_1_BIT) 
						{
							printf("%s: unit = 0x%x, enable = 0x%x, ts = 0x%x have already used...\r\n",
																						   __FUNCTION__,
																					  traffic_info.unit,
																					traffic_info.enable,
																								 index);

							return DEV_TS_UNAVAILABLE;
						}

						/* check ts pointer whether initialized */
						if (!flexe_mux_info[traffic_info.unit]->flexe_ts_info[index]) 
						{
							printf("%s: unit = 0x%x, enable = 0x%x, ts = 0x%x's pointer is null...\r\n",
																						   __FUNCTION__,
																					  traffic_info.unit,
																					traffic_info.enable,
																								 index);

							return DEV_POINTER_NULL;
						}
					}
				}
			}
			else 
			{
				printf("%s: unit = 0x%x, enable = 0x%x, ch = 0x%x pointer not initialized...\r\n",
																					 __FUNCTION__,
																				traffic_info.unit,
																			  traffic_info.enable,
																				 traffic_info.ch);

				return DEV_DEINITIALIZED;
			}
		}
		else 
		{
			printf("%s: unit = 0x%x, enable = 0x%x, ch = 0x%x have already used...\r\n",
																		   __FUNCTION__,
																	  traffic_info.unit,
																	traffic_info.enable,
																	   traffic_info.ch);
			
			return DEV_CH_UNAVAILABLE;
		}
	}
	else /* delete traffic */
	{
		/* check ch whether configed */
		if ((flexe_mux_info[traffic_info.unit]->ch_mask[traffic_info.ch / REG_WIDTH] >> (traffic_info.ch % REG_WIDTH)) & MASK_1_BIT) 
		{
			/* check ts whether configed */
			for (index = 0; index < TS_NUM; index++) 
			{
				if ((traffic_info.ts[index / REG_WIDTH] >> (index % REG_WIDTH)) & MASK_1_BIT) 
				{
					if (!((flexe_mux_info[traffic_info.unit]->ts_mask[index / REG_WIDTH] >> (index % REG_WIDTH)) & MASK_1_BIT)) 
					{
						printf("%s: unit = 0x%x, enable = 0x%x, ts = 0x%x not cfg...\r\n",
																			 __FUNCTION__,
																		traffic_info.unit,
																	  traffic_info.enable,
																				   index);

						return DEV_TS_UNAVAILABLE;
					}

					/* check ts pointer whether initialized */
					if (!flexe_mux_info[traffic_info.unit]->flexe_ts_info[index]) 
					{
						printf("%s: unit = 0x%x, enable = 0x%x, ts = 0x%x's pointer is null...\r\n",
																					   __FUNCTION__,
																				  traffic_info.unit,
																				traffic_info.enable,
																							 index);

						return DEV_POINTER_NULL;
					}
				}
			}

			/* check ch pointer whether initialized */
			if (!flexe_mux_info[traffic_info.unit]->flexe_ch_info[traffic_info.ch]) 
			{
				printf("%s: unit = 0x%x, enable = 0x%x, ch = 0x%x pointer not initialized...\r\n",
																					 __FUNCTION__,
																				traffic_info.unit,
																			  traffic_info.enable,
																				 traffic_info.ch);

				return DEV_DEINITIALIZED;
			}
		}
		else 
		{
			printf("%s: unit = 0x%x, enable = 0x%x, ch = 0x%x not cfg...\r\n",
																 __FUNCTION__,
															traffic_info.unit,
														  traffic_info.enable,
															 traffic_info.ch);
			
			return DEV_CH_UNAVAILABLE;
		}
	}

	/* do the flexe mux algorithm operation */
	rt = flexe_mux_algorithm_cfg(traffic_info);

	if (DEV_SUCCESS != rt) 
	{
		printf("%s: unit = 0x%x, rt = %d, operation mux algorithm fail...\r\n",
															      __FUNCTION__,
															 traffic_info.unit,
																		   rt);
		return rt;
	} 

	return DEV_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     config dev's mux traffic_b.
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
*    flexe_mux_traffic_info_t: dev_mux's traffic input info.
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
*      liming        2018-07-03         1.0            initial
*
******************************************************************************/
dev_err_t flexe_mux_traffic_b_cfg(flexe_mux_traffic_info_t traffic_info)
{
    UINT_8 index = 0;
    dev_err_t rt = DEV_DEFAULT;

    /* check unit whether normal */
    if (traffic_info.unit > (DEV_NUM - 1)) {
        printf("%s: invalid unit = 0x%x...\r\n", __FUNCTION__, traffic_info.unit);
        
        return DEV_PARA_ERR;
    }

    /* check traffic_enable info */
    if ((TRAFFIC_REMOVE != traffic_info.enable) && (TRAFFIC_ADD != traffic_info.enable)) {
        printf("%s: unit = 0x%x, invalid traffic_enable = 0x%x...\r\n",
                                                          __FUNCTION__,
                                                     traffic_info.unit,
                                                  traffic_info.enable);

        return DEV_PARA_ERR;
    }

    /* check ch info */
    if (traffic_info.ch > (CH_NUM - 1)) {
        printf("%s: unit = 0x%x, invalid traffic_ch_id = 0x%x...\r\n",
                                                         __FUNCTION__,
                                                    traffic_info.unit,
                                                     traffic_info.ch);

        return DEV_PARA_ERR;
    }

    /* check ts info */
    if (traffic_info.ts[TS_FIELD - 1] > 0xffff) {
        printf("%s: unit = 0x%x, invalid traffic_ts[%d] = 0x%x...\r\n",
                                                           __FUNCTION__,
                                                      traffic_info.unit,
                                                           TS_FIELD - 1,
                                         traffic_info.ts[TS_FIELD - 1]);

        return DEV_PARA_ERR;
    }

    /* check flexe_mux_module whether initialized */
    if(!flexe_mux_info[traffic_info.unit]) {
        printf("%s: unit = 0x%x not initialized...\r\n", __FUNCTION__, traffic_info.unit);

        return DEV_DEINITIALIZED;
    }

    /* add traffic */
    if (TRAFFIC_ADD == traffic_info.enable) {
        /* check ch whether used */
        if (!((flexe_mux_info[traffic_info.unit]->ch_mask_b[traffic_info.ch / REG_WIDTH] >> (traffic_info.ch % REG_WIDTH)) & MASK_1_BIT)) {
            /* check ch pointer whether initialized */
            if (flexe_mux_info[traffic_info.unit]->flexe_ch_info_b[traffic_info.ch]) {
                for (index = 0; index < TS_NUM; index++) {
                    if ((traffic_info.ts[index / REG_WIDTH] >> (index % REG_WIDTH)) & MASK_1_BIT) {
                        /* check ts whether used */
                        if ((flexe_mux_info[traffic_info.unit]->ts_mask_b[index / REG_WIDTH] >> (index % REG_WIDTH)) & MASK_1_BIT) {
                            printf("%s: unit = 0x%x, enable = 0x%x, ts = 0x%x have already used...\r\n",
                                                                                           __FUNCTION__,
                                                                                      traffic_info.unit,
                                                                                    traffic_info.enable,
                                                                                                 index);

                            return DEV_TS_UNAVAILABLE;
                        }

                        /* check ts pointer whether initialized */
                        if (!flexe_mux_info[traffic_info.unit]->flexe_ts_info_b[index]) {
                            printf("%s: unit = 0x%x, enable = 0x%x, ts = 0x%x's pointer is null...\r\n",
                                                                                           __FUNCTION__,
                                                                                      traffic_info.unit,
                                                                                    traffic_info.enable,
                                                                                                 index);

                            return DEV_POINTER_NULL;
                        }
                    }
                }
            }
            else {
                printf("%s: unit = 0x%x, enable = 0x%x, ch = 0x%x pointer not initialized...\r\n",
                                                                                     __FUNCTION__,
                                                                                traffic_info.unit,
                                                                              traffic_info.enable,
                                                                                 traffic_info.ch);

                return DEV_DEINITIALIZED;
            }
        }
        else {
            printf("%s: unit = 0x%x, enable = 0x%x, ch = 0x%x have already used...\r\n",
                                                                           __FUNCTION__,
                                                                      traffic_info.unit,
                                                                    traffic_info.enable,
                                                                       traffic_info.ch);
            
            return DEV_CH_UNAVAILABLE;
        }
    }
    /* delete traffic */
    else {
        /* check ch whether configed */
        if ((flexe_mux_info[traffic_info.unit]->ch_mask_b[traffic_info.ch / REG_WIDTH] >> (traffic_info.ch % REG_WIDTH)) & MASK_1_BIT) {
            /* check ts whether configed */
            for (index = 0; index < TS_NUM; index++) {
                if ((traffic_info.ts[index / REG_WIDTH] >> (index % REG_WIDTH)) & MASK_1_BIT) {
                    if (!((flexe_mux_info[traffic_info.unit]->ts_mask_b[index / REG_WIDTH] >> (index % REG_WIDTH)) & MASK_1_BIT)) {
                        printf("%s: unit = 0x%x, enable = 0x%x, ts = 0x%x not cfg...\r\n",
                                                                             __FUNCTION__,
                                                                        traffic_info.unit,
                                                                      traffic_info.enable,
                                                                                   index);

                        return DEV_TS_UNAVAILABLE;
                    }

                    /* check ts pointer whether initialized */
                    if (!flexe_mux_info[traffic_info.unit]->flexe_ts_info_b[index]) {
                        printf("%s: unit = 0x%x, enable = 0x%x, ts = 0x%x's pointer is null...\r\n",
                                                                                       __FUNCTION__,
                                                                                  traffic_info.unit,
                                                                                traffic_info.enable,
                                                                                             index);

                        return DEV_POINTER_NULL;
                    }
                }
            }

            /* check ch pointer whether initialized */
            if (!flexe_mux_info[traffic_info.unit]->flexe_ch_info_b[traffic_info.ch]) {
                printf("%s: unit = 0x%x, enable = 0x%x, ch = 0x%x pointer not initialized...\r\n",
                                                                                     __FUNCTION__,
                                                                                traffic_info.unit,
                                                                              traffic_info.enable,
                                                                                 traffic_info.ch);

                return DEV_DEINITIALIZED;
            }
        }
        else {
            printf("%s: unit = 0x%x, enable = 0x%x, ch = 0x%x not cfg...\r\n",
                                                                 __FUNCTION__,
                                                            traffic_info.unit,
                                                          traffic_info.enable,
                                                             traffic_info.ch);
            
            return DEV_CH_UNAVAILABLE;
        }
    }

    /* do the flexe mux algorithm operation */
    rt = flexe_mux_algorithm_b_cfg(traffic_info);

    if (DEV_SUCCESS != rt) {
        printf("%s: unit = 0x%x, rt = %d, operation mux algorithm fail...\r\n",
                                                                  __FUNCTION__,
                                                             traffic_info.unit,
                                                                           rt);
        return rt;
    } 

    return DEV_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     get dev's mux traffic config info.
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
*    traffic_info: dev_mux's traffic input info.
*    p_flexe_ch_info_out: dev_mux's traffic output_ch_info
*    p_flexe_ts_info_out: dev_mux's traffic output_ts_info
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
dev_err_t flexe_mux_traffic_out(flexe_mux_traffic_info_t traffic_info,
                                flexe_mux_ch_info_t *p_flexe_ch_info_out,
                                flexe_mux_ts_info_t *p_flexe_ts_info_out)
{
    UINT_8 ts_num = 0;
    UINT_8 col_index = 0;
    UINT_8 index = 0;

    /* check ch_ts_info whether is null */
	if (!p_flexe_ch_info_out || !p_flexe_ts_info_out) 
	{
		printf("%s: flexe_ch_info_out or flexe_ts_info_out pointer is null...\r\n", __FUNCTION__);
	
		return DEV_POINTER_NULL;
	}
    /* check unit whether normal */
    if (traffic_info.unit > (DEV_NUM - 1)) 
	{
        printf("%s: invalid unit = 0x%x...\r\n", __FUNCTION__, traffic_info.unit);
		
        return DEV_PARA_ERR;
    }
	/* check ch info */
	if (traffic_info.ch > (CH_NUM - 1)) 
	{
		printf("%s: unit = 0x%x, invalid traffic_ch_id = 0x%x...\r\n",
							                             __FUNCTION__,
							                        traffic_info.unit,
			                                         traffic_info.ch);

		return DEV_PARA_ERR;
	}
	/* check flexe_mux_module whether initialized */
    if(!flexe_mux_info[traffic_info.unit]) 
	{
		printf("%s: unit = 0x%x not initialized...\r\n", __FUNCTION__, traffic_info.unit);

		return DEV_DEINITIALIZED;
	}
	/* check ch whether configed */
	if (!((flexe_mux_info[traffic_info.unit]->ch_mask[traffic_info.ch / REG_WIDTH] >> (traffic_info.ch % REG_WIDTH)) & MASK_1_BIT)) 
	{
		printf("%s: unit = 0x%x, ch = 0x%x not configed...\r\n", __FUNCTION__, traffic_info.unit, traffic_info.ch);

		return DEV_CH_UNAVAILABLE;
	}

	/* check ch_info pointer whether normal */
	if (!(flexe_mux_info[traffic_info.unit]->flexe_ch_info[traffic_info.ch])) 
	{
		printf("%s: unit = 0x%x, ch = 0x%x not malloc memory...\r\n",
			                                            __FUNCTION__,
			                                       traffic_info.unit,
			                                        traffic_info.ch);

		return DEV_POINTER_NULL;
	}

	/* get ch_info result */
	p_flexe_ch_info_out->unit = flexe_mux_info[traffic_info.unit]->flexe_ch_info[traffic_info.ch]->unit;
	p_flexe_ch_info_out->ch_id = flexe_mux_info[traffic_info.unit]->flexe_ch_info[traffic_info.ch]->ch_id;

	for (index = 0; index < TS_FIELD; index++)
	{
		p_flexe_ch_info_out->ts[index] = flexe_mux_info[traffic_info.unit]->flexe_ch_info[traffic_info.ch]->ts[index];
	}
	
	p_flexe_ch_info_out->ts_num = flexe_mux_info[traffic_info.unit]->flexe_ch_info[traffic_info.ch]->ts_num;
	p_flexe_ch_info_out->ts_min = flexe_mux_info[traffic_info.unit]->flexe_ch_info[traffic_info.ch]->ts_min;
	p_flexe_ch_info_out->enable = flexe_mux_info[traffic_info.unit]->flexe_ch_info[traffic_info.ch]->enable;

	for (index = 0; index < TS_NUM; index++) 
	{
		/*searched the ch's ts */
		if ((p_flexe_ch_info_out->ts[index / REG_WIDTH] >> (index % REG_WIDTH)) & MASK_1_BIT) 
		{
			/* check ts_info pointer whether normal */
			if (!(flexe_mux_info[traffic_info.unit]->flexe_ts_info[index])) 
			{
				printf("%s: unit = 0x%x, ts = 0x%x not malloc memory...\r\n",
				                                                __FUNCTION__,
				                                           traffic_info.unit,
				                                                      index);

				return DEV_POINTER_NULL;

			}

			/* api caller must used struct array, the size is TS_NUM, or memory maybe out of bounds */
			/* get ts's result */
			p_flexe_ts_info_out[ts_num].unit = flexe_mux_info[traffic_info.unit]->flexe_ts_info[index]->unit;
			p_flexe_ts_info_out[ts_num].ch_id = flexe_mux_info[traffic_info.unit]->flexe_ts_info[index]->ch_id;
			p_flexe_ts_info_out[ts_num].ts_id = flexe_mux_info[traffic_info.unit]->flexe_ts_info[index]->ts_id;

			for (col_index = 0; col_index < TCYCLE; col_index++) 
			{
				p_flexe_ts_info_out[ts_num].r_col[col_index] = flexe_mux_info[traffic_info.unit]->flexe_ts_info[index]->r_col[col_index];
				p_flexe_ts_info_out[ts_num].w_col[col_index] = flexe_mux_info[traffic_info.unit]->flexe_ts_info[index]->w_col[col_index];
			}

			/* ts_num add one */
			ts_num++;	
		}
	}
        
    return DEV_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     get dev's mux traffic_b config info.
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
*    traffic_info: dev_mux's traffic input info.
*    p_flexe_ch_info_out: dev_mux's traffic output_ch_info
*    p_flexe_ts_info_out: dev_mux's traffic output_ts_info
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
*      liming        2018-07-03         1.0            initial
*
******************************************************************************/
dev_err_t flexe_mux_traffic_b_out(flexe_mux_traffic_info_t traffic_info,
                                  flexe_mux_ch_info_t *p_flexe_ch_info_out,
                                  flexe_mux_ts_info_t *p_flexe_ts_info_out)
{
    UINT_8 ts_num = 0;
    UINT_8 col_index = 0;
    UINT_8 index = 0;

    /* check ch_ts_info whether is null */
    if (!p_flexe_ch_info_out || !p_flexe_ts_info_out) {
        printf("%s: flexe_ch_info_out or flexe_ts_info_out pointer is null...\r\n", __FUNCTION__);
    
        return DEV_POINTER_NULL;
    }

    /* check unit whether normal */
    if (traffic_info.unit > (DEV_NUM - 1)) {
        printf("%s: invalid unit = 0x%x...\r\n", __FUNCTION__, traffic_info.unit);
        
        return DEV_PARA_ERR;
    }

    /* check ch info */
    if (traffic_info.ch > (CH_NUM - 1)) {
        printf("%s: unit = 0x%x, invalid traffic_ch_id = 0x%x...\r\n",
                                                         __FUNCTION__,
                                                    traffic_info.unit,
                                                     traffic_info.ch);

        return DEV_PARA_ERR;
    }

    /* check flexe_mux_module whether initialized */
    if(!flexe_mux_info[traffic_info.unit]) {
        printf("%s: unit = 0x%x not initialized...\r\n", __FUNCTION__, traffic_info.unit);

        return DEV_DEINITIALIZED;
    }

    /* check ch whether configed */
    if (!((flexe_mux_info[traffic_info.unit]->ch_mask_b[traffic_info.ch / REG_WIDTH] >> (traffic_info.ch % REG_WIDTH)) & MASK_1_BIT)) {
        printf("%s: unit = 0x%x, ch = 0x%x not configed...\r\n", __FUNCTION__, traffic_info.unit, traffic_info.ch);

        return DEV_CH_UNAVAILABLE;
    }

    /* check ch_info pointer whether normal */
    if (!(flexe_mux_info[traffic_info.unit]->flexe_ch_info_b[traffic_info.ch])) {
        printf("%s: unit = 0x%x, ch = 0x%x not malloc memory...\r\n",
                                                        __FUNCTION__,
                                                   traffic_info.unit,
                                                    traffic_info.ch);

        return DEV_POINTER_NULL;
    }

    /* get ch_info result */
    p_flexe_ch_info_out->unit = flexe_mux_info[traffic_info.unit]->flexe_ch_info_b[traffic_info.ch]->unit;
    p_flexe_ch_info_out->ch_id = flexe_mux_info[traffic_info.unit]->flexe_ch_info_b[traffic_info.ch]->ch_id;

    for (index = 0; index < TS_FIELD; index++)
        p_flexe_ch_info_out->ts[index] = flexe_mux_info[traffic_info.unit]->flexe_ch_info_b[traffic_info.ch]->ts[index];
    
    p_flexe_ch_info_out->ts_num = flexe_mux_info[traffic_info.unit]->flexe_ch_info_b[traffic_info.ch]->ts_num;
    p_flexe_ch_info_out->ts_min = flexe_mux_info[traffic_info.unit]->flexe_ch_info_b[traffic_info.ch]->ts_min;
    p_flexe_ch_info_out->enable = flexe_mux_info[traffic_info.unit]->flexe_ch_info_b[traffic_info.ch]->enable;

    for (index = 0; index < TS_NUM; index++) {
        /*searched the ch's ts */
        if ((p_flexe_ch_info_out->ts[index / REG_WIDTH] >> (index % REG_WIDTH)) & MASK_1_BIT) {
            /* check ts_info pointer whether normal */
            if (!(flexe_mux_info[traffic_info.unit]->flexe_ts_info_b[index])) {
                printf("%s: unit = 0x%x, ts = 0x%x not malloc memory...\r\n",
                                                                __FUNCTION__,
                                                           traffic_info.unit,
                                                                      index);

                return DEV_POINTER_NULL;

            }

            /* api caller must used struct array, the size is TS_NUM, or memory maybe out of bounds */
            /* get ts's result */
            p_flexe_ts_info_out[ts_num].unit = flexe_mux_info[traffic_info.unit]->flexe_ts_info_b[index]->unit;
            p_flexe_ts_info_out[ts_num].ch_id = flexe_mux_info[traffic_info.unit]->flexe_ts_info_b[index]->ch_id;
            p_flexe_ts_info_out[ts_num].ts_id = flexe_mux_info[traffic_info.unit]->flexe_ts_info_b[index]->ts_id;

            for (col_index = 0; col_index < TCYCLE; col_index++) {
                p_flexe_ts_info_out[ts_num].r_col[col_index] = flexe_mux_info[traffic_info.unit]->flexe_ts_info_b[index]->r_col[col_index];
                p_flexe_ts_info_out[ts_num].w_col[col_index] = flexe_mux_info[traffic_info.unit]->flexe_ts_info_b[index]->w_col[col_index];
            }

            /* ts_num add one */
            ts_num++;    
        }
    }
        
    return DEV_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     dump flexe's mux info.
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
*     unit: dev's index, increase one by one
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
const char *phy_mode_index[FLEXE_PHY_DEFAULT + 1] = {"F050","F100","P050","P100","P200","P400","___"};
dev_err_t flexe_mux_dump(UINT_8 unit)
{
    UINT_8 ch_index = 0;
	UINT_8 ts_index = 0;
	UINT_8 phy_index = 0;
	UINT_8 cycle = 0;
	UINT_8 index = 0;
	UINT_8 group_id = 0;
	
    /* check unit whether normal */
    if (unit > (DEV_NUM - 1)) 
	{
        printf("%s: invalid unit = 0x%x...\r\n", __FUNCTION__, unit);
        return DEV_PARA_ERR;
    }

	if (NULL == flexe_mux_info[unit])
	{
        printf("%s: unit = 0x%x not initialized...\r\n", __FUNCTION__, unit);
        return DEV_DEINITIALIZED;
	}
	

	printf("\r\n");
	printf("**********========== unit %d's flexe_mux info ==========**********\r\n", unit);
	printf("\r\n");

    printf("++++++++++++++++++++++ flexe_mux_ch_info ++++++++++++++++++++++\r\n");
	for (ch_index = 0; ch_index < CH_NUM;) 
	{
		printf("%s", bit_start);
		for (index = ch_index; index < (((ch_index + REG_WIDTH) < CH_NUM ? (ch_index + REG_WIDTH) : CH_NUM) - 2); index++) 
		{
			printf("%s", bit_middle);
		}
		printf("%s\r\n", bit_end);
		
		for (index = ch_index; index < ((ch_index + REG_WIDTH) < CH_NUM ? (ch_index + REG_WIDTH) : CH_NUM); index++) 
		{
			printf("%3d|", (int)index);
		}
		printf("\r\n");
		
		printf("%s", bit_start);
		for (index = ch_index; index < (((ch_index + REG_WIDTH) < CH_NUM ? (ch_index + REG_WIDTH) : CH_NUM) - 2); index++) 
		{
			printf("%s", bit_middle);
		}
		printf("%s\r\n", bit_end);
		
		for (index = ch_index; index < ((ch_index + REG_WIDTH) < CH_NUM ? (ch_index + REG_WIDTH) : CH_NUM); index++) 
		{
			/**/
			printf("%3d|", ((flexe_mux_info[unit]->ch_mask[ch_index / REG_WIDTH] >> (index % REG_WIDTH)) & MASK_1_BIT));
		}
		printf("\r\n");

		printf("%s", bit_start);
		for (index = ch_index; index < (((ch_index + REG_WIDTH) < CH_NUM ? (ch_index + REG_WIDTH) : CH_NUM) - 2); index++) 
		{
			printf("%s", bit_middle);
		}
		printf("%s\r\n", bit_end);

        if (ch_index + REG_WIDTH > CH_NUM)
        {
            break;
        }
		
        ch_index += REG_WIDTH;
	}
	printf("\r\n");

    printf("++++++++++++++++++++++ flexe_mux_ts_info ++++++++++++++++++++++\r\n");
	for (ts_index = 0; ts_index < TS_NUM;) 
	{
		printf("%s", bit_start);
		for (index = ts_index; index < (((ts_index + REG_WIDTH) < TS_NUM ? (ts_index + REG_WIDTH) : TS_NUM) - 2); index++) 
		{
			printf("%s", bit_middle);
		}
		printf("%s\r\n", bit_end);
		
		for (index = ts_index; index < ((ts_index + REG_WIDTH) < TS_NUM ? (ts_index + REG_WIDTH) : TS_NUM); index++) 
		{
			printf("%3d|", (int)index);
		}
		printf("\r\n");
		
		printf("%s", bit_start);
		for (index = ts_index; index < (((ts_index + REG_WIDTH) < TS_NUM ? (ts_index + REG_WIDTH) : TS_NUM) - 2); index++) 
		{
			printf("%s", bit_middle);
		}
		printf("%s\r\n", bit_end);
		
		for (index = ts_index; index < ((ts_index + REG_WIDTH) < TS_NUM ? (ts_index + REG_WIDTH) : TS_NUM); index++) 
		{
			/**/
			printf("%3d|", ((flexe_mux_info[unit]->ts_mask[ts_index / REG_WIDTH] >> (index % REG_WIDTH)) & MASK_1_BIT));
		}
		printf("\r\n");

		printf("%s", bit_start);
		for (index = ts_index; index < (((ts_index + REG_WIDTH) < TS_NUM ? (ts_index + REG_WIDTH) : TS_NUM) - 2); index++) {
			printf("%s", bit_middle);
		}
		printf("%s\r\n", bit_end);

        if (ts_index + REG_WIDTH > TS_NUM)
            break;
		
        ts_index += REG_WIDTH;
	}
	printf("\r\n");

	printf("++++++++++++++++++++++ flexe_mux_phy_mask_info(soft level) ++++++++++++++++++++++\r\n");
	for (phy_index = 0; phy_index < PHY_NUM;) 
	{
		printf("%s", bit_start);
		for (index = phy_index; index < (((phy_index + REG_WIDTH) < PHY_NUM ? (phy_index + REG_WIDTH) : PHY_NUM)); index++) 
		{
			printf("%s", bit_middle);
		}
		printf("%s\r\n", bit_end);
		
		for (index = phy_index; index < ((phy_index + REG_WIDTH) < PHY_NUM ? (phy_index + REG_WIDTH) : PHY_NUM); index++) 
		{
			printf("%4d|", (int)index);
		}
		printf("\r\n");
		
		printf("%s", bit_start);
		for (index = phy_index; index < (((phy_index + REG_WIDTH) < PHY_NUM ? (phy_index + REG_WIDTH) : PHY_NUM)); index++) 
		{
			printf("%s", bit_middle);
		}
		printf("%s\r\n", bit_end);
		
		for (index = phy_index; index < ((phy_index + REG_WIDTH) < PHY_NUM ? (phy_index + REG_WIDTH) : PHY_NUM); index++) 
		{
		    /* coverity[large_shift : FALSE] */
			printf("%4d|", (int)((flexe_mux_info[unit]->phy_mask >> (index % REG_WIDTH)) & MASK_1_BIT));
		}
		printf("\r\n");

		printf("%s", bit_start);
		for (index = phy_index; index < (((phy_index + REG_WIDTH) < PHY_NUM ? (phy_index + REG_WIDTH) : PHY_NUM)); index++) 
		{
			printf("%4s", bit_middle);
		}
		printf("%s\r\n", bit_end);

		for (index = phy_index; index < ((phy_index + REG_WIDTH) < PHY_NUM ? (phy_index + REG_WIDTH) : PHY_NUM); index++) 
		{
			printf("%s|", phy_mode_index[flexe_mux_info[unit]->phy[index].phy_mode]);
		}
		printf("\r\n");

		printf("%s", bit_start);
		for (index = phy_index; index < (((phy_index + REG_WIDTH) < PHY_NUM ? (phy_index + REG_WIDTH) : PHY_NUM)); index++) 
		{
			printf("%s", bit_middle);
		}
		printf("%s\r\n", bit_end);

        if (phy_index + REG_WIDTH > PHY_NUM)
        {
            break;
        }

		/* coverity[dead_error_line : FALSE] */
        phy_index += REG_WIDTH;
	}
	printf("\r\n");

	printf("++++++++++++++++++++++ flexe_mux_phy_group_info ++++++++++++++++++++++\r\n");
	printf("\r\n");

	printf("++++++++++++++++++++++ unit = 0x%x, group_num = 0x%x ++++++++++++++++++++++\r\n", unit, flexe_mux_info[unit]->group_num);
	printf("\r\n");

	for (group_id = 0; group_id < PHY_NUM; group_id++) 
	{
		if (0 != flexe_mux_info[unit]->group_info[group_id].phy_num) 
		{
			printf("group_id = %d, phy_num = %d\r\n", group_id, flexe_mux_info[unit]->group_info[group_id].phy_num);
			printf("%s\t%s\t%s\t%s\t%s\t%s\t\r\n", "phy_idx", "ts_mask", "phy_mod", "ts_num", "enable", "available");
			for (index = 0; index < flexe_mux_info[unit]->group_info[group_id].phy_num; index++) 
			{
				printf("%d\t0x%05x\t%d\t%4d\t%4d\t%4d\t\r\n", 
			        flexe_mux_info[unit]->group_info[group_id].phy_index[index],
			        flexe_mux_info[unit]->phy[flexe_mux_info[unit]->group_info[group_id].phy_index[index]].ts_mask,
					flexe_mux_info[unit]->group_info[group_id].group_phy_mode,
					flexe_mux_info[unit]->phy[flexe_mux_info[unit]->group_info[group_id].phy_index[index]].ts_num,
					flexe_mux_info[unit]->phy[flexe_mux_info[unit]->group_info[group_id].phy_index[index]].enable,
					flexe_mux_info[unit]->phy[flexe_mux_info[unit]->group_info[group_id].phy_index[index]].available);
			}
		}
	}

	printf("++++++++++++++++++++++ flexe_mux_w_cfg_info ++++++++++++++++++++++\r\n");
	printf("%s\t", "ts_id");
	for (index = 0; index < TCYCLE; index++)
	{
		printf("%s%d\t", "col_", index);
	}
	printf("\r\n");

	for (index = 0; index < TS_NUM; index++) 
	{
		printf("%s%2d\t", "id = ", index);
		for (cycle = 0; cycle < TCYCLE; cycle++)
		{
			printf("%5d\t", flexe_mux_info[unit]->flexe_ts_info[index]->w_col[cycle]);
		}
		printf("\r\n");
	}
	printf("\r\n");

	printf("++++++++++++++++++++++ flexe_mux_r_cfg_info ++++++++++++++++++++++\r\n");
	printf("%s\t", "ts_id");
	for (index = 0; index < TCYCLE; index++)
	{
		printf("%s%d\t", "col_", index);
	}
	printf("\r\n");

	for (index = 0; index < TS_NUM; index++) 
	{
		printf("%s%2d\t", "id = ", index);
		for (cycle = 0; cycle < TCYCLE; cycle++)
		{
			printf("%5d\t", flexe_mux_info[unit]->flexe_ts_info[index]->r_col[cycle]);
		}
		printf("\r\n");
	}
	printf("\r\n");

	return DEV_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     dump flexe's mux_ch info.
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
*     unit: dev's index, increase one by one
*     ch: ch_id
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
dev_err_t flexe_mux_ch_dump(UINT_8 unit, UINT_8 ch)
{
    UINT_8 ts_index = 0;
    UINT_8 index = 0;
    
    /* check unit whether normal */
    if (unit > (DEV_NUM - 1) || ch > (CH_NUM - 1)) 
	{
        printf("%s: invalid para, unit = 0x%x, ch = 0x%x...\r\n", __FUNCTION__, unit, ch);
        return DEV_PARA_ERR;
    }

	if (NULL == flexe_mux_info[unit])
	{
        printf("%s: unit = 0x%x not initialized...\r\n", __FUNCTION__, unit);
        return DEV_DEINITIALIZED;
	}

	printf("\r\n");
	printf("**********========== unit %d's flexe_mux_ch info ==========**********\r\n", unit);
    printf("++++++++++++++++++++++ ch_id = 0x%x ++++++++++++++++++++++\r\n", ch);
	printf("\r\n");

    printf("ch's ts info:\r\n");
	for (ts_index = 0; ts_index < TS_NUM;) 
	{
		printf("%s", bit_start);
		for (index = ts_index; index < (((ts_index + REG_WIDTH) < TS_NUM ? (ts_index + REG_WIDTH) : TS_NUM) - 2); index++) 
		{
			printf("%s", bit_middle);
		}
		printf("%s\r\n", bit_end);
		
		for (index = ts_index; index < ((ts_index + REG_WIDTH) < TS_NUM ? (ts_index + REG_WIDTH) : TS_NUM); index++) 
		{
			printf("%3d|", (int)index);
		}
		printf("\r\n");
		
		printf("%s", bit_start);
		for (index = ts_index; index < (((ts_index + REG_WIDTH) < TS_NUM ? (ts_index + REG_WIDTH) : TS_NUM) - 2); index++) 
		{
			printf("%s", bit_middle);
		}
		printf("%s\r\n", bit_end);
		
		for (index = ts_index; index < ((ts_index + REG_WIDTH) < TS_NUM ? (ts_index + REG_WIDTH) : TS_NUM); index++) 
		{
			/**/
			printf("%3d|", ((flexe_mux_info[unit]->flexe_ch_info[ch]->ts[index / REG_WIDTH] >> (index % REG_WIDTH)) & MASK_1_BIT));
		}
		printf("\r\n");

		printf("%s", bit_start);
		for (index = ts_index; index < (((ts_index + REG_WIDTH) < TS_NUM ? (ts_index + REG_WIDTH) : TS_NUM) - 2); index++) 
		{
			printf("%s", bit_middle);
		}
		printf("%s\r\n", bit_end);

        if (ts_index + REG_WIDTH > TS_NUM)
        {
            break;
        }
		
        ts_index += REG_WIDTH;
	}
	printf("\r\n");

	printf("ch's ts_num: %d\r\n", flexe_mux_info[unit]->flexe_ch_info[ch]->ts_num);
	printf("ch's ts_min: %d\r\n", flexe_mux_info[unit]->flexe_ch_info[ch]->ts_min);
	printf("ch's enable: %d\r\n", flexe_mux_info[unit]->flexe_ch_info[ch]->enable);
	printf("ch's group %d\r\n",flexe_mux_info[unit]->flexe_ch_info[ch]->group_id);
	printf("\r\n");

	return DEV_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     dump flexe's mux_ts info.
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
*     unit: dev's index, increase one by one
*     ts: ts_id
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
dev_err_t flexe_mux_ts_dump(UINT_8 unit, UINT_8 ts)
{
    UINT_8 index = 0;
    
    /* check unit whether normal */
    if (unit > (DEV_NUM - 1) || ts > (TS_NUM - 1)) {
        printf("%s: invalid para, unit = 0x%x, ts = 0x%x...\r\n", __FUNCTION__, unit, ts);
        
        return DEV_PARA_ERR;
    }

	if (NULL == flexe_mux_info[unit])
	{
        printf("%s: unit = 0x%x not initialized...\r\n", __FUNCTION__, unit);
        return DEV_DEINITIALIZED;
	}

	printf("\r\n");
	printf("**********========== unit %d's flexe_mux_ts info ==========**********\r\n", unit);
    printf("++++++++++++++++++++++ ts_id = 0x%x ++++++++++++++++++++++\r\n", ts);
	printf("\r\n");

    printf("ts info:\r\n");
	printf("ts's ch_id: 0x%x\r\n", flexe_mux_info[unit]->flexe_ts_info[ts]->ch_id);
	printf("w_algorithm result: ");
	for (index = 0; index < TCYCLE; index++)
	{
	    printf("0x%x   ", flexe_mux_info[unit]->flexe_ts_info[ts]->w_col[index]);
	}
	printf("\r\n\r\n");
	printf("r_algorithm result: ");
	for (index = 0; index < TCYCLE; index++)
	{
	    printf("0x%x   ", flexe_mux_info[unit]->flexe_ts_info[ts]->r_col[index]);
	}
	printf("\r\n\r\n");

	return DEV_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     flexe's mux dump function.
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
*      liming        2018-01-17         1.0            initial
*
******************************************************************************/
dev_err_t flexe_mux_debug(void)
{
    printf("+-------------------------mux's initialized/uninit function-------------------------+\r\n");
    printf("flexe_mux_init(UINT_8 unit)\r\n");
    printf("flexe_mux_uninit(UINT_8 unit)\r\n");
    printf("\r\n");

    printf("parameter description:\r\n");
    printf("unit: dev'index, default bit_start from zero, and increased one by one.\r\n");
    printf("+-----------------------------------------------------------------------------------+\r\n");
    printf("\r\n\r\n");

    printf("+-------------------------mux's dump function-------------------------+\r\n");
    printf("flexe_mux_dump(UINT_8 unit)\r\n");
    printf("flexe_mux_ch_dump(UINT_8 unit, UINT_8 ch)\r\n");
    printf("flexe_mux_ts_dump(UINT_8 unit, UINT_8 ts)\r\n");
    printf("\r\n");

    printf("parameter description:\r\n");
    printf("unit: dev'index, default bit_start from zero, and increased one by one.\r\n");
    printf("ch: traffic's channel_id, bit_start from zero.\r\n");
    printf("ts: traffic's ts_id, bit_start from zero.\r\n");
    printf("+---------------------------------------------------------------------+\r\n");
    printf("\r\n\r\n");

    return DEV_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     mux's phy config.
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
*      liming        2018-06-08         2.0        add asic_version
*
******************************************************************************/
dev_err_t flexe_mux_phy_group_cfg(UINT_8 unit,
                                  UINT_8 group_id,
                                  UINT_8 phy_index,
                                  flexe_phy_mode phy_mode,
                                  UINT_8 enable)
{
    UINT_8 index = 0;
    UINT_8 phy_num = 0;
    UINT_8 group_phy_index[PHY_NUM];
    flexe_mux_group_info_t group_info;
    dev_err_t rt = DEV_DEFAULT;
	bool flag = false;
	UINT_8 search_index = 0;
	UINT_8 combine_index = 0;
	UINT_8 combine_phy_index[PHY_NUM] = {0};
	UINT_8 logic_phy_flag[PHY_NUM] = {0};
    UINT_8 phy_num_origin = 0;
    UINT_8 bit_start_idx = 0;
	UINT_8 group_phy_index_origin[PHY_NUM] = {0};
	flexe_mux_group_info_t group_info_sort;
    int find_index = 0;

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

	if (NULL == flexe_mux_info[unit])
	{
	    printf("%s: unit = 0x%x not initialized...\r\n", __FUNCTION__, unit);
        return DEV_DEINITIALIZED;
	}

	/* add traffic's group_phy */
	if (TRAFFIC_ADD == enable) 
	{
	    /* check phy_index whether normal */
	    if (0 == phy_index) 
		{
	        printf("%s: unit = 0x%x, invalid phy_index = 0x%x...\r\n", __FUNCTION__, unit, phy_index);
	        return DEV_PARA_ERR;
	    }

	    /* check phy_mode whether normal */
	    if ((FLEXE_PHY_50G != phy_mode) && (FLEXE_PHY_100G != phy_mode) 
			 && (PCS_PHY_50G != phy_mode) && (PCS_PHY_100G != phy_mode) && (PCS_PHY_200G != phy_mode)) 
		{
	        printf("%s: unit = 0x%x, invalid phy_mode = 0x%x...\r\n", __FUNCTION__, unit, phy_mode);
	        return DEV_PARA_ERR;
	    }

        /* check group whether config */
		if (flexe_mux_info[unit]->group_info[group_id].phy_num < PHY_NUM) 
		{
			/* check phy's mask */
			if (phy_index & flexe_mux_info[unit]->phy_mask) 
			{
				printf("%s: unit = 0x%x, group_id = 0x%x, enable = 0x%x, phy have already cfg...\r\n", __FUNCTION__, unit, group_id, enable);
				return DEV_FAIL;
			}

		    /* set group_phy_index to default value */
			memset(group_phy_index, DEFAULT_VALUE, sizeof(group_phy_index));

			/* get the new add group's phy_index and phy_num, phy */
			for (index = 0; index < PHY_NUM; index++) 
			{
				if ((phy_index >> index) & MASK_1_BIT) 
				{
					group_phy_index[phy_num] = index; /*phy*/
					phy_num++;
				}
			}

			/* get the old add group's phy_index and phy_num Groupphy */
			if (0 != flexe_mux_info[unit]->group_info[group_id].phy_num) /*FlexEPCSGroup*/
			{
				/* check phy mode */
				if (phy_mode != flexe_mux_info[unit]->group_info[group_id].group_phy_mode) 
				{
					printf("%s: unit = 0x%x, group_id = 0x%x, phy mode err...\r\n", __FUNCTION__, unit, group_id);
					return DEV_FAIL;
				}

                /* old phy */
				for (index = 0; index < PHY_NUM; index++) 
				{
					if (group_id == flexe_mux_info[unit]->phy[index].group_id) 
					{
						group_phy_index_origin[phy_num_origin++] = flexe_mux_info[unit]->phy[index].phy_index;
					}
				}

				/* group_phy_index = new add phy + old phy phyphy */
				for (index = phy_num; index < phy_num + phy_num_origin; index++) 
				{
					group_phy_index[index] = group_phy_index_origin[index - phy_num];
				}
				phy_num += phy_num_origin;

                /* sort group_phy_index  */
				for (index = 0; index < phy_num - 1; index++) 
				{
					for (bit_start_idx = 0; bit_start_idx < phy_num - 1 - index; bit_start_idx++) 
					{
						if (group_phy_index[bit_start_idx] > group_phy_index[bit_start_idx + 1]) 
						{
							search_index = group_phy_index[bit_start_idx];
							group_phy_index[bit_start_idx] = group_phy_index[bit_start_idx + 1];
							group_phy_index[bit_start_idx + 1] = search_index;
						}
					}
				}

				/* old logic phy(phy,50g inst) */
				phy_num_origin = 0;
				for (index = 0; index < PHY_NUM; index++) 
				{
					if (DEFAULT_VALUE != flexe_mux_info[unit]->group_info[group_id].phy_index[index]) 
					{
						group_phy_index_origin[phy_num_origin++] = flexe_mux_info[unit]->group_info[group_id].phy_index[index];
					}
				}
				if (phy_num_origin != flexe_mux_info[unit]->group_info[group_id].phy_num) 
				{
					printf("%s: unit = 0x%x, group_id = 0x%x, get old logic phy err...\r\n", __FUNCTION__, unit, group_id);
					return DEV_FAIL;
				}
			}

			/* flexe mode */
			if ((FLEXE_PHY_100G == phy_mode) || (FLEXE_PHY_50G == phy_mode)) 
			{
				if (FLEXE_PHY_100G == phy_mode) /* flexe_100g_mode */
				{
					memset(combine_phy_index, 0, sizeof(combine_phy_index));
					memset(logic_phy_flag, 0, sizeof(logic_phy_flag));
					
					for (index = 0; index < phy_num_origin; index += 2) /* clear old group info Group */
					{
						/* phy's available have already check in add group, so it's no need to check again */
						flexe_mux_info[unit]->phy[group_phy_index_origin[index]].group_id = DEFAULT_VALUE;
						flexe_mux_info[unit]->phy[group_phy_index_origin[index]].phy_index = DEFAULT_VALUE;
						flexe_mux_info[unit]->phy[group_phy_index_origin[index]].phy_mode = FLEXE_PHY_DEFAULT;
						flexe_mux_info[unit]->phy[group_phy_index_origin[index]].available = 0;

						flexe_mux_info[unit]->logic_phy[group_phy_index_origin[index]].group_id = DEFAULT_VALUE;
						flexe_mux_info[unit]->logic_phy[group_phy_index_origin[index]].phy_index = DEFAULT_VALUE;
						flexe_mux_info[unit]->logic_phy[group_phy_index_origin[index]].phy_master = DEFAULT_VALUE;
						flexe_mux_info[unit]->logic_phy[group_phy_index_origin[index]].available = 0;
						flexe_mux_info[unit]->logic_phy[group_phy_index_origin[index]].phy_comb_index[0] = DEFAULT_VALUE;
						flexe_mux_info[unit]->logic_phy[group_phy_index_origin[index]].phy_comb_index[1] = DEFAULT_VALUE;

						flexe_mux_info[unit]->logic_phy[group_phy_index_origin[index + 1]].group_id = DEFAULT_VALUE;
						flexe_mux_info[unit]->logic_phy[group_phy_index_origin[index + 1]].phy_index = DEFAULT_VALUE;
						flexe_mux_info[unit]->logic_phy[group_phy_index_origin[index + 1]].phy_master = DEFAULT_VALUE;
						flexe_mux_info[unit]->logic_phy[group_phy_index_origin[index + 1]].available = 0;

						flexe_mux_info[unit]->phy_mask &= ~((UINT_8)(MASK_1_BIT << group_phy_index_origin[index]));

						for(find_index = 0;find_index < PHY_NUM;find_index++)
                        {
                        	if ((flexe_mux_info[unit]->group_info[group_id].phy_index[find_index]==group_phy_index_origin[index])
                                || (flexe_mux_info[unit]->group_info[group_id].phy_index[find_index]==group_phy_index_origin[index+1]))
                        	{
                        		flexe_mux_info[unit]->group_info[group_id].phy_index[find_index] = DEFAULT_VALUE;
                        	}
                        }
					}
					if (0 != phy_num_origin) 
					{
						flexe_mux_info[unit]->group_num--;
						flexe_mux_info[unit]->group_info[group_id].group_id = DEFAULT_VALUE;
						flexe_mux_info[unit]->group_info[group_id].group_phy_mode = FLEXE_PHY_DEFAULT;
						flexe_mux_info[unit]->group_info[group_id].phy_num = 0;
					}
                    
					for (index = 0; index < phy_num; index++) /* check */
					{
						if ((0 != flexe_mux_info[unit]->phy[group_phy_index[index]].available) ||
							(0 != flexe_mux_info[unit]->logic_phy[group_phy_index[index]].available)) 
						{
							printf("%s: unit = 0x%x, group_id = 0x%x, enable = 0x%x, phy_index = 0x%x not available...\r\n",
								                                                                               __FUNCTION__,
								                                                                                       unit,
								                                                                                   group_id,
								                                                                                     enable,
								                                                                    group_phy_index[index]);

							return DEV_FAIL;
						}
						else 
						{
							logic_phy_flag[group_phy_index[index]] = 1;
						}
					}

					/* search the combine logic_phy */
					for (index = 0; index < phy_num; index++) 
					{
						flag = false;
						
						for (search_index = 1; search_index < PHY_NUM; search_index++) 
						{
							combine_index = group_phy_index[index] + search_index;

							if (combine_index >= PHY_NUM) 
							{
								combine_index -= PHY_NUM;
							}

                            /* find the available logic_phyphy = 100g,250glogic_phy */
							if ((0 == logic_phy_flag[combine_index]) &&
								(0 == flexe_mux_info[unit]->logic_phy[combine_index].available)) 
							{
								combine_phy_index[index] = combine_index;
								logic_phy_flag[combine_index] = 1;
								flag = true;

								break;
							}
						}

						if (false == flag) 
						{
							printf("%s: unit = 0x%x, group_id = 0x%x, enable = 0x%x, phy_index = 0x%x not matched logic_phy...\r\n",
								                                                                                       __FUNCTION__,
								                                                                                               unit,
								                                                                                           group_id,
								                                                                                             enable,
								                                                                            group_phy_index[index]);

							return DEV_FAIL;
						}
					}

					/* storage the logic_phy and phy's info phyphy */
					for (index = 0; index < phy_num; index++) 
					{
						flexe_mux_info[unit]->phy[group_phy_index[index]].unit = unit;
						flexe_mux_info[unit]->phy[group_phy_index[index]].group_id = group_id;
						flexe_mux_info[unit]->phy[group_phy_index[index]].phy_index = group_phy_index[index];
						flexe_mux_info[unit]->phy[group_phy_index[index]].phy_mode = phy_mode;
						flexe_mux_info[unit]->phy[group_phy_index[index]].available = 1;

						flexe_mux_info[unit]->logic_phy[group_phy_index[index]].unit = unit;
						flexe_mux_info[unit]->logic_phy[group_phy_index[index]].group_id = group_id;
						flexe_mux_info[unit]->logic_phy[group_phy_index[index]].phy_index = group_phy_index[index];
						flexe_mux_info[unit]->logic_phy[group_phy_index[index]].phy_master = 1;
						flexe_mux_info[unit]->logic_phy[group_phy_index[index]].available = 1;
						flexe_mux_info[unit]->logic_phy[group_phy_index[index]].phy_comb_index[0] = group_phy_index[index];
						flexe_mux_info[unit]->logic_phy[group_phy_index[index]].phy_comb_index[1] = combine_phy_index[index];

						flexe_mux_info[unit]->logic_phy[combine_phy_index[index]].unit = unit;
						flexe_mux_info[unit]->logic_phy[combine_phy_index[index]].group_id = group_id;
						flexe_mux_info[unit]->logic_phy[combine_phy_index[index]].phy_index = combine_phy_index[index];
						flexe_mux_info[unit]->logic_phy[combine_phy_index[index]].phy_master = 0;
						flexe_mux_info[unit]->logic_phy[combine_phy_index[index]].available = 1;

						flexe_mux_info[unit]->phy_mask |= (UINT_8)(MASK_1_BIT << group_phy_index[index]);
					}

					/* storage the group_phy_info */
					flexe_mux_info[unit]->group_num++;

                    flexe_mux_info[unit]->group_info[group_id].group_id = group_id;
					flexe_mux_info[unit]->group_info[group_id].group_phy_mode = phy_mode;
					flexe_mux_info[unit]->group_info[group_id].phy_num = 2 * phy_num;

					for (index = 0; index < phy_num; index++) 
					{
						flexe_mux_info[unit]->group_info[group_id].phy_index[2 * index] = group_phy_index[index];
						flexe_mux_info[unit]->group_info[group_id].phy_index[2 * index + 1] = combine_phy_index[index];
					}
				}
				else /* flexe_50g_mode */
				{
					for (index = 0; index < phy_num_origin; index++) /* clear old group info */
					{
						/* phy's available have already check in add group, so it's no need to check again */
						flexe_mux_info[unit]->phy[group_phy_index_origin[index]].group_id = DEFAULT_VALUE;
						flexe_mux_info[unit]->phy[group_phy_index_origin[index]].phy_index = DEFAULT_VALUE;
						flexe_mux_info[unit]->phy[group_phy_index_origin[index]].phy_mode = FLEXE_PHY_DEFAULT;
						flexe_mux_info[unit]->phy[group_phy_index_origin[index]].available = 0;

						flexe_mux_info[unit]->logic_phy[group_phy_index_origin[index]].group_id = DEFAULT_VALUE;
						flexe_mux_info[unit]->logic_phy[group_phy_index_origin[index]].phy_index = DEFAULT_VALUE;
						flexe_mux_info[unit]->logic_phy[group_phy_index_origin[index]].phy_master = DEFAULT_VALUE;
						flexe_mux_info[unit]->logic_phy[group_phy_index_origin[index]].available = 0;

						flexe_mux_info[unit]->phy_mask &= ~((UINT_8)(MASK_1_BIT << group_phy_index_origin[index]));

						for(find_index = 0;find_index < PHY_NUM;find_index++)
                        {
                        	if (flexe_mux_info[unit]->group_info[group_id].phy_index[find_index]==group_phy_index_origin[index])
                        	{
                        		flexe_mux_info[unit]->group_info[group_id].phy_index[find_index] = DEFAULT_VALUE;
                        	}
                        }
					}
					if (0 != phy_num_origin) 
					{
						flexe_mux_info[unit]->group_num--;
						flexe_mux_info[unit]->group_info[group_id].group_id = DEFAULT_VALUE;
						flexe_mux_info[unit]->group_info[group_id].group_phy_mode = FLEXE_PHY_DEFAULT;
						flexe_mux_info[unit]->group_info[group_id].phy_num = 0;
					}

					for (index = 0; index < phy_num; index++) /* check */
					{
						if ((0 != flexe_mux_info[unit]->phy[group_phy_index[index]].available) ||
							(0 != flexe_mux_info[unit]->logic_phy[group_phy_index[index]].available)) 
						{
							printf("%s: unit = 0x%x, group_id = 0x%x, enable = 0x%x, phy_index = 0x%x not available...\r\n",
								                                                                               __FUNCTION__,
								                                                                                       unit,
								                                                                                   group_id,
								                                                                                     enable,
								                                                                    group_phy_index[index]);

							return DEV_FAIL;
						}
					}

                    /* storage the group_info */
					for (index = 0; index < phy_num; index++) 
					{
						flexe_mux_info[unit]->phy[group_phy_index[index]].unit = unit;
						flexe_mux_info[unit]->phy[group_phy_index[index]].group_id = group_id;
						flexe_mux_info[unit]->phy[group_phy_index[index]].phy_index = group_phy_index[index];
						flexe_mux_info[unit]->phy[group_phy_index[index]].phy_mode = phy_mode;
						flexe_mux_info[unit]->phy[group_phy_index[index]].available = 1;

						flexe_mux_info[unit]->logic_phy[group_phy_index[index]].unit = unit;
						flexe_mux_info[unit]->logic_phy[group_phy_index[index]].group_id = group_id;
						flexe_mux_info[unit]->logic_phy[group_phy_index[index]].phy_index = group_phy_index[index];
						flexe_mux_info[unit]->logic_phy[group_phy_index[index]].phy_master = 0;
						flexe_mux_info[unit]->logic_phy[group_phy_index[index]].available = 1;

						flexe_mux_info[unit]->phy_mask |= (UINT_8)(MASK_1_BIT << group_phy_index[index]);
					}

					/* storage the group_phy_info */
					flexe_mux_info[unit]->group_num++;

                    flexe_mux_info[unit]->group_info[group_id].group_id = group_id;
					flexe_mux_info[unit]->group_info[group_id].group_phy_mode = phy_mode;
					flexe_mux_info[unit]->group_info[group_id].phy_num = phy_num;

					for (index = 0; index < phy_num; index++) 
					{
						flexe_mux_info[unit]->group_info[group_id].phy_index[index] = group_phy_index[index];;
					}
				}
			}
			else /* pcs mode, bypass mode */
			{
                /* storage the group_info */
				for (index = 0; index < phy_num; index++) 
				{
					flexe_mux_info[unit]->phy[group_phy_index[index]].unit = unit;
					flexe_mux_info[unit]->phy[group_phy_index[index]].group_id = group_id;
					flexe_mux_info[unit]->phy[group_phy_index[index]].phy_index = group_phy_index[index];
					flexe_mux_info[unit]->phy[group_phy_index[index]].phy_mode = phy_mode;
					flexe_mux_info[unit]->phy[group_phy_index[index]].available = 1;
				}

				/* storage the group_phy_info */
				flexe_mux_info[unit]->phy_mask |= phy_index;
				flexe_mux_info[unit]->group_num++;

				flexe_mux_info[unit]->group_info[group_id].group_phy_mode = phy_mode;
				flexe_mux_info[unit]->group_info[group_id].phy_num = phy_num;

				for (index = 0; index < phy_num; index++) 
				{
					flexe_mux_info[unit]->group_info[group_id].phy_index[index] = group_phy_index[index];
				}
			}
		}
		else 
		{
			printf("%s: unit = 0x%x, enable = 0x%x, group_id = 0x%x have already cfg...\r\n",
				                                                                __FUNCTION__,
				                                                                        unit,
				                                                                      enable,
				                                                                   group_id);

			return DEV_FAIL;
		}
	}
	else /* remove group's phy_info */
	{
		/* get group's info */
		rt = flexe_mux_phy_group_info_get(unit, group_id, &group_info);

		if (DEV_SUCCESS != rt) 
		{
			printf("%s: unit = 0x%x, get group_id = 0x%x's info fail, rt = %d...\r\n", __FUNCTION__, unit, group_id, rt);
			return rt;
		}

        /* set group_phy_index to default value */
		memset(group_phy_index, DEFAULT_VALUE, sizeof(group_phy_index));

		/* get group's phy_index and phy_num phy */
		for (index = 0; index < PHY_NUM; index++) 
		{
			if ((phy_index >> index) & MASK_1_BIT) 
			{
				group_phy_index[phy_num] = index;
				phy_num++;
			}
		}

		/* judge del phy info */
		if (phy_num > group_info.phy_num) 
		{
			printf("%s: unit = 0x%x, phy_num = %d, group_info.phy_num = %d del phy num err\r\n", __FUNCTION__, unit, phy_num, group_info.phy_num);
			return DEV_FAIL;
		}

        /* set group_info_sort phy_index to default value */
		memset(group_info_sort.phy_index, DEFAULT_VALUE, sizeof(group_info_sort.phy_index));

		/*phygroup_info_sort*/
		if (FLEXE_PHY_50G == phy_mode) 
		{
			for (index = 0; index < phy_num; index++) 
			{
				for (bit_start_idx = 0; bit_start_idx < PHY_NUM; bit_start_idx++) 
				{
					if (group_phy_index[index] == group_info.phy_index[bit_start_idx]) 
					{
						group_info_sort.phy_index[index] = group_info.phy_index[bit_start_idx];
					}
				}

				if (DEFAULT_VALUE == group_info_sort.phy_index[index]) 
				{
					printf("%s: unit = 0x%x, group_id = 0x%x del phy is not exit\r\n", __FUNCTION__, unit, group_id);
					return DEV_FAIL;
				}
			}
		}
		else /*FLEXE_PHY_100G*/
		{ 
			for (index = 0; index < 2*phy_num; index=index+2) 
			{
				for (bit_start_idx = 0; bit_start_idx < PHY_NUM; bit_start_idx++) 
				{
					if (group_phy_index[index/2] == group_info.phy_index[bit_start_idx]) 
					{
						group_info_sort.phy_index[index] = group_info.phy_index[bit_start_idx];
						group_info_sort.phy_index[index + 1] = group_info.phy_index[(bit_start_idx + 1)%PHY_NUM];
					}
				}

				if (DEFAULT_VALUE == group_info_sort.phy_index[index]) 
				{
					printf("%s: unit = 0x%x, group_id = 0x%x del phy is not exit\r\n", __FUNCTION__, unit, group_id);
					return DEV_FAIL;
				}
			}
		}

		group_info_sort.phy_num = phy_num;
		group_info_sort.group_phy_mode = phy_mode;

        if (0 != group_info_sort.phy_num) 
		{
			if ((FLEXE_PHY_100G == group_info_sort.group_phy_mode) || (FLEXE_PHY_50G == group_info_sort.group_phy_mode)) 
			{
				/* check flexe_logic_phy state, group's all phys whether no payload ts */
				for (index = 0; index < group_info_sort.phy_num; index++) 
				{
					if (0 != flexe_mux_info[unit]->logic_phy[group_info_sort.phy_index[index]].phy_comb_ts_num ||
						0 != flexe_mux_info[unit]->logic_phy[group_info_sort.phy_index[index]].enable) 
					{
						printf("%s: unit = 0x%x, enable = 0x%x, group_id = 0x%x, phy_index = 0x%x in working state...\r\n",
						                                                                                      __FUNCTION__,
						                                                                                              unit,
						                                                                                            enable,
						                                                                                          group_id,
						                                                                      group_info_sort.phy_index[index]);

						return DEV_FAIL;
					}
				}

				/* remove flexe_100g_mode */
				if (FLEXE_PHY_100G == group_info_sort.group_phy_mode) 
				{
					/* set group's logic_phy_info to default value */
					for (index = 0; index < 2 * group_info_sort.phy_num; index += 2) 
					{
						/* phy's available have already check in add group, so it's no need to check again */
						flexe_mux_info[unit]->phy[group_info_sort.phy_index[index]].group_id = DEFAULT_VALUE;
						flexe_mux_info[unit]->phy[group_info_sort.phy_index[index]].phy_index = DEFAULT_VALUE;
						flexe_mux_info[unit]->phy[group_info_sort.phy_index[index]].phy_mode = FLEXE_PHY_DEFAULT;
						flexe_mux_info[unit]->phy[group_info_sort.phy_index[index]].available = 0;

						flexe_mux_info[unit]->logic_phy[group_info_sort.phy_index[index]].group_id = DEFAULT_VALUE;
						flexe_mux_info[unit]->logic_phy[group_info_sort.phy_index[index]].phy_index = DEFAULT_VALUE;
						flexe_mux_info[unit]->logic_phy[group_info_sort.phy_index[index]].phy_master = DEFAULT_VALUE;
						flexe_mux_info[unit]->logic_phy[group_info_sort.phy_index[index]].available = 0;
						flexe_mux_info[unit]->logic_phy[group_info_sort.phy_index[index]].phy_comb_index[0] = DEFAULT_VALUE;
						flexe_mux_info[unit]->logic_phy[group_info_sort.phy_index[index]].phy_comb_index[1] = DEFAULT_VALUE;

						flexe_mux_info[unit]->logic_phy[group_info_sort.phy_index[index + 1]].group_id = DEFAULT_VALUE;
						flexe_mux_info[unit]->logic_phy[group_info_sort.phy_index[index + 1]].phy_index = DEFAULT_VALUE;
						flexe_mux_info[unit]->logic_phy[group_info_sort.phy_index[index + 1]].phy_master = DEFAULT_VALUE;
						flexe_mux_info[unit]->logic_phy[group_info_sort.phy_index[index + 1]].available = 0;

						/* storage the group_phy_info */
                        flexe_mux_info[unit]->phy_mask &= ~((UINT_8)(MASK_1_BIT << group_info_sort.phy_index[index]));
                        flexe_mux_info[unit]->phy_mask &= ~((UINT_8)(MASK_1_BIT << group_info_sort.phy_index[index + 1]));


						for (bit_start_idx = 0; bit_start_idx < PHY_NUM; bit_start_idx++) {
							if (group_info_sort.phy_index[index] == flexe_mux_info[unit]->group_info[group_id].phy_index[bit_start_idx]) 
							{
								flexe_mux_info[unit]->group_info[group_id].phy_index[bit_start_idx] = DEFAULT_VALUE;
							}
							if (group_info_sort.phy_index[index + 1] == flexe_mux_info[unit]->group_info[group_id].phy_index[bit_start_idx])
							{
								flexe_mux_info[unit]->group_info[group_id].phy_index[bit_start_idx] = DEFAULT_VALUE;
							}
						}
					}

					flexe_mux_info[unit]->group_info[group_id].phy_num -= 2 * group_info_sort.phy_num;
					if (0 == flexe_mux_info[unit]->group_info[group_id].phy_num) 
					{
						flexe_mux_info[unit]->group_num--;
						flexe_mux_info[unit]->group_info[group_id].group_id = DEFAULT_VALUE;
						flexe_mux_info[unit]->group_info[group_id].group_phy_mode = FLEXE_PHY_DEFAULT;
					}
				}
				else /* remove flexe_50g_mode */
				{
					/* set group's phy_info to default value */
					for (index = 0; index < group_info_sort.phy_num; index++) 
					{
						/* phy's available have already check in add group, so it's no need to check again */
						flexe_mux_info[unit]->phy[group_info_sort.phy_index[index]].group_id = DEFAULT_VALUE;
						flexe_mux_info[unit]->phy[group_info_sort.phy_index[index]].phy_index = DEFAULT_VALUE;
						flexe_mux_info[unit]->phy[group_info_sort.phy_index[index]].phy_mode = FLEXE_PHY_DEFAULT;
						flexe_mux_info[unit]->phy[group_info_sort.phy_index[index]].available = 0;

						flexe_mux_info[unit]->logic_phy[group_info_sort.phy_index[index]].group_id = DEFAULT_VALUE;
						flexe_mux_info[unit]->logic_phy[group_info_sort.phy_index[index]].phy_index = DEFAULT_VALUE;
						flexe_mux_info[unit]->logic_phy[group_info_sort.phy_index[index]].phy_master = DEFAULT_VALUE;
						flexe_mux_info[unit]->logic_phy[group_info_sort.phy_index[index]].available = 0;

						/* storage the group_phy_info */
                        flexe_mux_info[unit]->phy_mask &= ~((UINT_8)(MASK_1_BIT << group_info_sort.phy_index[index]));

						for (bit_start_idx = 0; bit_start_idx < PHY_NUM; bit_start_idx++) 
						{
							if (group_info_sort.phy_index[index] == flexe_mux_info[unit]->group_info[group_id].phy_index[bit_start_idx])
							{
								flexe_mux_info[unit]->group_info[group_id].phy_index[bit_start_idx] = DEFAULT_VALUE;
							}
						}
					}
					
					flexe_mux_info[unit]->group_info[group_id].phy_num -= group_info_sort.phy_num;
					if (0 == flexe_mux_info[unit]->group_info[group_id].phy_num) 
					{
						flexe_mux_info[unit]->group_num--;
						flexe_mux_info[unit]->group_info[group_id].group_id = DEFAULT_VALUE;
						flexe_mux_info[unit]->group_info[group_id].group_phy_mode = FLEXE_PHY_DEFAULT;
					}
				}
			}
			else if ((PCS_PHY_200G == group_info_sort.group_phy_mode) ||
				     (PCS_PHY_100G == group_info_sort.group_phy_mode) ||
				     (PCS_PHY_50G == group_info_sort.group_phy_mode)) 
			{
				/* check pcs mode's enable state */
				for (index = 0; index < group_info_sort.phy_num; index++) 
				{
					if (0 != flexe_mux_info[unit]->phy[group_info_sort.phy_index[index]].enable) {
						printf("%s: unit = 0x%x, enable = 0x%x, group_id = 0x%x, phy_index = 0x%x in working state...\r\n",
						                                                                                      __FUNCTION__,
						                                                                                              unit,
						                                                                                            enable,
						                                                                                          group_id,
						                                                                      group_info.phy_index[index]);

						return DEV_FAIL;
					}
				}

				/* set group's phy_info to default value */
				for (index = 0; index < group_info_sort.phy_num; index++) 
				{
					/* phy's available have already check in add group, so it's no need to check again */
					flexe_mux_info[unit]->phy[group_info_sort.phy_index[index]].group_id = DEFAULT_VALUE;
					flexe_mux_info[unit]->phy[group_info_sort.phy_index[index]].phy_index = DEFAULT_VALUE;
					flexe_mux_info[unit]->phy[group_info_sort.phy_index[index]].phy_mode = FLEXE_PHY_DEFAULT;
					flexe_mux_info[unit]->phy[group_info_sort.phy_index[index]].available = 0;
				}

                /* storage the group_phy_info */
                for (index = 0; index < group_info_sort.phy_num; index++) 
				{
                    flexe_mux_info[unit]->phy_mask &= ~((UINT_8)(MASK_1_BIT << group_info_sort.phy_index[index]));
                }

                flexe_mux_info[unit]->group_info[group_id].phy_num -= group_info_sort.phy_num;

                if (0 == flexe_mux_info[unit]->group_info[group_id].phy_num) 
				{
                    flexe_mux_info[unit]->group_num--;
                    flexe_mux_info[unit]->group_info[group_id].group_id = DEFAULT_VALUE;
                    flexe_mux_info[unit]->group_info[group_id].group_phy_mode = FLEXE_PHY_DEFAULT;
                }

                for (index = 0; index < group_info_sort.phy_num; index++) 
				{
                    flexe_mux_info[unit]->group_info[group_id].phy_index[group_info_sort.phy_index[index]] = DEFAULT_VALUE;
                }
			}
			else 
			{
				printf("%s: unit = 0x%x, enable = 0x%x, group_id = 0x%x, phy not cfg...\r\n", __FUNCTION__, unit, enable, group_id);

				return DEV_FAIL;
			}
		}
		else 
		{
			printf("%s: unit = 0x%x, enable = 0x%x, group_id = 0x%x not cfg...\r\n", __FUNCTION__, unit, enable, group_id);

			return DEV_FAIL;
		}
	}
	return DEV_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     get mux's group_phy info.
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
*     group_info: group_info pointer
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
*
******************************************************************************/
dev_err_t flexe_mux_phy_group_info_get(UINT_8 unit,
                                       UINT_8 group_id,
                                       flexe_mux_group_info_t *group_info)
{
    UINT_8 index = 0;

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
    /* check pointer whether null */
    if (!group_info) 
	{
        printf("%s: unit = 0x%x, pointer is null...\r\n", __FUNCTION__, unit);
		
        return DEV_POINTER_NULL;
    }

	/* check unit whether initialized */
	if (flexe_mux_info[unit]) 
	{
		group_info->unit = flexe_mux_info[unit]->group_info[group_id].unit;
		group_info->group_id = flexe_mux_info[unit]->group_info[group_id].group_id;
		group_info->group_phy_mode = flexe_mux_info[unit]->group_info[group_id].group_phy_mode;
		group_info->phy_num = flexe_mux_info[unit]->group_info[group_id].phy_num;

		/*for (index = 0; index < flexe_mux_info[unit]->group_info[group_id].phy_num; index++) {*/
		for (index = 0; index < PHY_NUM; index++) 
		{
			group_info->phy_index[index] = flexe_mux_info[unit]->group_info[group_id].phy_index[index];
		}
	}
	/* unit's mux not initialized */
	else 
	{
        printf("%s: unit = 0x%x not initialized...\r\n", __FUNCTION__, unit);

        return DEV_DEINITIALIZED;
    }
    
    return DEV_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     get mux's phy info.
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
*     phy_index: phy's index pointer
*     phy_num: group's phy num
*     phy_info: phy's info out pointer
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
*
******************************************************************************/
dev_err_t flexe_mux_phy_group_info_check(UINT_8 unit,
                                         UINT_8 *phy_index,
                                         UINT_8 phy_num,
                                         flexe_mux_phy_info_t *phy_info)
{
    UINT_8 index = 0;
    
    /* check unit whether normal */
    if (unit > (DEV_NUM - 1)) {
        printf("%s: invalid unit = 0x%x...\r\n", __FUNCTION__, unit);
        
        return DEV_PARA_ERR;
    }

    /* check pointer whether null */
    if (!phy_index || !phy_info) {
        printf("%s: unit = 0x%x, pointer is null...\r\n", __FUNCTION__, unit);
        
        return DEV_POINTER_NULL;
    }

    /* check phy_index whether normal */
    if (phy_num > PHY_NUM) {
        printf("%s: unit = 0x%x, invalid phy_num = 0x%x...\r\n", __FUNCTION__, unit, phy_num);
        
        return DEV_PARA_ERR;
    }

    /* check unit whether initialized */
    if (flexe_mux_info[unit]) {
        /* check phy whether in one group */
        for (index = 0; index < phy_num; index++) {
            if (flexe_mux_info[unit]->phy[*phy_index].group_id !=
                flexe_mux_info[unit]->phy[*(phy_index + index)].group_id) {
                printf("%s: phy not in one group, unit = 0x%x...\r\n", __FUNCTION__, unit);

                return DEV_PARA_ERR;
            }
        }

        /* get the group's phy info */
        for (index = 0; index < phy_num; index++) {
            (*(phy_info + index)).unit = flexe_mux_info[unit]->phy[*(phy_index + index)].unit;
            (*(phy_info + index)).group_id = flexe_mux_info[unit]->phy[*(phy_index + index)].group_id;
            (*(phy_info + index)).phy_index = flexe_mux_info[unit]->phy[*(phy_index + index)].phy_index;
            (*(phy_info + index)).phy_mode = flexe_mux_info[unit]->phy[*(phy_index + index)].phy_mode;
            (*(phy_info + index)).ts_mask = flexe_mux_info[unit]->phy[*(phy_index + index)].ts_mask;
            (*(phy_info + index)).ts_num = flexe_mux_info[unit]->phy[*(phy_index + index)].ts_num;
            (*(phy_info + index)).enable = flexe_mux_info[unit]->phy[*(phy_index + index)].enable;
            (*(phy_info + index)).available = flexe_mux_info[unit]->phy[*(phy_index + index)].available;
        }
    }
    /* unit's mux not initialized */
    else {
        printf("%s: unit = 0x%x not initialized...\r\n", __FUNCTION__, unit);

        return DEV_DEINITIALIZED;
    }

    return DEV_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     set mux's phy info.
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
*     phy_info: phy's info
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
*
******************************************************************************/
dev_err_t flexe_mux_phy_info_cfg(UINT_8 unit, flexe_mux_phy_info_t phy_info)
{
    /* check unit whether normal */
    if (unit > (DEV_NUM - 1)) 
	{
        printf("%s: invalid unit = 0x%x...\r\n", __FUNCTION__, unit);
        return DEV_PARA_ERR;
    }

    /* check phy_index whether normal */
    if (phy_info.phy_index > (PHY_NUM - 1)) 
	{
        printf("%s: unit = 0x%x, invalid phy_index = 0x%x...\r\n", __FUNCTION__, unit, phy_info.phy_index);
        return DEV_PARA_ERR;
    }

	if (NULL == flexe_mux_info[unit])
	{
		printf("%s: unit = 0x%x not initialized...\r\n", __FUNCTION__, unit);
        return DEV_DEINITIALIZED;
	}

	flexe_mux_info[unit]->phy[phy_info.phy_index].ts_mask = phy_info.ts_mask;
	flexe_mux_info[unit]->phy[phy_info.phy_index].ts_num = phy_info.ts_num;
	flexe_mux_info[unit]->phy[phy_info.phy_index].enable = phy_info.enable;
	
    return DEV_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     get mux's phy info.
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
*     phy_index: phy's index
*     phy_info: phy's info pointer
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
*
******************************************************************************/
dev_err_t flexe_mux_phy_info_get(UINT_8 unit, UINT_8 phy_index, flexe_mux_phy_info_t *phy_info)
{
    /* check unit whether normal */
    if (unit > (DEV_NUM - 1)) 
	{
        printf("%s: invalid unit = 0x%x...\r\n", __FUNCTION__, unit);
        return DEV_PARA_ERR;
    }

    /* check phy_index whether normal */
    if (phy_index > (PHY_NUM - 1)) 
	{
        printf("%s: unit = 0x%x, invalid phy_index = 0x%x...\r\n", __FUNCTION__, unit, phy_index);
        return DEV_PARA_ERR;
    }

    /* check pointer whether null */
    if (!phy_info) 
	{
        printf("%s: unit = 0x%x, pointer is null...\r\n", __FUNCTION__, unit);
        return DEV_POINTER_NULL;
    }

	if (NULL == flexe_mux_info[unit])
	{
        printf("%s: unit = 0x%x not initialized...\r\n", __FUNCTION__, unit);
        return DEV_DEINITIALIZED;
	}

        phy_info->unit = flexe_mux_info[unit]->phy[phy_index].unit;
        phy_info->group_id = flexe_mux_info[unit]->phy[phy_index].group_id;
        phy_info->phy_index = flexe_mux_info[unit]->phy[phy_index].phy_index;
        phy_info->phy_mode = flexe_mux_info[unit]->phy[phy_index].phy_mode;
        phy_info->ts_mask = flexe_mux_info[unit]->phy[phy_index].ts_mask;
        phy_info->ts_num = flexe_mux_info[unit]->phy[phy_index].ts_num;
        phy_info->enable = flexe_mux_info[unit]->phy[phy_index].enable;
        phy_info->available = flexe_mux_info[unit]->phy[phy_index].available;

    return DEV_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     get mux_chip info.
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
*      liming        2018-03-21         1.0            initial
*
******************************************************************************/
dev_err_t flexe_mux_chip_info_get(UINT_8 unit, flexe_mux_info_t **flexe_mux_chip_info)
{    
    /* check unit whether normal */
    if (unit > (DEV_NUM - 1)) 
	{
        printf("%s: invalid unit = 0x%x...\r\n", __FUNCTION__, unit);
        return DEV_PARA_ERR;
    }

	/* check pointer whether normal */
	if (!flexe_mux_chip_info) 
	{
        printf("%s: invalid pointer, unit = 0x%x...\r\n", __FUNCTION__, unit);
        return DEV_POINTER_NULL;
	}

	if (NULL == flexe_mux_info[unit])
	{
        printf("%s: unit = 0x%x not initialized...\r\n", __FUNCTION__, unit);
        return DEV_DEINITIALIZED;
	}

	*flexe_mux_chip_info = flexe_mux_info[unit];

	return DEV_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     get mux's logic_phy info.
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
*     phy_index: phy's index pointer
*     phy_num: group's phy num
*     phy_info: phy's info out pointer
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
*      liming        2018-06-13         1.0            initial
*
******************************************************************************/
dev_err_t flexe_mux_logic_phy_group_info_get(UINT_8 unit,
                                             UINT_8 *phy_index,
                                             UINT_8 phy_num,
                                             flexe_mux_logic_phy_info_t *phy_info)
{
    UINT_8 index = 0;
    
    /* check unit whether normal */
    if (unit > (DEV_NUM - 1)) 
	{
        printf("%s: invalid unit = 0x%x...\r\n", __FUNCTION__, unit);
        return DEV_PARA_ERR;
    }
	
    /* check pointer whether null */
    if (!phy_index || !phy_info) 
	{
        printf("%s: unit = 0x%x, pointer is null...\r\n", __FUNCTION__, unit);
        return DEV_POINTER_NULL;
    }
	
    /* check phy_index whether normal */
    if (phy_num > PHY_NUM) 
	{
        printf("%s: unit = 0x%x, invalid phy_num = 0x%x...\r\n", __FUNCTION__, unit, phy_num);
        return DEV_PARA_ERR;
    }

	if (NULL == flexe_mux_info[unit])
	{
        printf("%s: unit = 0x%x not initialized...\r\n", __FUNCTION__, unit);
        return DEV_DEINITIALIZED;
	}

    /* check phy whether in one group */
	for (index = 0; index < phy_num; index++) 
	{
        if (flexe_mux_info[unit]->logic_phy[*phy_index].group_id !=
			flexe_mux_info[unit]->logic_phy[*(phy_index + index)].group_id) 
		{
			printf("%s: logic_phy not in one group, unit = 0x%x...\r\n", __FUNCTION__, unit);
			return DEV_PARA_ERR;
    	}

		if ((FLEXE_PHY_50G != flexe_mux_info[unit]->phy[*phy_index].phy_mode) &&
			(FLEXE_PHY_100G != flexe_mux_info[unit]->phy[*phy_index].phy_mode)) {
			printf("%s: logic_phy not working in flexe mode, unit = 0x%x...\r\n", __FUNCTION__, unit);
			return DEV_PARA_ERR;
		}
	}

	/* get the group's phy info */
	/* 100g_mode */
	if (1 == flexe_mux_info[unit]->logic_phy[*phy_index].phy_master) 
	{
		for (index = 0; index < phy_num; index++) 
		{
			(*(phy_info + index)).unit = flexe_mux_info[unit]->logic_phy[*(phy_index + index)].unit;
			(*(phy_info + index)).group_id = flexe_mux_info[unit]->logic_phy[*(phy_index + index)].group_id;
			(*(phy_info + index)).phy_index = flexe_mux_info[unit]->logic_phy[*(phy_index + index)].phy_index;
			(*(phy_info + index)).phy_master = flexe_mux_info[unit]->logic_phy[*(phy_index + index)].phy_master;
			(*(phy_info + index)).phy_comb_index[0] = flexe_mux_info[unit]->logic_phy[*(phy_index + index)].phy_comb_index[0];
			(*(phy_info + index)).phy_comb_index[1] = flexe_mux_info[unit]->logic_phy[*(phy_index + index)].phy_comb_index[1];
			(*(phy_info + index)).available = flexe_mux_info[unit]->logic_phy[*(phy_index + index)].available;
		}
	}
	else /* 50g_mode */
	{
		for (index = 0; index < phy_num; index++) 
		{
			(*(phy_info + index)).unit = flexe_mux_info[unit]->logic_phy[*(phy_index + index)].unit;
			(*(phy_info + index)).group_id = flexe_mux_info[unit]->logic_phy[*(phy_index + index)].group_id;
			(*(phy_info + index)).phy_index = flexe_mux_info[unit]->logic_phy[*(phy_index + index)].phy_index;
			(*(phy_info + index)).phy_master = flexe_mux_info[unit]->logic_phy[*(phy_index + index)].phy_master;
			(*(phy_info + index)).available = flexe_mux_info[unit]->logic_phy[*(phy_index + index)].available;
		}
	}

	return DEV_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     set mux's phy info.
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
*     phy_info: phy's info
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
*      liming        2018-06-14         1.0            initial
*
******************************************************************************/
dev_err_t flexe_mux_logic_phy_info_cfg(UINT_8 unit, flexe_mux_logic_phy_info_t phy_info)
{
    UINT_8 index = 0;
    
    /* check unit whether normal */
    if (unit > (DEV_NUM - 1)) 
	{
        printf("%s: invalid unit = 0x%x...\r\n", __FUNCTION__, unit);
        return DEV_PARA_ERR;
    }

    /* check phy_index whether normal */
    if (phy_info.phy_index > (PHY_NUM - 1)) 
	{
        printf("%s: unit = 0x%x, invalid phy_index = 0x%x...\r\n", __FUNCTION__, unit, phy_info.phy_index);
        return DEV_PARA_ERR;
    }

	if (NULL == flexe_mux_info[unit])
	{
        printf("%s: unit = 0x%x not initialized...\r\n", __FUNCTION__, unit);
        return DEV_DEINITIALIZED;
	}

	/* flexe_100g_mode */
	if (1 == phy_info.phy_master) 
	{
		/* add traffic */
		if (1 == phy_info.enable) 
		{
			for (index = 0; index < 4; index++) 
			{
				flexe_mux_info[unit]->logic_phy[phy_info.phy_index].phy_comb_ts_mask[index] |= phy_info.phy_comb_ts_mask[index];
			}

			flexe_mux_info[unit]->logic_phy[phy_info.phy_index].phy_comb_ts_num += phy_info.phy_comb_ts_num;
			flexe_mux_info[unit]->logic_phy[phy_info.phy_index].enable = phy_info.enable;
			/*flexe_mux_info[unit]->logic_phy[phy_info.phy_comb_index[1]].enable = phy_info.enable;*/
		}
		else /* delete traffic */
		{
			for (index = 0; index < 4; index++) 
			{
				flexe_mux_info[unit]->logic_phy[phy_info.phy_index].phy_comb_ts_mask[index] &= ~phy_info.phy_comb_ts_mask[index];
			}

			flexe_mux_info[unit]->logic_phy[phy_info.phy_index].phy_comb_ts_num -= phy_info.phy_comb_ts_num;

			if (0 == flexe_mux_info[unit]->logic_phy[phy_info.phy_index].phy_comb_ts_num) 
			{
				flexe_mux_info[unit]->logic_phy[phy_info.phy_index].enable = 0;
			}
		}	
	}
	else if (0 == phy_info.phy_master) /* flexe_50g_mode */
	{
		if (1 == phy_info.enable) /* add traffic */
		{
			for (index = 0; index < 2; index++) 
			{
				flexe_mux_info[unit]->logic_phy[phy_info.phy_index].phy_comb_ts_mask[index] |= phy_info.phy_comb_ts_mask[index];
			}

			flexe_mux_info[unit]->logic_phy[phy_info.phy_index].phy_comb_ts_num += phy_info.phy_comb_ts_num;
			flexe_mux_info[unit]->logic_phy[phy_info.phy_index].enable = phy_info.enable;
		}
		else /* delete traffic */
		{
			for (index = 0; index < 2; index++) 
			{
				flexe_mux_info[unit]->logic_phy[phy_info.phy_index].phy_comb_ts_mask[index] &= ~phy_info.phy_comb_ts_mask[index];
			}

			flexe_mux_info[unit]->logic_phy[phy_info.phy_index].phy_comb_ts_num -= phy_info.phy_comb_ts_num;

			if (0 == flexe_mux_info[unit]->logic_phy[phy_info.phy_index].phy_comb_ts_num) 
			{
				flexe_mux_info[unit]->logic_phy[phy_info.phy_index].enable = 0;
			}
		}
	}
	else 
	{
		printf("%s: unit = 0x%x, invalid phy_master_info = 0x%x...\r\n", __FUNCTION__, unit, phy_info.phy_master);
        return DEV_PARA_ERR;
	}
		
	return DEV_SUCCESS;
}

dev_err_t flexe_mux_logic_phy_out(UINT_8 unit, UINT_8 mux_logic_phy_info[])
{
	UINT_8 index = 0;

	if (NULL == flexe_mux_info[unit])
	{
		printf("%s: unit = 0x%x not initialized...\r\n", __FUNCTION__, unit);
        return DEV_DEINITIALIZED;
	}

	for (index = 0; index < PHY_NUM; index++) 
	{
		mux_logic_phy_info[index] = flexe_mux_info[unit]->logic_phy[index].available;
	}

	return DEV_SUCCESS;
}

dev_err_t flexe_mux_ch_belong_group(UINT_8 unit,UINT_8 ch,UINT_8 group_id,UINT_8* status)
{
	if (NULL == flexe_mux_info[unit])
	{
		printf("%s: unit = 0x%x not initialized...\r\n", __FUNCTION__, unit);
        return DEV_DEINITIALIZED;
	}

	if (group_id == flexe_mux_info[unit]->flexe_ch_info[ch]->group_id)
	{
		*status = true;
	}
	else
	{
		*status = false;
	}
	
	return DEV_SUCCESS;
}

dev_err_t flexe_mux_group_ch_delete(UINT_8 unit,UINT_8 group_id)
{
	UINT_8 channel = 0;
	if (NULL == flexe_mux_info[unit])
	{
		printf("%s: unit = 0x%x not initialized...\r\n", __FUNCTION__, unit);
        return DEV_DEINITIALIZED;
	}

	for(channel = 0;channel < CH_NUM;channel++)
	{
		if (group_id == flexe_mux_info[unit]->flexe_ch_info[channel]->group_id)
		{
			flexe_mux_info[unit]->flexe_ch_info[channel]->group_id = DEFAULT_VALUE;
		}
	}
		
	return DEV_SUCCESS;
}
