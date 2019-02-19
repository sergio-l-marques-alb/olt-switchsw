/*
 * $Id$
 * 
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
 *
 */

/*************
* INCLUDES  *
*************/
#include <soc/portmod/group_member_list.h>
#include <shared/error.h>
/*************
* TYPE DEFS *
*************/
struct member_entry_s{
     member_entry_id_t next;
     member_entry_id_t prev;
     group_entry_id_t group;
};

struct group_entry_s{
    member_entry_id_t first;
};



/*************
* MACROS    *
*************/    

#define _SHR_GROUP_MEMBER_LIST_MEMBER_ID_VALIDATE(gml, member_id) do {   \
        if (member_id >= gml->members_count) \
        { return _SHR_E_PARAM; } \
    } while (0)


#define _SHR_GROUP_MEMBER_LIST_GROUP_ID_VALIDATE(gml, group_id)  do {   \
    if(group_id >= gml->groups_count) \
        { return _SHR_E_PARAM; } \
    } while (0)

#define NULL_CHECK(param) do{ \
    if((param) == NULL){ \
        return _SHR_E_PARAM; \
        } \
    } while(0)

/*************
* FUNCTIONS  *
*************/

int group_member_list_init(group_member_list_t *gml){
    member_entry_id_t member_id;
    member_entry_id_t group_id;
    group_entry_t group;
    member_entry_t member;
    int rv;
    
    member.group = GROUP_MEM_LIST_END;
    member.next = GROUP_MEM_LIST_END;
    member.prev = GROUP_MEM_LIST_END;
    group.first = GROUP_MEM_LIST_END;
    /* init data arrays*/
    for(member_id = 0 ; member_id < gml->members_count ; member_id++)
    {
       rv = gml->member_set(gml->user_data, member_id, &member);
       _SHR_E_IF_ERROR_RETURN(rv);
    }
    for(group_id = 0 ; group_id < gml->groups_count ; group_id++)
    {
        rv = gml->group_set(gml->user_data, group_id, &group);
        _SHR_E_IF_ERROR_RETURN(rv);
    }

    return _SHR_E_NONE;
}

uint32 group_member_list_member_entry_size_get(void)
{
    return sizeof(member_entry_t);
}
uint32 group_member_list_group_entry_size_get(void)
{
    return sizeof(group_entry_t);
}


/**
 * add member_id as member of group group_id
 * 
 * @param gml - IN
 * @param group_id - IN 
 * @param member_id - IN
 * 
 * @return int 
 */
int group_member_list_member_add(group_member_list_t* gml, group_entry_id_t group_id, member_entry_id_t member_id)
{

    member_entry_t current_first, new_member;
    group_entry_t group;
    int rv;
    /*params check*/
    NULL_CHECK(gml);
    _SHR_GROUP_MEMBER_LIST_GROUP_ID_VALIDATE(gml, group_id);
    _SHR_GROUP_MEMBER_LIST_MEMBER_ID_VALIDATE(gml, member_id);

    rv = gml->member_get(gml->user_data, member_id, &new_member);
    _SHR_E_IF_ERROR_RETURN(rv);

    if(new_member.group != GROUP_MEM_LIST_END){
        return _SHR_E_PARAM;
    }

    new_member.group = group_id;
    new_member.next = GROUP_MEM_LIST_END;
    new_member.prev = GROUP_MEM_LIST_END; 
    /*we add new item to the list head*/
    rv = gml->group_get(gml->user_data, group_id, &group);
    _SHR_E_IF_ERROR_RETURN(rv);
    /*not a new group*/
    if(group.first != GROUP_MEM_LIST_END){ 
        rv = gml->member_get(gml->user_data, group.first, &current_first);
        _SHR_E_IF_ERROR_RETURN(rv);
        current_first.prev = member_id;
        new_member.next = group.first;
        /* update the neigbour member*/
        rv = gml->member_set(gml->user_data, group.first, &current_first);
        _SHR_E_IF_ERROR_RETURN(rv);
    }
    /* update the new member and the group*/
    group.first = member_id;
    rv = gml->member_set(gml->user_data, member_id, &new_member);
    _SHR_E_IF_ERROR_RETURN(rv);

    rv = gml->group_set(gml->user_data, group_id, &group);
    _SHR_E_IF_ERROR_RETURN(rv);

    return _SHR_E_NONE;
}

