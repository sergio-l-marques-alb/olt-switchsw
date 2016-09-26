/*! \file rx.c
 *
 * RX procedures for DNX.
 *
 * Here add DESCRIPTION.
 */
/*
 * $Copyright: Copyright 2016 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_RX
/*
 * Include files which are specifically for DNX. Final location.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <shared/shrextend/shrextend_error.h>
/*
 * }
 */
/*
 * Include files currently used for DNX. To be modified and moved to
 * final location.
 * {
 */
#include <shared/bslenum.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/pkt.h>

/*#include <bcm/rx.h>*/
#include <bcm_int/common/rx.h>

#ifdef CMODEL_SERVER_MODE
#include <soc/dnx/cmodel/cmodel_reg_access.h>
#include <fcntl.h>
#include <unistd.h>
#endif
/*
 * }
 */

/*
 * Macros and defines for rx thread management
 * {
 */
#ifdef CMODEL_SERVER_MODE

/* 
 * This is the default time between minimum token refreshes. It is also
 * the maximum time between RX thread wake-ups.
 */
#define BCM_RX_TOKEN_CHECK_US_DEFAULT   100000     /* 10 times/sec. */

#define BASE_SLEEP_VAL   500000                                            \

/* Set sleep to base value */
#define INIT_SLEEP    rx_control.sleep_cur = BASE_SLEEP_VAL

/* Lower sleep time if val is < current */
#define SLEEP_MIN_SET(val)                                           \
    (rx_control.sleep_cur = ((val) < rx_control.sleep_cur) ?         \
     (val) : rx_control.sleep_cur)
#endif
/*
 * }
 */

int bcm_cmodel_rx_start(int unit, bcm_rx_cfg_t *cfg);

int
bcm_dnx_rx_start(
  int unit, bcm_rx_cfg_t *cfg)
{

  SHR_FUNC_INIT_VARS(unit);

#ifdef CMODEL_SERVER_MODE

  SHR_IF_ERR_EXIT(bcm_cmodel_rx_start(unit, cfg));

#else

  
  SHR_ERR_EXIT(BCM_E_NONE, "Regular RX handling");

#endif /*CMODEL_SERVER_MODE*/

exit:
  SHR_FUNC_EXIT;
}

/* CMODEL functions for RX */
#ifdef CMODEL_SERVER_MODE

void cmodel_convert_rx_data_from_chars(int unit, int data_length_in_bits, unsigned char *data_chars, uint8 *data_binary)
{
  int convert_position = 0;
  int result_index = 0;
  char zero_char = '0';

  while (convert_position + 8 <= data_length_in_bits)
  {
    data_binary[result_index] = (((uint8)(data_chars[convert_position])-(uint8)(zero_char))     << 7 |
                                 ((uint8)(data_chars[convert_position + 1])-(uint8)(zero_char)) << 6 |
                                 ((uint8)(data_chars[convert_position + 2])-(uint8)(zero_char)) << 5 |
                                 ((uint8)(data_chars[convert_position + 3])-(uint8)(zero_char)) << 4 |
                                 ((uint8)(data_chars[convert_position + 4])-(uint8)(zero_char)) << 3 |
                                 ((uint8)(data_chars[convert_position + 5])-(uint8)(zero_char)) << 2 |
                                 ((uint8)(data_chars[convert_position + 6])-(uint8)(zero_char)) << 1 |
                                 ((uint8)(data_chars[convert_position + 7])-(uint8)(zero_char)));
    convert_position += 8;
    result_index++;
  }
}

/* Build the pkt struct */
void cmodel_build_rx_packet(int unit, cmodel_ms_id_e ms_id, uint32 nof_signals, uint32 src_port, int len, unsigned char *buf, bcm_pkt_t *pkt)
{
  /* Currently there are no signals, only the packet's data. Take the data after the signal ID and data size */
  cmodel_convert_rx_data_from_chars(unit, len, &(buf[8]), pkt->pkt_data->data);
  pkt->pkt_data->len = ((len-8) >> 3);    /* Convert from bit to bytes. The first 8 bytes are the signal ID and data length */
  pkt->_pkt_data.len = pkt->pkt_data->len;
  pkt->pkt_len = pkt->pkt_data->len;
  sal_memcpy(pkt->_pkt_data.data, pkt->pkt_data->data, pkt->pkt_data->len);

  pkt->blk_count = 1;                     /* Number of blocks in data array. */
  pkt->unit = unit;                       /* Unit number. */
  /*    pkt->cos; */                          /* The local COS queue to use. */
  /*   pkt->color;   */                      /* Packet color. */
  pkt->src_port = (int16)src_port;                     /* Source port used in header/tag. */
  /* dest_port; */                   /* Destination port used in header/tag. */
  pkt->opcode = BCM_HG_OPCODE_CPU;   /* BCM_HG_OPCODE_xxx. */

  /*   bcm_gport_t dst_gport;   */           /* Destination virtual port */
  /*   bcm_gport_t src_gport;   */           /* Source virtual port */
  /*   bcm_multicast_t multicast_group;  */  /* Destination multicast group. */
}

