#include <shared/bsl.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <soc/defs.h>
#include <soc/iproc.h>
#include <soc/cmicm.h>
#include <soc/sbusdma.h>
#include <soc/l2x.h>
#include <soc/mem.h>
#include <bcm/error.h>
#include <bcm/link.h>

#if defined(BCM_ESW_SUPPORT) && defined(BCM_CMICM_SUPPORT) && \
    defined(BCM_SBUSDMA_SUPPORT)

typedef struct sbusdma_params_s {
    int          unit;
    sal_thread_t tid;
    int          type;
    int          test_time;
    int          status;
    int          channel;
    soc_mem_t    mem;
} sbusdma_params_t;

typedef struct sbusdma_desc_s {
    uint32      control;
    uint32      request;
    uint32      count;
    uint32      opcode;
    uint32      sbus_base;
    uint32      hostmem_phy_base;
} sbusdma_desc_t;

#define SBUSDMA_STATUS_INVALID 0
#define SBUSDMA_STATUS_SUCCESS 1
#define SBUSDMA_STATUS_FAILURE 2
#define SBUSDMA_STATUS_RUNNING 3

#define SBUSDMA_TYPE_READ  0
#define SBUSDMA_TYPE_WRITE 1

#define SBUSDMA_CH_MAX_NUM (3 * SOC_SBUSDMA_CH_PER_CMC)
#define SBUSDMA_DESCRIPTOR_COUNT 3

static const char *const status_strings[4] = {"invalid", "success", "failure", "running"};
static const soc_mem_t test_mem[SBUSDMA_CH_MAX_NUM] = {NONUCAST_TRUNK_BLOCK_MASKm,
                                                       ING_FLEX_CTR_COUNTER_TABLE_0m,
                                                       ING_FLEX_CTR_COUNTER_TABLE_1m,
                                                       ING_FLEX_CTR_COUNTER_TABLE_2m,
                                                       ING_FLEX_CTR_COUNTER_TABLE_3m,
                                                       EGR_FLEX_CTR_COUNTER_TABLE_0m,
                                                       EGR_FLEX_CTR_COUNTER_TABLE_1m,
                                                       EGR_FLEX_CTR_COUNTER_TABLE_2m,
                                                       EGR_FLEX_CTR_COUNTER_TABLE_3m};
static sbusdma_params_t sbusdma_params[SBUSDMA_CH_MAX_NUM];

static void desc_dma_request_init(int unit, uint32 *request, int entry_width, int type)
{
    int endian, tmp;

    soc_endian_get(unit, &tmp, &tmp, &endian);

    soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr,
                      request, WORDSWAP_IN_64BIT_SBUSDATAf, 0);
    soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr,
                      request, HOSTMEMWR_ENDIANESSf, endian);
    soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr,
                      request, HOSTMEMRD_ENDIANESSf, endian);
    soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr,
                      request, PEND_CLOCKSf, 0);

    if (type == SBUSDMA_TYPE_WRITE) {
        soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr,
                          request, REQ_WORDSf, entry_width);
        soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr,
                          request, REP_WORDSf, 0);
    } else {
        soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr,
                          request, REQ_WORDSf, 0);
        soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr,
                          request, REP_WORDSf, entry_width);
    }
}

static void desc_dma_start(int unit, int cmc, int channel, sbusdma_desc_t *desc)
{
    uint32 rval;

    /* Configure descriptor mode */
    rval = soc_pci_read(unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, channel));
    soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_CONTROLr, &rval, MODEf, 1);
    soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_CONTROLr, &rval, STARTf, 0);
    soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_CONTROLr, &rval, ABORTf, 0);
    soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, channel), rval);

    /* Set the starting descriptor address */
    soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_DESCADDR(cmc, channel), soc_cm_l2p(unit, desc));

    /* Start the DMA */
    rval = soc_pci_read(unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, channel));
    soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_CONTROLr, &rval, STARTf, 1);
    soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, channel), rval);
}

