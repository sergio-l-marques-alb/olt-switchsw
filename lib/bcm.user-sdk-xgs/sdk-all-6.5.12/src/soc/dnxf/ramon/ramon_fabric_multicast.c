/*
 * $Id: ramon_fabric_multicast.c,v 1.7.48.1 Broadcom SDK $
 *
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * SOC RAMON FABRIC MULTICAST
 */
#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_FABRIC
#include <shared/bsl.h>
#include <soc/dnxc/legacy/error.h>
#include <soc/dnxc/legacy/dnxc_wb.h>
#include <soc/mem.h>

#include <soc/defs.h>
#include <soc/error.h>
#include <soc/mcm/allenum.h>
#include <soc/mcm/memregs.h>
#include <soc/sand/sand_mem.h>

#include <soc/dnxf/cmn/dnxf_drv.h>
#include <shared/bitop.h>
/*dnxf data*/
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_fabric.h>

#include <soc/dnxf/ramon/ramon_fabric_multicast.h>

#define _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE (32 * 1024)

#ifndef SOC_RAMON_MULTICAST_TABLE_UPDATE_READ_ENABLED

STATIC const soc_dnxf_multicast_read_range_info_t _soc_ramon_fabric_multicast_read_range_info_standard[] = {
    {
        0,
        _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE - 1,
        0,
        1
    },
    {
        _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE,
        2 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE - 1,
        1,
        1
    },
    {
        2 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE,
        3 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE - 1,
        2,
        1
    },
    {
        3 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE,
        4 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE - 1,
        3,
        1
    },
    {
        4 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE,
        5 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE - 1,
        4,
        1
    },
    {
        5 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE,
        6 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE - 1,
        5,
        1
    },
    {
        6 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE,
        7 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE - 1,
        6,
        1
    },
    {
        7 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE,
        8 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE - 1,
        7,
        1
    }
};

STATIC const soc_dnxf_multicast_read_range_info_t _soc_ramon_fabric_multicast_read_range_info_128k_half[] = {
    {
        0,
        _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE - 1,
        0,
        1
    },
    {
        _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE,
        2 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE - 1,
        1,
        1
    },
    {
        2 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE,
        3 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE - 1,
        0,
        0
    },
    {
        3 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE,
        4 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE - 1,
        1,
        0
    }
};

STATIC const soc_dnxf_multicast_read_range_info_t _soc_ramon_fabric_multicast_read_range_info_256k_half[] = {
    {
        0,
        _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE - 1,
        0,
        1
    },
    {
        _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE,
        2 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE - 1,
        1,
        1
    },
    {
        2 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE,
        3 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE - 1,
        2,
        1
    },
    {
        3 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE,
        4 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE - 1,
        3,
        1
    },
    {
        4 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE,
        5 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE - 1,
        0,
        0
    },
    {
        5 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE,
        6 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE - 1,
        1,
        0
    },
    {
        6 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE,
        7 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE - 1,
        2,
        0
    },
    {
        7 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE,
        8 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE - 1,
        3,
        0
    }
};

STATIC const soc_dnxf_multicast_read_range_info_t _soc_ramon_fabric_multicast_read_range_info_512k_half[] = {
    {
        0,
        _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE - 1,
        0,
        1
    },
    {
        _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE,
        2 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE - 1,
        1,
        1
    },
    {
        2 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE,
        3 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE - 1,
        2,
        1
    },
    {
        3 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE,
        4 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE - 1,
        3,
        1
    },
    {
        4 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE,
        5 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE - 1,
        4,
        1
    },
    {
        5 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE,
        6 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE - 1,
        5,
        1
    },
    {
        6 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE,
        7 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE - 1,
        6,
        1
    },
    {
        7 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE,
        8 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE - 1,
        7,
        1
    },
    {
        8 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE,
        9 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE - 1,
        0,
        0
    },
    {
        9 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE,
        10 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE - 1,
        1,
        0
    },
    {
        10 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE,
        11 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE - 1,
        2,
        0
    },
    {
        11 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE,
        12 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE - 1,
        3,
        0
    },
    {
        12 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE,
        13 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE - 1,
        4,
        0
    },
    {
        13 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE,
        14 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE - 1,
        5,
        0
    },
    {
        14 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE,
        15 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE - 1,
        6,
        0
    },
    {
        15 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE,
        16 * _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE - 1,
        7,
        0
    }
};

