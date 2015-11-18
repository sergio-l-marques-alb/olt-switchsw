/* $Id: jer_fabric.h,v 1.30 Broadcom SDK $
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


#ifndef __JER_INGRESS_SCHEDULER_INCLUDED__
/* { */

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>


#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/error.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */



/* } */

/*************
 * MACROS    *
 *************/
/* { */

/*************
 * TYPE DEFS *
 *************/
/* { */

/* } */

/*************
 * GLOBALS   *
 *************/
/* { */

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/*********************************************************************  
* NAME:
*     jer_ingress_scheduler_init
* TYPE:
*   PROC
* DATE:
*   Jun 15 2013
* FUNCTION:
*     This procedure init ingress scheduler
*     links.
* INPUT:
*  SOC_SAND_IN  int                 unit -
* REMARKS:
*     None.
*********************************************************************/
soc_error_t
  jer_ingress_scheduler_init(
    SOC_SAND_IN  int                 unit
  );
/*********************************************************************  
* NAME:
*     jer_ingress_scheduler_clos_slow_start_get
* TYPE:
*   PROC
* DATE:
*   Jun 15 2013
* FUNCTION:
*     This procedure init returns slow start config for clos
*     links.
* INPUT:
*  SOC_SAND_IN  int                 unit -
* REMARKS:
*     None.
*********************************************************************/
soc_error_t
jer_ingress_scheduler_clos_slow_start_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_OUT SOC_TMC_ING_SCH_CLOS_INFO   *clos_info
    );


/*********************************************************************  
* NAME:
*     jer_ingress_scheduler_clos_slow_start_set
* TYPE:
*   PROC
* DATE:
*   Jun 15 2013
* FUNCTION:
*     This procedure configures slow start config for clos
*     links.
* INPUT:
*  SOC_SAND_IN  int                 unit -
* REMARKS:
*     None.
*********************************************************************/
soc_error_t
jer_ingress_scheduler_clos_slow_start_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN SOC_TMC_ING_SCH_CLOS_INFO   *clos_info
    );


/*********************************************************************  
* NAME:
*     jer_ingress_scheduler_mesh_slow_start_get
* TYPE:
*   PROC
* DATE:
*   Jun 15 2013
* FUNCTION:
*     This procedure returns slow start config for mesh
*     links.
* INPUT:
*  SOC_SAND_IN  int                 unit -
* REMARKS:
*     None.
*********************************************************************/
soc_error_t
jer_ingress_scheduler_mesh_slow_start_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_OUT SOC_TMC_ING_SCH_SHAPER   *shaper_info
    );


/*********************************************************************  
* NAME:
*     jer_ingress_scheduler_mesh_slow_start_set
* TYPE:
*   PROC
* DATE:
*   Jun 15 2013
* FUNCTION:
*     This procedure configures slow start config for mesh
*     links.
* INPUT:
*  SOC_SAND_IN  int                 unit -
* REMARKS:
*     None.
*********************************************************************/
soc_error_t
jer_ingress_scheduler_mesh_slow_start_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN SOC_TMC_ING_SCH_SHAPER   *shaper_info
    );


/*********************************************************************  
* NAME:
*     jer_ingress_scheduler_clos_bandwidth_get
* TYPE:
*   PROC
* DATE:
*   Jun 15 2013
* FUNCTION:
*     This procedure returns rate for clos
*     links.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  int                 gport -  
*  SOC_SAND_IN  uint32              rate -
* REMARKS:
*     None.
*********************************************************************/
soc_error_t
  jer_ingress_scheduler_clos_bandwidth_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 gport, 
    SOC_SAND_OUT uint32              *rate
  );

/*********************************************************************  
* NAME:
*     jer_ingress_scheduler_clos_bandwidth_set
* TYPE:
*   PROC
* DATE:
*   Jun 15 2013
* FUNCTION:
*     This procedure configures rate for clos
*     links.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  int                 gport -  
*  SOC_SAND_IN  uint32              rate -
* REMARKS:
*     None.
*********************************************************************/
soc_error_t
  jer_ingress_scheduler_clos_bandwidth_set(
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 gport, 
    SOC_SAND_IN  uint32              rate
    );

/*********************************************************************  
* NAME:
*     jer_ingress_scheduler_mesh_bandwidth_get
* TYPE:
*   PROC
* DATE:
*   Jun 15 2013
* FUNCTION:
*     This procedure returns rate for mesh
*     links.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  int                 gport -  
*  SOC_SAND_IN  uint32              rate -
* REMARKS:
*     None.
*********************************************************************/
soc_error_t
  jer_ingress_scheduler_mesh_bandwidth_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 gport, 
    SOC_SAND_OUT uint32              *rate
  );

