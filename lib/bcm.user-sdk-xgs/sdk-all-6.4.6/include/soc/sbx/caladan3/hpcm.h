/*
 * $Id: hpcm.h,v 1.4.6.1 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
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
 *
 * hpcm  : Heap chunk memory manager
 *         allocates pool of dynamic memory & manages them without need for 
 *         multiple malloc's
 *
 *-----------------------------------------------------------------------------*/
#ifndef _SBX_CALADAN3_HPCM_H_
#define _SBX_CALADAN3_HPCM_H_

#include <soc/types.h>
#include <soc/sbx/sbDq.h>

#ifndef TAPS_MEM_DEBUG
/* Note this assumes only 1 chunk is used */
/* #define TAPS_MEM_DEBUG*/
#endif

/*
 * Single link
 */
typedef struct sl_s *sl_p_t;
typedef struct sl_s
{
  VOL sl_p_t next_node;
}sl_t;
#define HPCM_SL_INIT(l)  do { ((l)->next_node) = NULL; } while (0)

#define HPCM_SL_INSERT_HEAD(l, e)       \
do                                      \
{                                       \
    sl_p_t p_elem;                      \
    p_elem = (sl_p_t)(e);               \
    p_elem->next_node = (l)->next_node; \
    (l)->next_node = p_elem;            \
} while (0)

#define HPCM_SL_REMOVE_HEAD(l, e)       \
do                                      \
{                                       \
    sl_p_t p_elem;                      \
    p_elem = (sl_p_t)(e);               \
    p_elem = (l)->next_node;            \
    e = (void *)p_elem;                 \
    (l)->next_node = p_elem->next_node; \
} while (0)

struct soc_heap_mem_chunk_s;
typedef struct soc_heap_mem_chunk_s soc_heap_mem_chunk_t;

typedef struct soc_heap_mem_elem_s {
    dq_t list_node;
    void *elem;
    soc_heap_mem_chunk_t *parent;
#ifdef TAPS_MEM_DEBUG
    int  in_use;
#endif
} soc_heap_mem_elem_t;

struct soc_heap_mem_chunk_s {
    dq_t list_node;
    dq_t free_list;
    int  size;
    int  page_size;
    int  elem_size_bytes;
    int  alloc_count;
    int  free_count;
    soc_heap_mem_elem_t *elem_ctrl_pool;
    uint8 *elem_pool;
};

struct soc_heap_sl_mem_chunk_s;
typedef struct soc_heap_sl_mem_chunk_s soc_heap_sl_mem_chunk_t;

typedef struct soc_heap_sl_mem_elem_s {
    sl_t list_node;
    void *elem;
} soc_heap_sl_mem_elem_t;

struct soc_heap_sl_mem_chunk_s {
    dq_t list_node;
    sl_t free_list;
    int  size;
    int  page_size;
    int  elem_size_bytes;
    int  alloc_count;
    int  free_count;
    soc_heap_sl_mem_elem_t *elem_ctrl_pool;
    uint8 *elem_pool;
};

/* hpcm -> heap chunk manager */
extern int hpcm_init(int unit, int chunk_size, int elem_size, soc_heap_mem_chunk_t **hpcm);

extern int hpcm_destroy(int unit, soc_heap_mem_chunk_t *hpcm);

extern int hpcm_alloc(int unit, soc_heap_mem_chunk_t *hpcm, soc_heap_mem_elem_t **hpcm_elem);

extern int hpcm_free(int unit, soc_heap_mem_elem_t *hpcm_elem);

extern int hpcm_empty(int unit, soc_heap_mem_chunk_t *hpcm);

extern int hpcm_is_unused(int unit, soc_heap_mem_chunk_t *hpcm);

extern int hpcm_alloc_payload(int unit, soc_heap_mem_chunk_t *hpcm, void **payload);

extern int hpcm_free_payload(int unit, soc_heap_mem_chunk_t *hpcm, void *payload);

extern int hpcm_sl_init(int unit, int chunk_size, int elem_size_bytes, soc_heap_sl_mem_chunk_t **hpcm);

extern int hpcm_sl_alloc_payload(int unit, soc_heap_sl_mem_chunk_t *hpcm, void **payload, void **hpcm_elem_handle);

extern int hpcm_sl_free_payload(int unit, soc_heap_sl_mem_chunk_t *hpcm, void *hpcm_elem_handle);

#ifdef TAPS_MEM_DEBUG
extern int hpcm_alloc_dump(int unit, soc_heap_mem_chunk_t *hpcm);
#endif

#endif /* _SBX_CALADAN3_HPCM_H_ */