shr_error_e
soc_ramon_fabric_multicast_multi_read_info_get(int unit, const soc_dnxf_multicast_read_range_info_t **info, int *info_size)
{
    SHR_FUNC_INIT_VARS(unit);

    /*get mc table read info*/
    switch (dnxf_data_fabric.multicast.id_range_get(unit))
    {
       case soc_dnxf_multicast_table_mode_128k_half:
           *info = _soc_ramon_fabric_multicast_read_range_info_128k_half;
           *info_size = sizeof(_soc_ramon_fabric_multicast_read_range_info_128k_half) / sizeof(soc_dnxf_multicast_read_range_info_t);
           break;
       case soc_dnxf_multicast_table_mode_256k_half:
           *info = _soc_ramon_fabric_multicast_read_range_info_256k_half;
           *info_size = sizeof(_soc_ramon_fabric_multicast_read_range_info_256k_half) / sizeof(soc_dnxf_multicast_read_range_info_t);
           break;
       case soc_dnxf_multicast_table_mode_512k_half:
           *info = _soc_ramon_fabric_multicast_read_range_info_512k_half;
           *info_size = sizeof(_soc_ramon_fabric_multicast_read_range_info_512k_half) / sizeof(soc_dnxf_multicast_read_range_info_t);
           break;
       default:
           *info = _soc_ramon_fabric_multicast_read_range_info_standard;
           *info_size = sizeof(_soc_ramon_fabric_multicast_read_range_info_standard) / sizeof(soc_dnxf_multicast_read_range_info_t);
           break;
    }

    SHR_FUNC_EXIT;
}
#endif /* ifndef SOC_RAMON_MULTICAST_TABLE_UPDATE_READ_ENABLED */

static uint32* ramon_multi_entry_array = NULL;

#ifdef SOC_RAMON_MULTICAST_TABLE_UPDATE_READ_ENABLED
STATIC const
soc_dnxf_multicast_read_range_info_t _soc_ramon_fabric_multicast_read_range_info_standard[] = {
    {
        0,
        _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE - 1,
        0,
        1
    }
};
#endif /* SOC_RAMON_MULTICAST_TABLE_UPDATE_READ_ENABLED */

/* 
 *write range of multicast groups - the data can be separated per instance of MCT_MCTm
 */
shr_error_e
soc_ramon_fabric_multicast_multi_write_range(int unit, int mem_flags, soc_multicast_t group_min, soc_multicast_t group_max, uint32 *entry_array)
{
    int rv;
    SHR_FUNC_INIT_VARS(unit);

    rv = soc_mem_array_write_range(unit, mem_flags, RTP_MULTI_CAST_TABLE_UPDATEm, 0, MEM_BLOCK_ALL, group_min, group_max, entry_array);
    SHR_IF_ERR_EXIT(rv);


exit:
    SHR_FUNC_EXIT;
}

/*
 *read range of multicast groups
 */
#ifdef SOC_RAMON_MULTICAST_TABLE_UPDATE_READ_ENABLED
shr_error_e
soc_ramon_fabric_multicast_multi_read_range(int unit, soc_multicast_t group_min, soc_multicast_t group_max, uint32 *entry_array)
{
    int rv;
    SHR_FUNC_INIT_VARS(unit);

    rv = soc_mem_array_read_range(unit, RTP_MULTI_CAST_TABLE_UPDATEm, 0, MEM_BLOCK_ALL, group_min, group_max, entry_array);
    SHR_IF_ERR_EXIT(rv);


exit:
    SHR_FUNC_EXIT;
}
#endif /* SOC_RAMON_MULTICAST_TABLE_UPDATE_READ_ENABLED */

shr_error_e
soc_ramon_fabric_multicast_local_route_default_init(int unit)
{
    bcm_fabric_module_vector_t data;
    uint32 mc_table_size = 0;
    uint32 mc_local_route_bit = 0;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(data, 0, sizeof(data));

    if (!SAL_BOOT_PLISIM)
    {
        /* Disable multicast local route by default */
        SHR_IF_ERR_EXIT(soc_ramon_fabric_multicast_local_route_bit_get(unit, &mc_local_route_bit));

        SHR_BITSET(data, mc_local_route_bit);

        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_multicast_table_size_get, (unit, &mc_table_size)));

        SHR_IF_ERR_EXIT(sand_fill_partial_table_with_entry(unit, RTP_MULTI_CAST_TABLE_UPDATEm, 0, 0, MEM_BLOCK_ALL, 0, mc_table_size-1, data));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get multicast local route bit.
 *   
 *   For each MC-ID there is a bitmap of groups that belongs to it.
 *   Right after the last group there is another bit that indicates
 *   whether multicast local route is enabled for this MC-ID.
 * \param [in] unit -
 *   The unit number.
 * \param [out] mc_local_route_bit -
 *   The index of the bit that indicated whether multicast local route
 *   is enabled.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
