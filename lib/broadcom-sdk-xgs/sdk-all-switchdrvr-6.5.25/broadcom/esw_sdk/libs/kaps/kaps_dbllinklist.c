/*
 **************************************************************************************
 Copyright 2009-2019 Broadcom Corporation

 This program is the proprietary software of Broadcom Corporation and/or its licensors,
 and may only be used, duplicated, modified or distributed pursuant to the terms and
 conditions of a separate, written license agreement executed between you and
 Broadcom (an "Authorized License").Except as set forth in an Authorized License,
 Broadcom grants no license (express or implied),right to use, or waiver of any kind
 with respect to the Software, and Broadcom expressly reserves all rights in and to
 the Software and all intellectual property rights therein.
 IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY
 WAY,AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

 Except as expressly set forth in the Authorized License,

 1. This program, including its structure, sequence and organization, constitutes the
    valuable trade secrets of Broadcom, and you shall use all reasonable efforts to
    protect the confidentiality thereof,and to use this information only in connection
    with your use of Broadcom integrated circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" AND WITH
    ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER
    EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM
    SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
    NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS LICENSORS
    BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES
    WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY TO USE
    THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF
    OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
    ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
 **************************************************************************************
 */

#include "kaps_dbllinklist.h"

kaps_dbl_link_list *
kaps_dbl_link_list_init(
    kaps_dbl_link_list * head)
{
    head->m_back_p = head;
    head->m_next_p = head;

    return head;
}

void
kaps_dbl_link_list_insert(
    kaps_dbl_link_list * self,
    kaps_dbl_link_list * node)
{
    kaps_dbl_link_list *next = (kaps_dbl_link_list *) self->m_next_p;

    self->m_next_p = node;
    node->m_back_p = self;

    next->m_back_p = node;
    node->m_next_p = next;
}

void
kaps_dbl_link_list_remove(
    kaps_dbl_link_list * node,
    kaps_dbl_link_list_destroy_node_t destroyNode,
    void *arg)
{
    kaps_dbl_link_list *back = (kaps_dbl_link_list *) node->m_back_p;
    kaps_dbl_link_list *next = (kaps_dbl_link_list *) node->m_next_p;

    back->m_next_p = next;
    next->m_back_p = back;

    destroyNode(node, arg);
}

void
kaps_dbl_link_list_destroy(
    kaps_dbl_link_list * head,
    kaps_dbl_link_list_destroy_node_t destroyNode,
    void *arg)
{
    kaps_dbl_link_list *here = (kaps_dbl_link_list *) head->m_next_p;
    kaps_dbl_link_list *next;

    while (here != head)
    {
        next = (kaps_dbl_link_list *) here->m_next_p;
        destroyNode(here, arg);

        here = next;
    }

    destroyNode(head, arg);
}
