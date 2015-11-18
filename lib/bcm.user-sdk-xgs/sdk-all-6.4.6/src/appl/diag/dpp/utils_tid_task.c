/* $Id$
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
*/
/*
 * Basic_include_file.
 */

/*
 * Tasks information.
 */
#include <appl/diag/dpp/tasks_info.h>
/*
 * Utilities include file.
 */
#include <appl/diag/dpp/utils_defi.h>

#include <appl/diag/dpp/utils_defx.h>


/*
 * Object: Task IDs.
 * {
 */
/*
 * Task IDs
 */
static
   int
   Tid_task[NUM_TASKS] ;
/*
 * Initialize all task IDs to -1 and use to indicate whether
 * a true ID has been obtained.
 */
void
   init_task_id(
        void
         )
{
  unsigned short
      us ;
  for (us = 0 ; us < NUM_TASKS ; us++)
  {
    Tid_task[us] = -1 ;
  }
  return ;
}
void
   set_task_id(
         int task_private_index,
         int tid
        )
{
  if (task_private_index<NUM_TASKS)
  {
    Tid_task[task_private_index] = tid ;
  }
  return ;
}
/*
 * Given private tasd id, get VxWorks task id.
 * If none is found, return -1.
 */
int
   get_task_id(
         int task_private_index
        )
{
  int
      ret ;
  if (task_private_index >= NUM_TASKS)
  {
    ret = -1 ;
  }
  else
  {
    ret = Tid_task[task_private_index] ;
  }
  return(ret) ;
}
/*
 * Given private task id, check whether it is alive.
 */
int
   is_task_alive(
         int task_private_index
        )
{
  int
      ret ;
  if (task_private_index >= NUM_TASKS)
  {
    ret = FALSE ;
  }
  else
  {
    if (Tid_task[task_private_index] == -1)
    {
      ret = FALSE ;
    }
    else
    {
      ret = TRUE ;
    }
  }
  return (ret) ;
}
/*
 * Given private tasd id, clear its entry (load -1)
 * to indicate there is no such task any more.
 * This accompabies task deletion.
 */
void
   clear_task_id(
         int task_private_index
        )
{
  /*
   * If internal task id is out of range,
   * ignore this command.
   */
  if (task_private_index < NUM_TASKS)
  {
    Tid_task[task_private_index] = -1 ;
  }
  return ;
}
/*
 * Given VxWorks task id, get private tasdk id.
 * If none is found, return -1.
 */
int
   get_private_task_id(
             int task_id
            )
{
  int
      ret ;
  int
      ii ;
  ret = -1 ;
  for (ii = 0 ; ii < NUM_TASKS ; ii++)
  {
    if (Tid_task[ii]  == task_id)
    {
      ret = ii ;
      break ;
    }
  }
  return(ret) ;
}
/*
 * End object
 * }
 */