shr_error_e
soc_ramon_fabric_multicast_local_route_bit_get(int unit, uint32 *mc_local_route_bit)
{
    uint32 mc_table_entry_size = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_multicast_table_entry_size_get, (unit, &mc_table_entry_size)));
    *mc_local_route_bit = mc_table_entry_size;

exit:
    SHR_FUNC_EXIT;
}


/*
 * Function:
 *      soc_ramon_fabric_multicast_low_prio_drop_select_priority_set
 * Purpose:
 *      Select the highest priority to be considered as low
 * Parameters:
 *      unit      - (IN)  Unit number.
 *      prio      - (IN)  Highest priority to be considered as low
 * Returns:
 *      _SHR_E_xxx
 */
shr_error_e 
soc_ramon_fabric_multicast_low_prio_drop_select_priority_set(int unit, soc_dnxf_fabric_priority_t prio)
{
    uint32 reg_val, low_prio_select;
    int blk, nof_qrh, nof_dch;
    SHR_FUNC_INIT_VARS(unit);
    
    /*Low priority multicast priority mapping.
      bit 4 - mapping of priority 00 - 1 for low priority, 0 for normal priority.
      bit 5 - mapping of priority 01 - 1 for low priority, 0 for normal priority.
      bit 6 - mapping of priority 10 - 1 for low priority, 0 for normal priority.
      bit 7 - mapping of priority 11 - 1 for low priority, 0 for normal priority*/
    switch(prio)
    {
        case soc_dnxf_fabric_priority_0:
            low_prio_select = 0x1;
            break;
        case soc_dnxf_fabric_priority_1:
            low_prio_select = 0x3;
            break;
        case soc_dnxf_fabric_priority_2:
            low_prio_select = 0x7;
            break;
        case soc_dnxf_fabric_priority_3:
            low_prio_select = 0xf;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong priority %d",prio);
    }

    /*Set MC low priority at RTP*/
    nof_qrh = dnxf_data_device.blocks.nof_instances_qrh_get(unit);
    for (blk = 0; blk < nof_qrh; blk++) {
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, QRH_LOW_PR_MUL_CTRLr, blk, 0, &reg_val));
        soc_reg_field_set(unit, QRH_LOW_PR_MUL_CTRLr, &reg_val, LOW_PR_MUL_PRIOSELf, low_prio_select);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, QRH_LOW_PR_MUL_CTRLr, blk, 0, reg_val));
    }

    /*Set MC low priority at DCH*/
    nof_dch =  dnxf_data_device.blocks.nof_instances_dch_get(unit);
    for (blk = 0; blk < nof_dch; blk++)
    {
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, DCH_DCH_ENABLERS_REGISTER_1r, blk, 0, &reg_val));
        soc_reg_field_set(unit, DCH_DCH_ENABLERS_REGISTER_1r, &reg_val, LOW_PR_DROP_ENf, low_prio_select);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, DCH_DCH_ENABLERS_REGISTER_1r, blk, 0, reg_val));
    }


      
exit:
    SHR_FUNC_EXIT;
  
}


/*
 * Function:
 *      soc_ramon_fabric_multicast_low_prio_drop_select_priority_get
 * Purpose:
 *      Get the highest priority to be considered as low
 * Parameters:
 *      unit      - (IN)  Unit number.
 *      prio      - (OUT) Highest priority to be considered as low
 * Returns:
 *      _SHR_E_xxx
 */
shr_error_e 
soc_ramon_fabric_multicast_low_prio_drop_select_priority_get(int unit, soc_dnxf_fabric_priority_t* prio)
{
    uint32 reg_val, low_prio_select;
    SHR_FUNC_INIT_VARS(unit);
    
    SHR_IF_ERR_EXIT(soc_reg32_get(unit, QRH_LOW_PR_MUL_CTRLr, 0, 0, &reg_val));
    low_prio_select = soc_reg_field_get(unit, QRH_LOW_PR_MUL_CTRLr, reg_val, LOW_PR_MUL_PRIOSELf);
    
    switch(low_prio_select)
    {
        case 0x1:
            *prio = soc_dnxf_fabric_priority_0;
            break;
        case 0x3:
            *prio = soc_dnxf_fabric_priority_1;
            break;
        case 0x7:
            *prio = soc_dnxf_fabric_priority_2;
            break; 
        case 0xf:
            *prio = soc_dnxf_fabric_priority_3;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unrecognized priority %d",low_prio_select);
    }
    
exit:
    SHR_FUNC_EXIT;
  
}

/*
 * Function:
 *      soc_ramon_fabric_multicast_low_prio_threshold_validate
 * Purpose:
 *      Validate thresholds for low priority multicast drop
 * Parameters:
 *      unit      - (IN) Unit number.
 *      type      - (IN) bcmFabricMcLowPrioDropThUp or bcmFabricMcLowPrioDropThDown
 *      arg       - (IN) Threshold
 * Returns:
 *      _SHR_E_xxx
 */