static void desc_dma_stop(int unit, int cmc, int channel)
{
    uint32 rval;
    soc_timeout_t to;

    /* Stop the DMA */
    rval = soc_pci_read(unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, channel));
    soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_CONTROLr, &rval, ABORTf, 1);
    soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, channel), rval);

    /* Wait DMA done */
    soc_timeout_init(&to, 1000000, 0);
    do {
        rval = soc_pci_read(unit, CMIC_CMCx_SBUSDMA_CHy_STATUS(cmc, channel));
        if (soc_reg_field_get(unit, CMIC_CMC0_SBUSDMA_CH0_STATUSr, rval, DONEf)) {
           break;
       }

       if (soc_timeout_check(&to)) {
           cli_out("DMA abort timeout!\n");
           break;
       }
    } while(1);

    rval = soc_pci_read(unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, channel));
    soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_CONTROLr, &rval, STARTf, 0);
    soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_CONTROLr, &rval, ABORTf, 0);
    soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, channel), rval);
}

static void dma_data_init(uint32 *mdata1, uint32 *mdata2, int size, int type)
{
    uint8 *ptr1;
    uint8 *ptr2;
    uint8  tmp;
    int    index;

    if (type == SBUSDMA_TYPE_WRITE) {
        /* Fill the table data with a pattern */
        ptr1 = (uint8 *) mdata1;
        ptr2 = (uint8 *) mdata2;

        for (index = 0; index < size; index++) {
            tmp = (0x3 - (index & 0x3)) | (index & 0xFC);
            *ptr1++ = tmp;
            *ptr2++ = 0xFF - tmp;
        }
    } else {
        sal_memset(mdata1, 0xA5, size);
        sal_memset(mdata2, 0xA5, size);
    }
}

static void schan_header_init(int unit, int type, schan_header_t *header,
                              int dst_blk, int acc_type, int data_byte_len)
{
    if (type == SBUSDMA_TYPE_WRITE) {
        soc_schan_header_cmd_set(unit, header, WRITE_MEMORY_CMD_MSG, dst_blk, 0,
                                 acc_type, data_byte_len, 0, 0);
    } else {
        soc_schan_header_cmd_set(unit, header, READ_MEMORY_CMD_MSG, dst_blk, 0,
                                 acc_type, 0, 0, 0);
    }
}

