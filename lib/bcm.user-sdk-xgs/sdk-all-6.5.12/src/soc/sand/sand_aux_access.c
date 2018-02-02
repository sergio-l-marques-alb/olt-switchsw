/*
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:    access_pack.c
 * Purpose: Miscellaneous routine for device db access
 */

#include <sal/appl/io.h>
#include <sal/core/libc.h>
#include <sal/appl/sal.h>
#include <soc/defs.h>
#include <soc/drv.h>

#include <shared/bitop.h>

#include <shared/dbx/dbx_xml.h>
#include <shared/dbx/dbx_file.h>
#include <shared/utilex/utilex_rhlist.h>
#include <shared/utilex/utilex_str.h>
#include <shared/shrextend/shrextend_debug.h>

#include <soc/sand/sand_aux_access.h>
#include <soc/sand/sand_signals.h>

#include <bcm/types.h>

#define BSL_LOG_MODULE BSL_LS_SOC_COMMON

/* Initialized by device_init */
static device_t *sand_device_array[SOC_MAX_NUM_DEVICES];

static shr_reg_data_t reg_data[NUM_SOC_REG] = {
        {0}
};

static shr_mem_data_t mem_data[NUM_SOC_MEM] = {
        {0}
};

char*
shr_access_get_sand_name(int unit)
{
    char *sand_name;
    if(SOC_IS_DNX(unit))
    {
        sand_name = "dnx";
    }
    else if(SOC_IS_DNXF(unit))
    {
        sand_name = "dnxf";
    }
    else if(SOC_IS_DPP(unit))
    {
        sand_name = "dpp";
    }
    else if(SOC_IS_DFE(unit))
    {
        sand_name = "dfe";
    }
    else
    {
        sand_name = "unknown";
    }

    return sand_name;
}

int
shr_access_reg_no_read_get(
        int         unit,
        soc_reg_t   reg)
{
    if(reg < 0 || reg >= NUM_SOC_REG)
    {
        /* No such register in list - means no limitations on read */
        return 0;
    }

    return ((reg_data[reg].flags & ACC_NO_READ) ? 1 : 0);
}

int
shr_access_mem_no_read_get(
        int         unit,
        soc_mem_t   mem)
{
    if(mem < 0 || mem >= NUM_SOC_MEM)
    {
        /* No such register in list - means no limitations on read */
        return 0;
    }

    /* By default all explicitly uninitialized fields will be 0, so nothing happens, object may be read */
    return ((mem_data[mem].flags & ACC_NO_READ) ? 1 : 0);
}

int
shr_access_reg_no_wb_get(
        int         unit,
        soc_reg_t   reg)
{
    if(reg < 0 || reg >= NUM_SOC_REG)
    {
        /* No such register in list - means no limitations on read */
        return 0;
    }

    return ((reg_data[reg].flags & ACC_NO_WB) ? 1 : 0);
}

int
shr_access_mem_no_wb_get(
        int         unit,
        soc_mem_t   mem)
{
    if(mem < 0 || mem >= NUM_SOC_MEM)
    {
        /* No such register in list - means no limitations on read */
        return 0;
    }

    /* By default all explicitly uninitialized fields will be 0, so nothing happens, object may be read */
    return ((mem_data[mem].flags & ACC_NO_WB) ? 1 : 0);
}

shr_error_e
shr_access_reg_present_in_block(
    int                unit,
    int                cmic_block,
    soc_block_types_t  reg_block_list,
    int*               block_id_p)
{
    shr_error_e rv = _SHR_E_NOT_FOUND;
    int i_bl;

    for (i_bl = 0; SOC_BLOCK_TYPE(unit, i_bl) >= 0; i_bl++)
    {
       if(SOC_BLOCK2OFFSET(unit, i_bl) == cmic_block)
       {
           if(SOC_BLOCK_IS_TYPE(unit, i_bl, reg_block_list))
           {
               if(block_id_p != NULL)
               {
                   *block_id_p = i_bl;
               }
               rv = _SHR_E_NONE;
           }
           break;
       }
    }

    return rv;
}