shr_error_e 
soc_ramon_fabric_multicast_low_prio_threshold_validate(int unit, bcm_fabric_control_t type, int arg)
{
    uint64  val64;
    SHR_FUNC_INIT_VARS(unit);
      
    COMPILER_64_SET(val64, 0, arg);
    switch(type)
    {
        case bcmFabricMcLowPrioDropThUp:
            if(SOC_FAILURE(soc_reg64_field_validate(unit, QRH_LOW_PR_MULTHr, LOW_PR_MUL_DROP_TH_HIGHf, val64))) {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Argument is too big %d",arg);
            }
            break;
        case bcmFabricMcLowPrioDropThDown:
            if(SOC_FAILURE(soc_reg64_field_validate(unit, QRH_LOW_PR_MULTHr, LOW_PR_MUL_DROP_TH_LOWf, val64))) {
                 SHR_ERR_EXIT(_SHR_E_PARAM, "Argument is too big %d",arg);
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported control type %d",type);
    }
     
exit:
    SHR_FUNC_EXIT;
  
}

/*
 * Function:
 *      soc_ramon_fabric_multicast_low_prio_threshold_set
 * Purpose:
 *      Set thresholds for low priority multicast drop
 * Parameters:
 *      unit      - (IN) Unit number.
 *      type      - (IN) bcmFabricMcLowPrioDropThUp or bcmFabricMcLowPrioDropThDown
 *      arg       - (IN) Threshold
 * Returns:
 *      _SHR_E_xxx
 */
shr_error_e 
soc_ramon_fabric_multicast_low_prio_threshold_set(int unit, bcm_fabric_control_t type, int arg)
{
    uint64 reg_val, val64;
    int blk, nof_qrh;
    SHR_FUNC_INIT_VARS(unit);
	
    nof_qrh = dnxf_data_device.blocks.nof_instances_qrh_get(unit);
    COMPILER_64_SET(val64, 0, arg);
    switch(type)
    {
        case bcmFabricMcLowPrioDropThUp:
            for (blk = 0; blk < nof_qrh; blk++) {
                SHR_IF_ERR_EXIT(soc_reg64_get(unit, QRH_LOW_PR_MULTHr, blk, 0, &reg_val));
                soc_reg64_field_set(unit, QRH_LOW_PR_MULTHr, &reg_val, LOW_PR_MUL_DROP_TH_HIGHf, val64);
                SHR_IF_ERR_EXIT(soc_reg64_set(unit, QRH_LOW_PR_MULTHr, blk, 0, reg_val));
            }
            break;
        case bcmFabricMcLowPrioDropThDown:
            for (blk = 0; blk < nof_qrh; blk++) {
                SHR_IF_ERR_EXIT(soc_reg64_get(unit, QRH_LOW_PR_MULTHr, blk, 0, &reg_val));
                soc_reg64_field_set(unit, QRH_LOW_PR_MULTHr, &reg_val, LOW_PR_MUL_DROP_TH_LOWf, val64);
                SHR_IF_ERR_EXIT(soc_reg64_set(unit, QRH_LOW_PR_MULTHr, blk, 0, reg_val));
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported control type %d",type);
    }
      
exit:
    SHR_FUNC_EXIT;
  
}

/*
 * Function:
 *      soc_ramon_fabric_multicast_low_prio_threshold_get
 * Purpose:
 *      Get thresholds for low priority multicast drop
 * Parameters:
 *      unit      - (IN)  Unit number.
 *      type      - (IN)  bcmFabricMcLowPrioDropThUp or bcmFabricMcLowPrioDropThDown
 *      arg       - (OUT) Threshold
 * Returns:
 *      _SHR_E_xxx
 */
shr_error_e 
soc_ramon_fabric_multicast_low_prio_threshold_get(int unit, bcm_fabric_control_t type, int* arg)
{
    uint64 reg_val;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_reg64_get(unit, QRH_LOW_PR_MULTHr, 0, 0, &reg_val));

    switch(type)
    {
        case bcmFabricMcLowPrioDropThUp:
            *arg = soc_reg_field_get(unit, QRH_LOW_PR_MULTHr, COMPILER_64_LO(reg_val), LOW_PR_MUL_DROP_TH_HIGHf);
            break;
        case bcmFabricMcLowPrioDropThDown:
            *arg = soc_reg_field_get(unit, QRH_LOW_PR_MULTHr, COMPILER_64_LO(reg_val), LOW_PR_MUL_DROP_TH_LOWf);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported control type %d",type);
    }
    
exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      soc_ramon_fabric_multicast_low_priority_drop_enable_set
 * Purpose:
 *      Enable / Disable multicast low priority drop
 * Parameters:
 *      unit     - (IN)  Unit number.
 *      arg      - (IN)  Enable (1) or Disable (0) multicast law priority drop
 * Returns:
 *      _SHR_E_xxx
 */
shr_error_e 
soc_ramon_fabric_multicast_low_priority_drop_enable_set(int unit, int arg)
{
    uint32 reg_val;
    int blk, nof_qrh;
    SHR_FUNC_INIT_VARS(unit);
    
    nof_qrh = dnxf_data_device.blocks.nof_instances_qrh_get(unit);
    for (blk = 0; blk < nof_qrh; blk++) {
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, QRH_LOW_PR_MUL_CTRLr, blk, 0, &reg_val));
        soc_reg_field_set(unit, QRH_LOW_PR_MUL_CTRLr, &reg_val, LOW_PR_MUL_ENf, (arg ? 1 : 0));
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, QRH_LOW_PR_MUL_CTRLr, blk, 0, reg_val));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      soc_ramon_fabric_multicast_low_priority_drop_enable_get
 * Purpose:
 *      Get multicast low priority drop state (Enabled / Disabled)
 * Parameters:
 *      unit     - (IN)  Unit number.
 *      arg      - (OUT) Enable (1) or Disable (0) multicast low priority drop
 * Returns:
 *      _SHR_E_xxx
 */
