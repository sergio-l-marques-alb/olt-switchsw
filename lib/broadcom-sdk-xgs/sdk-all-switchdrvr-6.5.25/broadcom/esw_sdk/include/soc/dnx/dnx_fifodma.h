/**
 * \file dnx_fifodma.h
 *
 *  Internal DNX FIFODMA Managment APIs
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *  All Rights Reserved.
 *
 */

#ifndef DNX_FIFODMA_H_INCLUDED
/*
 * {
 */
#define DNX_FIFODMA_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif /* */

#ifdef DNX_DATA_INTERNAL
#undef DNX_DATA_INTERNAL
#endif
#include <shared/shrextend/shrextend_debug.h>

/**
 * }
 */

/*
 * Defines
 * {
 */

/**
 * }
 */

/*
 * MACROs
 * {
 */

/**
 * }
 */

/*
 * TYPEDEFs
 * {
 */

/** Callback function which is used to handle entries in host memory provided by Users.
 * Callback may be called multiple times per interrupt.
 * Each call includes information on how many times it will be called in total and which callback number the current callback is.
 * entry_number will be numbered 1 to total_amount_of_entries.
 * user_data may be used to communicate between different calls
 *   */
typedef void (
    *soc_dnx_fifodma_interrupt_handler) (
    int unit,
    void *entry,
    int entry_size,
    int entry_number,
    int total_amount_of_entries,
    void **user_data);

/*
 * Possible fifodma source in 1-1 map, index to fifodma_map of dnx_data_fifodma.general
 */
typedef enum soc_dnx_fifodma_src_e
{
    soc_dnx_fifodma_src_oam_event,
    soc_dnx_fifodma_src_oam_status,
    soc_dnx_fifodma_src_olp,
    soc_dnx_fifodma_src_crps0_dma0,
    soc_dnx_fifodma_src_crps0_dma1,
    soc_dnx_fifodma_src_crps1_dma0,
    soc_dnx_fifodma_src_crps1_dma1,
    soc_dnx_fifodma_src_event_fifo,
    soc_dnx_fifodma_src_8_reserved,
    soc_dnx_fifodma_src_9_reserved,
    soc_dnx_fifodma_src_10_reserved,
    soc_dnx_fifodma_src_11_reserved,
    soc_dnx_fifodma_src_reserved = 0xf
} soc_dnx_fifodma_src_t;

/**
 * Configration information set by user
 * Input parameter to soc_dnx_fifodma_channel_start()
 */
typedef struct
{
    /**
     * Maximum number of entries.
     */
    uint32 max_entries;
    /**
     * Boolean value, '0' - register is used; '1' - memory is used.
     */
    int is_mem;
    /**
     * Name of memory.
     */
    int mem;                    /* soc_mem_t */
    /**
     * Name of register.
     */
    int reg;                    /* soc_reg_t */
    /**
     * Index in register or memory array.
     */
    int array_index;
    /**
     * Block parameter to calculate memory address or Port paramenter to calculate register address.
     * For register, it could be port number or REG_PORT_ANY or SOC_CORE_ALL, default is REG_PORT_ANY
     */
    int copyno;
    /**
     * Specified entry size which does not match the register or memory size, ignore when it equals to 0.
     */
    int force_entry_size;
    /**
     * Number of valid entries in host memory exceeds which an interrupt will be raised.
     */
    int threshold;
    /**
     * Time before triggering a FIFO_DMA_TIMEOUT interrupt, in microseconds.
     */
    int timeout;
    /**
     * Boolean value, '0' - is in interrupt mode; '1' - is in poll mode.
     */
    int is_poll;
    /**
     * Size of entry
     */
    uint32 entry_size;
    /**
     * Boolean value, '0' - current fifodma channel is not used; '1' - current fifodma channel is used.
     */
    int is_alloced;
    /**
     * Boolean value, '0' - current fifodma channel is not started; '1' - current fifodma channel is started.
     */
    int is_started;
    /**
     * Boolean value, '0' - current fifodma channel is not paused; '1' - current fifodma channel is paused.
     */
    int is_paused;
    /**
     * Current entry location in host memory
     */
    int last_entry_id;
    /**
     * Host memory buffer
     */
    uint8 *host_buff;
} soc_dnx_fifodma_config_t;