/**
 * run callback_func with params member_id, param1, param2  for 
 * each member of group_id
 * 
 * @param gml - IN
 * @param group_id - IN
 * @param callback_func - IN
 * @param param1 - IN
 * @param param2  - IN
 * 
 * @return int 
 */
int group_member_list_group_members_func(
    group_member_list_t* gml,
    group_entry_id_t group_id,
    group_member_list_members_func_f callback_func,
    int param1,
    int param2
 ){
    int rv;
    member_entry_id_t member_id;
    member_entry_t member;
    group_entry_t group;

    /*params check*/
    NULL_CHECK(gml);
    NULL_CHECK(callback_func);
    _SHR_GROUP_MEMBER_LIST_GROUP_ID_VALIDATE(gml, group_id);

    rv = gml->group_get(gml->user_data, group_id, &group);
    _SHR_E_IF_ERROR_RETURN(rv);
    /*first member*/
    member_id = group.first;
    while(member_id != GROUP_MEM_LIST_END){
        rv = callback_func(member_id, param1, param2);
        _SHR_E_IF_ERROR_RETURN(rv);

        /*move to next member*/
        rv = gml->member_get(gml->user_data, member_id, &member);
        _SHR_E_IF_ERROR_RETURN(rv);
        member_id = member.next;
    }

    return _SHR_E_NONE;
}

/**
 * get the next member of the list. 
 * in the first call need to be called with INVALID_ID as 
 * member_id. 
 * in the last call member_id will contain INVALID_ID
 * 
 * @param gml - IN
 * @param group_id - IN 
 * @param member_id - IN OUT - get the current member and return 
 *                  the next one
 * 
 * @return int 
 */
int group_member_list_members_iter_get(group_member_list_t* gml, group_entry_id_t group_id, member_entry_id_t *member_id){
    member_entry_t member;
    group_entry_t group;
    int rv;

    /*params check*/
    NULL_CHECK(gml);
    NULL_CHECK(member_id);
    _SHR_GROUP_MEMBER_LIST_GROUP_ID_VALIDATE(gml, group_id);
    
    if(*member_id == GROUP_MEM_LIST_END){
        rv = gml->group_get(gml->user_data, group_id, &group);
        _SHR_E_IF_ERROR_RETURN(rv);
        *member_id = group.first;
    }
    else{
        _SHR_GROUP_MEMBER_LIST_MEMBER_ID_VALIDATE(gml, *member_id);
        rv = gml->member_get(gml->user_data, *member_id, &member);
        _SHR_E_IF_ERROR_RETURN(rv);
        if(member.group != group_id){
            return _SHR_E_PARAM;
        }
        *member_id =  member.next;
    }

    return _SHR_E_NONE;
}

/**
 * get the group_id of the group member_id belongs to.
 * 
 * @param gml - IN
 * @param member_id - IN
 * @param group_id - OUT
 * 
 * @return int 
 */
int group_member_list_group_get(group_member_list_t* gml, member_entry_id_t member_id, group_entry_id_t *group_id)
{
    member_entry_t member;
    int rv;
    /*params check*/
    NULL_CHECK(gml);
    NULL_CHECK(group_id);
    _SHR_GROUP_MEMBER_LIST_MEMBER_ID_VALIDATE(gml, member_id);

    rv = gml->member_get(gml->user_data, member_id, &member);
    _SHR_E_IF_ERROR_RETURN(rv);
    *group_id = member.group;
    return _SHR_E_NONE;
}

/**
 * get the member of group_id(maximum: max_members). if there 
 * are more than  max_members the function will return 
 * _SHR_E_FULL as error code. 
 * 
 * @param gml - IN
 * @param group_id - IN 
 * @param max_members - IN
 * @param member_ids - OUT the members ids
 * @param members_count - count of members
 * 
 * @return int 
 */
int group_member_list_group_members_get(
    group_member_list_t* gml,
    group_entry_id_t group_id,
    uint32 max_members,
    member_entry_id_t *member_ids,
    uint32 *members_count)
{
    int rv;
    member_entry_id_t member_id;
    member_entry_t member;
    group_entry_t group;

    /*params check*/
    NULL_CHECK(gml);
    NULL_CHECK(member_ids);
    NULL_CHECK(members_count);
    _SHR_GROUP_MEMBER_LIST_GROUP_ID_VALIDATE(gml, group_id);

    *members_count = 0;

    rv = gml->group_get(gml->user_data, group_id, &group);
    _SHR_E_IF_ERROR_RETURN(rv);
    /*first member*/
    member_id = group.first;
    while(member_id != GROUP_MEM_LIST_END){
        if (*members_count == max_members){
            return _SHR_E_FULL;
        }
        member_ids[*members_count] = member_id;
        (*members_count)++;

        /*move to next member*/
        rv = gml->member_get(gml->user_data, member_id, &member);
        _SHR_E_IF_ERROR_RETURN(rv);
        member_id = member.next;
    }
    return _SHR_E_NONE;    
}