shr_error_e 
soc_ramon_fabric_multicast_low_priority_drop_enable_get(int unit, int* arg)
{
    uint32 reg_val;
    SHR_FUNC_INIT_VARS(unit);
    
    SHR_IF_ERR_EXIT(soc_reg32_get(unit, QRH_LOW_PR_MUL_CTRLr, 0, 0, &reg_val));
    *arg = soc_reg_field_get(unit, QRH_LOW_PR_MUL_CTRLr, reg_val, LOW_PR_MUL_ENf);
  
exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      soc_ramon_fabric_multicast_priority_range_validate
 * Purpose:
 *      Validate multicast priority ranges
 * Parameters:
 *      unit    - (IN) Unit number.
 *      type    - (IN) Range to set (Low / Mid x Min / Max)
  *     arg     - (IN) Multicast group number
 * Returns:
 *      _SHR_E_xxx
 */
shr_error_e 
soc_ramon_fabric_multicast_priority_range_validate(int unit, bcm_fabric_control_t type, int arg)
{
    uint32 max_id ;
    SHR_FUNC_INIT_VARS(unit);
    
    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_multicast_table_size_get, (unit, &max_id)));

     if(arg < 0 || arg >= max_id) {
         SHR_ERR_EXIT(_SHR_E_PARAM, "arg %d is invalid for type %d, outside of multicast id range",arg, type);
     }
    
exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      soc_ramon_fabric_multicast_priority_range_set
 * Purpose:
 *      Set multicast priority ranges
 * Parameters:
 *      unit    - (IN) Unit number.
 *      type    - (IN) Range to set (Low / Mid x Min / Max)
  *     arg     - (IN) Multicast group number
 * Returns:
 *      _SHR_E_xxx
 */