shr_error_e
shr_access_reg_get(int unit, uint32 base_address, int cmic_block_id, int offset, uint8 opcode, rhlist_t *hit_list)
{
    soc_reg_t reg;
    rhhandle_t temp = NULL;
    shr_hit_entry_t *hit_entry;
    int block_id, blk;
    uint8 acc_type;

    SHR_FUNC_INIT_VARS(unit);

    for (reg = 0; reg < NUM_SOC_REG; reg++)
    {
        soc_reg_info_t *reginfo;

        if (!SOC_REG_IS_VALID(unit, reg))
            continue;

        reginfo = &SOC_REG_INFO(unit, reg);

        if(base_address != SOC_REG_BASE(unit, reg))
            continue;

        if((shr_access_reg_present_in_block(unit, cmic_block_id, reginfo->block, &block_id)) != _SHR_E_NONE)
            continue;

        if((offset < 0) || (offset >= SOC_REG_NUMELS(unit, reg)))
        {
            LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META_U(0, "Invalid offset:0x%08x for Register:%s\n"), offset, SOC_REG_NAME(unit, reg)));
            continue;
        }
        else if((block_id < 0) || (block_id >= SOC_MAX_NUM_BLKS))
        {
            LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META_U(0, "Invalid block id:%d for Register:%s\n"), block_id, SOC_REG_NAME(unit, reg)));
            continue;
        }

        SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(hit_list, SOC_REG_NAME(unit, reg), reg, &temp));

        hit_entry = temp;
        hit_entry->opcode = opcode;
        hit_entry->offset = offset;
        hit_entry->address = soc_reg_addr_get(unit, reg, SOC_BLOCK_PORT(unit, block_id), offset, SOC_REG_ADDR_OPTION_NONE, &blk, &acc_type);
        hit_entry->block_id = block_id;
        hit_entry->cmic_block_id = cmic_block_id;
        sal_strncpy(hit_entry->block_n, SOC_BLOCK_NAME(unit, block_id), RHNAME_MAX_SIZE - 1);

    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
shr_access_mem_present_in_block(
    int                unit,
    int                cmic_block,
    soc_mem_t          mem,
    int*               block_id_p)
{
    shr_error_e rv = _SHR_E_NOT_FOUND;
    int i_bl;

    SOC_MEM_BLOCK_ITER(unit, mem, i_bl)
    {
        if(SOC_BLOCK2OFFSET(unit, i_bl) == cmic_block)
        {
            if(block_id_p != NULL)
            {
                *block_id_p = i_bl;
            }
            rv = _SHR_E_NONE;
        }
        break;
    }

    return rv;
}

shr_error_e
shr_access_mem_get(int unit, uint32 base_address, int cmic_block_id, int offset, uint8 opcode, rhlist_t *hit_list)
{
    soc_mem_t mem;
    rhhandle_t temp = NULL;
    shr_hit_entry_t *hit_entry;
    soc_mem_info_t *meminfo;
    int block_id, entry_id, index_id;
    uint8 acc_type;

    SHR_FUNC_INIT_VARS(unit);

    for (mem = 0; mem < NUM_SOC_MEM; mem++)
    {
        if (!SOC_MEM_IS_VALID(unit, mem))
            continue;

        /* Check that base addresses match */
        if(base_address != SOC_MEM_INFO(unit, mem).base)
            continue;

        if((shr_access_mem_present_in_block(unit, cmic_block_id, mem, &block_id)) != _SHR_E_NONE)
            continue;

        meminfo = &SOC_MEM_INFO(unit, mem);
        if((meminfo->flags & SOC_MEM_FLAG_IS_ARRAY) && (SOC_MEM_ARRAY_INFOP(unit, mem) != NULL))
        {
            entry_id = offset % SOC_MEM_ELEM_SKIP_SAFE(unit, mem);
            index_id = offset / SOC_MEM_ELEM_SKIP_SAFE(unit, mem);

            if(index_id >= SOC_MEM_NUMELS_SAFE(unit, mem))
            {
                LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META_U(0, "Offset:0x%08x is too big for memory:%s(0x%08x)\n"), offset, SOC_MEM_NAME(unit, mem), base_address));
                continue;
            }
        }
        else
        {
            entry_id = offset;
            index_id = 0;
        }

        SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(hit_list, SOC_MEM_NAME(unit, mem), mem, &temp));

        hit_entry = temp;
        hit_entry->opcode = opcode;
        hit_entry->offset = offset;
        hit_entry->address = soc_mem_addr_get(unit, mem, index_id, block_id, entry_id, &acc_type);
        hit_entry->cmic_block_id = cmic_block_id;
        sal_strncpy(hit_entry->block_n, SOC_BLOCK_NAME(unit, block_id), RHNAME_MAX_SIZE - 1);
    }