/**
 * remove group and all its members
 * 
 * @param gml - IN
 * @param group_id - IN 
 * 
 * @return int 
 */
int group_member_list_group_remove(group_member_list_t* gml, group_entry_id_t group_id)
{
    group_entry_t group;
    int rv;
    /*params check*/
    NULL_CHECK(gml);
    _SHR_GROUP_MEMBER_LIST_GROUP_ID_VALIDATE(gml, group_id);

    rv = gml->group_get(gml->user_data, group_id, &group);
    _SHR_E_IF_ERROR_RETURN(rv);
    while(group.first != GROUP_MEM_LIST_END)
    { 
        rv = group_member_list_member_remove(gml, group.first);
        _SHR_E_IF_ERROR_RETURN(rv);

        rv = gml->group_get(gml->user_data, group_id, &group);
        _SHR_E_IF_ERROR_RETURN(rv);
    }

    return _SHR_E_NONE;
}




/**
 * return if group has members or not
 * 
 * @param gml - IN
 * @param group_id - IN 
 * @param is_empty_group - return value. True if no member is 
 *                       belog to the group.
 * 
 * @return int 
 */
int group_member_list_is_empty_group(group_member_list_t* gml, group_entry_id_t group_id, uint32 *is_empty_group)
{
    group_entry_t group;
    int rv;
    /*params check*/
    NULL_CHECK(gml);
    _SHR_GROUP_MEMBER_LIST_GROUP_ID_VALIDATE(gml, group_id);

    rv = gml->group_get(gml->user_data, group_id, &group);
    _SHR_E_IF_ERROR_RETURN(rv);
    *is_empty_group = (group.first == GROUP_MEM_LIST_END);
    
    return _SHR_E_NONE;
}



/**
 * remove a member
 * 
 * @param gml - IN
 * @param member_id - IN 
 * 
 * @return int 
 */
int group_member_list_member_remove(group_member_list_t* gml, member_entry_id_t member_id)
{
    member_entry_t member, next_member, prev_member;
    group_entry_t group;
    int rv, group_id;
    /*params check*/
    NULL_CHECK(gml);
    _SHR_GROUP_MEMBER_LIST_MEMBER_ID_VALIDATE(gml, member_id);


    rv = gml->member_get(gml->user_data, member_id, &member);
    _SHR_E_IF_ERROR_RETURN(rv);
    group_id = member.group;
    rv = gml->group_get(gml->user_data, group_id, &group);
    _SHR_E_IF_ERROR_RETURN(rv);
    /*if first- update the group*/
    if(group.first == member_id){
        group.first = member.next;
    }
    else{
        /*there is someone before - update him!*/
        rv = gml->member_get(gml->user_data, member.prev, &prev_member);
        _SHR_E_IF_ERROR_RETURN(rv);
        prev_member.next = member.next;
        rv = gml->member_set(gml->user_data, member.prev, &prev_member);
        _SHR_E_IF_ERROR_RETURN(rv);
    }
    /*not last - update the next*/
    if(member.next != GROUP_MEM_LIST_END){
        rv = gml->member_get(gml->user_data, member.next, &next_member);
        _SHR_E_IF_ERROR_RETURN(rv);
        next_member.prev = member.prev;
        rv = gml->member_set(gml->user_data, member.next, &next_member);
        _SHR_E_IF_ERROR_RETURN(rv);
    }
    /*clean member*/
    member.next = GROUP_MEM_LIST_END;
    member.prev = GROUP_MEM_LIST_END;
    member.group = GROUP_MEM_LIST_END;
    rv = gml->member_set(gml->user_data, member_id, &member);
    _SHR_E_IF_ERROR_RETURN(rv);

    rv = gml->group_set(gml->user_data, group_id, &group);
    _SHR_E_IF_ERROR_RETURN(rv);

    return _SHR_E_NONE;
}