shr_error_e 
soc_ramon_fabric_multicast_priority_range_set(int unit, bcm_fabric_control_t type, int arg)
{
    uint32 reg_val = 0;
    SHR_FUNC_INIT_VARS(unit);
    
    switch(type)
    {    
        case bcmFabricMcLowPrioMin:
            soc_reg_field_set(unit, BRDC_DCH_LOW_PR_MUL_0r, &reg_val, LOW_PR_MUL_QUE_LOWf, arg);
            SHR_IF_ERR_EXIT(WRITE_BRDC_DCH_LOW_PR_MUL_0r(unit, reg_val));
            break;
        case bcmFabricMcLowPrioMax:
            soc_reg_field_set(unit, BRDC_DCH_LOW_PR_MUL_1r, &reg_val, LOW_PR_MUL_QUE_HIGHf, arg);
            SHR_IF_ERR_EXIT(WRITE_BRDC_DCH_LOW_PR_MUL_1r(unit, reg_val));
            break;
        case bcmFabricMcMidPrioMin:
            soc_reg_field_set(unit, BRDC_DCH_MID_PR_MUL_0r, &reg_val, MID_PR_MUL_QUE_LOWf, arg);
            SHR_IF_ERR_EXIT(WRITE_BRDC_DCH_MID_PR_MUL_0r(unit, reg_val));
            break;
        case bcmFabricMcMidPrioMax:
            soc_reg_field_set(unit, BRDC_DCH_MID_PR_MUL_1r, &reg_val, MID_PR_MUL_QUE_HIGHf, arg);
            SHR_IF_ERR_EXIT(WRITE_BRDC_DCH_MID_PR_MUL_1r(unit, reg_val));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported control type %d",type);
    }
    
exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      soc_ramon_fabric_multicast_priority_range_get
 * Purpose:
 *      Get multicast priority ranges
 * Parameters:
 *      unit    - (IN)  Unit number.
 *      type    - (IN)  Range to get (Low / Mid x Min / Max)
  *     arg     - (OUT) Multicast group number
 * Returns:
 *      _SHR_E_xxx
 */
shr_error_e 
soc_ramon_fabric_multicast_priority_range_get(int unit, bcm_fabric_control_t type, int* arg)
{
    uint32 reg_val = 0;
    SHR_FUNC_INIT_VARS(unit);
    
    switch(type)
    {    
        case bcmFabricMcLowPrioMin:
            SHR_IF_ERR_EXIT(READ_DCH_LOW_PR_MUL_0r(unit, 0, &reg_val));
            *arg = soc_reg_field_get(unit, DCH_LOW_PR_MUL_0r, reg_val, LOW_PR_MUL_QUE_LOWf);
            break;
        case bcmFabricMcLowPrioMax:
            SHR_IF_ERR_EXIT(READ_DCH_LOW_PR_MUL_1r(unit, 0, &reg_val));
            *arg = soc_reg_field_get(unit, DCH_LOW_PR_MUL_1r, reg_val, LOW_PR_MUL_QUE_HIGHf);
            break;
        case bcmFabricMcMidPrioMin:
            SHR_IF_ERR_EXIT(READ_DCH_MID_PR_MUL_0r(unit, 0, &reg_val));
            *arg = soc_reg_field_get(unit, DCH_MID_PR_MUL_0r, reg_val, MID_PR_MUL_QUE_LOWf);
            break;
        case bcmFabricMcMidPrioMax:
            SHR_IF_ERR_EXIT(READ_DCH_MID_PR_MUL_1r(unit, 0, &reg_val));
            *arg = soc_reg_field_get(unit, DCH_MID_PR_MUL_1r, reg_val, MID_PR_MUL_QUE_HIGHf);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported control type %d",type);
    }
    
exit:
    SHR_FUNC_EXIT;
}


/*
 * \brief
 *   Set destinations and local route state for multiple groups using DMA.
 * \param [in] dest_array -
 *   Destinations (local module IDs) bitmaps for each group.
 *   Set to NULL to skip destinations update.
 * \param [in] local_route_enable_array -
 *   Enable multicast local route (Enable=1 / Disable=0) for each
 *   group.
 *   Set to NULL to skip local route update.
 */
int 
soc_ramon_fabric_multicast_multi_set(
    int unit, 
    uint32 flags, 
    uint32 ngroups, 
    soc_multicast_t *groups, 
    bcm_fabric_module_vector_t *dest_array,
    int *local_route_enable_array) 
{
    int rv, i;
    uint32* entry = NULL;
    int size;
    int index_max = 0, index_min = 0;
    soc_multicast_t group;
    int entry_words;
    uint32 mem_flags = 0;
    uint32 mc_table_entry_size = 0;
    uint32 mc_local_route_bit = 0;
    bcm_fabric_module_vector_t data = {0};
    SHR_FUNC_INIT_VARS(unit);

    /*parametes are required only for set stage*/
    if (!flags || (flags & _SHR_FABRIC_MULTICAST_SET_ONLY))
    {
        
        /*calc the required index range*/
        index_min = soc_mem_index_max(unit, RTP_MULTI_CAST_TABLE_UPDATEm);
        index_max = soc_mem_index_min(unit, RTP_MULTI_CAST_TABLE_UPDATEm);
        for (i = 0; i < ngroups; i++)
        {
            /*update index_min*/
            if (groups[i] < index_min)
            {
                index_min = groups[i];
            }
            /*update index_max*/
            if (groups[i] > index_max)
            {
                index_max = groups[i];
            }
        }

        if (index_min > index_max)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong params - groups\n");
        }

        entry_words = BYTES2WORDS(soc_mem_entry_bytes(unit, RTP_MULTI_CAST_TABLE_UPDATEm));
        /*calc required size*/
        size = (index_max - index_min + 1) * WORDS2BYTES(entry_words);
        /*allocate dma memory*/
        ramon_multi_entry_array = soc_cm_salloc(unit, size, "RTP_MULTI_CAST_TABLE_UPDATEm");

        if (ramon_multi_entry_array == NULL) {
             SHR_ERR_EXIT(_SHR_E_PARAM, "failed to allocate memory");
        }

        rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fabric_multicast_multi_read_range, (unit, index_min, index_max, ramon_multi_entry_array));
        SHR_IF_ERR_EXIT(rv);

        rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_multicast_table_entry_size_get, (unit, &mc_table_entry_size));
        SHR_IF_ERR_EXIT(rv);

        rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fabric_multicast_local_route_bit_get, (unit, &mc_local_route_bit));
        SHR_IF_ERR_EXIT(rv);

        for (i = 0; i < ngroups; i++)
        {
            group = groups[i];
            entry = ramon_multi_entry_array + (entry_words * (group - index_min));
            SHR_BITCOPY_RANGE(data, 0, entry, 0, BCM_FABRIC_MAX_MULTICAST_TABLE_ENTRY_SIZE);

            if (local_route_enable_array != NULL)
            {
                /* 
                 * Update local route.
                 * When local route is enabled bit is reset.
                 * When local route is disabled bit is set.
                 */
                SHR_BITWRITE(data, mc_local_route_bit, !local_route_enable_array[i]);
            }

            if (dest_array != NULL)
            {
                /* Update group's destinations */
                SHR_BITCOPY_RANGE(data, 0, dest_array[i], 0, mc_table_entry_size);
            }

            soc_mem_field_set(unit, RTP_MULTI_CAST_TABLE_UPDATEm, entry, LINK_BIT_MAPf, data);
        }
    }

    if (flags & _SHR_FABRIC_MULTICAST_COMMIT_ONLY) {
        mem_flags = SOC_MEM_WRITE_COMMIT_ONLY;
    } else if (flags & _SHR_FABRIC_MULTICAST_SET_ONLY) {
        mem_flags = SOC_MEM_WRITE_SET_ONLY;
    } else if (flags & _SHR_FABRIC_MULTICAST_STATUS_ONLY) {
        mem_flags = SOC_MEM_WRITE_STATUS_ONLY;
    } else {
        mem_flags = 0;
    }

    rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fabric_multicast_multi_write_range, (unit, mem_flags, index_min, index_max, ramon_multi_entry_array));
    SHR_IF_ERR_EXIT(rv);