static void continuous_dma_entry(void *void_param)
{
    sbusdma_params_t *const params = (sbusdma_params_t *) void_param;
    int cmc = params->channel / SOC_SBUSDMA_CH_PER_CMC;
    int ch = params->channel % SOC_SBUSDMA_CH_PER_CMC;
    int entries, entry_bytes, entry_width, cur_time;
    sbusdma_desc_t *dma_desc;
    soc_mem_info_t *minfo;
    uint32     *mdata1;
    uint32     *mdata2;
    uint32      rval;

    params->status = SBUSDMA_STATUS_RUNNING;

    /* Allocate memory for the SBUS DMA descriptors and the table data */
    dma_desc = soc_cm_salloc(params->unit, SBUSDMA_DESCRIPTOR_COUNT * sizeof(sbusdma_desc_t),
                             "sbusdma_descriptors");
    if (!dma_desc) {
        cli_out("Could not allocate memory for SBUS DMA descriptors\n");
        params->status = SBUSDMA_STATUS_FAILURE;
        return;
    }
    sal_memset(dma_desc, 0, SBUSDMA_DESCRIPTOR_COUNT * sizeof(sbusdma_desc_t));

    minfo = &SOC_MEM_INFO(params->unit, params->mem);
    entries = minfo->index_max - minfo->index_min + 1;  /* Compute mem size */
    entry_width = ((minfo->bytes + 3) / 4) * 4;
    entry_bytes = entries * entry_width;

    mdata1 = soc_cm_salloc(params->unit, entry_bytes, "table_data");
    if (!mdata1) {
        cli_out("Could not allocate memory for table data 1\n");
        soc_cm_sfree(params->unit, dma_desc);
        params->status = SBUSDMA_STATUS_FAILURE;
        return;
    }

    mdata2 = soc_cm_salloc(params->unit, entry_bytes, "table_data");
    if (!mdata2) {
        cli_out("Could not allocate memory for table data 2\n");
        soc_cm_sfree(params->unit, dma_desc);
        soc_cm_sfree(params->unit, mdata1);
        params->status = SBUSDMA_STATUS_FAILURE;
        return;
    }

    dma_data_init(mdata1, mdata2, entry_bytes, params->type);

    /* Form the SBUS DMA descriptors
     * desc[0]: DMA mdata1 to the table
     * desc[1]: DMA mdata2 to the table
     * desc[2]: Jump back to desc[0] */
    dma_desc[0].control = 0;
    dma_desc[0].request = 0;
    dma_desc[0].count = entries;
    dma_desc[0].sbus_base = minfo->base + minfo->index_min;
    dma_desc[0].hostmem_phy_base = soc_cm_l2p(params->unit, mdata1);
    desc_dma_request_init(params->unit, &dma_desc[0].request, (entry_width/4), params->type);

    schan_header_init(params->unit,
                      params->type,
                      (schan_header_t *)&dma_desc[0].opcode,
                      SOC_BLOCK2SCH(params->unit, minfo->minblock),
                      SOC_MEM_ACC_TYPE(params->unit, params->mem),
                      entry_width);

    sal_memcpy(&dma_desc[1], &dma_desc[0], sizeof(sbusdma_desc_t));
    dma_desc[1].hostmem_phy_base = soc_cm_l2p(params->unit, mdata2);

    dma_desc[2].control = SOC_SBUSDMA_CTRL_JUMP;
    dma_desc[2].hostmem_phy_base = soc_cm_l2p(params->unit, dma_desc);

    desc_dma_start(params->unit, cmc, ch, dma_desc);

    cli_out("DMA channel%d test has been started: Runtime %d seconds\n",
            params->channel, params->test_time);

    for (cur_time = 0; cur_time < params->test_time; cur_time++) {
        sal_sleep(1);

        rval = soc_pci_read(params->unit, CMIC_CMCx_SBUSDMA_CHy_STATUS(cmc, ch));
        if (rval & 0x7FE) {
            /* One of the error bits is set */
            cli_out("Error in SBUS DMA STATUS: %08X\n", rval);
            params->status = SBUSDMA_STATUS_FAILURE;
            break;
        }
    }

    if (params->status != SBUSDMA_STATUS_FAILURE) {
        params->status = SBUSDMA_STATUS_SUCCESS;
    }
    cli_out("DMA channel%d test done: %d seconds run time\n", params->channel, cur_time);

    desc_dma_stop(params->unit, cmc, ch);

    soc_cm_sfree(params->unit, dma_desc);
    soc_cm_sfree(params->unit, mdata1);
    soc_cm_sfree(params->unit, mdata2);

    return;
}

void sbusdma_params_init(sbusdma_params_t* params, int unit, int type, int test_secs, int chan, soc_mem_t mem)
{
    params->unit      = unit;
    params->type      = type;
    params->test_time = test_secs;
    params->status    = SBUSDMA_STATUS_INVALID;
    params->channel   = chan;
    params->mem       = mem;
}

static const char sbusdma_stress_test_usage[] =
  "SBUS DMA stress test usage:\n"
  "RdChanBitmap=<hex> - specify which of the 9 available CMC/channels to read\n"
  "                     default is channel 0 for read, multi channels can be assigned to read\n"
  "WrChanBitmap=<hex> - specify which of the 9 available CMC/channels to write\n"
  "                     default is channel 1 for write, multi channels can be assigned to write\n"
  "Seconds=<int>      - specify test time, default is 10 seconds\n";