/*********************************************************************  
* NAME:
*     jer_ingress_scheduler_mesh_bandwidth_set
* TYPE:
*   PROC
* DATE:
*   Jun 15 2013
* FUNCTION:
*     This procedure configures rate for mesh
*     links.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  int                 gport -  
*  SOC_SAND_IN  uint32              rate -
* REMARKS:
*     None.
*********************************************************************/
soc_error_t
  jer_ingress_scheduler_mesh_bandwidth_set(
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 gport, 
    SOC_SAND_IN  uint32              rate
    );


/*********************************************************************  
* NAME:
*     jer_ingress_scheduler_clos_burst_get
* TYPE:
*   PROC
* DATE:
*   Jun 15 2013
* FUNCTION:
*     This procedure returns burst for clos
*     links.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  int                 gport -  
*  SOC_SAND_IN  uint32              burst -
* REMARKS:
*     None.
*********************************************************************/
soc_error_t
  jer_ingress_scheduler_clos_burst_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 gport, 
    SOC_SAND_OUT int                 *burst
  );

/*********************************************************************  
* NAME:
*     jer_ingress_scheduler_clos_burst_set
* TYPE:
*   PROC
* DATE:
*   Jun 15 2013
* FUNCTION:
*     This procedure configures burst for clos
*     links.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  int                 gport -  
*  SOC_SAND_IN  uint32              burst -
* REMARKS:
*     None.
*********************************************************************/
soc_error_t
  jer_ingress_scheduler_clos_burst_set(
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 gport, 
    SOC_SAND_IN  int                 burst
    );

/*********************************************************************  
* NAME:
*     jer_ingress_scheduler_mesh_burst_get
* TYPE:
*   PROC
* DATE:
*   Jun 15 2013
* FUNCTION:
*     This procedure returns burst for mesh
*     links.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  int                 gport -  
*  SOC_SAND_IN  uint32              burst -
* REMARKS:
*     None.
*********************************************************************/
soc_error_t
  jer_ingress_scheduler_mesh_burst_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 gport, 
    SOC_SAND_OUT int                 *burst
  );

/*********************************************************************  
* NAME:
*     jer_ingress_scheduler_mesh_burst_set
* TYPE:
*   PROC
* DATE:
*   Jun 15 2013
* FUNCTION:
*     This procedure configures burst for mesh
*     links.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  int                 gport -  
*  SOC_SAND_IN  uint32              burst -
* REMARKS:
*     None.
*********************************************************************/
soc_error_t
  jer_ingress_scheduler_mesh_burst_set(
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 gport, 
    SOC_SAND_IN  int                 burst
    );

/*********************************************************************  
* NAME:
*     jer_ingress_scheduler_clos_sched_get
* TYPE:
*   PROC
* DATE:
*   Jun 15 2013
* FUNCTION:
*     This procedure returns weight for clos
*     links.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  int                 gport -  
*  SOC_SAND_IN  uint32              weight -
* REMARKS:
*     None.
*********************************************************************/
soc_error_t
  jer_ingress_scheduler_clos_sched_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 gport, 
    SOC_SAND_OUT int                 *weight
  );

/*********************************************************************  
* NAME:
*     jer_ingress_scheduler_clos_sched_set
* TYPE:
*   PROC
* DATE:
*   Jun 15 2013
* FUNCTION:
*     This procedure configures weight for clos
*     links.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  int                 gport -  
*  SOC_SAND_IN  uint32              weight -
* REMARKS:
*     None.
*********************************************************************/
soc_error_t
  jer_ingress_scheduler_clos_sched_set(
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 gport, 
    SOC_SAND_IN  int                 weight
    );

/*********************************************************************  
* NAME:
*     jer_ingress_scheduler_mesh_sched_get
* TYPE:
*   PROC
* DATE:
*   Jun 15 2013
* FUNCTION:
*     This procedure returns weight for mesh
*     links.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  int                 gport -  
*  SOC_SAND_IN  uint32              weight -
* REMARKS:
*     None.
*********************************************************************/
soc_error_t
  jer_ingress_scheduler_mesh_sched_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 gport, 
    SOC_SAND_OUT int                 *weight
  );

/*********************************************************************  
* NAME:
*     jer_ingress_scheduler_mesh_sched_set
* TYPE:
*   PROC
* DATE:
*   Jun 15 2013
* FUNCTION:
*     This procedure configures weight for mesh
*     links.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  int                 gport -  
*  SOC_SAND_IN  uint32              weight -
* REMARKS:
*     None.
*********************************************************************/
soc_error_t
  jer_ingress_scheduler_mesh_sched_set(
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 gport, 
    SOC_SAND_IN  int                 weight
    );

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __JER_INGRESS_SCHEDULER_INCLUDED__*/
#endif