exit:
    if ((ramon_multi_entry_array != NULL) && (mem_flags == SOC_MEM_WRITE_STATUS_ONLY || mem_flags == 0)) {
        soc_cm_sfree(unit, ramon_multi_entry_array);
        ramon_multi_entry_array = NULL;
    }
    _DNXC_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Get destinations and local route state for multiple groups using DMA.
 */
int 
soc_ramon_fabric_multicast_multi_get(
    int unit, 
    uint32 flags, 
    uint32 ngroups, 
    soc_multicast_t *groups, 
    bcm_fabric_module_vector_t *dest_array,
    int *local_route_enable_array) 
{
    int rv,i;
    uint32 *entry_array = NULL, *entry = NULL;
    int size;
    int index_max, index_min;
    bcm_fabric_module_vector_t temp_dest = {0};
    soc_multicast_t group;
    int entry_words;
    uint32 mc_local_route_bit = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*calc the required index range*/
    index_min = soc_mem_index_max(unit, RTP_MULTI_CAST_TABLE_UPDATEm);
    index_max = soc_mem_index_min(unit, RTP_MULTI_CAST_TABLE_UPDATEm);
    for (i = 0; i < ngroups; i++)
    {
        /*update index_min*/
        if (groups[i] < index_min)
        {
            index_min = groups[i];
        }
        /*update index_max*/
        if (groups[i] > index_max)
        {
            index_max = groups[i];
        }
    }

    if (index_min > index_max)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong params - groups\n");
    }

    entry_words = BYTES2WORDS(soc_mem_entry_bytes(unit, RTP_MULTI_CAST_TABLE_UPDATEm));
    /*calc required size*/
    size = (index_max - index_min + 1) * WORDS2BYTES(entry_words);
    /*allocate dma memory*/
    entry_array = soc_cm_salloc(unit, size, "RTP_MULTI_CAST_TABLE_UPDATEm");
    if (entry_array == NULL) 
    {
         SHR_ERR_EXIT(_SHR_E_PARAM, "failed to allocate memory");
    }

    rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fabric_multicast_multi_read_range, (unit, index_min, index_max, entry_array));
    SHR_IF_ERR_EXIT(rv);

    rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fabric_multicast_local_route_bit_get, (unit, &mc_local_route_bit));
    SHR_IF_ERR_EXIT(rv);

    for (i=0; i<ngroups; i++) 
    {
        group = groups[i];
        entry = entry_array + (entry_words * (group - index_min));
        soc_mem_field_get(unit, RTP_MULTI_CAST_TABLE_UPDATEm, entry, LINK_BIT_MAPf, temp_dest);

        if (local_route_enable_array != NULL)
        {
            /* 
             * Return local route state.
             * When bit is reset, local route is enabled.
             * When bit is set, local route is disabled.
             */
            local_route_enable_array[i] = !(SHR_BITGET(temp_dest, mc_local_route_bit));
        }

        if (dest_array != NULL)
        {
            SHR_BITCOPY_RANGE(dest_array[i], 0, temp_dest, 0, BCM_FABRIC_MAX_MULTICAST_TABLE_ENTRY_SIZE);

            /* 
             * Local route bit is not part of the destinations.
             */
            SHR_BITCLR(dest_array[i], mc_local_route_bit);
        }
    }