exit:
    SHR_FUNC_EXIT;
}

#ifdef ADAPTER_SERVER_MODE
#include <soc/dnx/adapter/adapter_reg_access.h>
#define ADAPTER_HIT_INFO_SIZE 17

static int
shr_access_hit_exists(int unit, uint32 base_address, int cmic_block_id, int offset, uint8 opcode, rhlist_t *hit_list)
{
    shr_hit_entry_t *hit_entry;
    RHITERATOR(hit_entry, hit_list)
    {
        if((hit_entry->address == base_address) && (hit_entry->offset == offset)
                && (hit_entry->cmic_block_id == cmic_block_id) && (hit_entry->opcode == opcode))
        {
            hit_entry->count++;
            return TRUE;
        }
    }
    return FALSE;
}

char *shr_access_hit_opcode_name(int opcode)
{
    switch (opcode )
    {
    case UPDATE_TO_SOC_REG_READ:
        return "reg read";
    case UPDATE_TO_SOC_MEM_READ:
        return "mem read";
    case UPDATE_TO_SOC_REG_WRITE:
        return "reg write";
    case UPDATE_TO_SOC_MEM_WRITE:
        return "mem write";
    }
    return NULL;
}

shr_error_e
shr_access_hit_get(
    int unit,
    rhlist_t ** hit_list_p,
    int flags)
{
    uint8 opcode;
    int units_code;
    int cmic_block_id;
    uint32 base_address;
    int offset;

    char *hit_data;
    uint32 hit_data_length;

    rhlist_t *hit_list = NULL;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(adapter_read_hit_bits(unit, 0, &hit_data_length, &hit_data));

    if((hit_data_length % ADAPTER_HIT_INFO_SIZE) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Bad hit buffer size:%d\n", hit_data_length);
    }

    if ((hit_list = utilex_rhlist_create("hits", sizeof(shr_hit_entry_t), 0)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "No memory for hits list\n");
    }

    while(hit_data_length >= ADAPTER_HIT_INFO_SIZE)
    {

        opcode = *hit_data++;

        units_code    = bcm_ntohl(*((int *)hit_data));    hit_data += sizeof(int);
        cmic_block_id = bcm_ntohl(*((int *)hit_data));    hit_data += sizeof(int);
        base_address  = bcm_ntohl(*((uint32 *)hit_data)); hit_data += sizeof(uint32);
        offset        = bcm_ntohl(*((int *)hit_data));    hit_data += sizeof(int);

        hit_data_length -= ADAPTER_HIT_INFO_SIZE;

        /* If avoid repetition flag was set, check existence and only if not exists continue */
        if((flags & HIT_AVOID_REPEAT) &&
                (shr_access_hit_exists(unit, base_address, cmic_block_id, offset, opcode, hit_list) == TRUE))
            continue;

        switch(opcode)
        {
            case UPDATE_TO_SOC_REG_READ:
            case UPDATE_TO_SOC_REG_WRITE:
                SHR_IF_ERR_EXIT(shr_access_reg_get(unit, base_address, cmic_block_id, offset, opcode, hit_list));
                break;
            case UPDATE_TO_SOC_MEM_READ:
            case UPDATE_TO_SOC_MEM_WRITE:
                SHR_IF_ERR_EXIT(shr_access_mem_get(unit, base_address, cmic_block_id, offset, opcode, hit_list));
                break;
            default:
                break;
        }
    }

exit:
    if(SHR_FUNC_ERR())
    {
        if(hit_list != NULL)
            utilex_rhlist_free_all(hit_list);
    }
    else if(hit_list_p != NULL)
    {
        *hit_list_p = hit_list;
    }
    SHR_FUNC_EXIT;
}
#endif /* ADAPTER_SERVER_MODE */