static void cmodel_rx_pkt_thread(void *param)
{
  int unit = 0;
  unsigned char buf[MAX_PACKET_SIZE_CMODEL];
  uint8 data_buf[MAX_PACKET_SIZE_CMODEL];
  int len;
  int rv = BCM_E_NONE;
  uint32 src_port;
  cmodel_ms_id_e ms_id = 0;
  uint32 nof_signals = 0;
  bcm_pkt_t pkt;
  bcm_pkt_blk_t pkt_data;

  pkt.pkt_data = &pkt_data;
  pkt.pkt_data->data = data_buf;
  pkt._pkt_data.data = data_buf;

  INIT_SLEEP;
  while (rx_control.thread_running)
  {
    /* Lock system rx start/stop mechanism. */
    _BCM_RX_SYSTEM_LOCK;

    /* Wait until a packet is ready and read it */
    rv = cmodel_read_buffer(unit, &ms_id, &nof_signals, &src_port, &len, &(buf[0]));

    /* Packet was read from buffer */
    if (rv == BCM_E_NONE) {
      /* Build the packets struct */
      cmodel_build_rx_packet(unit, ms_id, nof_signals, src_port, len, &(buf[0]), &pkt);

      /* Process the packet */
      rx_cmodel_process_packet(unit, &pkt);
    }

    /* Unlock system rx start/stop mechanism. */
    _BCM_RX_SYSTEM_UNLOCK;

    _BCM_RX_CHECK_THREAD_DONE;

    SLEEP_MIN_SET(BASE_SLEEP_VAL);

    sal_sem_take(rx_control.pkt_notify, rx_control.sleep_cur);
    rx_control.pkt_notify_given = FALSE;

    INIT_SLEEP;
  }

  /* Done using self-pipe, close fds */
  close(pipe_fds[0]);
  close(pipe_fds[1]);

  rx_control.thread_exit_complete = TRUE;
  sal_thread_exit(0);
}

/* Init the Rx thread that deals with packets to the CPU */
int bcm_cmodel_rx_start(int unit, bcm_rx_cfg_t *cfg)
{
  int priority = RX_THREAD_PRI_DFLT;
  int flags;

  SHR_FUNC_INIT_VARS(unit);

  /* Timer/Event semaphore thread sleeping on. */
  if (NULL == rx_control.pkt_notify) {
    rx_control.pkt_notify = sal_sem_create("RX pkt ntfy", sal_sem_BINARY, 0);
    if (NULL == rx_control.pkt_notify) {
      SHR_ERR_EXIT(BCM_E_MEMORY, "NULL == rx_control.pkt_notify");
    }
    rx_control.pkt_notify_given = FALSE;
  }

  /* RX start/stop on one of the units protection mutex. */
  if (NULL == rx_control.system_lock) {
    rx_control.system_lock = sal_mutex_create("RX system lock");
    if (NULL == rx_control.system_lock) {
      sal_sem_destroy(rx_control.pkt_notify);
      SHR_ERR_EXIT(BCM_E_MEMORY, "NULL == rx_control.system_lock");
    }
  }

  /* Setup pipe for select exit notification.
     We use a "self-pipe" trick:
        - The write end is maintained by the main thread
        - The read end is selected in the RX thread
     When we need to close the RX thread, we simply write
     to the pipe, and we exit the blocking select call */
  if (pipe(pipe_fds) == -1) {
    SHR_ERR_EXIT(BCM_E_FAIL, "pipe(pipe_fds) == -1");
  }

  /* Make read and write ends of pipe nonblocking:
     Get read end flags */
  flags = fcntl(pipe_fds[0], F_GETFL);
  if (flags == -1) {
    SHR_ERR_EXIT(BCM_E_FAIL, "fcntl(pipe_fds[0], F_GETFL)=-1");
  }

  /* Make read end nonblocking */
  flags |= O_NONBLOCK;
  if (fcntl(pipe_fds[0], F_SETFL, flags) == -1) {
    SHR_ERR_EXIT(BCM_E_FAIL, "fcntl(pipe_fds[0], F_SETFL, flags) == -1");
  }

  /* Get write end flags */
  flags = fcntl(pipe_fds[1], F_GETFL);
  if (flags == -1) {
    SHR_ERR_EXIT(BCM_E_FAIL, "fcntl(pipe_fds[1], F_GETFL)= -1");
  }

  /* Make write end nonblocking */
  flags |= O_NONBLOCK;
  if (fcntl(pipe_fds[1], F_SETFL, flags) == -1) {
    SHR_ERR_EXIT(BCM_E_FAIL, "fcntl(pipe_fds[1], F_SETFL, flags) == -1");
  }

  /* Start rx thread. */
  rx_control.thread_running = TRUE;
  rx_control.cmodel_rx_tid = sal_thread_create("bcmCmodelRX",
                                        SAL_THREAD_STKSZ,
                                        priority,
                                        cmodel_rx_pkt_thread, NULL);

  /* Indicate RX is running */
  rx_ctl[unit]->flags |= BCM_RX_F_STARTED;

exit:
  SHR_FUNC_EXIT;
}

/* End of C model functions */
#endif /*CMODEL_SERVER_MODE*/