exit:
    if (entry_array != NULL)
    {
        soc_cm_sfree(unit, entry_array);
    }
    SHR_FUNC_EXIT;
}

/* 
 *read range of multicast groups - the data can be separated per instance of MCT_MCTm
 *Adjust the data so each raw will represent a multicast group
 */
#ifndef SOC_RAMON_MULTICAST_TABLE_UPDATE_READ_ENABLED
shr_error_e
soc_ramon_fabric_multicast_multi_read_range(int unit, soc_multicast_t group_min, soc_multicast_t group_max, uint32 *entry_array)
{
    int rv;
    uint32 *cur_entry_array, *entry;
    int entry_index,info_index,  entry_words;
    int read_index_min, read_index_max;
    uint32 data[_SHR_BITDCLSIZE(DNXF_DATA_MAX_FABRIC_TOPOLOGY_NOF_LOCAL_MODID)];
    uint32 mc_table_entry_size;
    const soc_dnxf_multicast_read_range_info_t *info;
    int info_size;
    SHR_FUNC_INIT_VARS(unit);

    /*get info*/
    entry_words = BYTES2WORDS(soc_mem_entry_bytes(unit, RTP_MULTI_CAST_TABLE_UPDATEm));
    rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_multicast_table_entry_size_get, (unit, &mc_table_entry_size));
    SHR_IF_ERR_EXIT(rv);

    /*get mc table read info*/
    rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fabric_multicast_multi_read_info_get, (unit, &info, &info_size));
    SHR_IF_ERR_EXIT(rv);

    for (info_index = 0; 
         info_index < info_size; 
         info_index++)
    {
        if (group_min < info[info_index].group_max && group_max > info[info_index].group_min) /*check if entries are required from this part*/
        {
            read_index_min = (group_min > info[info_index].group_min) ? (group_min - info[info_index].group_min) : 0;
            read_index_max = (group_max < info[info_index].group_max) ? (group_max - info[info_index].group_min) : _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE - 1;
            cur_entry_array = entry_array + (read_index_min + info[info_index].group_min - group_min) * entry_words;

            rv = soc_mem_array_read_range(unit, MCT_MCTm, info[info_index].table , MEM_BLOCK_ANY, read_index_min, read_index_max, cur_entry_array);
            SHR_IF_ERR_EXIT(rv);

            /*adjust offset if required*/
            if (info[info_index].is_first_half == 0)
            {
                for (entry_index = read_index_min; entry_index <= read_index_max; entry_index++)
                {
                    
                    entry = cur_entry_array + (entry_words * entry_index);
                    soc_mem_field_get(unit,MCT_MCTm, entry, LINK_BIT_MAPf, data);
                    SHR_BITCOPY_RANGE(data, 0, data, mc_table_entry_size, mc_table_entry_size);
                    soc_mem_field_set(unit, MCT_MCTm, entry, LINK_BIT_MAPf, data /*value to set*/);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e 
soc_ramon_fabric_multicast_read_info(int unit, soc_multicast_t group, int *mc_table_index, int *mc_table_entry, int *is_first_half)
{
    const soc_dnxf_multicast_read_range_info_t *info;
    int info_index, info_size, found, rv;
    SHR_FUNC_INIT_VARS(unit);

    /*get mc table read info*/
    rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fabric_multicast_multi_read_info_get, (unit, &info, &info_size));
    SHR_IF_ERR_EXIT(rv);    

    /*look for the relevant info*/
    found = 0;
    for (info_index = 0;
         info_index < info_size; 
         info_index++)
    {
        if (group <= info[info_index].group_max && group >= info[info_index].group_min)
        {
            *mc_table_index = info[info_index].table;
            *mc_table_entry = group - info[info_index].group_min;
            *is_first_half = info[info_index].is_first_half;
            found = 1;
            break;
        }
    }

    if (found == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "group (%d) read info wasn't found\n", group);
    }
exit:
    SHR_FUNC_EXIT;
}

#endif /* ifndef SOC_RAMON_MULTICAST_TABLE_UPDATE_READ_ENABLED */