#if (!defined(NO_FILEIO) && !defined(STATIC_SIGNAL_TEST))
/*
 * {
 */
static
int
shr_access_reg_init(xml_node curTop)
{
    int res = _SHR_E_NONE;
    xml_node curSubTop, cur;

    if ((curSubTop = dbx_xml_child_get_first(curTop, "registers")) == NULL)
    {
        res = _SHR_E_NOT_FOUND;
        goto exit;
    }

    RHDATA_ITERATOR(cur, curSubTop, "reg")
    {
        char reg_name[RHSTRING_MAX_SIZE];
        soc_reg_t reg;

        RHDATA_GET_LSTR_CONT(cur, "name", reg_name);

        for(reg = 0; reg < NUM_SOC_REG; reg++)
        {
            if(!sal_strcmp(SOC_REG_NAME(unit, reg), reg_name))
                break;
        }
        if(reg == NUM_SOC_REG)
        {
            LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META_U(0, "Register:%s does not exist\n"), reg_name));
        }
        else
        {
            int tmp;
            if(reg_data[reg].flags != 0)
            {
                LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(0, "Register:%s appeared twice in auxiliary list\n"), reg_name));
            }
            RHDATA_GET_INT_DEF(cur, "no_read", tmp, 0);
            /* Registers that cannot be read cannot participate in wb test, no need to mention it explicitly */
            if(tmp == 1)
            {
                reg_data[reg].flags = ACC_NO_READ | ACC_NO_WB;
            }
            else
            {
                RHDATA_GET_INT_DEF(cur, "no_wb", tmp, 0);
                reg_data[reg].flags = ACC_NO_WB;
            }
        }
    }

exit:
    return res;
}

static
int
shr_access_mem_init(xml_node curTop)
{
    int res = _SHR_E_NONE;
    xml_node curSubTop, cur;

    if ((curSubTop = dbx_xml_child_get_first(curTop, "memories")) == NULL)
    {
        res = _SHR_E_NOT_FOUND;
        goto exit;
    }

    RHDATA_ITERATOR(cur, curSubTop, "mem")
    {
        char mem_name[RHSTRING_MAX_SIZE];
        soc_mem_t mem;

        RHDATA_GET_LSTR_CONT(cur, "name", mem_name);

        for(mem = 0; mem < NUM_SOC_MEM; mem++)
        {
            if(!sal_strcmp(SOC_MEM_NAME(unit, mem), mem_name))
                break;
        }
        if(mem == NUM_SOC_MEM)
        {
            LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META_U(0, "Memory:%s does not exist\n"), mem_name));
        }
        else
        {
            int tmp;
            if(mem_data[mem].flags)
            {
                LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(0, "Memory:%s appeared twice in auxilary list\n"), mem_name));
            }
            RHDATA_GET_INT_DEF(cur, "no_read", tmp, 0);
            /* Memories that cannot be read cannot participate in wb test, no need to mention it explicitly */
            if(tmp == 1)
            {
                mem_data[mem].flags = ACC_NO_READ | ACC_NO_WB;
            }
            else
            {
                RHDATA_GET_INT_DEF(cur, "no_wb", tmp, 0);
                mem_data[mem].flags = ACC_NO_WB;
            }
        }
    }