/*
 * }
 */

/**
 * \brief - init fifodma config info.
 *
 * \param [in]  unit - unit id
 * \param [in]  fifodma_info - pointer to fifodma config info
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   Initialize DMA config info. This procedure must be invoked before calling soc_dnx_fifodma_channel_start()
 *
 * \see
 *
 */
shr_error_e soc_dnx_fifodma_config_t_init(
    int unit,
    soc_dnx_fifodma_config_t * fifodma_info);

/**
 * \brief - get available fifodma channel id.
 *
 * \param [in]  unit - unit id
 * \param [in]  source - dma source id for a specific application
 * \param [out] chan - fifodma channel id
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   get a free fifodma channel id per fifodma source, the map of fifodma channel and source is defined in dnx data
 *
 * \see
 *
 */
shr_error_e soc_dnx_fifodma_channel_alloc(
    int unit,
    const soc_dnx_fifodma_src_t source,
    int *chan);

/**
 * \brief - get allocated fifodma channel id.
 *
 * \param [in]  unit - unit id
 * \param [in]  source - dma source id for a specific application
 * \param [out] chan - fifodma channel id
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   get an allocated fifodma channel id according to fifo dma source
 *   in the case that the channel has not been allocated, return error
 *
 * \see
 *
 */
shr_error_e soc_dnx_fifodma_channel_get(
    int unit,
    const soc_dnx_fifodma_src_t source,
    int *chan);

/**
 * \brief - Check if a fifodma source is started. No errors are returned if the source is not allocated or started.
 *
 * \param [in]  unit - unit id
 * \param [in]  source - dma source id for a specific application
 * \param [out] is_started - return 1 if the fifodma is started; 0 otherwise.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   get an allocated fifodma channel id according to fifo dma source. Return the is_started parameter.
 *
 * \see
 *
 */
shr_error_e soc_dnx_fifodma_channel_is_started(
    int unit,
    soc_dnx_fifodma_src_t source,
    int *is_started);

/**
 * \brief - start fifodma channel.
 *
 * \param [in]  unit - unit id
 * \param [in]  chan - fifodma channel id
 * \param [in]  fifodma_info - fifodma control information
 * \param [in]  callback_name - callback function name from User
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   soc_dnx_fifodma_config_t_init() must be called before setting fifodma_info
 *   following parameters are mandatory:
 *     1. max_entries - max number of entries
 *     2. is_mem - mark the source connected to fifodma, memory or register
 *     3. mem - name of memory, entry size is size of memory is memory is used
 *     4. reg - name of register, entry size is size of register is register is used
 *     5. array_index - index of register or memory, by default is 0
 *     6. threshold - an interrupt will be raised if number of valid entries in host memory exceeds this threshold value
 *     7. timeout - the time before triggering a FIFO_DMA_TIMEOUT interrupt, in macroseconds
 *     8. is_poll - mark current works in pool mode or interrupt mode
 *     9. callback_name - name of callback function from user which is taken as fifodma interrupt handler
 *   the main process is:
 *     1. validity check
 *     2. allocate shareable memory
 *     3. set threshold, timeout,endian
 *     4. enable interrupt in interrupt mode
 *     5. start fifodma channel
 *     6. update fifodma database
 * \see
 *
 */
shr_error_e soc_dnx_fifodma_channel_start(
    int unit,
    int chan,
    const soc_dnx_fifodma_config_t * fifodma_info,
    char *callback_name);

/**
 * \brief - stop fifodma channel.
 *
 * \param [in]  unit - unit id
 * \param [in]  chan - fifodma channel id
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   1. disable interrupt
 *   2. stop fifodma channel.
 *   3. free host memory
 * \see
 *  soc_dnx_fifodma_channel_free
 */
shr_error_e soc_dnx_fifodma_channel_stop(
    int unit,
    int chan);

/**
 * \brief - free fifodma channel.
 *
 * \param [in]  unit - unit id
 * \param [in]  chan - fifodma channel id
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   1. if current fifodma channel is running, stop it at first
 *   2. reset ECI_FIFO_DMA_SEL
 * \see
 */
shr_error_e soc_dnx_fifodma_channel_free(
    int unit,
    int chan);

