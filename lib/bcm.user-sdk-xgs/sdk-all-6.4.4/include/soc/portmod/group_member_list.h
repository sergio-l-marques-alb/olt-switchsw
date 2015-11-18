/*
 * $Id$
 * 
 * $Copyright: Copyright 2012 Broadcom Corporation.
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
 */

#ifndef _GROUP_MEMBER_LIST_H_
#define _GROUP_MEMBER_LIST_H_

#include <sal/types.h>

typedef struct member_entry_s member_entry_t;
typedef struct group_entry_s group_entry_t;
typedef uint32 member_entry_id_t;
typedef uint32 group_entry_id_t;

#define GROUP_MEM_LIST_END (0xffffffff)


typedef int (*group_member_list_group_set_f)(void *user_data, group_entry_id_t group_id, group_entry_t* group);
typedef int (*group_member_list_group_get_f)(void *user_data, group_entry_id_t group_id, group_entry_t* group);
typedef int (*group_member_list_member_set_f)(void *user_data, member_entry_id_t member_id, member_entry_t* member);
typedef int (*group_member_list_member_get_f)(void *user_data, member_entry_id_t member_id, member_entry_t* member);

typedef struct group_member_list_s{
    uint32 groups_count;
    uint32 members_count;
    group_member_list_group_set_f group_set;
    group_member_list_member_set_f member_set;
    group_member_list_group_get_f group_get;
    group_member_list_member_get_f member_get;
    void *user_data;
}group_member_list_t;



typedef int (*group_member_list_members_func_f)(member_entry_id_t member_id, int param_a, int param_b);




int group_member_list_init(group_member_list_t* gml);
uint32 group_member_list_member_entry_size_get(void);
uint32 group_member_list_group_entry_size_get(void);


int group_member_list_member_add(group_member_list_t* gml, group_entry_id_t group_id, member_entry_id_t member_id);


int group_member_list_members_iter_get(group_member_list_t* gml, group_entry_id_t group_id, member_entry_id_t *member_id);
int group_member_list_is_empty_group(group_member_list_t* gml, group_entry_id_t group_id, uint32 *is_empty_group);
int group_member_list_group_get(group_member_list_t* gml, member_entry_id_t member_id, group_entry_id_t *group_id);
int group_member_list_group_members_get(group_member_list_t* gml,
                                               group_entry_id_t group_id,
                                               uint32 max_members,
                                               member_entry_id_t *member_ids,
                                               uint32 *members_count);

/* call callback func for all the group members. the func get additional two params*/
int group_member_list_group_members_func(group_member_list_t* gml,
                                                group_entry_id_t group_id,
                                                group_member_list_members_func_f callback_func,
                                                int param1,
                                                int param2
 );


int group_member_list_group_remove(group_member_list_t* gml, group_entry_id_t group_id);
int group_member_list_member_remove(group_member_list_t* gml, member_entry_id_t member_id);

#endif /*_GROUP_MEMBER_LIST_H_*/