exit:
    return res;
}

static int
shr_access_obj_init(device_t *device)
{
    int res = _SHR_E_NONE;
    xml_node curTop = NULL;

    /* Look for global file first */
    if ((curTop = dbx_file_get_xml_top(NULL, "AccessObjects.xml", "top", 0)) == NULL)
    {
        res = _SHR_E_NOT_FOUND;
        goto exit;
    }

    if((res = shr_access_reg_init(curTop)) != _SHR_E_NONE)
    {
        goto exit;
    }

    if((res = shr_access_mem_init(curTop)) != _SHR_E_NONE)
    {
        goto exit;
    }

    /* Then for per device add-ons if any */
    if ((curTop = dbx_file_get_xml_top(RHNAME(device), "AccessObjects.xml", "top", CONF_OPEN_NO_ERROR_REPORT)) == NULL)
    {
        goto exit;
    }

    if((res = shr_access_reg_init(curTop)) != _SHR_E_NONE)
    {
        goto exit;
    }

    if((res = shr_access_mem_init(curTop)) != _SHR_E_NONE)
    {
        goto exit;
    }

exit:
    dbx_xml_top_close(curTop);
    return res;
}

static int
shr_access_obj_deinit(device_t *device)
{
    sal_memset(reg_data, 0, sizeof(shr_reg_data_t) * NUM_SOC_REG);
    sal_memset(mem_data, 0, sizeof(shr_mem_data_t) * NUM_SOC_MEM);
    return _SHR_E_NONE;
}

static rhlist_t    *sand_db_list = NULL;

static shr_error_e
shr_access_global_init(void)
{
    shr_error_e rv = _SHR_E_NONE;
    xml_node curTop = NULL,
                    curSubTop = NULL,
                    cur = NULL;
    char device_n[RHNAME_MAX_SIZE];
    char db_name[RHNAME_MAX_SIZE];
    device_t *device;
    rhhandle_t temp = NULL;

    /* If it is not first device global device list may be already initialized */
    if (sand_db_list != NULL)
        goto  exit;

    memset(sand_device_array, 0, sizeof(device_t *) * SOC_MAX_NUM_DEVICES);

    if ((curTop = dbx_file_get_xml_top(NULL, "DNX-Devices.xml", "top", 0)) == NULL)
    {
        rv = _SHR_E_NOT_FOUND;
        goto exit;
    }

    if ((curSubTop = dbx_xml_child_get_first(curTop, "devices")) == NULL)
    {
        rv = _SHR_E_NOT_FOUND;
        goto exit;
    }

    if ((sand_db_list = utilex_rhlist_create("devices", sizeof(device_t), 0)) == NULL)
    {
        rv = _SHR_E_MEMORY;
        goto exit;
    }

    RHDATA_ITERATOR(cur, curSubTop, "device")
    {
        RHDATA_GET_STR_CONT(cur, "chip", device_n);
        RHDATA_GET_STR_CONT(cur, "db_name", db_name);
        if ((device = utilex_rhlist_entry_get_by_name(sand_db_list, db_name)) == NULL)
        {
            /*
             * No such db in the list
             */
            if ((rv = utilex_rhlist_entry_add_tail(sand_db_list, db_name, RHID_TO_BE_GENERATED, &temp)) != _SHR_E_NONE)
            {
                /*
                 * No more place any more - return with what you have until now
                 */
                goto exit;
            }
            device = temp;
            /*
             * Create list of chips supported by this device
             */
            if ((device->chip_list = utilex_rhlist_create("devices", sizeof(rhentry_t), 0)) == NULL)
            {
                /*
                 * Couldn't create new list - still we can leave with what we already have
                 */
                goto exit;
            }
        }

        if ((rv = utilex_rhlist_entry_add_tail(device->chip_list, device_n, RHID_TO_BE_GENERATED, &temp)) != _SHR_E_NONE)
        {
            /*
             * No more place any more - return with what you have until now
             */
            goto exit;
        }
    }

exit:
    dbx_xml_top_close(curTop);
    return rv;
}
/*
 * }
 */