/**
 * \brief - start fifodma channel.
 *
 * \param [in]  unit - unit id
 * \param [in]  chan - fifodma channel id
 * \param [out]  fifodma_info - information of current fifodma channel
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 * \see
 *
 */
shr_error_e soc_dnx_fifodma_channel_info_get(
    int unit,
    int chan,
    soc_dnx_fifodma_config_t * fifodma_info);

/**
 * \brief - get the number of available entries, the pointer of entries buffer and the size of entry.
 *
 * \param [in]  unit - unit id
 * \param [in]  chan - fifodma channel id
 * \param [out]  count - the number of available entries
 * \param [out]  entries - pointer to entries buffer
 * \param [out]  entry_size - size of entry
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   1. get the number of entries which could be read
 *   2. modify the number of entries if there is wrap around
 *   3. get the pointer to next entry
 * \see
 */
shr_error_e soc_dnx_fifodma_num_entries_get(
    int unit,
    int chan,
    int *count,
    uint8 **entries,
    uint32 *entry_size);

/**
 * \brief - set the number of entries read by S/W from host memory.
 *
 * \param [in]  unit - unit id
 * \param [in]  chan - fifodma channel id
 * \param [in]  count - the number of entries which have been handled
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   1. update the position of next entry in host memory per count
 *   2. notify DMA the number of entries which has been read
 * \see
 */
shr_error_e soc_dnx_fifodma_set_entries_read(
    int unit,
    int chan,
    int count);

/**
 * \brief - resume fifodma channel which is paused.
 *
 * \param [in]  unit - unit id
 * \param [in]  chan - fifodma channel id
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   1. get fifodma control information from swstate database
 *   2. start fifodma channel
 *   3. update swstate database
 * \see
 *
 */
shr_error_e soc_dnx_fifodma_channel_resume(
    int unit,
    int chan);

/**
 * \brief - pause fifodma channel.
 *
 * \param [in]  unit - unit id
 * \param [in]  chan - fifodma channel id
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   1. stop fifodma channel without de-allocating host memory
 *   2. marked as is_paused
 * \see
 *
 */
shr_error_e soc_dnx_fifodma_channel_pause(
    int unit,
    int chan);

/**
 * \brief - common fifodma interrupt handler function, follow the format of sal_dpc_fn_t.
 *
 * \param [in]  unit_ptr - pointer to unit id
 * \param [in]  chan_ptr - pointer to fifodma channel id
 * \param [in]  unused_p3 - unused parameter
 * \param [in]  unused_p4 - unused parameter
 * \param [in]  unused_p5 - unused parameter
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   common fifodma interrupt handler function.
 *   1. get the entry count in host memory
 *   2. loop to handle valid entries one by one, application specific callback is called here
 *   3. handle possible host memory wrap around
 *   4. tell fifodma the entries number that has been handled
 * \see
 *  cmicx_dnx_fifodma_done
 */
void dnx_fifodma_intr_handler(
    void *unit_ptr,
    void *chan_ptr,
    void *unused_p3,
    void *unused_p4,
    void *unused_p5);

/**
 * \brief - example of callback function from user.
 *
 * \param [in]  unit - unit id
 * \param [in]  entry - pointer to entry
 * \param [in]  entry_size - size of entry
 * \param [in] entry_number - Entry number within the DMA events processed in given interrupt
 * \param [in] total_amount_of_entries - Total number of events processed in given interrupt
 * \param [in,out] user_data - Memory managed by user.
 * \return
 * \remark
 * \see
 */
void dnx_fifodma_interrupt_handler_example(
    int unit,
    void *entry,
    int entry_size,
    int entry_number,
    int total_amount_of_entries,
    void **user_data);

/**
 * \brief
 *  Initialize fifodma.
 *
 * \param [in] unit - Unit-ID
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *   1. initialize common fifodma
 *   2. allocate and initialize fifodma table
 * \see
 *
 */
shr_error_e soc_dnx_fifodma_init(
    int unit);

/**
 * \brief
 *  De-Initialize fifodma
 *
 * \param [in] unit - Unit-ID
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *   1. free allocated fifodma channel
 *   2. de-initialize common fifodma
 *   3. free fifodma table
 * \see
 *
 */
shr_error_e soc_dnx_fifodma_deinit(
    int unit);

#endif /* DNX_FIFODMA_H_INCLUDED */