int sb_stress_test(int unit, args_t *a, void *pa)
{
    char thread_name[64];
    parse_table_t pt;
    int rdchbmp = 0, wrchbmp = 0, seconds = 10, index, count, done;
    uint32 addr_remap;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "WrChanBitmap", PQ_DFL | PQ_INT, 0, &wrchbmp, 0);
    parse_table_add(&pt, "RdChanBitmap", PQ_DFL | PQ_INT, 0, &rdchbmp, 0);
    parse_table_add(&pt, "Seconds", PQ_DFL | PQ_INT, 0, &seconds, 0);

    if ((parse_arg_eq(a, &pt)) < 0 || (ARG_CNT(a) > 0)) {
        cli_out("%s", sbusdma_stress_test_usage);
        cli_out("%s: ERROR: Unknown option: %s\n", ARG_CMD(a),
                ARG_CUR(a) ? ARG_CUR(a) : "*");
        parse_arg_eq_done(&pt);
        return BCM_E_FAIL;
    }

    parse_arg_eq_done(&pt);

    if (wrchbmp & rdchbmp) {
        cli_out("WrChanBitmap=0x%x and RdChanBitmap=0x%x are conflict\n", wrchbmp, rdchbmp);
        return BCM_E_FAIL;
    } else if ((wrchbmp == 0) && (rdchbmp == 0)) {
        /* Default channel */
        rdchbmp = 0x1;
        wrchbmp = 0x2;
    }

    /* Turn off background thread */
    SOC_IF_ERROR_RETURN(soc_counter_stop(unit));
#ifdef INCLUDE_MEM_SCAN
    SOC_IF_ERROR_RETURN(soc_mem_scan_stop(unit));
#endif
#ifdef BCM_SRAM_SCAN_SUPPORT
    SOC_IF_ERROR_RETURN(soc_sram_scan_stop(unit));
#endif
    SOC_IF_ERROR_RETURN(bcm_linkscan_enable_set(unit, 0));
    if (soc_feature(unit, soc_feature_arl_hashed)) {
        SOC_IF_ERROR_RETURN(soc_l2x_stop(unit));
    }

    /* Program host memory address remap */
    for (index = 1; index < SOC_CMCS_NUM(unit); index++) {
        addr_remap = soc_pci_read(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_0_OFFSET(0));
        soc_pci_write(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_0_OFFSET(index), addr_remap);
        addr_remap = soc_pci_read(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_1_OFFSET(0));
        soc_pci_write(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_1_OFFSET(index), addr_remap);
        addr_remap = soc_pci_read(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_2_OFFSET(0));
        soc_pci_write(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_2_OFFSET(index), addr_remap);
        addr_remap = soc_pci_read(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_3_OFFSET(0));
        soc_pci_write(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_3_OFFSET(index), addr_remap);
    }

    count = 0;
    for (index = 0; index < SBUSDMA_CH_MAX_NUM; index++) {
        if (wrchbmp & (1 << index)) {
            sbusdma_params_init(&sbusdma_params[count], unit, SBUSDMA_TYPE_WRITE, seconds, index, test_mem[count]);
            sal_sprintf(thread_name, "Write-channel%d", index);

            sbusdma_params[count].tid = sal_thread_create(thread_name,
                                                          SAL_THREAD_STKSZ,
                                                          100,
                                                          continuous_dma_entry,
                                                          &sbusdma_params[count]);

            count++;
        } else if (rdchbmp & (1 << index)) {
            sbusdma_params_init(&sbusdma_params[count], unit, SBUSDMA_TYPE_READ, seconds, index, test_mem[count]);
            sal_sprintf(thread_name, "Read-channel%d", index);

            sbusdma_params[count].tid = sal_thread_create(thread_name,
                                                          SAL_THREAD_STKSZ,
                                                          100,
                                                          continuous_dma_entry,
                                                          &sbusdma_params[count]);

            count++;
        }
    }

    if (count == 0) {
        cli_out("No valid channel assigned\n");
        return BCM_E_FAIL;
    }

    seconds += 1;
    do {
        sal_sleep(seconds);

        done = 1;
        for (index = 0; index < count; index++) {
            if (sbusdma_params[index].status == SBUSDMA_STATUS_RUNNING) {
                done = 0;
                break;
            }
        }

        seconds = 1;
    } while (!done);

    for (index = 0; index < count; index++) {
        cli_out("Channel%d status: %s\n", sbusdma_params[index].channel,
                status_strings[sbusdma_params[index].status]);
    }

    return BCM_E_NONE;
}
#endif