#endif /* (!defined(NO_FILEIO) && !defined(STATIC_SIGNAL_TEST)) */

device_t *
sand_signal_device_get(
    int unit)
{
    if(sand_device_array[unit] == NULL) {
        LOG_WARN(BSL_LS_SOC_COMMON,
              (BSL_META_U(0, "Signal Data for %s(%s) was not initialized\n"), soc_dev_name(unit), SOC_CHIP_STRING(unit)));
    }
    return sand_device_array[unit];
}

shr_error_e
shr_access_device_init(
    int unit)
{
    shr_error_e rv = _SHR_E_NONE;
#if (defined(NO_FILEIO) || defined(STATIC_SIGNAL_TEST))
    if((sand_device_array[unit] = sand_signal_static_init(unit)) == NULL)
    {
        rv = _SHR_E_NOT_FOUND;
        goto exit;
    }
#else
    device_t *device;
    char *chip_n = (char *) soc_dev_name(unit);

    /* Initialize global DB list with supported chips */
    if((rv = shr_access_global_init()) != _SHR_E_NONE)
        goto exit;

    /* Look in the list by specific device ID */
    RHITERATOR(device, sand_db_list)
    {
        if ((utilex_rhlist_entry_get_by_name(device->chip_list, chip_n)) != NULL)
        {
            sand_device_array[unit] = device;
            break;
        }
    }

    /* Look in the list by family name */
    if (sand_device_array[unit] == NULL)
    {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(0, "%s was not found in Device DB - looking by:%s\n"), chip_n, SOC_CHIP_STRING(unit)));
        RHITERATOR(device, sand_db_list)
        {
            if ((utilex_rhlist_entry_get_by_name(device->chip_list, SOC_CHIP_STRING(unit))) != NULL)
            {
                sand_device_array[unit] = device;
                break;
            }
        }
    }

    if (sand_device_array[unit] == NULL)
    {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(0, "No %s neither %s was found in Device DB\n"), chip_n, SOC_CHIP_STRING(unit)));
        rv = _SHR_E_NOT_FOUND;
        goto exit;
    }

    /*
     * Initialize Access Objects - failure should not prevent init, means no file defined, no special access
     */
    shr_access_obj_init(sand_device_array[unit]);

    /* Initialize Signals Data */
    if ((rv = sand_signal_init(unit, sand_device_array[unit])) != _SHR_E_NONE)
        goto exit;

#endif /* end of else on (defined(NO_FILEIO) || defined(STATIC_SIGNAL_TEST)) */
exit:
    return rv;
}

shr_error_e
shr_access_device_deinit(
    int unit)
{
    shr_error_e rv = _SHR_E_NONE;
#if (defined(NO_FILEIO) || defined(STATIC_SIGNAL_TEST))
    /* Static Signals init do not require deinit for now - no external DB to be updated on */
#else
    device_t *device;

    /* Look in the list by family name */
    if ((device = sand_device_array[unit]) == NULL)
    { /* Not initialized - leave peacefully */
        goto exit;
    }

    /* Initialize Access Objects */
    if ((rv = shr_access_obj_deinit(device)) != _SHR_E_NONE)
        goto exit;

    /* Initialize Signals Data */
    if ((rv = sand_signal_deinit(unit, device)) != _SHR_E_NONE)
        goto exit;

    sand_device_array[unit] = NULL;
exit:
#endif /* end of else on (defined(NO_FILEIO) || defined(STATIC_SIGNAL_TEST)) */
    return rv;
}
