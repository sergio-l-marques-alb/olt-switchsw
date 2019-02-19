/* $Id: sand_workload_status.c,v 1.5 Broadcom SDK $
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
*/



#include <shared/bsl.h>
#include <soc/dpp/drv.h>



#include <soc/dpp/SAND/Utils/sand_workload_status.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#if SOC_SAND_DEBUG
/* { */
/* $Id: sand_workload_status.c,v 1.5 Broadcom SDK $
 */


/*
 * Total work to be done, per device.
 */
static
  uint32
    Soc_sand_workload_status_total[SOC_SAND_MAX_DEVICE]
  = {0};

/*
 * Current percent of work that we already did, per device
 */
static
  uint32
    Soc_sand_workload_status_percent[SOC_SAND_MAX_DEVICE]
  = {0};


/*
 * Printing workload as percentage of total work to be done, per device.
 * Note, It is user responsibility to prevent from several printing to occur together.
 */
STATIC
  void
    soc_sand_workload_status_print(
      int unit
    )
{
  if(unit < SOC_SAND_MAX_DEVICE)
  {
    LOG_CLI((BSL_META_U(unit,
                        "\r%3u%%\r"),
             Soc_sand_workload_status_percent[unit]
             ));
  }
}
/*
 * Set the total work to be done, per device.
 */
void
  soc_sand_workload_status_start(
    int  unit,
    uint32 total_work_load
  )
{
  if(unit >= SOC_SAND_MAX_DEVICE)
  {
    goto exit;
  }

  Soc_sand_workload_status_total[unit] = total_work_load;
  if (0 == Soc_sand_workload_status_total[unit])
  {
    Soc_sand_workload_status_total[unit] = 1;
  }
  /*
   */
  Soc_sand_workload_status_percent[unit] = 0;

  soc_sand_workload_status_print(unit);

exit:
  return;
}

/*
 * Advances workload per device.
 */
void
  soc_sand_workload_status_run_no_print(
    int  unit,
    uint32 current_workload
  )
{
  uint32
    percent;
  /*
   */
  percent = 0;
  /*
   */

  if(unit >= SOC_SAND_MAX_DEVICE)
  {
    goto exit;
  }


  /*
   */
  if (0 == Soc_sand_workload_status_total[unit])
  {
    percent = 100;
  }
  else
  {
    /*
     * Avoid overflow/underflow.
     */
    if ( current_workload < (0xFFFFFFFF/100))
    {
      percent = (current_workload * 100) / Soc_sand_workload_status_total[unit] ;
    }
    else
    {
      percent = current_workload  / (Soc_sand_workload_status_total[unit]/100) ;
    }
  }

  Soc_sand_workload_status_percent[unit] = percent;

exit:
  return;

}

void
  soc_sand_workload_status_get(
    int  unit,
    uint32 *percent
  )
{
  if(unit >= SOC_SAND_MAX_DEVICE)
  {
    goto exit;
  }

  *percent = Soc_sand_workload_status_percent[unit];

exit:
  return;
}

/*
 * Advances workload and prints percentage if change detected, per device.
 */
void
  soc_sand_workload_status_run(
    int  unit,
    uint32 current_workload
  )
{
  uint32
    percent_old,
    percent_new;

  if (unit >= SOC_SAND_MAX_DEVICE)
  {
    goto exit;
  }

  soc_sand_workload_status_get(unit, &percent_old);
  soc_sand_workload_status_run_no_print(unit, current_workload);
  soc_sand_workload_status_get(unit, &percent_new);

  if (percent_old < percent_new)
  {
    soc_sand_workload_status_print(unit);
  }

exit:
  return;

}

/* } */
#else
/* { */

/*
 * If not in debug mode empty implementation.
 */

void
  soc_sand_workload_status_start(
    int unit,
    uint32 total_work_load
  )
{
  return;
}

void
  soc_sand_workload_status_run(
    int unit,
    uint32 current_workload
  )
{
  return;
}

void
  soc_sand_workload_status_get(
    int  unit,
    uint32 *percent
  )
{
  return;
}

void
  soc_sand_workload_status_run_no_print(
    int  unit,
    uint32 current_workload
  )
{
  return;
}


/* } */
#endif

