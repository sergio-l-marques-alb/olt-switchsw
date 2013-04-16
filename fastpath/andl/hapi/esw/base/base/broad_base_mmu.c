/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
**********************************************************************
*
* @filename  broad_mmu.c
*
* @purpose   Modifications to the MMU for IBP,HOL,Init, ...
*
* @component 
*
* @create    12/01/2008
*
* @author    mbaucom 
* @end
*
*********************************************************************/

#include "l7_common.h"
#include "broad_mmu.h"

#include "soc/mem.h"
#include "dtl_exports.h"
#include "soc/drv.h"
#include "ibde.h"
#include "l7_usl_bcm_port.h"
#include "l7_usl_port_db.h"
#include "l7_usl_common.h"
#include "sysbrds.h"
#include "logger.h"   /* PTin added */

static int system_flowcontrol_mode = 0;
static int mmu_drop_mode = MMU_DROP_MODE_EGRESS;
static void *mmu_lock=L7_NULLPTR;

#ifdef BCM_FIREBOLT_SUPPORT

#define FB_IBPCELL_LIMIT_E       576
#define FB_IBPPKT_LIMIT_E        64

/* 72K / 128 = 576 cells */
#define FB_E2EIBPCELL_LIMIT_E    576
#define FB_E2EIBPPKT_LIMIT_E     64

#define FB_HOLCOS_LIMIT_E        256
#define FB_HOLCOS_LIMIT_HG        256
/* CPU HOL CoS must be less than the IBPPKT */
#define FB_HOLCOS_LIMIT_CPU       48

/* Assuming a fanin of 8, such that 576 * 8 = 4608 */
#define FB_DYNCELL_LIMIT_E       4608
#define FB_DYNCELLRESET_LIMIT_E  4584
#define FB_DYNCELL_LIMIT_HG       9216
#define FB_DYNCELLRESET_LIMIT_HG  9192
#define FB_DYNCELL_LIMIT_CPU      256
#define FB_DYNCELLRESET_LIMIT_CPU 232

#define FB_HOLCOS_MAX_PKTSETLIMIT_E 2048
#define FB_HOLCOS_MAX_PKTSETLIMIT_CPU 1536
  
/*********************************************************************
* @purpose  Modify MMU for Firebolt and Firebolt-2
*
* @param    unit - bcm unit number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t hapiBroadFireboltMmuConfigModify(L7_uint32 unit)
{
  int cpu_port,t1,t2;
  soc_port_t  port;
  int treg,cos;
  int ncells;
  int static_cells = 0;;
  int dynamic_cells = 0;
  int dyncell_e,dyncell_hg;
  int ibpcell_e;
  uint32 field_max = 0;

  /* calculate the static allocation */
  PBMP_ALL_ITER(unit,port)
  {
    for (cos = 0;cos < 8;cos++)
    {
      treg = 0;
      if (READ_LWMCOSCELLSETLIMITr(unit, port, cos, &treg) < 0) return L7_FAILURE;
      t1 = soc_reg_field_get(unit, LWMCOSCELLSETLIMITr, treg, CELLSETLIMITf);
      static_cells+=t1;
    }
  }

  cpu_port = CMIC_PORT(unit);
  
  /* The max number of cells */
  ncells = soc_mem_index_count(unit, MMU_CBPDATA0m);

  dynamic_cells = ncells - static_cells - 1;  

  /* Quick sanity of the cells to insure less than max dynamic 
   * it is not a good thing to allow all dynamic cells to be allocated on ibp or dyn
   */
  dyncell_e = (FB_DYNCELL_LIMIT_E > dynamic_cells)?dynamic_cells:FB_DYNCELL_LIMIT_E;
  dyncell_hg = (FB_DYNCELL_LIMIT_HG > dynamic_cells)?dynamic_cells:FB_DYNCELL_LIMIT_HG;
  ibpcell_e = (FB_IBPCELL_LIMIT_E > dynamic_cells)?dynamic_cells:FB_IBPCELL_LIMIT_E;

  /* ALL ETHERNET FE, GE, XE, ... configuration */
  PBMP_E_ITER(unit, port)
  {
    /* IBPCELLSETLIMT must be set high enough to prevent a jumbo packet
     * from triggering pause 
     */
    treg = 0;
    SOC_IF_ERROR_RETURN(READ_IBPCELLSETLIMITr(unit, port, &treg));
    soc_reg_field_set(unit, IBPCELLSETLIMITr, &treg, CELLSETLIMITf, ibpcell_e);
    /* 75% of the cell limit */
    soc_reg_field_set(unit, IBPCELLSETLIMITr, &treg, RESETLIMITSELf, 0);
    if (WRITE_IBPCELLSETLIMITr(unit, port, treg) < 0) return L7_FAILURE;
   
    /* IBPPKTSETLIMIT must be lower than the HOLCOSPKTSETLIMT or HOL will
     * drop the packet prior to port entering pause
     */
    treg = 0;
    if (READ_IBPPKTSETLIMITr(unit, port, &treg) < 0) return L7_FAILURE;
    soc_reg_field_set(unit, IBPPKTSETLIMITr, &treg, PKTSETLIMITf,FB_IBPPKT_LIMIT_E);

    /* 75% of the pkt limit */
    soc_reg_field_set(unit, IBPPKTSETLIMITr, &treg, RESETLIMITSELf,0);

    if (WRITE_IBPPKTSETLIMITr(unit, port, treg) < 0) return L7_FAILURE;
  
    /* IBPDISCARDSETLIMIT and E2EIBPDISCARDSETLIMIT should be set very high 
     * if we can't pause effectively the discard should happen when moving 
     * to the egress resources (HOLD)
     */
    treg = 0;
    if (READ_IBPDISCARDSETLIMITr(unit, port, &treg) < 0) return L7_FAILURE;
    soc_reg_field_set(unit, IBPDISCARDSETLIMITr, &treg, DISCARDSETLIMITf, ncells-1);

    if (WRITE_IBPDISCARDSETLIMITr(unit, port, treg) < 0) return L7_FAILURE;

    treg = 0;
    SOC_IF_ERROR_RETURN(READ_E2EIBPDISCARDSETLIMITr(unit, port, &treg));
    soc_reg_field_set(unit, E2EIBPDISCARDSETLIMITr, &treg, DISCARDSETLIMITf, ncells-1);

    if (WRITE_E2EIBPDISCARDSETLIMITr(unit, port, treg) < 0) return L7_FAILURE;

    /* E2EIBPCELLSETLIMIT and E2EIBPPKTSETLIMIT should be set relative to the
     * IBP settings
     */
    treg = 0;
    if (READ_E2EIBPCELLSETLIMITr(unit, port, &treg) < 0) return L7_FAILURE;

    /* 75% of the reset limit */
    soc_reg_field_set(unit,E2EIBPCELLSETLIMITr, &treg, RESETLIMITSELf,0);
    soc_reg_field_set(unit,E2EIBPCELLSETLIMITr, &treg, CELLSETLIMITf,FB_E2EIBPCELL_LIMIT_E);

    if (WRITE_E2EIBPCELLSETLIMITr(unit, port, treg) < 0) return L7_FAILURE;
 

    treg = 0; 
    if (READ_E2EIBPPKTSETLIMITr(unit, port, &treg) < 0) return L7_FAILURE;

    /* 75% of the reset limit */
    soc_reg_field_set(unit,E2EIBPPKTSETLIMITr, &treg, RESETLIMITSELf,0);
    soc_reg_field_set(unit,E2EIBPPKTSETLIMITr, &treg, PKTSETLIMITf,FB_E2EIBPPKT_LIMIT_E);

    if (WRITE_E2EIBPPKTSETLIMITr(unit, port, treg) < 0) return L7_FAILURE;

    /* The HOLCOSPKTSETLIMIT and DYNCELLLIMIT fields are chip specific, make
     * sure to account for them 
     */
    /* HOLCOSPKTSETLIMIT set to a value higher than the IBPPKTSETLIMIT for all 
     * Ethernet ports.  The CPU port must be lower than the IBPPKTSETLIMIT so
     * so that the CPU will throttle an ethernet port
     */
    treg = 0;
    t1 = FB_HOLCOS_MAX_PKTSETLIMIT_E / (HAPI_BROAD_EGRESS_HIGH_PRIORITY_COS+1);

    soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &treg, PKTSETLIMITf,t1);

    if (soc_reg_field_valid(unit, HOLCOSPKTSETLIMITr, RESETLIMITSELf)) 
    {
      /* 75% of the pktsetlimit */
      soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &treg, RESETLIMITSELf,0);
    }
    else if (soc_reg_field_valid(unit, HOLCOSPKTSETLIMITr, RESETLIMITf)) 
    {
      /* 75% of the pktsetlimit */
      t2 = t1 * 75 / 100;
      soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &treg, RESETLIMITf,t2);
    }
    else
    {
      /* unknown device, need to bail */
      return L7_FAILURE;
    }

    for (cos = 0;cos < 8;cos++)
    {
      if (cos <= HAPI_BROAD_EGRESS_HIGH_PRIORITY_COS)
      {
        if (WRITE_HOLCOSPKTSETLIMITr(unit, port, cos, treg) < 0) return L7_FAILURE;
      }
      else
      {
        int zero_reg=0;
        if (WRITE_HOLCOSPKTSETLIMITr(unit, port, cos, zero_reg) < 0) return L7_FAILURE;
      }
    }

    /* DYNCELLLIMIT should be set so that a large burst, TCP win can be absorbed
     * additionally, it should be set much higher than the IBPCELLSETLIMIT so that
     * some amount of FANIN can be paused.  
     */
    treg = 0;
    if (soc_reg_field_valid(unit,DYNCELLLIMITr , DYNCELLSETLIMITf))
    {
      soc_reg_field_set(unit, DYNCELLLIMITr, &treg, DYNCELLSETLIMITf,dyncell_e);
    }
    if (soc_reg_field_valid(unit,DYNCELLLIMITr , DYNCELLLIMITf))
    {
      soc_reg_field_set(unit, DYNCELLLIMITr, &treg, DYNCELLLIMITf,dyncell_e);
    }
    
    if (soc_reg_field_valid(unit,DYNCELLLIMITr , DYNCELLRESETLIMITf))
    {
      /* 75% of the cell limit */
      soc_reg_field_set(unit, DYNCELLLIMITr, &treg, DYNCELLRESETLIMITf,(dyncell_e*75)/100);
    }
    else if (soc_reg_field_valid(unit,DYNCELLLIMITr , DYNCELLRESETLIMITSELf))
    {
      /* 75% of the cell limit */
      soc_reg_field_set(unit, DYNCELLLIMITr, &treg, DYNCELLRESETLIMITSELf,0);
    }
    else
    {
      /* Unknown device, need to bail */
      return L7_FAILURE;
    }

    if (WRITE_DYNCELLLIMITr(unit, port, treg) < 0) return L7_FAILURE;
  } 

  /* CPU configuration */
  /* CPU port needs to have it's drop threshold set lower than the IBP limits
   * E2EIBP limits so that it will not contribute to the pause.
   */
 
  /* The HOLCOSPKTSETLIMIT and DYNCELLLIMIT fields are chip specific, make
   * sure to account for them 
   */
  /* HOLCOSPKTSETLIMIT set to a value higher than the IBPPKTSETLIMIT for all 
   * Ethernet ports.  The CPU port must be lower than the IBPPKTSETLIMIT so
   * so that the CPU will throttle an ethernet port
   */
  treg = 0;
  t1 = FB_HOLCOS_MAX_PKTSETLIMIT_CPU / 8;
  if (t1 > FB_HOLCOS_LIMIT_CPU) t1 = FB_HOLCOS_LIMIT_CPU;              

  soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &treg, PKTSETLIMITf,t1);

  if (soc_reg_field_valid(unit, HOLCOSPKTSETLIMITr, RESETLIMITSELf)) 
  {
    /* 75% of the pktsetlimit */
    soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &treg, RESETLIMITSELf,0);
  }
  else if (soc_reg_field_valid(unit, HOLCOSPKTSETLIMITr, RESETLIMITf)) 
  {
    /* 75% of the pktsetlimit */
    t2 = t1 * 75 / 100;
    soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &treg, RESETLIMITf,t2);
  }
  else
  {
    /* unknown device, need to bail */
    return L7_FAILURE;
  }

  for (cos = 0;cos < 8;cos++)
  {
    if (WRITE_HOLCOSPKTSETLIMITr(unit, cpu_port, cos, treg) < 0) return L7_FAILURE;
  }

  /* DYNCELLLIMIT should be set so that a large burst, TCP win can be absorbed
   * additionally, it should be set much higher than the IBPCELLSETLIMIT so that
   * some amount of FANIN can be paused.  
   */
  treg = 0;
  if (soc_reg_field_valid(unit,DYNCELLLIMITr ,DYNCELLSETLIMITf )) 
  {
    soc_reg_field_set(unit, DYNCELLLIMITr, &treg, DYNCELLSETLIMITf,FB_DYNCELL_LIMIT_CPU);
  }
  
  if (soc_reg_field_valid(unit,DYNCELLLIMITr ,DYNCELLLIMITf )) 
  {
    soc_reg_field_set(unit, DYNCELLLIMITr, &treg, DYNCELLLIMITf,FB_DYNCELL_LIMIT_CPU);
  }

  if (soc_reg_field_valid(unit,DYNCELLLIMITr , DYNCELLRESETLIMITf))
  {
    /* 75% of the cell limit */
    soc_reg_field_set(unit, DYNCELLLIMITr, &treg, DYNCELLRESETLIMITf,(FB_DYNCELL_LIMIT_CPU*75)/100);
  }
  else if (soc_reg_field_valid(unit,DYNCELLLIMITr , DYNCELLRESETLIMITSELf))
  {
    /* 75% of the cell limit */
    soc_reg_field_set(unit, DYNCELLLIMITr, &treg, DYNCELLRESETLIMITSELf,0);
  }
  else
  {
    /* Unknown device, need to bail */
    return L7_FAILURE;
  }

  if (WRITE_DYNCELLLIMITr(unit, cpu_port, treg) < 0) return L7_FAILURE;

  /* handle the cpu backpressure issue */
  /* set the cpu IBP set limit to the max so no bp is allowed on cpu */
  treg = 0;
  field_max = ( 1 << soc_reg_field_length(unit,IBPPKTSETLIMITr, PKTSETLIMITf)) - 1;
  if (READ_IBPPKTSETLIMITr(unit, cpu_port, &treg) < 0) {
      return L7_FAILURE;
  }
  soc_reg_field_set(unit, IBPPKTSETLIMITr, &treg, PKTSETLIMITf, field_max);

  if (WRITE_IBPPKTSETLIMITr(unit, cpu_port, treg) < 0) {
      return L7_FAILURE;
  }

  /* Set CPU IBP Cell Limit */
  treg = 0;
  field_max = ( 1 << soc_reg_field_length(unit,IBPCELLSETLIMITr, CELLSETLIMITf)) - 1;
  if (READ_IBPCELLSETLIMITr(unit, cpu_port, &treg) < 0) {
      return L7_FAILURE;
  }
  soc_reg_field_set(unit, IBPCELLSETLIMITr, &treg, CELLSETLIMITf, field_max);

  if (WRITE_IBPCELLSETLIMITr(unit, cpu_port, treg) < 0) {
      return L7_FAILURE;
  }

  /* Set CPU E2E IBP PKT Limit */
  treg = 0;
  field_max = ( 1 << soc_reg_field_length(unit,E2EIBPPKTSETLIMITr, PKTSETLIMITf)) - 1;
  if (READ_E2EIBPPKTSETLIMITr(unit, cpu_port, &treg) < 0) {
      return L7_FAILURE;
  }
  soc_reg_field_set(unit, E2EIBPPKTSETLIMITr, &treg, PKTSETLIMITf, field_max);

  if (WRITE_E2EIBPPKTSETLIMITr(unit, cpu_port, treg) < 0) {
      return L7_FAILURE;
  }

  /* Set CPU E2E IBP Cell Limit */
  treg = 0;
  field_max = ( 1 << soc_reg_field_length(unit,E2EIBPCELLSETLIMITr, CELLSETLIMITf)) - 1;
  if (READ_E2EIBPCELLSETLIMITr(unit, cpu_port, &treg) < 0) {
      return L7_FAILURE;
  }
  soc_reg_field_set(unit, E2EIBPCELLSETLIMITr, &treg, CELLSETLIMITf, field_max);

  if (WRITE_E2EIBPCELLSETLIMITr(unit, cpu_port, treg) < 0) {
      return L7_FAILURE;
  }

  /* HG PORT Configuration */
  PBMP_HG_ITER(unit, port)
  {
    treg = 0;
    t1 = FB_HOLCOS_MAX_PKTSETLIMIT_E / 8;

    soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &treg, PKTSETLIMITf,t1);

    if (soc_reg_field_valid(unit, HOLCOSPKTSETLIMITr, RESETLIMITSELf)) 
    {
      /* 75% of the pktsetlimit */
      soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &treg, RESETLIMITSELf,0);
    }
    else if (soc_reg_field_valid(unit, HOLCOSPKTSETLIMITr, RESETLIMITf)) 
    {
      /* 75% of the pktsetlimit */
      t2 = t1 * 75 / 100;
      soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &treg, RESETLIMITf,t2);
    }
    else
    {
      /* unknown device, need to bail */
      return L7_FAILURE;
    }

    /* HOLCOSPKT */  
    for (cos = 0;cos < 8;cos++)
    {
      if (WRITE_HOLCOSPKTSETLIMITr(unit, port, cos, treg) < 0) return L7_FAILURE;
    }

    /* DYNCELLLIMIT */
    treg = 0;
    if (soc_reg_field_valid(unit,DYNCELLLIMITr , DYNCELLSETLIMITf))
    {
      soc_reg_field_set(unit, DYNCELLLIMITr, &treg, DYNCELLSETLIMITf,dyncell_hg );
    }
    if (soc_reg_field_valid(unit,DYNCELLLIMITr , DYNCELLLIMITf))
    {
      soc_reg_field_set(unit, DYNCELLLIMITr, &treg, DYNCELLLIMITf,dyncell_hg );
    }
    
    if (soc_reg_field_valid(unit,DYNCELLLIMITr , DYNCELLRESETLIMITf))
    {
      /* 75% of the cell limit */
      soc_reg_field_set(unit, DYNCELLLIMITr, &treg, DYNCELLRESETLIMITf,(dyncell_hg*75)/100);
    }
    else if (soc_reg_field_valid(unit,DYNCELLLIMITr , DYNCELLRESETLIMITSELf))
    {
      /* 75% of the cell limit */
      soc_reg_field_set(unit, DYNCELLLIMITr, &treg, DYNCELLRESETLIMITSELf,0);
    }
    else
    {
      /* Unknown device, need to bail */
      return L7_FAILURE;
    }

    if (WRITE_DYNCELLLIMITr(unit, port, treg) < 0) return L7_FAILURE;
  }
  return L7_SUCCESS;
}
#endif

#ifdef BCM_HELIX_SUPPORT

#define FP_HELIX_IBPCELL_MIN (0x180)
#define FP_HELIX_IBPPKT_MIN_E (0x20)
#define FP_HELIX_IBPPKT_MIN_HG (0x20)

/*********************************************************************
* @purpose  Modify MMU for HELIX
*
* @param    unit - bcm unit number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t hapiBroadHelixMmuConfigModify(L7_uint32 unit)
{
  int ge_port,cpu_port,t1,t2;
  soc_port_t  port;
  int treg,cos;
  int ncells;

  /* The max number of cells */
  ncells = soc_mem_index_count(unit, MMU_CBPDATA0m);

  cpu_port = CMIC_PORT(unit);
  ge_port = SOC_PORT_MIN(unit,ge);

  /* Set the IBP Pkt and Cell limits */
  /* modify the E2E settings to be in sync with the IBP settings */
  PBMP_ALL_ITER(unit, port)
  {
    treg = 0;

    /* begins IBP and E2E Cell limits */
    if (READ_IBPCELLSETLIMITr(unit, port, &treg) < 0) 
      return L7_FAILURE;

    t1 = soc_reg_field_get(unit, IBPCELLSETLIMITr, treg,CELLSETLIMITf );
    if (port != cpu_port)
    {
      if (t1 < FP_HELIX_IBPCELL_MIN ) 
      {
        t1 = FP_HELIX_IBPCELL_MIN ;
        soc_reg_field_set(unit, IBPCELLSETLIMITr, &treg, CELLSETLIMITf, t1);
        if (WRITE_IBPCELLSETLIMITr(unit, port, treg) < 0) return L7_FAILURE ;
      }
    }

    if (READ_E2EIBPCELLSETLIMITr(unit, port, &treg) < 0) 
      return L7_FAILURE;

    t2 = soc_reg_field_get(unit, E2EIBPCELLSETLIMITr, treg,CELLSETLIMITf );
    if (t2 < t1)
    {
      soc_reg_field_set(unit, E2EIBPCELLSETLIMITr, &treg, CELLSETLIMITf, t1);
      if (WRITE_E2EIBPCELLSETLIMITr(unit, port, treg) < 0)
        return L7_FAILURE;
    }
    /* ends IBP and E2E Cell limits */

    /* begins IBP and E2E PKT limits */
    treg = t1 = t2 = 0;
    if (READ_IBPPKTSETLIMITr(unit, port, &treg) < 0)
      return L7_FAILURE;

    t1 = soc_reg_field_get(unit, IBPPKTSETLIMITr, treg, PKTSETLIMITf);
    if (IS_E_PORT(unit,port) && (t1 < FP_HELIX_IBPPKT_MIN_E ))
    {
      t1 = FP_HELIX_IBPPKT_MIN_E ;
      soc_reg_field_set(unit, IBPPKTSETLIMITr, &treg, PKTSETLIMITf,t1);
      if (WRITE_IBPPKTSETLIMITr(unit, port, treg) < 0)
        return L7_FAILURE;
    }

    if (IS_HG_PORT(unit,port) && ( t1 < FP_HELIX_IBPPKT_MIN_HG ))
    {
      t1 = FP_HELIX_IBPPKT_MIN_HG ;
      soc_reg_field_set(unit, IBPPKTSETLIMITr, &treg, PKTSETLIMITf,t1);
      if (WRITE_IBPPKTSETLIMITr(unit, port, treg) < 0) 
        return L7_FAILURE;
    }

    if (READ_E2EIBPPKTSETLIMITr(unit, port, &treg) < 0)
      return L7_FAILURE;

    t2 = soc_reg_field_get(unit, E2EIBPPKTSETLIMITr, treg, PKTSETLIMITf);
    if (t2 < t1)
    {
      soc_reg_field_set(unit, E2EIBPPKTSETLIMITr, &treg, PKTSETLIMITf,t1);
      if (WRITE_E2EIBPPKTSETLIMITr(unit, port, treg) < 0)
        return L7_FAILURE;
    }
    /* ends IBP and E2E PKT limits */


    /* set the ibp and e2e discard limits */
    treg = 0;
    soc_reg_field_set(unit, E2EIBPDISCARDSETLIMITr, &treg, DISCARDSETLIMITf, ncells-1);

    if (WRITE_E2EIBPDISCARDSETLIMITr(unit, port, treg) < 0) return L7_FAILURE;

    treg = 0;
    soc_reg_field_set(unit, IBPDISCARDSETLIMITr, &treg, DISCARDSETLIMITf, ncells-1);

    if (WRITE_IBPDISCARDSETLIMITr(unit, port, treg) < 0) return L7_FAILURE;
  }

  /* modify the CPU HOL PKT limits to prevent Flow control on Front panel ports */
  /* the HOLCOSPKTSETLIMIT needs to be 1/2 of the IBPPKTSETLIMIT */
  if (READ_IBPPKTSETLIMITr(unit, ge_port, &treg) < 0)
    return L7_FAILURE;

  t1 = soc_reg_field_get(unit, IBPPKTSETLIMITr, treg, PKTSETLIMITf);
  for (cos = 0; cos < HAPI_BROAD_EGRESS_HIGH_PRIORITY_COS; cos++)
  {
    if (READ_HOLCOSPKTSETLIMITr(unit, cpu_port, cos, &treg) < 0)
      return L7_FAILURE;

    t2 = t1 / 2;
    soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &treg, PKTSETLIMITf,t2);
    if (WRITE_HOLCOSPKTSETLIMITr(unit, cpu_port, cos, treg) < 0)
      return L7_FAILURE;
  }
  
  /* set the DYN settings for the CPU to much less than IBP for Front panel ports */
  /* THE DYNCELLSETLIMIT needs to be 1/2 the IBPCELLSETLIMIT at the highest */
  if (READ_IBPCELLSETLIMITr(unit, ge_port, &treg) < 0)
    return L7_FAILURE;

  t1 = soc_reg_field_get(unit, IBPCELLSETLIMITr, treg,CELLSETLIMITf );
  if (READ_DYNCELLLIMITr(unit, cpu_port, &treg) < 0)
    return L7_FAILURE;

  t2 = t1 / 2;
  if (SOC_IS_HELIX1(unit))
  {
    soc_reg_field_set(unit, DYNCELLLIMITr, &treg, DYNCELLLIMITf,t2);
  }
  else
  {
    soc_reg_field_set(unit, DYNCELLLIMITr, &treg, DYNCELLSETLIMITf,t2);
    /* set the reset limit to 75% of limit */
    soc_reg_field_set(unit, DYNCELLLIMITr, &treg, DYNCELLRESETLIMITf,(t2*75)/100);
  }
  if (WRITE_DYNCELLLIMITr(unit, cpu_port, treg) < 0)
    return L7_FAILURE;

  return L7_SUCCESS;
}
#endif /* HELIX SUPPORT */


#ifdef BCM_FIREBOLT_SUPPORT
/*********************************************************************
* @purpose  Modify MMU for FB/FB2/Helix devices
*
* @param    unit - bcm unit number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments CPU CoS queues 4-7 will be given more cells
*
* @end
*********************************************************************/
L7_RC_t hapiBroadFbHxMmuModify(L7_uint32 unit)
{
 int   cos, cpuExtraCellsSum, numq, totalDynCellLimit, totalDyncellResetLimit = 0;
#ifdef L7_STACKING_PACKAGE
  int   cpuExtraCells[8] = {0, 0, 0, 0, 12, 12, 12, 52}; /* We want 8K static for Cos 7 */
#else
  int   cpuExtraCells[8] = {0, 0, 0, 0, 12, 12, 12, 12};
#endif
  int val;
  int   scells, sreset;
  soc_port_t  port;

  cpuExtraCellsSum = 0;
  numq = 8;
  for (cos = 0; cos < numq; cos++)
  {
    cpuExtraCellsSum += cpuExtraCells[cos];
  }

  /* get the total dyn cell limit */

  if (SOC_IS_FIREBOLT2(unit))
  {
    val = 0;
    SOC_IF_ERROR_RETURN(READ_TOTALDYNCELLLIMITr(unit, &val));

    totalDynCellLimit = soc_reg_field_get(unit, TOTALDYNCELLLIMITr, val,
                       SETLIMITf);
    totalDynCellLimit = totalDynCellLimit - (cpuExtraCellsSum);
    if (totalDynCellLimit <= 0)
    {
      /* We reduce dyn cell limit and add the difference to cpu cos. If we dont
         have room for that, then dont change anything just return.
      */
      return L7_SUCCESS;
    }
    soc_reg_field_set(unit, TOTALDYNCELLLIMITr,
                          &val, SETLIMITf, totalDynCellLimit);
    SOC_IF_ERROR_RETURN(WRITE_TOTALDYNCELLLIMITr(unit, val));

    val = 0;
    SOC_IF_ERROR_RETURN(READ_TOTALDYNCELLRESETLIMITr(unit, &val));
    totalDyncellResetLimit =  soc_reg_field_get(unit, TOTALDYNCELLRESETLIMITr,
                          val, RESETLIMITf);
    if (totalDyncellResetLimit != 0)
    {
      totalDyncellResetLimit = totalDyncellResetLimit - (cpuExtraCellsSum);
      if (totalDyncellResetLimit <= 0)
        totalDyncellResetLimit = 1;
    }
    soc_reg_field_set(unit, TOTALDYNCELLRESETLIMITr,
                          &val, RESETLIMITf, totalDyncellResetLimit);
    SOC_IF_ERROR_RETURN(WRITE_TOTALDYNCELLRESETLIMITr(unit, val));
  }
  else
  {
    val = 0;
    SOC_IF_ERROR_RETURN(READ_TOTALDYNCELLLIMITr(unit, &val));
    totalDynCellLimit = soc_reg_field_get(unit, TOTALDYNCELLLIMITr, val,
                       TOTALDYNCELLLIMITf);
    totalDynCellLimit = totalDynCellLimit - (cpuExtraCellsSum);
    if (totalDynCellLimit <= 0)
    {
      /* We reduce dyn cell limit and add the difference to cpu cos. If we dont
         have room for that, then dont change anything just return.
      */
      return L7_SUCCESS;
    }
    soc_reg_field_set(unit, TOTALDYNCELLLIMITr,
                          &val, TOTALDYNCELLLIMITf, totalDynCellLimit);

    if (soc_reg_field_valid(unit, TOTALDYNCELLLIMITr,
                                TOTALDYNCELLRESETLIMITf))
    {
      totalDyncellResetLimit = soc_reg_field_get(unit, TOTALDYNCELLLIMITr,
                               val, TOTALDYNCELLRESETLIMITf);
      if (totalDyncellResetLimit != 0)
      {
        totalDyncellResetLimit = totalDyncellResetLimit - (cpuExtraCellsSum);
        if (totalDyncellResetLimit <= 0)
          totalDyncellResetLimit = 1;

        soc_reg_field_set(unit, TOTALDYNCELLLIMITr,
                                &val, TOTALDYNCELLRESETLIMITf, totalDyncellResetLimit);
      }
    }

    SOC_IF_ERROR_RETURN(WRITE_TOTALDYNCELLLIMITr(unit, val));
  }

  /* Increase the static cell limit for higher cpu cos 4-7 */
  /* calculate static limits first */
  scells = 0;
  sreset = 0;
  port = CMIC_PORT(unit);
  for (cos = 4; cos < 8; cos++)
  {
    val = 0;

    SOC_IF_ERROR_RETURN(READ_LWMCOSCELLSETLIMITr(unit, port, cos, &val));

    if (soc_reg_field_valid(unit, LWMCOSCELLSETLIMITr, CELLSETLIMITf))
    {
      scells = soc_reg_field_get(unit, LWMCOSCELLSETLIMITr, val, CELLSETLIMITf);
    }
    scells = scells + cpuExtraCells[cos];

    if (soc_reg_field_valid(unit, LWMCOSCELLSETLIMITr,CELLRESETLIMITf))
    {
      sreset = soc_reg_field_get(unit, LWMCOSCELLSETLIMITr, val, CELLRESETLIMITf);
    }
    sreset = sreset + cpuExtraCells[cos];

    if (soc_reg_field_valid(unit, LWMCOSCELLSETLIMITr, CELLSETLIMITf))
    {
      soc_reg_field_set(unit, LWMCOSCELLSETLIMITr,&val, CELLSETLIMITf, scells);
    }

    if (soc_reg_field_valid(unit, LWMCOSCELLSETLIMITr,CELLRESETLIMITf))
    {
      soc_reg_field_set(unit, LWMCOSCELLSETLIMITr,&val, CELLRESETLIMITf, sreset);
    }

    SOC_IF_ERROR_RETURN(WRITE_LWMCOSCELLSETLIMITr(unit, port, cos, val));

  }
#ifdef BCM_HELIX_SUPPORT
  if (SOC_IS_HELIX(unit)) 
  {
    /* MMU settings specific to 563xx chips*/
    if (hapiBroadHelixMmuConfigModify(unit) != L7_SUCCESS) return L7_FAILURE;
  }
#endif
#ifdef BCM_FIREBOLT_SUPPORT
  if (SOC_IS_FIREBOLT(unit) || SOC_IS_FIREBOLT2(unit))
  {
    if (hapiBroadFireboltMmuConfigModify(unit) != L7_SUCCESS) return L7_FAILURE;
  }
#endif

  return L7_SUCCESS;
}
#endif

#ifdef BCM_TRIUMPH_SUPPORT

#ifndef SAFC_DISABLED

/* 
* Map internal priority 0-4 to PG 0
* Only do this at the beginning of time.  Not to be done while
* traffic is operational
*/
static pg_cosmap_t cosmap[] ={{0,0}, {1,0}, {2,0}, {3,0}, {4,0}, {5,1}, {6,1}, {7,7} };
static pg_cosmap_t enduro_cosmap[] ={{0,7}, {1,7}, {2,7}, {3,7}, {4,7}, {5,7}, 
{6,7}, {7,7} };

/* 
 *  Priority Group settings
 */
static pg_vals_t pg_vals[TR_MMU_NUM_PG] = {
/* pg,min_cell,min_pkt,floor_cell,reset_offset_cell,reset_offset_pkt,hdrm_cell,hdrm_pkt,thresh */
  {0, TR_MMU_IN_PORT_MIN_CELLS, TR_MMU_IN_PORT_MIN_PKTS, 0, 
   TR_MMU_HG_PG_RESET_OFFSET_CELLS, TR_MMU_HG_PG_RESET_OFFSET_PKTS, 
   TR_MMU_PG_HDRM_LIMIT_CELLS, TR_MMU_HG_PG_HDRM_LIMIT_PKTS, 15}, 
  {1, TR_MMU_IN_PORT_MIN_CELLS, TR_MMU_IN_PORT_MIN_PKTS, 0, 
   TR_MMU_HG_PG_RESET_OFFSET_CELLS, TR_MMU_HG_PG_RESET_OFFSET_PKTS, 
   TR_MMU_PG_HDRM_LIMIT_CELLS, TR_MMU_HG_PG_HDRM_LIMIT_PKTS, 15}, 
  {2, 0,0,0,0,0,0,0,0}, 
  {3, 0,0,0,0,0,0,0,0}, 
  {4, 0,0,0,0,0,0,0,0}, 
  {5, 0,0,0,0,0,0,0,0}, 
  {6, 0,0,0,0,0,0,0,0}, 
  {7, TR_MMU_IN_PORT_MIN_CELLS, TR_MMU_IN_PORT_MIN_PKTS, 0, 
   TR_MMU_HG_PG_RESET_OFFSET_CELLS, TR_MMU_HG_PG_RESET_OFFSET_PKTS, 
   TR_MMU_PG_HDRM_LIMIT_CELLS, TR_MMU_HG_PG_HDRM_LIMIT_PKTS, 15} /* Last */ 
};

/* 
 *  Priority Group settings
 */
static pg_vals_t en_pg_vals[TR_MMU_NUM_PG] = {
/* pg,min_cell,min_pkt,floor_cell,reset_offset_cell,reset_offset_pkt,hdrm_cell,hdrm_pkt,thresh */
  {0, 0,0,0,0,0,0,0,0},
  {1, 0,0,0,0,0,0,0,0},
  {2, 0,0,0,0,0,0,0,0}, 
  {3, 0,0,0,0,0,0,0,0}, 
  {4, 0,0,0,0,0,0,0,0}, 
  {5, 0,0,0,0,0,0,0,0}, 
  {6, 0,0,0,0,0,0,0,0}, 
  {7, EN_MMU_IN_PORT_MIN_CELLS, EN_MMU_IN_PORT_MIN_PKTS, 0, 
   EN_MMU_HG_PG_RESET_OFFSET_CELLS, EN_MMU_HG_PG_RESET_OFFSET_PKTS, 
   EN_MMU_PG_HDRM_LIMIT_CELLS, EN_MMU_HG_PG_HDRM_LIMIT_PKTS, 15} /* Last */ 
};


/**********************************************************
 * @purpose  Modify MMU for Priority Group Usage on Triumph
 *
 * @param    unit     - bcm unit number
 * @param    cosmap   - the cosmap to priority group mapping
 * @param    num_cos  - the number of elements in the cosmap
 * @param    pg_vals  - the priority group settings
 * @param    num_pg_vals - the number of elements in the pg_vals
 *
 * @returns  
 *
 * @comments Resources reserved for the Priority Group must be
 *           accounted for in the shared pools
 *
 * @end
 *
 **********************************************************/
int 
hapiBroadPgInit(int unit, pg_cosmap_t *cosmap,int num_cos, 
                pg_vals_t *pg_vals,int num_pg_vals)
{
  int port;
  int rval0,rval1;
  int i;

  /* assume caller doesn't want to modify the cosmap or pgs if NULL passed in */
  if (!cosmap || !pg_vals) return BCM_E_NONE;

  PBMP_HG_ITER(unit,port) {
    SOC_IF_ERROR_RETURN(READ_PORT_PRI_GRP0r(unit, port, &rval0));
    SOC_IF_ERROR_RETURN(READ_PORT_PRI_GRP1r(unit, port, &rval1));

    /* set the fields according to the cosmap defined above */
    for (i =0;i < num_cos;i++)
    {
      switch(cosmap[i].cos){
      case 0:
        soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval0, PRI0_GRPf, cosmap[i].pg);
        break;
      case 1:
        soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval0, PRI1_GRPf, cosmap[i].pg);
        break;
      case 2:
        soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval0, PRI2_GRPf, cosmap[i].pg);
        break;
      case 3:
        soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval0, PRI3_GRPf, cosmap[i].pg);
        break;
      case 4:
        soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval0, PRI4_GRPf, cosmap[i].pg);
        break;
      case 5:
        soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval0, PRI5_GRPf, cosmap[i].pg);
        break;
      case 6:
        soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval0, PRI6_GRPf, cosmap[i].pg);
        break;
      case 7:
        /* PTin updated: platform */
        if (SOC_IS_APOLLO(unit) || SOC_IS_TRIUMPH2(unit) || SOC_IS_VALKYRIE2(unit) || SOC_IS_TRIDENT(unit) ||
            SOC_IS_TRIUMPH3(unit))        /* PTin added: new switch BCM56643 */
        {
            soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval1, PRI7_GRPf, cosmap[i].pg);
        } else {
            soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval1, PRI7_GRPf, cosmap[i].pg);
        }
        break;
      case 8:
        soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval1, PRI8_GRPf, cosmap[i].pg);
        break;
      case 9:
        soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval1, PRI9_GRPf, cosmap[i].pg);
        break;
      case 10:
        soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval1, PRI10_GRPf, cosmap[i].pg);
        break;
      case 11:
        soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval1, PRI11_GRPf, cosmap[i].pg);
        break;
      case 12:
        soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval1, PRI12_GRPf, cosmap[i].pg);
        break;
      case 13:
        soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval1, PRI13_GRPf, cosmap[i].pg);
        break;
      case 14:
        soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval1, PRI14_GRPf, cosmap[i].pg);
        break;
      case 15:
        soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval1, PRI15_GRPf, cosmap[i].pg);
        break;
      default:
        break;
      }
    }

    SOC_IF_ERROR_RETURN(WRITE_PORT_PRI_GRP0r(unit, port, rval0));
    SOC_IF_ERROR_RETURN(WRITE_PORT_PRI_GRP1r(unit, port, rval1));

    for(i=0;i < num_pg_vals;i++)
    {
      int field = 0;
      int rval;
      int pg;

      pg = pg_vals[i].pg;

      rval = 0;
      SOC_IF_ERROR_RETURN(READ_PG_MIN_CELLr(unit, port, pg, &rval));
      soc_reg_field_set(unit,PG_MIN_CELLr, &rval, PG_MINf, pg_vals[i].min_cell);
      SOC_IF_ERROR_RETURN(WRITE_PG_MIN_CELLr(unit, port, pg, rval));

      rval = 0;
      SOC_IF_ERROR_RETURN(READ_PG_MIN_PACKETr(unit, port, pg, &rval));
      soc_reg_field_set(unit,PG_MIN_PACKETr, &rval, PG_MINf, pg_vals[i].min_pkt);
      SOC_IF_ERROR_RETURN(WRITE_PG_MIN_PACKETr(unit, port, pg, rval));
      
      rval = 0;
      SOC_IF_ERROR_RETURN(READ_PG_HDRM_LIMIT_CELLr(unit, port, pg, &rval));
      soc_reg_field_set(unit,PG_HDRM_LIMIT_CELLr, &rval, PG_GEf, 0);
      soc_reg_field_set(unit,PG_HDRM_LIMIT_CELLr, &rval, PG_HDRM_LIMITf, pg_vals[i].hdrm_cell);
      SOC_IF_ERROR_RETURN(WRITE_PG_HDRM_LIMIT_CELLr(unit, port, pg, rval));

      rval = 0;
      SOC_IF_ERROR_RETURN(READ_PG_HDRM_LIMIT_PACKETr(unit, port, pg, &rval));
      soc_reg_field_set(unit,PG_HDRM_LIMIT_PACKETr, &rval, PG_HDRM_LIMITf, pg_vals[i].hdrm_pkt);
      SOC_IF_ERROR_RETURN(WRITE_PG_HDRM_LIMIT_PACKETr(unit, port, pg, rval));

      rval = 0;
      SOC_IF_ERROR_RETURN(READ_PG_RESET_OFFSET_CELLr(unit, port, pg, &rval));
      soc_reg_field_set(unit,PG_RESET_OFFSET_CELLr, &rval, PG_RESET_OFFSETf, pg_vals[i].reset_offset_cell);
      SOC_IF_ERROR_RETURN(WRITE_PG_RESET_OFFSET_CELLr(unit, port, pg, rval));

      rval = 0;
      SOC_IF_ERROR_RETURN(READ_PG_RESET_OFFSET_PACKETr(unit, port, pg, &rval));
      soc_reg_field_set(unit,PG_RESET_OFFSET_PACKETr, &rval, PG_RESET_OFFSETf, pg_vals[i].reset_offset_pkt);
      SOC_IF_ERROR_RETURN(WRITE_PG_RESET_OFFSET_PACKETr(unit, port, pg, rval));
       
      rval = 0;
      SOC_IF_ERROR_RETURN(READ_PG_RESET_FLOOR_CELLr(unit, port, pg, &rval));
      soc_reg_field_set(unit,PG_RESET_FLOOR_CELLr, &rval, PG_RESET_FLOORf, pg_vals[i].floor_cell);
      SOC_IF_ERROR_RETURN(WRITE_PG_RESET_FLOOR_CELLr(unit, port, pg, rval));

      rval = 0;
      SOC_IF_ERROR_RETURN(READ_PG_THRESH_SELr(unit, port, &rval));
      switch (pg_vals[i].pg)
      {
        case 0:
          field = PG0_THRESH_SELf;
          break;
        case 1:
          field = PG1_THRESH_SELf;
          break;
        case 2:
          field = PG2_THRESH_SELf;
          break;
        case 3:
          field = PG3_THRESH_SELf;
          break;
        case 4:
          field = PG4_THRESH_SELf;
          break;
        case 5:
          field = PG5_THRESH_SELf;
          break;
        case 6:
          field = PG6_THRESH_SELf;
          break;
        case 7:
          field = -1;
          break;
        default:
          field = -1;
          break;
      }

      if (field > 0)
      {
        soc_reg_field_set(unit,PG_THRESH_SELr, &rval, field, pg_vals[i].thresh );
        SOC_IF_ERROR_RETURN(WRITE_PG_THRESH_SELr(unit, port, rval));
      }
    }
  }

  return BCM_E_NONE;
}


/* 
 * Enable or disable transmission of pause frames and honoring received
 * pause frames on a port.
 */
extern int bcm_port_pause_set(
    int unit, 
    int port, 
    int pause_tx, 
    int pause_rx);

/**********************************************************
 * @purpose  Modify MMU for SAFC on Triumph
 *
 * @param    unit - bcm unit number
 *
 * @returns  
 *
 * @comments Modifies the Priority groups to enable/disable pause
 *           All HG ports are assumed to be using SAFC
 *           Assuming a one-to-one mapping for the pg/cos
 *
 * @end
 *
 **********************************************************/
int 
hapiBroadSafcSet(int unit,int enable)
{
  int port;
  int rval;
  int prio;

  uint64 rval64;             
  enable = (enable)?1:0;
 
  /* Configure the HG ports for SAFC */
  PBMP_HG_ITER(unit,port) {
    rval = 0;
    SOC_IF_ERROR_RETURN(READ_XPORT_CONFIGr(unit, port, &rval));
    soc_reg_field_set(unit, XPORT_CONFIGr, &rval, LLFC_ENf, enable);
    SOC_IF_ERROR_RETURN(WRITE_XPORT_CONFIGr(unit, port, rval));

    COMPILER_64_ZERO(rval64);
    SOC_IF_ERROR_RETURN(READ_MAC_TXLLFCCTRLr(unit, port, &rval64));
    soc_reg64_field_set(unit,MAC_TXLLFCCTRLr,&rval64,LLFC_IMGf,0x18);
    soc_reg64_field_set(unit,MAC_TXLLFCCTRLr,&rval64,LLFC_CUT_THROUGH_MODEf,0);
    soc_reg64_field_set(unit,MAC_TXLLFCCTRLr,&rval64,LLFC_IN_IPG_ONLYf,0);
    soc_reg64_field_set(unit,MAC_TXLLFCCTRLr,&rval64,LLFC_ENf,enable);
    SOC_IF_ERROR_RETURN(WRITE_MAC_TXLLFCCTRLr(unit, port, rval64));

    COMPILER_64_ZERO(rval64);
    SOC_IF_ERROR_RETURN(READ_MAC_RXLLFCMSGFLDSr(unit, port, &rval64));
    soc_reg64_field_set(unit,MAC_RXLLFCMSGFLDSr,&rval64,LLFC_ENf,enable);
    SOC_IF_ERROR_RETURN(WRITE_MAC_RXLLFCMSGFLDSr(unit, port, rval64));
    
    /* Enable Pause on both the Rx and Tx of the HG ports */
    bcm_port_pause_set(unit,port,enable,enable);

    /* 
     * Only enable Priority based Pause on cos0-6
     */
    rval = 0;
    soc_reg_field_set(unit, PORT_PRI_XON_ENABLEr, &rval, 
                      PORT_PRI_XON_ENABLEf, (enable)?TR_COS_PRI_XON_ENABLE:0);
    SOC_IF_ERROR_RETURN(WRITE_PORT_PRI_XON_ENABLEr(unit, port, rval));
  }

  /* 
   * Using a 1-to-1 mapping for prio to pause cos
   */
  for (prio=0;prio < TR_MMU_NUM_PG;prio++)
  {
    SOC_IF_ERROR_RETURN(READ_PRIO2COS_LLFCr(unit, prio, &rval));

    if (enable)
      rval |= (1 << prio);
    else
      rval &= ~(1 << prio);

    SOC_IF_ERROR_RETURN(WRITE_PRIO2COS_LLFCr(unit, prio, rval));
  }

  return SOC_E_NONE;
}

#endif /* SAFC_DISABLED */

/**********************************************************
 * @purpose  Calculate common variables for Triumph MMU functions
 *
 * @param    unit - bcm unit number
 *
 * @returns  
 *
 * @comments 
 *
 * @end
 *
 **********************************************************/
void hapiBroadMmuCellLimits(int unit, int *total_cells, int *total_packets, 
                            int *in_reserved_cells, int *in_reserved_packets, 
                            int *out_reserved_cells, int *out_reserved_packets)
{
    int port, sum_all_ports, sum_all_nonhg_ports, i;

    /* Total number of cells */
    if (SOC_IS_VALKYRIE(unit) || SOC_IS_APOLLO(unit) || SOC_IS_VALKYRIE2(unit)) {
        *total_cells = 24 * 1024; /* 24K cells */
    }else if SOC_IS_TRIDENT(unit) { /* PTin added: new switch BCM56843 */
       *total_cells = 45 * 1024; /* 45K cells */
    }else{ 
       *total_cells = 32 * 1024; /* 32K cells */
    }

     /* Total number of packet pointers */
     *total_packets = 11 * 1024; /* 11K packet pointers */
    
    /*
     * Reserved space calculation:
     *   Input port:
     *     per-port minimum
     *     per-PG minimum (config to 0)
     *     per-PG headroom
     *     per-device headroom
     *     per-port minimum for SC and QM traffic (config to 0)
     *   Output port:
     *     per-port per-COS minimum space
     * Shared space calculation:
     *   Input port: total - input port reserved - output port reserved
     *   Output port: total - output port reserved
     */

    if (soc_feature(unit, soc_feature_internal_loopback)) {
        if (soc_feature(unit, soc_feature_wlan)) {
            sum_all_ports = NUM_ALL_PORT(unit) + 2; /* including 55 and 56 */ 
        } else {
            sum_all_ports = NUM_ALL_PORT(unit) + 1; /* including 56 only */ 
        } 
    } else{
        sum_all_ports = NUM_ALL_PORT(unit); /* no loopback */
    }
    sum_all_nonhg_ports = sum_all_ports;
    PBMP_HG_ITER(unit,port) {
      sum_all_nonhg_ports--;
    }
    *in_reserved_cells = sum_all_nonhg_ports * TR_MMU_IN_PORT_MIN_CELLS +
        (sum_all_nonhg_ports - 1) * TR_MMU_PG_HDRM_LIMIT_CELLS +
        TR_MMU_GLOBAL_HDRM_LIMIT_CELLS;

    *in_reserved_packets = sum_all_nonhg_ports * TR_MMU_IN_PORT_MIN_PKTS +
        (sum_all_nonhg_ports - 1) * TR_MMU_PG_HDRM_LIMIT_PKTS;

    *out_reserved_cells = 
        (((sum_all_ports - 1) * TR_MMU_OUT_PORT_MIN_CELLS) +
        (1 * TR_MMU_OUT_QUEUE_CELL_CPU_MIN_CELL)) * 
        TR_MMU_NUM_COS;

    *out_reserved_packets =
        (((sum_all_ports - 1) * TR_MMU_OUT_PORT_MIN_PKTS) +
        (1 * TR_MMU_OUT_QUEUE_PKT_CPU_MIN_PKT)) * 
        TR_MMU_NUM_COS;
#ifndef SAFC_DISABLED
    PBMP_HG_ITER(unit,port) {
      for(i=0;i<(sizeof(pg_vals)/sizeof(pg_vals_t));i++) {
	*in_reserved_cells += pg_vals[i].min_cell + pg_vals[i].hdrm_cell;
	*in_reserved_packets += pg_vals[i].min_pkt + pg_vals[i].hdrm_pkt;
      }
    }
#endif
}

/**********************************************************
 * @purpose  Calculate common variables for Enduro MMU functions
 *
 * @param    unit - bcm unit number
 *
 * @returns  
 *
 * @comments 
 *
 * @end
 *
 **********************************************************/
void hapiBroadEnduroMmuCellLimits(int unit, int *total_cells, int *total_packets, 
                            int *in_reserved_cells, int *in_reserved_packets, 
                            int *out_reserved_cells, int *out_reserved_packets)
{
    int port, sum_all_ports, sum_all_nonhg_ports, i;

    /* Total number of packet pointers */
    *total_packets = 6 * 1024; /* 6K packet pointers */
    *total_cells = 16 * 1024;  /* 16K cells */
    /*
     * Reserved space calculation:
     *   Input port:
     *     per-port minimum
     *     per-PG minimum (config to 0)
     *     per-PG headroom
     *     per-device headroom
     *     per-port minimum for SC and QM traffic (config to 0)
     *   Output port:
     *     per-port per-COS minimum space
     * Shared space calculation:
     *   Input port: total - input port reserved - output port reserved
     *   Output port: total - output port reserved
     */

   sum_all_ports = NUM_PORT(unit);
    sum_all_nonhg_ports = sum_all_ports;
    PBMP_HG_ITER(unit,port) {
      sum_all_nonhg_ports--;
    }
    *in_reserved_cells = sum_all_nonhg_ports * EN_MMU_IN_PORT_MIN_CELLS +
        (sum_all_nonhg_ports) * EN_MMU_PG_HDRM_LIMIT_CELLS +
        EN_MMU_GLOBAL_HDRM_LIMIT_CELLS;

    *in_reserved_packets = sum_all_nonhg_ports * EN_MMU_IN_PORT_MIN_PKTS +
        (sum_all_nonhg_ports) * EN_MMU_PG_HDRM_LIMIT_PKTS;

    *out_reserved_cells = 
        ((sum_all_ports * EN_MMU_OUT_PORT_MIN_CELLS) +
        (1 * EN_MMU_OUT_QUEUE_CELL_CPU_MIN_CELL)) * 
        TR_MMU_NUM_COS;

    *out_reserved_packets =
        ((sum_all_ports * EN_MMU_OUT_PORT_MIN_PKTS) +
        (1 * EN_MMU_OUT_QUEUE_PKT_CPU_MIN_PKT)) * 
        TR_MMU_NUM_COS;
        
#ifndef SAFC_DISABLED
    PBMP_HG_ITER(unit,port) {
      for(i=0;i<(sizeof(en_pg_vals)/sizeof(pg_vals_t));i++) {
  *in_reserved_cells += en_pg_vals[i].min_cell + en_pg_vals[i].hdrm_cell;
  *in_reserved_packets += en_pg_vals[i].min_pkt + en_pg_vals[i].hdrm_pkt;
      }
    }
#endif
}

/**********************************************************
 * @purpose  Modify MMU Enduro ingress/egress settings for pause
 *
 * @param    unit - bcm unit number
 *
 * @returns  
 *
 * @comments 
 *
 * @end
 *
 **********************************************************/
int hapiBroadMmuEnduroPauseSet(int unit, int pause)
{
    uint32              cell_rval, pkt_rval;
    int                 port,idx;
    int                 total_cells, total_pkts;
    int                 in_reserved_cells, in_reserved_pkts;
    int                 out_reserved_cells, out_reserved_pkts;
    int                 cpu_port;
    int                 port_cell_limit, port_pkt_limit;
    int                 port_cell_dynamic, port_pkt_dynamic;

    /* used for cpu, doesn't need to be exact with regard to loopback/wlan */
    int               num_ports = NUM_ALL_PORT(unit);

    cpu_port = CMIC_PORT(unit);

    hapiBroadEnduroMmuCellLimits(unit, &total_cells, &total_pkts, &in_reserved_cells, 
                           &in_reserved_pkts, &out_reserved_cells, &out_reserved_pkts);

    /* We need headroom if pause is on. If pause is off, shared space has no limit */
    if (pause == L7_TRUE) {
        port_cell_limit = EN_MMU_PORT_SHARED_LIMIT_CELL;
        port_pkt_limit = EN_MMU_PORT_SHARED_LIMIT_PKT;
        port_cell_dynamic = EN_MMU_PORT_SHARED_DYNAMIC_CELL;
        port_pkt_dynamic = EN_MMU_PORT_SHARED_DYNAMIC_PKT;
    } else {
        in_reserved_pkts = in_reserved_cells = 1;
        out_reserved_pkts = out_reserved_cells = 0;
        port_cell_limit = total_cells - 1;
        port_pkt_limit = total_pkts - 1;
        port_cell_dynamic = port_pkt_dynamic = 0;
    }

    /* Input port per-port shared space limit - dynamic when pause is on, 
        unlimited when pause is off */
   
    cell_rval = 0;
    pkt_rval = 0;
    soc_reg_field_set(unit, TOTAL_SHARED_LIMIT_CELLr, &cell_rval,
                      TOTAL_SHARED_LIMITf,
                      total_cells - in_reserved_cells - out_reserved_cells - 
                      num_ports);
                    
    SOC_IF_ERROR_RETURN(WRITE_TOTAL_SHARED_LIMIT_CELLr(unit, cell_rval));
    
    pkt_rval = 0;
    soc_reg_field_set(unit, TOTAL_SHARED_LIMIT_PACKETr, &pkt_rval,
                      TOTAL_SHARED_LIMITf,
                      total_pkts - in_reserved_pkts - out_reserved_pkts - 
                      num_ports);
                 
    SOC_IF_ERROR_RETURN(WRITE_TOTAL_SHARED_LIMIT_PACKETr(unit, pkt_rval));

    
    /* Input port per-port shared space limit - no limit */
    cell_rval = 0;
    soc_reg_field_set(unit, PORT_SHARED_LIMIT_CELLr, &cell_rval,
                      PORT_SHARED_LIMITf, port_cell_limit);
    soc_reg_field_set(unit, PORT_SHARED_LIMIT_CELLr, &cell_rval,
                      PORT_SHARED_DYNAMICf, port_cell_dynamic);                    
    pkt_rval = 0;
    soc_reg_field_set(unit, PORT_SHARED_LIMIT_PACKETr, &pkt_rval,
                      PORT_SHARED_LIMITf, port_pkt_limit);
    soc_reg_field_set(unit, PORT_SHARED_LIMIT_PACKETr, &pkt_rval,
                      PORT_SHARED_DYNAMICf, port_pkt_dynamic);                   
    PBMP_ALL_ITER(unit, port) {
        if (port == cpu_port) continue;  
        SOC_IF_ERROR_RETURN(WRITE_PORT_SHARED_LIMIT_CELLr(unit, port,
                                                          cell_rval));
        SOC_IF_ERROR_RETURN(WRITE_PORT_SHARED_LIMIT_PACKETr(unit, port,
                                                            pkt_rval));
    }

    /*
     * Output ports threshold
     */
    /* Reserved space: Output port per-port per-COS minimum space */
    cell_rval = 0;
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_CELLr, &cell_rval, Q_MIN_CELLf,
                      EN_MMU_OUT_PORT_MIN_CELLS);
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_CELLr, &cell_rval,
                      Q_LIMIT_ENABLE_CELLf, !pause);
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_CELLr, &cell_rval,
                      Q_LIMIT_DYNAMIC_CELLf, 0x1);
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_CELLr, &cell_rval,
                      Q_SHARED_LIMIT_CELLf, EN_MMU_OUT_QUEUE_CELL_ALPHA_IDX);

    pkt_rval = 0;
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_PACKETr, &pkt_rval, Q_MIN_PACKETf,
                      EN_MMU_OUT_PORT_MIN_PKTS);
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_PACKETr, &pkt_rval,
                      Q_LIMIT_ENABLE_PACKETf, !pause);

    soc_reg_field_set(unit, OP_QUEUE_CONFIG_PACKETr, &pkt_rval,
                      Q_LIMIT_DYNAMIC_PACKETf, 0x1);
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_PACKETr, &pkt_rval,
                      Q_SHARED_LIMIT_PACKETf, EN_MMU_OUT_QUEUE_PKT_ALPHA_IDX);

    PBMP_ALL_ITER(unit, port) {
        if (port == cpu_port) continue;
        for (idx = 0; idx < TR_MMU_NUM_COS; idx++) {
            SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG_CELLr(unit, port, idx,
                                                            cell_rval));
            SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG_PACKETr(unit, port, idx,
                                                              pkt_rval));
        }
    }

#ifndef SAFC_DISABLED
    /* set the SAFC */
    hapiBroadSafcSet( unit, pause);
#endif

    return SOC_E_NONE;
}

/**********************************************************
 * @purpose  Modify MMU Triumph ingress/egress settings for pause
 *
 * @param    unit - bcm unit number
 *
 * @returns  
 *
 * @comments 
 *
 * @end
 *
 **********************************************************/
int hapiBroadMmuTriumphPauseSet(int unit, int pause)
{
    uint32              cell_rval, pkt_rval;
    int                 port,idx;
    int                 total_cells, total_pkts;
    int                 in_reserved_cells, in_reserved_pkts;
    int                 out_reserved_cells, out_reserved_pkts;
    int                 cpu_port;
    int                 port_cell_limit, port_pkt_limit;
    int                 port_cell_dynamic, port_pkt_dynamic;

    /* used for cpu, doesn't need to be exact with regard to loopback/wlan */
    int                 num_ports = NUM_ALL_PORT(unit);

    cpu_port = CMIC_PORT(unit);

    hapiBroadMmuCellLimits(unit, &total_cells, &total_pkts, &in_reserved_cells, 
                           &in_reserved_pkts, &out_reserved_cells, &out_reserved_pkts);

    /* We need headroom if pause is on. If pause is off, shared space has no limit */
    if (pause == L7_TRUE) {
        port_cell_limit = TR_MMU_PORT_SHARED_LIMIT_CELL;
        port_pkt_limit = TR_MMU_PORT_SHARED_LIMIT_PKT;
        port_cell_dynamic = TR_MMU_PORT_SHARED_DYNAMIC_CELL;
        port_pkt_dynamic = TR_MMU_PORT_SHARED_DYNAMIC_PKT;
    } else {
        in_reserved_pkts = in_reserved_cells = 1;
        out_reserved_pkts = out_reserved_cells = 0;
        port_cell_limit = total_cells - 1;
        port_pkt_limit = total_pkts - 1;
        port_cell_dynamic = port_pkt_dynamic = 0;
    }

    /* Input port shared space */
    cell_rval = 0;
    pkt_rval = 0;
    /* PTin updated: platform */
    if (SOC_IS_APOLLO(unit) || SOC_IS_TRIUMPH2(unit) || SOC_IS_VALKYRIE2(unit) || SOC_IS_TRIDENT(unit) ||
        SOC_IS_TRIUMPH3(unit))        /* PTin added: new switch BCM56643 */
    { /* Everything in service pool 0 */
        soc_reg_field_set(unit, BUFFER_CELL_LIMIT_SPr, &cell_rval, LIMITf,
                          total_cells - in_reserved_cells - out_reserved_cells - num_ports);
        SOC_IF_ERROR_RETURN(WRITE_BUFFER_CELL_LIMIT_SPr(unit, 0, cell_rval));
        soc_reg_field_set(unit, BUFFER_PACKET_LIMIT_SPr, &pkt_rval, LIMITf,
                          total_pkts - in_reserved_pkts - out_reserved_pkts - num_ports);
        SOC_IF_ERROR_RETURN(WRITE_BUFFER_PACKET_LIMIT_SPr(unit, 0, pkt_rval));
    } 
    else 
    { /* Not Triumph2 */
        soc_reg_field_set(unit, TOTAL_SHARED_LIMIT_CELLr, &cell_rval,
                          TOTAL_SHARED_LIMITf,
                          total_cells - in_reserved_cells - out_reserved_cells - num_ports);
        SOC_IF_ERROR_RETURN(WRITE_TOTAL_SHARED_LIMIT_CELLr(unit, cell_rval));
        soc_reg_field_set(unit, TOTAL_SHARED_LIMIT_PACKETr, &pkt_rval,
                          TOTAL_SHARED_LIMITf,
                          total_pkts - in_reserved_pkts - out_reserved_pkts - num_ports);
        SOC_IF_ERROR_RETURN(WRITE_TOTAL_SHARED_LIMIT_PACKETr(unit, pkt_rval));
    }

    /* Input port per-port shared space limit - dynamic when pause is on, 
        unlimited when pause is off */
    cell_rval = 0;

    soc_reg_field_set(unit, PORT_SHARED_LIMIT_CELLr, &cell_rval,
                      PORT_SHARED_LIMITf, port_cell_limit);
    soc_reg_field_set(unit, PORT_SHARED_LIMIT_CELLr, &cell_rval,
                      PORT_SHARED_DYNAMICf, port_cell_dynamic);

    pkt_rval = 0;
    soc_reg_field_set(unit, PORT_SHARED_LIMIT_PACKETr, &pkt_rval,
                      PORT_SHARED_LIMITf, port_pkt_limit);
    soc_reg_field_set(unit, PORT_SHARED_LIMIT_PACKETr, &pkt_rval,
                      PORT_SHARED_DYNAMICf, port_pkt_dynamic);

    PBMP_ALL_ITER(unit, port) {
        if (port==cpu_port) {
            continue;
        }
        SOC_IF_ERROR_RETURN(WRITE_PORT_SHARED_LIMIT_CELLr(unit, port,
                                                          cell_rval));
        SOC_IF_ERROR_RETURN(WRITE_PORT_SHARED_LIMIT_PACKETr(unit, port,
                                                            pkt_rval));
    }
    if (soc_feature(unit, soc_feature_internal_loopback)) {
        /* EP redirection */
        SOC_IF_ERROR_RETURN(WRITE_PORT_SHARED_LIMIT_CELLr(unit, 56,
                                                          cell_rval));
        SOC_IF_ERROR_RETURN(WRITE_PORT_SHARED_LIMIT_PACKETr(unit, 56,
                                                            pkt_rval));
        /* WLAN */
        if (soc_feature(unit, soc_feature_wlan)) {
            SOC_IF_ERROR_RETURN(WRITE_PORT_SHARED_LIMIT_CELLr(unit, 55,
                                                              cell_rval));
            SOC_IF_ERROR_RETURN(WRITE_PORT_SHARED_LIMIT_PACKETr(unit, 55,
                                                                pkt_rval));
        }
    }

    /*
     * Output ports threshold
     */
    /* Reserved space: Output port per-port per-COS minimum space */
    cell_rval = 0;
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_CELLr, &cell_rval, Q_MIN_CELLf,
                      TR_MMU_OUT_PORT_MIN_CELLS);
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_CELLr, &cell_rval,
                      Q_LIMIT_ENABLE_CELLf, !pause);
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_CELLr, &cell_rval,
                      Q_LIMIT_DYNAMIC_CELLf, 0x1);
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_CELLr, &cell_rval,
                      Q_SHARED_LIMIT_CELLf, TR_MMU_OUT_QUEUE_CELL_ALPHA_IDX);

    pkt_rval = 0;
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_PACKETr, &pkt_rval, Q_MIN_PACKETf,
                      TR_MMU_OUT_PORT_MIN_PKTS);
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_PACKETr, &pkt_rval,
                      Q_LIMIT_ENABLE_PACKETf, !pause);

    soc_reg_field_set(unit, OP_QUEUE_CONFIG_PACKETr, &pkt_rval,
                      Q_LIMIT_DYNAMIC_PACKETf, 0x1);
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_PACKETr, &pkt_rval,
                      Q_SHARED_LIMIT_PACKETf, TR_MMU_OUT_QUEUE_PKT_ALPHA_IDX);

    PBMP_ALL_ITER(unit, port) {
        if (port == cpu_port) continue;
        for (idx = 0; idx < TR_MMU_NUM_COS; idx++) {
            SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG_CELLr(unit, port, idx,
                                                            cell_rval));
            SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG_PACKETr(unit, port, idx,
                                                              pkt_rval));
        }
    }

#ifndef SAFC_DISABLED
    /* set the SAFC */
    hapiBroadSafcSet( unit, pause);
#endif

    return SOC_E_NONE;
}

/*********************************************************************
* @purpose  Modify MMU for Triumph/Valkyrie devices
*
* @param    unit - bcm unit number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Roughly taken from soc_triumph_mmu_init(int unit)
*           Mods for values, cpu queues, and safc
*
* @end
*********************************************************************/
L7_RC_t hapiBroadTrVlMmuModify(L7_uint32 unit)
{
    uint64              rval64;
    uint32              rval, rval0, rval1, cell_rval, pkt_rval;
    uint32              cell_rval_cpu,pkt_rval_cpu,field_max;
    int                 port;
    int                 total_cells, total_pkts;
    int                 in_reserved_cells, in_reserved_pkts;
    int                 out_reserved_cells, out_reserved_pkts;
    int                 out_shared_cells, out_shared_pkts;
    int                 idx;
    int                 triumph2_family;
    soc_pbmp_t          pbmp_8pg, pbmp_2pg, temp;

    int cpu_port = CMIC_PORT(unit);

    /* used for cpu, doesn't need to be exact with regard to loopback/wlan */
    int                 num_ports = NUM_ALL_PORT(unit);

    hapiBroadMmuCellLimits(unit, &total_cells, &total_pkts, &in_reserved_cells, 
                           &in_reserved_pkts, &out_reserved_cells, &out_reserved_pkts);
    /* PTin updated: platform */
    triumph2_family = SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || SOC_IS_VALKYRIE2(unit) ||
                      SOC_IS_TRIUMPH3(unit);        /* PTin added: new switch BCM56643 */

    /* Ports with 8PG (1PG for other ports) */
    SOC_PBMP_CLEAR(pbmp_2pg);
    SOC_PBMP_CLEAR(pbmp_8pg);
    if (triumph2_family) {
        /* 8PG_PORTS = [26..30,34,38,42,46,50,54] */
        SOC_PBMP_PORT_ADD(pbmp_8pg, 26);
        SOC_PBMP_PORT_ADD(pbmp_8pg, 27);
        SOC_PBMP_PORT_ADD(pbmp_8pg, 28);
        SOC_PBMP_PORT_ADD(pbmp_8pg, 29);
        SOC_PBMP_PORT_ADD(pbmp_8pg, 30); 
        SOC_PBMP_PORT_ADD(pbmp_8pg, 34);
        SOC_PBMP_PORT_ADD(pbmp_8pg, 38);
        SOC_PBMP_PORT_ADD(pbmp_8pg, 42);
        SOC_PBMP_PORT_ADD(pbmp_8pg, 46);
        SOC_PBMP_PORT_ADD(pbmp_8pg, 50);
        if (soc_feature(unit, soc_feature_internal_loopback)) {
            SOC_PBMP_PORT_ADD(pbmp_8pg, 54);
        }
        /* 2PG_PORTS = [0,1..25,31..33,35..37,39..41,43..45,47..49,51..53,55,56] */
        SOC_PBMP_ASSIGN(pbmp_2pg, PBMP_ALL(unit));
        SOC_PBMP_PORT_REMOVE(pbmp_2pg, 26);
        SOC_PBMP_PORT_REMOVE(pbmp_2pg, 27);
        SOC_PBMP_PORT_REMOVE(pbmp_2pg, 28);
        SOC_PBMP_PORT_REMOVE(pbmp_2pg, 29);
        SOC_PBMP_PORT_REMOVE(pbmp_2pg, 30); 
        SOC_PBMP_PORT_REMOVE(pbmp_2pg, 34);
        SOC_PBMP_PORT_REMOVE(pbmp_2pg, 38);
        SOC_PBMP_PORT_REMOVE(pbmp_2pg, 42);
        SOC_PBMP_PORT_REMOVE(pbmp_2pg, 46);
        SOC_PBMP_PORT_REMOVE(pbmp_2pg, 50);
        SOC_PBMP_PORT_REMOVE(pbmp_2pg, 54);
        if (soc_feature(unit, soc_feature_internal_loopback)) {
            if (soc_feature(unit, soc_feature_wlan)) {
                SOC_PBMP_PORT_ADD(pbmp_2pg, 55);
            }
            SOC_PBMP_PORT_ADD(pbmp_2pg, 56);
        }        
    } else { /* Not Triumph2 */
      SOC_PBMP_WORD_SET(pbmp_8pg, 0, 0xfc004004); /* 2, 14, 26-31 */
    }
    SOC_PBMP_AND(pbmp_8pg, PBMP_ALL(unit));

    /*
     * Input ports threshold
     */
    /* Reserved space: Input port per-port minimum */
    cell_rval = 0;
    soc_reg_field_set(unit, PORT_MIN_CELLr, &cell_rval, PORT_MINf,
                      TR_MMU_IN_PORT_MIN_CELLS);
    pkt_rval = 0;
    soc_reg_field_set(unit, PORT_MIN_PACKETr, &pkt_rval, PORT_MINf,
                      TR_MMU_IN_PORT_MIN_PKTS);


    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_PORT_MIN_CELLr(unit, port, cell_rval));
        SOC_IF_ERROR_RETURN(WRITE_PORT_MIN_PACKETr(unit, port, pkt_rval));
    }


    /* handle the cpu backpressure issue */
    /* set the cpu port_min to the max so no bp is allowed on cpu */
    cell_rval_cpu = 0;
    pkt_rval_cpu = 0;

    field_max = ( 1 << soc_reg_field_length(unit,PORT_MIN_CELLr,PORT_MINf)) - 1;
    soc_reg_field_set(unit, PORT_MIN_CELLr, &cell_rval_cpu, PORT_MINf,field_max);
    SOC_IF_ERROR_RETURN(WRITE_PORT_MIN_CELLr(unit, cpu_port, cell_rval_cpu));

    field_max = ( 1 << soc_reg_field_length(unit,PORT_MIN_PACKETr,PORT_MINf)) - 1;
    soc_reg_field_set(unit, PORT_MIN_PACKETr, &pkt_rval_cpu, PORT_MINf,field_max);
    SOC_IF_ERROR_RETURN(WRITE_PORT_MIN_PACKETr(unit, cpu_port, pkt_rval_cpu));

    /* static memory and the entire space for the cpu */
    cell_rval_cpu = 0;
    pkt_rval_cpu = 0;

    field_max = ( 1 << soc_reg_field_length(unit,PORT_SHARED_LIMIT_CELLr,PORT_SHARED_LIMITf)) - 1;
    soc_reg_field_set(unit, PORT_SHARED_LIMIT_CELLr, &cell_rval_cpu,PORT_SHARED_LIMITf, field_max);
    soc_reg_field_set(unit, PORT_SHARED_LIMIT_CELLr, &cell_rval_cpu, PORT_SHARED_DYNAMICf, 0);
    SOC_IF_ERROR_RETURN(WRITE_PORT_SHARED_LIMIT_CELLr(unit, cpu_port, cell_rval_cpu));

    field_max = ( 1 << soc_reg_field_length(unit,PORT_SHARED_LIMIT_PACKETr,PORT_SHARED_LIMITf)) - 1;
    soc_reg_field_set(unit, PORT_SHARED_LIMIT_PACKETr, &pkt_rval_cpu, PORT_SHARED_LIMITf, field_max);
    soc_reg_field_set(unit, PORT_SHARED_LIMIT_PACKETr, &pkt_rval_cpu, PORT_SHARED_DYNAMICf, 0);
    SOC_IF_ERROR_RETURN(WRITE_PORT_SHARED_LIMIT_PACKETr(unit, cpu_port, pkt_rval_cpu));

    /* Deal with loopback / wlan ports. */
    if (soc_feature(unit, soc_feature_internal_loopback)) {
        /* Redirect port */
        SOC_IF_ERROR_RETURN(WRITE_PORT_MIN_CELLr(unit, 56, cell_rval));
        SOC_IF_ERROR_RETURN(WRITE_PORT_MIN_PACKETr(unit, 56, pkt_rval));
        rval = 0;
        soc_reg_field_set(unit, RDE_PORT_SHARED_LIMIT_PACKETr, &rval, 
                          PORT_SHARED_LIMITf, 1024);
        SOC_IF_ERROR_RETURN(WRITE_RDE_PORT_SHARED_LIMIT_PACKETr(unit, rval));
        rval = 0;
        soc_reg_field_set(unit, PG_RDE_RESET_OFFSET_PACKETr, &rval, 
                          PG_RESET_OFFSETf, 4);
        SOC_IF_ERROR_RETURN(WRITE_PG_RDE_RESET_OFFSET_PACKETr(unit, 0, rval));
        SOC_IF_ERROR_RETURN(WRITE_PG_RDE_RESET_OFFSET_PACKETr(unit, 1, rval));
        rval = 0;
        soc_reg_field_set(unit, PG_RDE_THRESH_SEL2r, &rval, 
                          PG0_THRESH_SELf, 8);
        SOC_IF_ERROR_RETURN(WRITE_PG_RDE_THRESH_SEL2r(unit, rval));
        /* PG_RDE_MIN_PACKET = 0 : default */
        /* WLAN port */
        if (soc_feature(unit, soc_feature_wlan)) {
            SOC_IF_ERROR_RETURN(WRITE_PORT_MIN_CELLr(unit, 55, cell_rval));
            SOC_IF_ERROR_RETURN(WRITE_PORT_MIN_PACKETr(unit, 55, pkt_rval));
            rval = 0;
            soc_reg_field_set(unit, WL_PORT_SHARED_LIMIT_CELLr, &rval, 
                              PORT_SHARED_LIMITf, 8192);
            SOC_IF_ERROR_RETURN(WRITE_WL_PORT_SHARED_LIMIT_CELLr(unit, rval));
            rval = 0;
            soc_reg_field_set(unit, WL_PORT_SHARED_LIMIT_PACKETr, &rval, 
                              PORT_SHARED_LIMITf, 2048);
            SOC_IF_ERROR_RETURN(WRITE_WL_PORT_SHARED_LIMIT_PACKETr(unit, rval));
            rval = 0;
            soc_reg_field_set(unit, PG_WL_RESET_OFFSET_CELLr, &rval, 
                              PG_RESET_OFFSETf, 4);
            SOC_IF_ERROR_RETURN(WRITE_PG_WL_RESET_OFFSET_CELLr(unit, 0, rval));
            SOC_IF_ERROR_RETURN(WRITE_PG_WL_RESET_OFFSET_CELLr(unit, 1, rval));
            rval = 0;
            soc_reg_field_set(unit, PG_WL_RESET_OFFSET_PACKETr, &rval, 
                              PG_RESET_OFFSETf, 4);
            SOC_IF_ERROR_RETURN
                (WRITE_PG_WL_RESET_OFFSET_PACKETr(unit, 0, rval));
            SOC_IF_ERROR_RETURN
                (WRITE_PG_WL_RESET_OFFSET_PACKETr(unit, 1, rval));
            rval = 0;
            soc_reg_field_set(unit, WL_DROP_POLICYr, &rval, 
                              DROP_PG0_1ST_FRAGMENTf, 1);
            soc_reg_field_set(unit, WL_DROP_POLICYr, &rval, 
                              DROP_PG0_ANY_FRAGMENTf, 1);
            soc_reg_field_set(unit, WL_DROP_POLICYr, &rval, 
                              DROP_PG0_NON_FRAGMENTf, 1);
            SOC_IF_ERROR_RETURN(WRITE_WL_DROP_POLICYr(unit, rval));
        }
    }

    /* Reserved space: Input port per-PG minimum
     * Use defalt value 0
     * With only one PG in use PORT_MIN should be sufficient */

    /* PG_WL_RESET_FLOOR_CELL, PG_WL_MIN_CELL and PG_WL_MIN_PACKET 
     * are zeroes (default) */

    /* Reserved space: Input port per-PG headroom
     * Use only 1PG (highest priority PG for the port) */
    cell_rval = 0;
    soc_reg_field_set(unit, PG_HDRM_LIMIT_CELLr, &cell_rval, PG_HDRM_LIMITf,
                      TR_MMU_PG_HDRM_LIMIT_CELLS);
    soc_reg_field_set(unit, PG_HDRM_LIMIT_CELLr, &cell_rval, PG_GEf, 1);
    pkt_rval = 0;
    soc_reg_field_set(unit, PG_HDRM_LIMIT_PACKETr, &pkt_rval, PG_HDRM_LIMITf,
                      TR_MMU_PG_HDRM_LIMIT_PKTS);
    PBMP_PORT_ITER(unit, port) {
      if (SOC_PBMP_MEMBER(pbmp_8pg, port)) {
	idx = TR_MMU_NUM_PG - 1;
      } else if (SOC_PBMP_MEMBER(pbmp_2pg, port)) {
	idx = 1;
      } else {
	idx = 0;
      }
        SOC_IF_ERROR_RETURN(WRITE_PG_HDRM_LIMIT_CELLr(unit, port, idx,
                                                      cell_rval));
        SOC_IF_ERROR_RETURN(WRITE_PG_HDRM_LIMIT_PACKETr(unit, port, idx,
                                                        pkt_rval));
    }
    if (soc_feature(unit, soc_feature_internal_loopback)) {
        /* Loopback ingress */
        idx = TR_MMU_NUM_PG - 1;
        SOC_IF_ERROR_RETURN(WRITE_PG_HDRM_LIMIT_CELLr(unit, 54, idx,
                                                      cell_rval));
        SOC_IF_ERROR_RETURN(WRITE_PG_HDRM_LIMIT_PACKETr(unit, 54, idx,
                                                        pkt_rval));
        /* EP redirection */
        SOC_IF_ERROR_RETURN(WRITE_PG_HDRM_LIMIT_CELLr(unit, 56, 1,
                                                      cell_rval));
        SOC_IF_ERROR_RETURN(WRITE_PG_HDRM_LIMIT_PACKETr(unit, 56, 1,
                                                        pkt_rval));
        /* WLAN */
        if (soc_feature(unit, soc_feature_wlan)) {
            SOC_IF_ERROR_RETURN(WRITE_PG_HDRM_LIMIT_CELLr(unit, 55, 1,
                                                          cell_rval));
            SOC_IF_ERROR_RETURN(WRITE_PG_HDRM_LIMIT_PACKETr(unit, 55, 1,
                                                            pkt_rval));
        }
    }

    /* Reserved space: Input port per-device headroom */
    cell_rval = 0;
    soc_reg_field_set(unit, GLOBAL_HDRM_LIMITr, &cell_rval, GLOBAL_HDRM_LIMITf,
                      TR_MMU_GLOBAL_HDRM_LIMIT_CELLS);
    SOC_IF_ERROR_RETURN(WRITE_GLOBAL_HDRM_LIMITr(unit, cell_rval));

    /* Global and port shared limits set in hapiBroadMmuTriumphPauseSet() */

    /* Input port per-PG reset offset
     * Use only 1PG (highest priority PG for the port)
     * Use default value 0 for CPU */
    cell_rval = 0;
    soc_reg_field_set(unit, PG_RESET_OFFSET_CELLr, &cell_rval,
                      PG_RESET_OFFSETf, TR_MMU_PG_RESET_OFFSET_CELLS);
    pkt_rval = 0;
    soc_reg_field_set(unit, PG_RESET_OFFSET_PACKETr, &pkt_rval,
                      PG_RESET_OFFSETf, TR_MMU_PG_RESET_OFFSET_PKTS);
    PBMP_PORT_ITER(unit, port) {
        idx = SOC_PBMP_MEMBER(pbmp_8pg, port) ? TR_MMU_NUM_PG - 1 : 0;
        if ((idx == 0) && triumph2_family) {
            idx = 1;
        }
        SOC_IF_ERROR_RETURN(WRITE_PG_RESET_OFFSET_CELLr(unit, port, idx,
                                                        cell_rval));
        SOC_IF_ERROR_RETURN(WRITE_PG_RESET_OFFSET_PACKETr(unit, port, idx,
                                                          pkt_rval));
    }
    if (soc_feature(unit, soc_feature_internal_loopback)) {
        /* Loopback ingress */
        idx = TR_MMU_NUM_PG - 1;
        SOC_IF_ERROR_RETURN(WRITE_PG_RESET_OFFSET_CELLr(unit, 54, idx,
                                                        cell_rval));
        SOC_IF_ERROR_RETURN(WRITE_PG_RESET_OFFSET_PACKETr(unit, 54, idx,
                                                          pkt_rval));
        /* EP redirection */
        SOC_IF_ERROR_RETURN(WRITE_PG_RESET_OFFSET_CELLr(unit, 56, 1,
                                                        cell_rval));
        SOC_IF_ERROR_RETURN(WRITE_PG_RESET_OFFSET_PACKETr(unit, 56, 1,
                                                          pkt_rval));
        /* WLAN */
        if (soc_feature(unit, soc_feature_wlan)) {
            SOC_IF_ERROR_RETURN(WRITE_PG_RESET_OFFSET_CELLr(unit, 55, 1,
                                                            cell_rval));
            SOC_IF_ERROR_RETURN(WRITE_PG_RESET_OFFSET_PACKETr(unit, 55, 1,
                                                              pkt_rval));
        }
    }

    /* Input port per-PG reset floor (cell). Use default value 0 */

    /* Reserved space: Input port per-port minimum for SC and QM traffic
     * Use default value 0 */

    /* Input port priority XON disable */

    /* Input port max packet size in cells */
    rval0 = 0;
    soc_reg_field_set(unit, PORT_MAX_PKT_SIZEr, &rval0, PORT_MAX_PKT_SIZEf,
                      TR_MMU_JUMBO_FRAME_CELLS);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_PORT_MAX_PKT_SIZEr(unit, port, rval0));
    }
    if (soc_feature(unit, soc_feature_wlan)) {
        SOC_IF_ERROR_RETURN(WRITE_PORT_MAX_PKT_SIZEr(unit, 55, rval0));
    }

    /* Input port per-PG threshold */
    rval0 = 0;
    soc_reg_field_set(unit, PG_THRESH_SELr, &rval0, PG0_THRESH_SELf, 0x8);
    soc_reg_field_set(unit, PG_THRESH_SELr, &rval0, PG1_THRESH_SELf, 0x8);
    soc_reg_field_set(unit, PG_THRESH_SELr, &rval0, PG2_THRESH_SELf, 0x8);
    soc_reg_field_set(unit, PG_THRESH_SELr, &rval0, PG3_THRESH_SELf, 0x8);
    soc_reg_field_set(unit, PG_THRESH_SELr, &rval0, PG4_THRESH_SELf, 0x8);
    soc_reg_field_set(unit, PG_THRESH_SELr, &rval0, PG5_THRESH_SELf, 0x8);
    soc_reg_field_set(unit, PG_THRESH_SELr, &rval0, PG6_THRESH_SELf, 0x8);
    PBMP_ITER(pbmp_8pg, port) {
        SOC_IF_ERROR_RETURN(WRITE_PG_THRESH_SELr(unit, port, rval0));
    }
    if (triumph2_family) {
        rval0 = 0;
        soc_reg_field_set(unit, PG_THRESH_SEL2r, &rval0, PG0_THRESH_SELf, 0x8);
        PBMP_ITER(pbmp_2pg, port) {
            SOC_IF_ERROR_RETURN(WRITE_PG_THRESH_SEL2r(unit, port, rval0));
        }
    }

    idx = TR_MMU_NUM_PG - 1;
    rval0 = 0;
    soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval0, PRI0_GRPf, idx);
    soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval0, PRI1_GRPf, idx);
    soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval0, PRI2_GRPf, idx);
    soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval0, PRI3_GRPf, idx);
    soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval0, PRI4_GRPf, idx);
    soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval0, PRI5_GRPf, idx);
    soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval0, PRI6_GRPf, idx);
    rval1 = 0;
    if (triumph2_family) {
        soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval0, PRI7_GRPf, idx);
        soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval1, PRI14_GRPf, idx);
        soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval1, PRI15_GRPf, idx);
    } else {
        soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval1, PRI7_GRPf, idx);
    }
    soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval1, PRI8_GRPf, idx);
    soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval1, PRI9_GRPf, idx);
    soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval1, PRI10_GRPf, idx);
    soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval1, PRI11_GRPf, idx);
    soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval1, PRI12_GRPf, idx);
    soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval1, PRI13_GRPf, idx);
    PBMP_ITER(pbmp_8pg, port) {
        SOC_IF_ERROR_RETURN(WRITE_PORT_PRI_GRP0r(unit, port, rval0));
        SOC_IF_ERROR_RETURN(WRITE_PORT_PRI_GRP1r(unit, port, rval1));
    }
    if (triumph2_family) {
      rval0 = 0xffff; /* 16 priorities, all in PG 1 */
      PBMP_ITER(pbmp_2pg, port) {
	SOC_IF_ERROR_RETURN(WRITE_PORT_PRI_GRP2r(unit, port, rval0));
      }
    }

    /* Input port pause enable */
    COMPILER_64_ZERO(rval64);
    soc_reg64_field32_set(unit, PORT_PAUSE_ENABLE_64r, &rval64,
        PORT_PAUSE_ENABLE_LOf,
        SOC_PBMP_WORD_GET(PBMP_ALL(unit), 0));
    soc_reg64_field32_set(unit, PORT_PAUSE_ENABLE_64r, &rval64,
        PORT_PAUSE_ENABLE_HIf,
        SOC_PBMP_WORD_GET(PBMP_ALL(unit), 1));
    SOC_IF_ERROR_RETURN(WRITE_PORT_PAUSE_ENABLE_64r(unit, rval64));

    /* PO_QUEUE_CONfIG_CELL/PACKET handled by hapiBroadMmuTriumphPauseSet() */

    if (soc_feature(unit, soc_feature_internal_loopback)) {
        /* EP redirection */
        pkt_rval = 0;
        soc_reg_field_set(unit, OP_QUEUE_REDIRECT_XQ_CONFIG_PACKETr, &pkt_rval, 
                          Q_MIN_PACKETf, TR_MMU_OUT_PORT_MIN_PKTS);
        soc_reg_field_set(unit, OP_QUEUE_REDIRECT_XQ_CONFIG_PACKETr, &pkt_rval,
                          Q_LIMIT_DYNAMIC_PACKETf, 0x1);
        soc_reg_field_set(unit, OP_QUEUE_REDIRECT_XQ_CONFIG_PACKETr, &pkt_rval,
                          Q_SHARED_ALPHA_PACKETf, TR_MMU_OUT_QUEUE_PKT_ALPHA_IDX); 
        rval = 0;
        soc_reg_field_set(unit, OP_QUEUE_REDIRECT_CONFIG_PACKETr, &rval, 
                          Q_MIN_PACKETf, TR_MMU_OUT_PORT_MIN_PKTS);
        soc_reg_field_set(unit, OP_QUEUE_REDIRECT_CONFIG_PACKETr, &rval,
                          Q_LIMIT_DYNAMIC_PACKETf, 0x1);
        soc_reg_field_set(unit, OP_QUEUE_REDIRECT_CONFIG_PACKETr, &rval,
                          Q_SHARED_ALPHA_PACKETf, TR_MMU_OUT_QUEUE_PKT_ALPHA_IDX);
        cell_rval = 0;
        soc_reg_field_set(unit, OP_QUEUE_REDIRECT_CONFIG_CELLr, &cell_rval, 
                          Q_MIN_CELLf, TR_MMU_OUT_PORT_MIN_CELLS);
        soc_reg_field_set(unit, OP_QUEUE_REDIRECT_CONFIG_CELLr, &cell_rval,
                          Q_LIMIT_DYNAMIC_CELLf, 0x1);
        soc_reg_field_set(unit, OP_QUEUE_REDIRECT_CONFIG_CELLr, &cell_rval,
                          Q_SHARED_ALPHA_CELLf, TR_MMU_OUT_QUEUE_PKT_ALPHA_IDX); 

        for (idx = 0; idx < TR_MMU_NUM_COS; idx++) {
            SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_REDIRECT_XQ_CONFIG_PACKETr
                                (unit, 54, idx, pkt_rval));
            SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_REDIRECT_CONFIG_PACKETr
                                (unit, 54, idx, rval));
            SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_REDIRECT_CONFIG_CELLr
                                (unit, 54, idx, cell_rval));
        }
        /* WLAN */
        if (soc_feature(unit, soc_feature_wlan)) {
            /* Max out (disable) first fragment shared limits */
            cell_rval = (1 << soc_reg_field_length(unit, 
                         OP_QUEUE_FIRST_FRAGMENT_CONFIG_CELLr, 
                         Q_SHARED_LIMIT_CELLf)) - 1;
            pkt_rval = (1 << soc_reg_field_length(unit, 
                        OP_QUEUE_FIRST_FRAGMENT_CONFIG_PACKETr, 
                        Q_SHARED_LIMIT_PACKETf)) - 1;
            for (idx = 0; idx < TR_MMU_NUM_COS; idx++) {
                SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_FIRST_FRAGMENT_CONFIG_PACKETr
                                    (unit, 54, idx, pkt_rval));
                SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_FIRST_FRAGMENT_CONFIG_CELLr
                                    (unit, 54, idx, cell_rval));
            }
        }
    }

    /* Addition to SDK default: Set up custom output limits for CPU port */
    port = CMIC_PORT(unit);

    cell_rval = 0;
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_CELLr, &cell_rval, Q_MIN_CELLf, TR_MMU_OUT_QUEUE_CELL_CPU_MIN_CELL);
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_CELLr, &cell_rval, Q_SHARED_LIMIT_CELLf, TR_MMU_OUT_QUEUE_CELL_CPU_MIN_CELL/2);
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_CELLr, &cell_rval, Q_LIMIT_ENABLE_CELLf, TR_MMU_OUT_QUEUE_CELL_CPU_LIMIT_ENABLE);
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_CELLr, &cell_rval, Q_LIMIT_DYNAMIC_CELLf,TR_MMU_OUT_QUEUE_CELL_CPU_LIMIT_DYNAMIC);

    pkt_rval = 0;
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_PACKETr, &pkt_rval, Q_MIN_PACKETf, TR_MMU_OUT_QUEUE_PKT_CPU_MIN_PKT);
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_PACKETr, &pkt_rval, Q_SHARED_LIMIT_PACKETf, TR_MMU_OUT_QUEUE_PKT_CPU_MIN_PKT/2);
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_PACKETr, &pkt_rval, Q_LIMIT_ENABLE_PACKETf, TR_MMU_OUT_QUEUE_PKT_CPU_LIMIT_ENABLE);
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_PACKETr, &pkt_rval, Q_LIMIT_DYNAMIC_PACKETf,TR_MMU_OUT_QUEUE_PKT_CPU_LIMIT_DYNAMIC);

    for (idx = 0; idx < TR_MMU_NUM_COS; idx++) {
      SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG_CELLr(unit, port, idx, cell_rval));
      SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG_PACKETr(unit, port, idx, pkt_rval));
    }
    /* End CPU port output buffering customization */

    /* Output port per-port per-COS reset offset */
    cell_rval = 0;
    soc_reg_field_set(unit, OP_QUEUE_RESET_OFFSET_CELLr, &cell_rval,
                      Q_RESET_OFFSET_CELLf, TR_MMU_OUT_RESET_OFFSET_CELLS);
    pkt_rval = 0;
    soc_reg_field_set(unit, OP_QUEUE_RESET_OFFSET_PACKETr, &pkt_rval,
                      Q_RESET_OFFSET_PACKETf, TR_MMU_OUT_RESET_OFFSET_PKTS);
    PBMP_ALL_ITER(unit, port) {
        for (idx = 0; idx < TR_MMU_NUM_COS; idx++) {
            SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_RESET_OFFSET_CELLr(unit, port,
                                                                  idx,
                                                                  cell_rval));
            SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_RESET_OFFSET_PACKETr(unit, port,
                                                                    idx,
                                                                    pkt_rval));
        }
    }

    out_shared_cells = total_cells - out_reserved_cells;
    out_shared_pkts = total_pkts - out_reserved_pkts;

    /* Output port per-device shared */
    cell_rval = 0;
    soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_CELLr, &cell_rval,
                      OP_BUFFER_SHARED_LIMIT_CELLf, out_shared_cells - num_ports);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_CELLr(unit, cell_rval));
    pkt_rval = 0;
    soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_PACKETr, &pkt_rval,
                      OP_BUFFER_SHARED_LIMIT_PACKETf, out_shared_pkts - num_ports);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_PACKETr(unit, pkt_rval));

    /* Output port per-device shared for YELLOW traffic */
    cell_rval = 0;
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_YELLOW_CELLr, &cell_rval,
                      OP_BUFFER_LIMIT_YELLOW_CELLf, out_shared_cells >> 3);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_YELLOW_CELLr(unit, cell_rval));
    pkt_rval = 0;
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_YELLOW_PACKETr, &pkt_rval,
                      OP_BUFFER_LIMIT_YELLOW_PACKETf, out_shared_pkts >> 3);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_YELLOW_PACKETr(unit, pkt_rval));

    /* Output port per-device shared for RED traffic */
    cell_rval = 0;
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_RED_CELLr, &cell_rval,
                      OP_BUFFER_LIMIT_RED_CELLf, out_shared_cells >> 3);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RED_CELLr(unit, cell_rval));
    pkt_rval = 0;
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_RED_PACKETr, &pkt_rval,
                      OP_BUFFER_LIMIT_RED_PACKETf, out_shared_pkts >> 3);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RED_PACKETr(unit, pkt_rval));

    /*
     * Output port per-port shared
     * Limit check disabled, however still keep code as reference
     * OP_SHARED_LIMIT set to 3/4 of total shared space
     * OP_SHARED_RESET_VALUE set to 1/2 of total shared space
     */
    cell_rval = 0;
    soc_reg_field_set(unit, OP_PORT_CONFIG_CELLr, &cell_rval,
                      OP_SHARED_LIMIT_CELLf, out_shared_cells * 3 / 4);
    soc_reg_field_set(unit, OP_PORT_CONFIG_CELLr, &cell_rval,
                      OP_SHARED_RESET_VALUE_CELLf, out_shared_cells / 2);
    soc_reg_field_set(unit, OP_PORT_CONFIG_CELLr, &cell_rval,
                      PORT_LIMIT_ENABLE_CELLf, 0);
    pkt_rval = 0;
    soc_reg_field_set(unit, OP_PORT_CONFIG_PACKETr, &pkt_rval,
                      OP_SHARED_LIMIT_PACKETf, out_shared_pkts * 3 / 4);
    soc_reg_field_set(unit, OP_PORT_CONFIG_PACKETr, &pkt_rval,
                      OP_SHARED_RESET_VALUE_PACKETf, out_shared_pkts / 2);
    soc_reg_field_set(unit, OP_PORT_CONFIG_PACKETr, &pkt_rval,
                      PORT_LIMIT_ENABLE_PACKETf, 0);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_OP_PORT_CONFIG_CELLr(unit, port, cell_rval));
        SOC_IF_ERROR_RETURN(WRITE_OP_PORT_CONFIG_PACKETr(unit, port,
                                                         pkt_rval));
    }

/* CPU output port modifications */
    port = CMIC_PORT(unit);


    cell_rval = 0;
    soc_reg_field_set(unit, OP_PORT_CONFIG_CELLr, &cell_rval,
                      OP_SHARED_LIMIT_CELLf, TR_MMU_OUT_PORT_CPU_CELL_SHARED_LIMIT);
    soc_reg_field_set(unit, OP_PORT_CONFIG_CELLr, &cell_rval,
                      OP_SHARED_RESET_VALUE_CELLf, TR_MMU_OUT_PORT_CPU_CELL_SHARED_LIMIT/2);
    soc_reg_field_set(unit, OP_PORT_CONFIG_CELLr, &cell_rval,
                      PORT_LIMIT_ENABLE_CELLf, TR_MMU_OUT_PORT_CPU_CELL_SHARED_ENABLE);

    pkt_rval = 0;
    soc_reg_field_set(unit, OP_PORT_CONFIG_PACKETr, &pkt_rval,
                      OP_SHARED_LIMIT_PACKETf, TR_MMU_OUT_PORT_CPU_PKT_SHARED_LIMIT);
    soc_reg_field_set(unit, OP_PORT_CONFIG_PACKETr, &pkt_rval,
                      OP_SHARED_RESET_VALUE_PACKETf,TR_MMU_OUT_PORT_CPU_PKT_SHARED_LIMIT/2 );
    soc_reg_field_set(unit, OP_PORT_CONFIG_PACKETr, &pkt_rval,
                      PORT_LIMIT_ENABLE_PACKETf, TR_MMU_OUT_PORT_CPU_PKT_SHARED_ENABLE);

    SOC_IF_ERROR_RETURN(WRITE_OP_PORT_CONFIG_CELLr(unit, port, cell_rval));
    SOC_IF_ERROR_RETURN(WRITE_OP_PORT_CONFIG_PACKETr(unit, port, pkt_rval));

    /* Output port per-port shared for YELLOW traffic */
    cell_rval = 0;
    soc_reg_field_set(unit, OP_PORT_LIMIT_YELLOW_CELLr, &cell_rval,
                      OP_PORT_LIMIT_YELLOW_CELLf,
                      (out_shared_cells * 3 / 4) >> 3);
    pkt_rval = 0;
    soc_reg_field_set(unit, OP_PORT_LIMIT_YELLOW_PACKETr, &pkt_rval,
                      OP_PORT_LIMIT_YELLOW_PACKETf,
                      (out_shared_pkts * 3 / 4) >> 3);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_OP_PORT_LIMIT_YELLOW_CELLr(unit, port,
                                                             cell_rval));
        SOC_IF_ERROR_RETURN(WRITE_OP_PORT_LIMIT_YELLOW_PACKETr(unit, port,
                                                               pkt_rval));
    }

    /* Output port per-port shared for RED traffic */
    cell_rval = 0;
    soc_reg_field_set(unit, OP_PORT_LIMIT_RED_CELLr, &cell_rval,
                      OP_PORT_LIMIT_RED_CELLf,
                      (out_shared_cells * 3 / 4) >> 3);
    pkt_rval = 0;
    soc_reg_field_set(unit, OP_PORT_LIMIT_RED_PACKETr, &pkt_rval,
                      OP_PORT_LIMIT_RED_PACKETf,
                      (out_shared_pkts * 3 / 4) >> 3);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_OP_PORT_LIMIT_RED_CELLr(unit, port,
                                                          cell_rval));
        SOC_IF_ERROR_RETURN(WRITE_OP_PORT_LIMIT_RED_PACKETr(unit, port,
                                                            pkt_rval));
    }
    /* End CPU port modifications */

    /* Output port configuration */
    rval0 = 0;
    soc_reg_field_set(unit, OP_THR_CONFIGr, &rval0,
                      MOP_POLICYf, TR_MMU_MOP_POLICY);
    soc_reg_field_set(unit, OP_THR_CONFIGr, &rval0,
                      SOP_POLICYf, TR_MMU_SOP_POLICY);
    SOC_IF_ERROR_RETURN(WRITE_OP_THR_CONFIGr(unit, rval0));

    /* Input port enable */
    COMPILER_64_ZERO(rval64);
    soc_reg64_field32_set(unit, OUTPUT_PORT_RX_ENABLE_64r, &rval64,
        OUTPUT_PORT_RX_ENABLE_LOf,
        SOC_PBMP_WORD_GET(PBMP_ALL(unit), 0));
    soc_reg64_field32_set(unit, OUTPUT_PORT_RX_ENABLE_64r, &rval64,
        OUTPUT_PORT_RX_ENABLE_HIf,
        SOC_PBMP_WORD_GET(PBMP_ALL(unit), 1));
    SOC_IF_ERROR_RETURN(WRITE_OUTPUT_PORT_RX_ENABLE_64r(unit, rval64));

    COMPILER_64_ZERO(rval64);
    SOC_PBMP_CLEAR(temp);
    SOC_PBMP_ASSIGN(temp, PBMP_ALL(unit));
    if (triumph2_family) {
      SOC_PBMP_OR(temp, PBMP_MMU(unit));
    }
    soc_reg64_field32_set(unit, INPUT_PORT_RX_ENABLE_64r, &rval64, 
        INPUT_PORT_RX_ENABLE_LOf,
        SOC_PBMP_WORD_GET(temp, 0));
    soc_reg64_field32_set(unit, INPUT_PORT_RX_ENABLE_64r, &rval64, 
        INPUT_PORT_RX_ENABLE_HIf,
        SOC_PBMP_WORD_GET(temp, 1));
    SOC_IF_ERROR_RETURN(WRITE_INPUT_PORT_RX_ENABLE_64r(unit, rval64));

#ifndef SAFC_DISABLED
    SOC_IF_ERROR_RETURN(hapiBroadPgInit(unit,cosmap,sizeof(cosmap)/sizeof(pg_cosmap_t),
                        pg_vals,sizeof(pg_vals)/sizeof(pg_vals_t)));
#endif

    /* set to the no pause first */
    SOC_IF_ERROR_RETURN(hapiBroadMmuPauseSet(unit, 0));

    return SOC_E_NONE;
}

/*********************************************************************
* @purpose  Modify MMU for Enduro
*
* @param    unit - bcm unit number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Roughly taken from soc_enduro_mmu_init(int unit)
*           Mods for values, cpu queues, and safc
*
* @end
*********************************************************************/

int
hapiBroadEnduroMmuModify(int unit)
{
    uint32              rval0, rval1, cell_rval, pkt_rval;
    uint32              cell_rval_cpu,pkt_rval_cpu,field_max;
    int                 port;
    int                 total_cells, total_pkts;
    int                 in_reserved_cells, in_reserved_pkts;
    int                 out_reserved_cells, out_reserved_pkts;
    int                 out_shared_cells, out_shared_pkts;
    int                 idx;
    soc_pbmp_t          pbmp_8pg;
    uint32              in_cell = 0,out_cell = 0;
    
    int cpu_port = CMIC_PORT(unit);

    in_cell = out_cell = 0;
    out_shared_cells = out_shared_pkts = 0;
    
    /* Ports with 8PG (1PG for other ports) */
    SOC_PBMP_CLEAR(pbmp_8pg);
    SOC_PBMP_WORD_SET(pbmp_8pg, 0, 0x3c000000); /* 26-29 */
    SOC_PBMP_AND(pbmp_8pg, PBMP_ALL(unit));

    hapiBroadEnduroMmuCellLimits(unit, &total_cells, &total_pkts, &in_reserved_cells, 
                           &in_reserved_pkts, &out_reserved_cells, &out_reserved_pkts);
                  
    
    /*
     * Input ports threshold
     */
    /* Reserved space: Input port per-port minimum */
    cell_rval = 0;
    soc_reg_field_set(unit, PORT_MIN_CELLr, &cell_rval, PORT_MINf,
                      EN_MMU_IN_PORT_MIN_CELLS);
    pkt_rval = 0;
    soc_reg_field_set(unit, PORT_MIN_PACKETr, &pkt_rval, PORT_MINf,
                      EN_MMU_IN_PORT_MIN_PKTS);
                      
    /*Since we are setting same PORT_MIN for ge as well as hg ports 
    PBMP_ALL_ITER is fine*/   
    
    PBMP_ALL_ITER(unit, port) {
        
        SOC_IF_ERROR_RETURN(WRITE_PORT_MIN_CELLr(unit, port, cell_rval));
        SOC_IF_ERROR_RETURN(WRITE_PORT_MIN_PACKETr(unit, port, pkt_rval));
    }

    /* handle the cpu backpressure issue */
    /* set the cpu port_min to the max so no bp is allowed on cpu */
    cell_rval_cpu = 0;
    pkt_rval_cpu = 0;

    field_max = ( 1 << soc_reg_field_length(unit,PORT_MIN_CELLr,PORT_MINf)) - 1;
    soc_reg_field_set(unit, PORT_MIN_CELLr, &cell_rval_cpu, PORT_MINf,field_max);
    SOC_IF_ERROR_RETURN(WRITE_PORT_MIN_CELLr(unit, cpu_port, cell_rval_cpu));

    field_max = ( 1 << soc_reg_field_length(unit,PORT_MIN_PACKETr,PORT_MINf)) - 1;
    soc_reg_field_set(unit, PORT_MIN_PACKETr, &pkt_rval_cpu, PORT_MINf,field_max);
    SOC_IF_ERROR_RETURN(WRITE_PORT_MIN_PACKETr(unit, cpu_port, pkt_rval_cpu));

     /* Reserved space: Input port per-PG minimum
     * Use defalt value 0
     * With only one PG in use PORT_MIN should be sufficient */

    /* Reserved space: Input port per-PG headroom
     * Use only 1PG (highest priority PG for the port) */
    cell_rval = 0;
    soc_reg_field_set(unit, PG_HDRM_LIMIT_CELLr, &cell_rval, PG_HDRM_LIMITf,
                      EN_MMU_PG_HDRM_LIMIT_CELLS);
    soc_reg_field_set(unit, PG_HDRM_LIMIT_CELLr, &cell_rval, PG_GEf, 1);
    pkt_rval = 0;
    soc_reg_field_set(unit, PG_HDRM_LIMIT_PACKETr, &pkt_rval, PG_HDRM_LIMITf,
                      EN_MMU_PG_HDRM_LIMIT_PKTS);
    PBMP_PORT_ITER(unit, port) {
        idx = SOC_PBMP_MEMBER(pbmp_8pg, port) ? TR_MMU_NUM_PG - 1 : 0;
        SOC_IF_ERROR_RETURN(WRITE_PG_HDRM_LIMIT_CELLr(unit, port, idx,
                                                      cell_rval));
        SOC_IF_ERROR_RETURN(WRITE_PG_HDRM_LIMIT_PACKETr(unit, port, idx,
                                                        pkt_rval));
    }

    /* Reserved space: Input port per-device headroom */
    cell_rval = 0;
    soc_reg_field_set(unit, GLOBAL_HDRM_LIMITr, &cell_rval, GLOBAL_HDRM_LIMITf,
                      EN_MMU_GLOBAL_HDRM_LIMIT_CELLS);
    SOC_IF_ERROR_RETURN(WRITE_GLOBAL_HDRM_LIMITr(unit, cell_rval));

  

    /* static memory and the entire space for the cpu */
    cell_rval_cpu = 0;
    pkt_rval_cpu = 0;

    field_max = ( 1 << soc_reg_field_length(unit,PORT_SHARED_LIMIT_CELLr,PORT_SHARED_LIMITf)) - 1;
    soc_reg_field_set(unit, PORT_SHARED_LIMIT_CELLr, &cell_rval_cpu,PORT_SHARED_LIMITf, field_max);
    soc_reg_field_set(unit, PORT_SHARED_LIMIT_CELLr, &cell_rval_cpu, PORT_SHARED_DYNAMICf, 0);
    SOC_IF_ERROR_RETURN(WRITE_PORT_SHARED_LIMIT_CELLr(unit, cpu_port, cell_rval_cpu));

    field_max = ( 1 << soc_reg_field_length(unit,PORT_SHARED_LIMIT_PACKETr,PORT_SHARED_LIMITf)) - 1;
    soc_reg_field_set(unit, PORT_SHARED_LIMIT_PACKETr, &pkt_rval_cpu, PORT_SHARED_LIMITf, field_max);
    soc_reg_field_set(unit, PORT_SHARED_LIMIT_PACKETr, &pkt_rval_cpu, PORT_SHARED_DYNAMICf, 0);
    SOC_IF_ERROR_RETURN(WRITE_PORT_SHARED_LIMIT_PACKETr(unit, cpu_port, pkt_rval_cpu)); 
   

    /* Input port per-PG reset offset
     * Use only 1PG (highest priority PG for the port)
     * Use default value 0 for CPU */
    cell_rval = 0;
    soc_reg_field_set(unit, PG_RESET_OFFSET_CELLr, &cell_rval,
                      PG_RESET_OFFSETf, EN_MMU_PG_RESET_OFFSET_CELLS);
    pkt_rval = 0;
    soc_reg_field_set(unit, PG_RESET_OFFSET_PACKETr, &pkt_rval,
                      PG_RESET_OFFSETf, EN_MMU_PG_RESET_OFFSET_PKTS);
    PBMP_PORT_ITER(unit, port) {
        idx = SOC_PBMP_MEMBER(pbmp_8pg, port) ? TR_MMU_NUM_PG - 1 : 0;
        SOC_IF_ERROR_RETURN(WRITE_PG_RESET_OFFSET_CELLr(unit, port, idx,
                                                        cell_rval));
        SOC_IF_ERROR_RETURN(WRITE_PG_RESET_OFFSET_PACKETr(unit, port, idx,
                                                          pkt_rval));
    }

    /* Input port per-PG reset floor (cell). Use default value 0 */

    /* Reserved space: Input port per-port minimum for SC and QM traffic
     * Use default value 0 */

    /* Input port priority XON disable */

    /* Input port max packet size in cells */
    rval0 = 0;
    soc_reg_field_set(unit, PORT_MAX_PKT_SIZEr, &rval0, PORT_MAX_PKT_SIZEf,
                      TR_MMU_JUMBO_FRAME_CELLS);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_PORT_MAX_PKT_SIZEr(unit, port, rval0));
    }

    /* Input port per-PG threshold */
    rval0 = 0;
    soc_reg_field_set(unit, PG_THRESH_SELr, &rval0, PG0_THRESH_SELf, 0x8);
    soc_reg_field_set(unit, PG_THRESH_SELr, &rval0, PG1_THRESH_SELf, 0x8);
    soc_reg_field_set(unit, PG_THRESH_SELr, &rval0, PG2_THRESH_SELf, 0x8);
    soc_reg_field_set(unit, PG_THRESH_SELr, &rval0, PG3_THRESH_SELf, 0x8);
    soc_reg_field_set(unit, PG_THRESH_SELr, &rval0, PG4_THRESH_SELf, 0x8);
    soc_reg_field_set(unit, PG_THRESH_SELr, &rval0, PG5_THRESH_SELf, 0x8);
    soc_reg_field_set(unit, PG_THRESH_SELr, &rval0, PG6_THRESH_SELf, 0x8);
    PBMP_ITER(pbmp_8pg, port) {
        SOC_IF_ERROR_RETURN(WRITE_PG_THRESH_SELr(unit, port, rval0));
    }

    idx = TR_MMU_NUM_PG - 1;
    rval0 = 0;
    soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval0, PRI0_GRPf, idx);
    soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval0, PRI1_GRPf, idx);
    soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval0, PRI2_GRPf, idx);
    soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval0, PRI3_GRPf, idx);
    soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval0, PRI4_GRPf, idx);
    soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval0, PRI5_GRPf, idx);
    soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval0, PRI6_GRPf, idx);
    rval1 = 0;
    soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval1, PRI7_GRPf, idx);
    soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval1, PRI8_GRPf, idx);
    soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval1, PRI9_GRPf, idx);
    soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval1, PRI10_GRPf, idx);
    soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval1, PRI11_GRPf, idx);
    soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval1, PRI12_GRPf, idx);
    soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval1, PRI13_GRPf, idx);
    PBMP_ITER(pbmp_8pg, port) {
        SOC_IF_ERROR_RETURN(WRITE_PORT_PRI_GRP0r(unit, port, rval0));
        SOC_IF_ERROR_RETURN(WRITE_PORT_PRI_GRP1r(unit, port, rval1));
    }
     /* Addition to SDK default: Set up custom output limits for CPU port */
    port = CMIC_PORT(unit);

    cell_rval = 0;
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_CELLr, &cell_rval, Q_MIN_CELLf, 
                      EN_MMU_OUT_QUEUE_CELL_CPU_MIN_CELL);
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_CELLr, &cell_rval, Q_SHARED_LIMIT_CELLf, 
                      EN_MMU_OUT_QUEUE_CELL_CPU_MIN_CELL/2);
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_CELLr, &cell_rval, Q_LIMIT_ENABLE_CELLf,
                      EN_MMU_OUT_QUEUE_CELL_CPU_LIMIT_ENABLE);
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_CELLr, &cell_rval, Q_LIMIT_DYNAMIC_CELLf,
                      EN_MMU_OUT_QUEUE_CELL_CPU_LIMIT_DYNAMIC);

    pkt_rval = 0;
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_PACKETr, &pkt_rval, Q_MIN_PACKETf, 
                      EN_MMU_OUT_QUEUE_PKT_CPU_MIN_PKT);
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_PACKETr, &pkt_rval, Q_SHARED_LIMIT_PACKETf,
                      EN_MMU_OUT_QUEUE_PKT_CPU_MIN_PKT/2);
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_PACKETr, &pkt_rval, Q_LIMIT_ENABLE_PACKETf, 
                      EN_MMU_OUT_QUEUE_PKT_CPU_LIMIT_ENABLE);
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_PACKETr, &pkt_rval, Q_LIMIT_DYNAMIC_PACKETf,
                      EN_MMU_OUT_QUEUE_PKT_CPU_LIMIT_DYNAMIC);

    for (idx = 0; idx < TR_MMU_NUM_COS /2 ; idx++) {
      SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG_CELLr(unit, port, idx, cell_rval));
      SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG_PACKETr(unit, port, idx, pkt_rval));
    }
    /*For cpu queues 4,5,6,7 we set more cells and buffers aside*/
    
    cell_rval = 0;
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_CELLr, &cell_rval, Q_MIN_CELLf, 
                      EN_MMU_OUT_HIGH_QUEUE_CELL_CPU_MIN_CELL);
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_CELLr, &cell_rval, Q_SHARED_LIMIT_CELLf, 
                      EN_MMU_OUT_HIGH_QUEUE_CELL_CPU_MIN_CELL/2);
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_CELLr, &cell_rval, Q_LIMIT_ENABLE_CELLf,
                      EN_MMU_OUT_QUEUE_CELL_CPU_LIMIT_ENABLE);
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_CELLr, &cell_rval, Q_LIMIT_DYNAMIC_CELLf,
                      EN_MMU_OUT_QUEUE_CELL_CPU_LIMIT_DYNAMIC);
    
    pkt_rval = 0;
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_PACKETr, &pkt_rval, Q_MIN_PACKETf, 
                      EN_MMU_OUT_HIGH_QUEUE_PKT_CPU_MIN_PKT);
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_PACKETr, &pkt_rval, Q_SHARED_LIMIT_PACKETf,
                      EN_MMU_OUT_HIGH_QUEUE_PKT_CPU_MIN_PKT/2);
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_PACKETr, &pkt_rval, Q_LIMIT_ENABLE_PACKETf, 
                      EN_MMU_OUT_QUEUE_PKT_CPU_LIMIT_ENABLE);
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_PACKETr, &pkt_rval, Q_LIMIT_DYNAMIC_PACKETf,
                      EN_MMU_OUT_QUEUE_PKT_CPU_LIMIT_DYNAMIC);
    
        for (idx = 4; idx < TR_MMU_NUM_COS  ; idx++) {
          SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG_CELLr(unit, port, idx, cell_rval));
          SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG_PACKETr(unit, port, idx, pkt_rval));
        }
    /* End CPU port output buffering customization */
    
    /* Output port per-port per-COS reset offset */
    cell_rval = 0;
    soc_reg_field_set(unit, OP_QUEUE_RESET_OFFSET_CELLr, &cell_rval,
                      Q_RESET_OFFSET_CELLf, EN_MMU_OUT_RESET_OFFSET_CELLS);
    pkt_rval = 0;
    soc_reg_field_set(unit, OP_QUEUE_RESET_OFFSET_PACKETr, &pkt_rval,
                      Q_RESET_OFFSET_PACKETf, EN_MMU_OUT_RESET_OFFSET_PKTS);
    PBMP_ALL_ITER(unit, port) {
        for (idx = 0; idx < TR_MMU_NUM_COS; idx++) {
            SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_RESET_OFFSET_CELLr(unit, port,
                                                                  idx,
                                                                  cell_rval));
            SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_RESET_OFFSET_PACKETr(unit, port,
                                                                    idx,
                                                                    pkt_rval));
        }
    }

    out_shared_cells = total_cells - out_reserved_cells;
    out_shared_pkts = total_pkts - out_reserved_pkts;

    /* Output port per-device shared */
    cell_rval = 0;
    soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_CELLr, &cell_rval,
                      OP_BUFFER_SHARED_LIMIT_CELLf, out_shared_cells);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_CELLr(unit, cell_rval));
    pkt_rval = 0;
    soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_PACKETr, &pkt_rval,
                      OP_BUFFER_SHARED_LIMIT_PACKETf, out_shared_pkts);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_PACKETr(unit, pkt_rval));

    /* Output port per-device shared for YELLOW traffic */
    cell_rval = 0;
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_YELLOW_CELLr, &cell_rval,
                      OP_BUFFER_LIMIT_YELLOW_CELLf, out_shared_cells >> 3);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_YELLOW_CELLr(unit, cell_rval));
    pkt_rval = 0;
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_YELLOW_PACKETr, &pkt_rval,
                      OP_BUFFER_LIMIT_YELLOW_PACKETf, out_shared_pkts >> 2);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_YELLOW_PACKETr(unit, pkt_rval));

    /* Output port per-device shared for RED traffic */
    cell_rval = 0;
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_RED_CELLr, &cell_rval,
                      OP_BUFFER_LIMIT_RED_CELLf, out_shared_cells >> 3);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RED_CELLr(unit, cell_rval));
    pkt_rval = 0;
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_RED_PACKETr, &pkt_rval,
                      OP_BUFFER_LIMIT_RED_PACKETf, out_shared_pkts >> 2);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RED_PACKETr(unit, pkt_rval));

    /* Output port per-device shared  */
    cell_rval = 0;
    soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_RESUME_CELLr, &cell_rval,
                      OP_BUFFER_SHARED_LIMIT_RESUME_CELLf, out_shared_cells * 85 / 100);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_RESUME_CELLr(unit, cell_rval));
    pkt_rval = 0;
    soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_RESUME_PACKETr, &pkt_rval,
                      OP_BUFFER_SHARED_LIMIT_RESUME_PACKETf, out_shared_pkts * 85 / 100);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_RESUME_PACKETr(unit, pkt_rval));

    /*
     * Output port per-port shared
     * Limit check disabled, however still keep code as reference
     * OP_SHARED_LIMIT set to 3/4 of total shared space
     * OP_SHARED_RESET_VALUE set to 1/2 of total shared space
     */
    cell_rval = 0;
    soc_reg_field_set(unit, OP_PORT_CONFIG_CELLr, &cell_rval,
                      OP_SHARED_LIMIT_CELLf, out_shared_cells * 3 / 4);
    soc_reg_field_set(unit, OP_PORT_CONFIG_CELLr, &cell_rval,
                      OP_SHARED_RESET_VALUE_CELLf, out_shared_cells / 2);
    soc_reg_field_set(unit, OP_PORT_CONFIG_CELLr, &cell_rval,
                      PORT_LIMIT_ENABLE_CELLf, 0);
    pkt_rval = 0;
    soc_reg_field_set(unit, OP_PORT_CONFIG_PACKETr, &pkt_rval,
                      OP_SHARED_LIMIT_PACKETf, out_shared_pkts * 3 / 4);
    soc_reg_field_set(unit, OP_PORT_CONFIG_PACKETr, &pkt_rval,
                      OP_SHARED_RESET_VALUE_PACKETf, out_shared_pkts / 2);
    soc_reg_field_set(unit, OP_PORT_CONFIG_PACKETr, &pkt_rval,
                      PORT_LIMIT_ENABLE_PACKETf, 0);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_OP_PORT_CONFIG_CELLr(unit, port, cell_rval));
        SOC_IF_ERROR_RETURN(WRITE_OP_PORT_CONFIG_PACKETr(unit, port,
                                                         pkt_rval));
    }

    /* Output port per-port shared for YELLOW traffic */
    cell_rval = 0;
    soc_reg_field_set(unit, OP_PORT_LIMIT_YELLOW_CELLr, &cell_rval,
                      OP_PORT_LIMIT_YELLOW_CELLf,
                      (out_shared_cells * 3 / 4) >> 3);
    pkt_rval = 0;
    soc_reg_field_set(unit, OP_PORT_LIMIT_YELLOW_PACKETr, &pkt_rval,
                      OP_PORT_LIMIT_YELLOW_PACKETf,
                      (out_shared_pkts * 3 / 4) >> 2);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_OP_PORT_LIMIT_YELLOW_CELLr(unit, port,
                                                             cell_rval));
        SOC_IF_ERROR_RETURN(WRITE_OP_PORT_LIMIT_YELLOW_PACKETr(unit, port,
                                                               pkt_rval));
    }

    /* Output port per-port shared for RED traffic */
    cell_rval = 0;
    soc_reg_field_set(unit, OP_PORT_LIMIT_RED_CELLr, &cell_rval,
                      OP_PORT_LIMIT_RED_CELLf,
                      (out_shared_cells * 3 / 4) >> 3);
    pkt_rval = 0;
    soc_reg_field_set(unit, OP_PORT_LIMIT_RED_PACKETr, &pkt_rval,
                      OP_PORT_LIMIT_RED_PACKETf,
                      (out_shared_pkts * 3 / 4) >> 2);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_OP_PORT_LIMIT_RED_CELLr(unit, port,
                                                          cell_rval));
        SOC_IF_ERROR_RETURN(WRITE_OP_PORT_LIMIT_RED_PACKETr(unit, port,
                                                            pkt_rval));
    }
    /* Output port configuration */
    rval0 = 0;
    soc_reg_field_set(unit, OP_THR_CONFIGr, &rval0,
                      MOP_POLICYf, TR_MMU_MOP_POLICY);
    soc_reg_field_set(unit, OP_THR_CONFIGr, &rval0,
                      SOP_POLICYf, TR_MMU_SOP_POLICY);
    SOC_IF_ERROR_RETURN(WRITE_OP_THR_CONFIGr(unit, rval0));   
    
#ifndef SAFC_DISABLED
    SOC_IF_ERROR_RETURN(hapiBroadPgInit(unit,enduro_cosmap,sizeof(enduro_cosmap)/sizeof(
    pg_cosmap_t),
                            en_pg_vals,sizeof(en_pg_vals)/sizeof(pg_vals_t)));
#endif
    
    /* set to the no pause first */
    SOC_IF_ERROR_RETURN(hapiBroadMmuPauseSet(unit, 0));
        
     return SOC_E_NONE;
}

void dumpCellLimits(int unit)
{
   int                 in_reserved_cells, in_reserved_pkts;
   int                 out_reserved_cells, out_reserved_pkts;
   int                 total_cells, total_pkts;
   
   if (SOC_IS_ENDURO(unit))
   {
     hapiBroadEnduroMmuCellLimits(unit, &total_cells, &total_pkts, &in_reserved_cells, 
                           &in_reserved_pkts, &out_reserved_cells, &out_reserved_pkts);
   }
   else
   {
     hapiBroadMmuCellLimits(unit, &total_cells, &total_pkts, &in_reserved_cells, 
                           &in_reserved_pkts, &out_reserved_cells, &out_reserved_pkts);
   }
   printf("\nTotal cells = %d Total pkts = %d",total_cells,total_pkts);
   printf("\nin_reserved_cells = %d in_reserved_pkts = %d",in_reserved_cells,in_reserved_pkts);
   printf("\nout_reserved_cells = %d out_reserved_pkts = %d",out_reserved_cells,out_reserved_pkts);
}

#endif /* BCM_TRIUMPH_SUPPORT */

#ifdef BCM_RAVEN_SUPPORT

/* LWM: Allow one 1536 bytes packet */
#define FP_RAVEN_MMU_LWMLIMIT_PER_COS 12 

/* HOL_COS: 1024 XQ pointers / 8 cosq */
#define FP_RAVEN_MMU_HOL_COS_PKT_LIMIT 128 

#define FP_RAVEN_MMU_HOL_CPU_COS_PKT_LIMIT 64

/* Fine tuned */
#define FP_RAVEN_MMU_OVERSUBSCRIPTION_RATIO 7

/* 5 gives a fanin of 4 in certain cases  */
#define FP_RAVEN_MMU_FANIN            5    

/* RESET limits: if you change one, don't forget to update
 * the other
 */
#define FP_RAVEN_MMU_RESET_LIMIT_PERCENT      75
#define FP_RAVEN_MMU_RESET_LIMIT_SEL      0 


/*********************************************************************
* @purpose  Modify MMU for Raven devices
*
* @param    unit - bcm unit number
*
* @returns  BCM_E_XX
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t hapiBroadRavenMmuModify(L7_uint32 unit)
{
  uint32     regVal, totalNumOfCells,  staticCellsReqd;
  uint32     totalDynCellLimit, totalDynCellResetLimit;
  uint32     dynCellLimit, dynCellLimitCpu, dynCellLimitEthPort, dynCellLimitHlPort;
  uint32     dynCellResetLimit, dynCellResetLimitCpu, dynCellResetLimitEthPort, dynCellResetLimitHlPort;
  uint32     ibpCellSetLimit, ibpCellSetLimitCpu, ibpCellSetLimitEthPort, ibpCellSetLimitHlPort;
  uint32     ibpCellResetLimit, ibpCellResetLimitCpu, ibpCellResetLimitEthPort, ibpCellResetLimitHlPort;
  uint32     ibpPktSetLimit, ibpPktSetLimitCpu, ibpPktSetLimitEthPort, ibpPktSetLimitHlPort;
  uint32     ibpPktResetLimit, ibpPktResetLimitCpu, ibpPktResetLimitEthPort, ibpPktResetLimitHlPort;
  uint32     holCosPktSetLimit, holCosPktSetLimitEthPort, holCosPktSetLimitCpu;
  uint32     holCosPktResetLimit, holCosPktResetLimitCpu, holCosPktResetLimitEthPort;
  uint32     lwmCosCellSetLimit, lwmCosCellResetLimit, fieldMax;
  uint32     numPorts, cos, numCosq, cpuExtraCellsSum = 0;
  bcm_pbmp_t pbmpAll;
  soc_port_t  port;
#ifdef L7_STACKING_PACKAGE
  int   cpuExtraCells[8] = {0, 0, 0, 0, 12, 12, 12, 52}; /* We want 8K static for Cos 7 */
#else
  int   cpuExtraCells[8] = {0, 0, 0, 0, 12, 12, 12, 12};
#endif
  
  /* Calculate the number of ports. There are several raven variants with
   * varying number of ports. The algorithm below should handle all the
   * types.
   */
  BCM_PBMP_CLEAR(pbmpAll);
  BCM_PBMP_ASSIGN(pbmpAll, PBMP_ALL(unit));
  BCM_PBMP_COUNT(pbmpAll, numPorts);

  SOC_IF_ERROR_RETURN(READ_CFAPCONFIGr(unit, &regVal));

  totalNumOfCells = soc_reg_field_get(unit, CFAPCONFIGr, regVal, CFAPPOOLSIZEf);

  /* Calculate the number of static cells to be reserved */

  numCosq = sizeof(cpuExtraCells) / sizeof(int);
  for (cos = 0; cos < numCosq; cos++)
  {
    cpuExtraCellsSum += cpuExtraCells[cos];
  }

  staticCellsReqd = (numCosq * FP_RAVEN_MMU_LWMLIMIT_PER_COS * numPorts) + 
                       cpuExtraCellsSum;

  if (staticCellsReqd >= totalNumOfCells)
  {
    return BCM_E_FAIL;
  }

  totalDynCellLimit = totalNumOfCells - staticCellsReqd;
  totalDynCellResetLimit = (totalDynCellLimit * FP_RAVEN_MMU_RESET_LIMIT_PERCENT) / (100);

  /* Set the Total Dyn cell limits */
  regVal = 0;
  SOC_IF_ERROR_RETURN(READ_TOTALDYNCELLSETLIMITr(unit, &regVal));

  soc_reg_field_set(unit, TOTALDYNCELLSETLIMITr,
                    &regVal, TOTALDYNCELLSETLIMITf, totalDynCellLimit);

  SOC_IF_ERROR_RETURN(WRITE_TOTALDYNCELLSETLIMITr(unit, regVal));

  regVal = 0;
  SOC_IF_ERROR_RETURN(READ_TOTALDYNCELLRESETLIMITr(unit, &regVal));

  soc_reg_field_set(unit, TOTALDYNCELLRESETLIMITr,
                    &regVal, TOTALDYNCELLRESETLIMITf, totalDynCellResetLimit);
  
  SOC_IF_ERROR_RETURN(WRITE_TOTALDYNCELLRESETLIMITr(unit, regVal));

  /* Front-panel port settings */
  dynCellLimitEthPort = (totalDynCellLimit / numPorts) * 
                                    (FP_RAVEN_MMU_OVERSUBSCRIPTION_RATIO);
  dynCellResetLimitEthPort = (dynCellLimitEthPort * FP_RAVEN_MMU_RESET_LIMIT_PERCENT) / (100);

  holCosPktSetLimitEthPort = FP_RAVEN_MMU_HOL_COS_PKT_LIMIT;
  holCosPktResetLimitEthPort = (holCosPktSetLimitEthPort * FP_RAVEN_MMU_RESET_LIMIT_PERCENT) / (100);

  ibpCellSetLimitEthPort = (dynCellLimitEthPort) / (FP_RAVEN_MMU_FANIN);
  ibpCellResetLimitEthPort = (ibpCellSetLimitEthPort * FP_RAVEN_MMU_RESET_LIMIT_PERCENT) / (100);

  ibpPktSetLimitEthPort = (holCosPktSetLimitEthPort) / (FP_RAVEN_MMU_FANIN);
  ibpPktResetLimitEthPort = (ibpPktSetLimitEthPort * FP_RAVEN_MMU_RESET_LIMIT_PERCENT) / (100);

  lwmCosCellSetLimit = FP_RAVEN_MMU_LWMLIMIT_PER_COS;
  lwmCosCellResetLimit = (lwmCosCellSetLimit * FP_RAVEN_MMU_RESET_LIMIT_PERCENT) / (100);

  /* HL settings */
  dynCellLimitHlPort = dynCellLimitEthPort * 2;
  dynCellResetLimitHlPort = (dynCellLimitHlPort * FP_RAVEN_MMU_RESET_LIMIT_PERCENT) / (100);

  fieldMax = ( 1 << soc_reg_field_length(unit,IBPPKTSETLIMITr, PKTSETLIMITf)) - 1;
  ibpCellSetLimitHlPort = fieldMax;
  ibpCellResetLimitHlPort = (ibpCellSetLimitHlPort * FP_RAVEN_MMU_RESET_LIMIT_PERCENT) / (100);

  fieldMax = ( 1 << soc_reg_field_length(unit,IBPCELLSETLIMITr, CELLSETLIMITf)) - 1;
  ibpPktSetLimitHlPort = fieldMax;
  ibpPktResetLimitHlPort = (ibpPktSetLimitHlPort * FP_RAVEN_MMU_RESET_LIMIT_PERCENT) / (100);

  /* Cpu port settings */
  dynCellLimitCpu = (ibpCellResetLimitEthPort / 2);
  dynCellResetLimitCpu = (dynCellLimitCpu * FP_RAVEN_MMU_RESET_LIMIT_PERCENT) / (100);

  holCosPktSetLimitCpu = (ibpPktResetLimitEthPort / 2);
  holCosPktResetLimitCpu = (holCosPktSetLimitCpu * FP_RAVEN_MMU_RESET_LIMIT_PERCENT) / (100);

  fieldMax = ( 1 << soc_reg_field_length(unit,IBPPKTSETLIMITr, PKTSETLIMITf)) - 1;
  ibpCellSetLimitCpu = fieldMax;
  ibpCellResetLimitCpu = (ibpCellSetLimitCpu * FP_RAVEN_MMU_RESET_LIMIT_PERCENT) / (100);

  fieldMax = ( 1 << soc_reg_field_length(unit,IBPCELLSETLIMITr, CELLSETLIMITf)) - 1;
  ibpPktSetLimitCpu = fieldMax;
  ibpPktResetLimitCpu = (ibpPktSetLimitCpu * FP_RAVEN_MMU_RESET_LIMIT_PERCENT) / (100);

  /* Set all the registers */
  PBMP_ALL_ITER(unit, port)
  {
    ibpCellSetLimit = ibpCellSetLimitEthPort;
    ibpCellResetLimit = ibpCellResetLimitEthPort;
  
    ibpPktSetLimit = ibpPktSetLimitEthPort;
    ibpPktResetLimit = ibpPktResetLimitEthPort;
  
    holCosPktSetLimit = holCosPktSetLimitEthPort;
    holCosPktResetLimit = holCosPktResetLimitEthPort;

    dynCellLimit = dynCellLimitEthPort;
    dynCellResetLimit = dynCellResetLimitEthPort;

    if (IS_CPU_PORT(unit, port))
    {
      ibpCellSetLimit = ibpCellSetLimitCpu;
      ibpCellResetLimit = ibpCellResetLimitCpu;
  
      ibpPktSetLimit = ibpPktSetLimitCpu;
      ibpPktResetLimit = ibpPktResetLimitCpu;
  
      holCosPktSetLimit = holCosPktSetLimitCpu;
      holCosPktResetLimit = holCosPktResetLimitCpu;

      dynCellLimit = dynCellLimitCpu;
      dynCellResetLimit = dynCellResetLimitCpu;
      
    }
    else if (IS_HL_PORT(unit, port))
    {
      ibpCellSetLimit = ibpCellSetLimitHlPort;
      ibpCellResetLimit = ibpCellResetLimitHlPort;
  
      ibpPktSetLimit = ibpPktSetLimitHlPort;
      ibpPktResetLimit = ibpPktResetLimitHlPort;

      dynCellLimit = dynCellLimitHlPort;
      dynCellResetLimit = dynCellResetLimitHlPort;
    }

    /* Validity checks */
    if ((ibpCellSetLimit == 0) || (ibpPktSetLimit == 0) ||
        (holCosPktSetLimit == 0) || (dynCellLimit == 0) ||
        (ibpCellSetLimit < ibpCellResetLimit) ||
        (ibpPktSetLimit < ibpPktResetLimit) ||
        (holCosPktSetLimit < holCosPktResetLimit) ||
        (dynCellLimit < dynCellResetLimit))
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                    "Warning: Skipping MMU config for unit/port %d/%d as MMU limits"
                    " are incorrectly configured. \n",
                    unit, port);
      continue;        
    }

    /* Program all the registers */
    regVal = 0;
    SOC_IF_ERROR_RETURN(READ_IBPCELLSETLIMITr(unit, port, &regVal));
    soc_reg_field_set(unit, IBPCELLSETLIMITr, &regVal, CELLSETLIMITf, ibpCellSetLimit);
    soc_reg_field_set(unit, IBPCELLSETLIMITr, &regVal, RESETLIMITSELf,
                      FP_RAVEN_MMU_RESET_LIMIT_SEL);

    SOC_IF_ERROR_RETURN(WRITE_IBPCELLSETLIMITr(unit, port, regVal)); 
   
    regVal = 0;
    SOC_IF_ERROR_RETURN(READ_IBPPKTSETLIMITr(unit, port, &regVal));

    soc_reg_field_set(unit, IBPPKTSETLIMITr, &regVal, PKTSETLIMITf, ibpPktSetLimit);
    soc_reg_field_set(unit, IBPPKTSETLIMITr, &regVal, RESETLIMITSELf,
                      FP_RAVEN_MMU_RESET_LIMIT_SEL);

    SOC_IF_ERROR_RETURN(WRITE_IBPPKTSETLIMITr(unit, port, regVal));
  
    regVal = 0;
    SOC_IF_ERROR_RETURN(READ_IBPDISCARDSETLIMITr(unit, port, &regVal));

    soc_reg_field_set(unit, IBPDISCARDSETLIMITr, &regVal, DISCARDSETLIMITf, 
                      totalNumOfCells-1);

    SOC_IF_ERROR_RETURN(WRITE_IBPDISCARDSETLIMITr(unit, port, regVal));

    for (cos = 0;cos < numCosq; cos++)
    {
      if (IS_CPU_PORT(unit, port))
      {
        lwmCosCellSetLimit = FP_RAVEN_MMU_LWMLIMIT_PER_COS + cpuExtraCells[cos];
        lwmCosCellResetLimit = (lwmCosCellSetLimit * FP_RAVEN_MMU_RESET_LIMIT_PERCENT) / (100);

        if (cos >= HAPI_BROAD_INGRESS_HIGH_PRIORITY_COS)
        {
          holCosPktSetLimit = FP_RAVEN_MMU_HOL_CPU_COS_PKT_LIMIT;
          holCosPktResetLimit = (holCosPktSetLimit * FP_RAVEN_MMU_RESET_LIMIT_PERCENT) / (100);
        }
        
      }
      else
      {
        lwmCosCellSetLimit = FP_RAVEN_MMU_LWMLIMIT_PER_COS;
        lwmCosCellResetLimit = (lwmCosCellSetLimit * FP_RAVEN_MMU_RESET_LIMIT_PERCENT) / (100);
      }

      regVal = 0;
      SOC_IF_ERROR_RETURN(READ_HOLCOSPKTSETLIMITr(unit, port, cos, &regVal));

      soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &regVal, PKTSETLIMITf,
                        holCosPktSetLimit);

      SOC_IF_ERROR_RETURN(WRITE_HOLCOSPKTSETLIMITr(unit, port, cos, regVal));

      regVal = 0;
      SOC_IF_ERROR_RETURN(READ_HOLCOSPKTRESETLIMITr(unit, port, cos, &regVal));

      soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &regVal, PKTRESETLIMITf,
                        holCosPktResetLimit);

      SOC_IF_ERROR_RETURN(WRITE_HOLCOSPKTRESETLIMITr(unit, port, cos, regVal));
      
      regVal = 0;
      SOC_IF_ERROR_RETURN(READ_LWMCOSCELLSETLIMITr(unit, port, cos, &regVal));

      soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &regVal, CELLSETLIMITf,
                        lwmCosCellSetLimit);
      soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &regVal, CELLRESETLIMITf,
                        lwmCosCellResetLimit);

      SOC_IF_ERROR_RETURN(WRITE_LWMCOSCELLSETLIMITr(unit, port, cos, regVal));

    }

    regVal = 0;
    SOC_IF_ERROR_RETURN(READ_DYNCELLLIMITr(unit, port, &regVal));

    soc_reg_field_set(unit, DYNCELLLIMITr, &regVal, DYNCELLSETLIMITf,
                      dynCellLimit);
    soc_reg_field_set(unit, DYNCELLLIMITr, &regVal, DYNCELLRESETLIMITf,
                      dynCellResetLimit);

    SOC_IF_ERROR_RETURN(WRITE_DYNCELLLIMITr(unit, port, regVal));
  } 

  regVal = 0;
  SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &regVal));
  soc_reg_field_set(unit, MISCCONFIGr, &regVal, DYN_XQ_ENf, 0);
  soc_reg_field_set(unit, MISCCONFIGr, &regVal, HOL_CELL_SOP_DROP_ENf, 1);
  SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, regVal));

  return BCM_E_NONE;
}
#endif

#ifdef BCM_RAPTOR_SUPPORT

/* LWM: Allow one 1536 bytes packet */
#define FP_RAPTOR_MMU_LWMLIMIT_PER_COS 12 

/* HOL_COS: 512 XQ pointers / 8 cosq */
#define FP_RAPTOR_MMU_HOL_COS_PKT_LIMIT 64 

#define FP_RAPTOR_MMU_HOL_CPU_COS_PKT_LIMIT 48 

/* Fine tuned */
#define FP_RAPTOR_MMU_OVERSUBSCRIPTION_RATIO 20

/* 3 gives a fanin of 2 in certain cases  */
#define FP_RAPTOR_MMU_FANIN            3    

/* RESET limits: if you change one, don't forget to update
 * the other
 */
#define FP_RAPTOR_MMU_RESET_LIMIT_PERCENT  75
#define FP_RAPTOR_MMU_RESET_LIMIT_SEL      0 

/*********************************************************************
* @purpose  Modify MMU for Raven devices
*
* @param    unit - bcm unit number
*
* @returns  BCM_E_XX
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t hapiBroadRaptorMmuModify(L7_uint32 unit)
{
  uint32     regVal, totalNumOfCells,  staticCellsReqd;
  uint32     totalDynCellLimit, totalDynCellResetLimit;
  uint32     dynCellLimit, dynCellLimitCpu, dynCellLimitEthPort;
  uint32     dynCellLimitHlPort;
  uint32     dynCellResetLimit, dynCellResetLimitCpu, dynCellResetLimitEthPort;
  uint32     dynCellResetLimitHlPort;
  uint32     ibpCellSetLimit, ibpCellSetLimitCpu, ibpCellSetLimitEthPort;
  uint32     ibpCellSetLimitHlPort;
  uint32     ibpCellResetLimit, ibpCellResetLimitCpu, ibpCellResetLimitEthPort;
  uint32     ibpCellResetLimitHlPort;
  uint32     ibpPktSetLimit, ibpPktSetLimitCpu, ibpPktSetLimitEthPort;
  uint32     ibpPktSetLimitHlPort;
  uint32     ibpPktResetLimit, ibpPktResetLimitCpu, ibpPktResetLimitEthPort;
  uint32     ibpPktResetLimitHlPort;
  uint32     holCosPktSetLimit, holCosPktSetLimitEthPort, holCosPktSetLimitCpu;
  uint32     holCosPktResetLimit, holCosPktResetLimitCpu, holCosPktResetLimitEthPort;
  uint32     lwmCosCellSetLimit, lwmCosCellResetLimit, fieldMax;
  uint32     numPorts, cos, numCosq, cpuExtraCellsSum = 0, deviceSupported = 0;
  uint32     raptor48PortDevice = 0, raptor48PortStaticCosq = 2;
  uint16     devId;
  uint8      revId;
  bcm_pbmp_t pbmpAll;
  soc_port_t  port;
#ifdef L7_STACKING_PACKAGE
  int   cpuExtraCells[8] = {0, 0, 0, 0, 12, 12, 12, 52}; /* We want 8K static for Cos 7 */
#else
  int   cpuExtraCells[8] = {0, 0, 0, 0, 12, 12, 12, 12};
#endif

  /* Customization for 5621x devices only */  
  if (soc_cm_get_id(unit, &devId, &revId) != BCM_E_NONE)
  {
    return BCM_E_FAIL;
  }

  switch (devId)
  {
    case BCM56218_DEVICE_ID:
    case BCM56218X_DEVICE_ID:
    case BCM56219_DEVICE_ID:
    case BCM56218R_DEVICE_ID:
    case BCM56219R_DEVICE_ID:
      raptor48PortDevice = L7_TRUE;
      deviceSupported = L7_TRUE;
      break;
      
    case BCM56214_DEVICE_ID:
    case BCM56215_DEVICE_ID:
    case BCM56215_A0_REV_ID:
    case BCM56214R_DEVICE_ID:
    case BCM56215R_DEVICE_ID:
    case BCM56216_DEVICE_ID:
    case BCM56217_DEVICE_ID:
    case BCM56212_DEVICE_ID:
    case BCM56213_DEVICE_ID:
      raptor48PortDevice = L7_FALSE;
      deviceSupported = L7_TRUE;
      break;

    default:
      deviceSupported = L7_FALSE;
      break;
    
  }

  if (deviceSupported == L7_FALSE)
  {
    return BCM_E_NONE;      
  }

  BCM_PBMP_CLEAR(pbmpAll);
  BCM_PBMP_ASSIGN(pbmpAll, PBMP_ALL(unit));
  BCM_PBMP_COUNT(pbmpAll, numPorts);


  SOC_IF_ERROR_RETURN(READ_CFAPCONFIGr(unit, &regVal));

  totalNumOfCells = soc_reg_field_get(unit, CFAPCONFIGr, regVal, CFAPPOOLSIZEf);

  /* Calculate the number of static cells to be reserved */

  numCosq = sizeof(cpuExtraCells) / sizeof(int);
  for (cos = 0; cos < numCosq; cos++)
  {
    cpuExtraCellsSum += cpuExtraCells[cos];
  }

  /* Due to small packet buffer memory, on 48 port devices
   * we allocate static cells to egress control packet queues only.
   */
  if (raptor48PortDevice == L7_TRUE)
  {
    staticCellsReqd = (raptor48PortStaticCosq * FP_RAPTOR_MMU_LWMLIMIT_PER_COS * numPorts) + 
                         cpuExtraCellsSum;
  }
  else
  {
    staticCellsReqd = (numCosq * FP_RAPTOR_MMU_LWMLIMIT_PER_COS * numPorts) + 
                         cpuExtraCellsSum;
  }

  if (staticCellsReqd >= totalNumOfCells)
  {
    return BCM_E_FAIL;
  }

  totalDynCellLimit = totalNumOfCells - staticCellsReqd;
  totalDynCellResetLimit = (totalDynCellLimit * FP_RAPTOR_MMU_RESET_LIMIT_PERCENT) / (100);

  /* Set the Total Dyn cell limits */
  regVal = 0;
  SOC_IF_ERROR_RETURN(READ_TOTALDYNCELLSETLIMITr(unit, &regVal));

  soc_reg_field_set(unit, TOTALDYNCELLSETLIMITr,
                    &regVal, TOTALDYNCELLSETLIMITf, totalDynCellLimit);

  SOC_IF_ERROR_RETURN(WRITE_TOTALDYNCELLSETLIMITr(unit, regVal));

  regVal = 0;
  SOC_IF_ERROR_RETURN(READ_TOTALDYNCELLRESETLIMITr(unit, &regVal));

  soc_reg_field_set(unit, TOTALDYNCELLRESETLIMITr,
                    &regVal, TOTALDYNCELLRESETLIMITf, totalDynCellResetLimit);

  SOC_IF_ERROR_RETURN(WRITE_TOTALDYNCELLRESETLIMITr(unit, regVal));

  /* Front-panel port settings */
  dynCellLimitEthPort = (totalDynCellLimit / numPorts) * 
                                    (FP_RAPTOR_MMU_OVERSUBSCRIPTION_RATIO);
  dynCellResetLimitEthPort = (dynCellLimitEthPort * FP_RAPTOR_MMU_RESET_LIMIT_PERCENT) / (100);

  holCosPktSetLimitEthPort = FP_RAPTOR_MMU_HOL_COS_PKT_LIMIT;
  holCosPktResetLimitEthPort = (holCosPktSetLimitEthPort * FP_RAPTOR_MMU_RESET_LIMIT_PERCENT) / (100);

  ibpCellSetLimitEthPort = (dynCellLimitEthPort) / (FP_RAPTOR_MMU_FANIN);
  ibpCellResetLimitEthPort = (ibpCellSetLimitEthPort * FP_RAPTOR_MMU_RESET_LIMIT_PERCENT) / (100);

  ibpPktSetLimitEthPort = (holCosPktSetLimitEthPort) / (FP_RAPTOR_MMU_FANIN);
  ibpPktResetLimitEthPort = (ibpPktSetLimitEthPort * FP_RAPTOR_MMU_RESET_LIMIT_PERCENT) / (100);

  lwmCosCellSetLimit = FP_RAPTOR_MMU_LWMLIMIT_PER_COS;
  lwmCosCellResetLimit = (lwmCosCellSetLimit * FP_RAPTOR_MMU_RESET_LIMIT_PERCENT) / (100);

  /* HL settings */
  dynCellLimitHlPort = dynCellLimitEthPort * 2;
  dynCellResetLimitHlPort = (dynCellLimitHlPort * FP_RAPTOR_MMU_RESET_LIMIT_PERCENT) / (100);

  fieldMax = ( 1 << soc_reg_field_length(unit,IBPPKTSETLIMITr, PKTSETLIMITf)) - 1;
  ibpCellSetLimitHlPort = fieldMax;
  ibpCellResetLimitHlPort = (ibpCellSetLimitHlPort * FP_RAPTOR_MMU_RESET_LIMIT_PERCENT) / (100);

  fieldMax = ( 1 << soc_reg_field_length(unit,IBPCELLSETLIMITr, CELLSETLIMITf)) - 1;
  ibpPktSetLimitHlPort = fieldMax;
  ibpPktResetLimitHlPort = (ibpPktSetLimitHlPort * FP_RAPTOR_MMU_RESET_LIMIT_PERCENT) / (100);

  /* Cpu port settings */
  dynCellLimitCpu = (ibpCellResetLimitEthPort )/(2);
  dynCellResetLimitCpu = (dynCellLimitCpu * FP_RAPTOR_MMU_RESET_LIMIT_PERCENT) / (100);

  holCosPktSetLimitCpu = (ibpPktResetLimitEthPort) / (2);
  holCosPktResetLimitCpu = (holCosPktSetLimitCpu * FP_RAPTOR_MMU_RESET_LIMIT_PERCENT) / (100);

  fieldMax = ( 1 << soc_reg_field_length(unit,IBPPKTSETLIMITr, PKTSETLIMITf)) - 1;
  ibpCellSetLimitCpu = fieldMax;
  ibpCellResetLimitCpu = (ibpCellSetLimitCpu * FP_RAPTOR_MMU_RESET_LIMIT_PERCENT) / (100);

  fieldMax = ( 1 << soc_reg_field_length(unit,IBPCELLSETLIMITr, CELLSETLIMITf)) - 1;
  ibpPktSetLimitCpu = fieldMax;
  ibpPktResetLimitCpu = (ibpPktSetLimitCpu * FP_RAPTOR_MMU_RESET_LIMIT_PERCENT) / (100);

  /* Set all the registers */
  PBMP_ALL_ITER(unit, port)
  {
    ibpCellSetLimit = ibpCellSetLimitEthPort;
    ibpCellResetLimit = ibpCellResetLimitEthPort;
  
    ibpPktSetLimit = ibpPktSetLimitEthPort;
    ibpPktResetLimit = ibpPktResetLimitEthPort;
  
    holCosPktSetLimit = holCosPktSetLimitEthPort;
    holCosPktResetLimit = holCosPktResetLimitEthPort;

    dynCellLimit = dynCellLimitEthPort;
    dynCellResetLimit = dynCellResetLimitEthPort;

    if (IS_CPU_PORT(unit, port))
    {
      ibpCellSetLimit = ibpCellSetLimitCpu;
      ibpCellResetLimit = ibpCellResetLimitCpu;
  
      ibpPktSetLimit = ibpPktSetLimitCpu;
      ibpPktResetLimit = ibpPktResetLimitCpu;
  
      holCosPktSetLimit = holCosPktSetLimitCpu;
      holCosPktResetLimit = holCosPktResetLimitCpu;

      dynCellLimit = dynCellLimitCpu;
      dynCellResetLimit = dynCellResetLimitCpu;
      
    }
    else if (IS_HL_PORT(unit, port))
    {
      ibpCellSetLimit = ibpCellSetLimitHlPort;
      ibpCellResetLimit = ibpCellResetLimitHlPort;
  
      ibpPktSetLimit = ibpPktSetLimitHlPort;
      ibpPktResetLimit = ibpPktResetLimitHlPort;

      dynCellLimit = dynCellLimitHlPort;
      dynCellResetLimit = dynCellResetLimitHlPort;
    }

    /* Sanity check */
    if ((ibpCellSetLimit == 0) || (ibpPktSetLimit == 0) ||
        (holCosPktSetLimit == 0) || (dynCellLimit == 0) ||
        (ibpCellSetLimit < ibpCellResetLimit) ||
        (ibpPktSetLimit < ibpPktResetLimit) ||
        (holCosPktSetLimit < holCosPktResetLimit) ||
        (dynCellLimit < dynCellResetLimit))
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                    "Warning: Skipping MMU config for unit/port %d/%d as MMU limits"
                    " are incorrectly configured. \n",
                    unit, port);
      continue;        
    }

    /* Program all registers */
    regVal = 0;
    SOC_IF_ERROR_RETURN(READ_IBPCELLSETLIMITr(unit, port, &regVal));
    soc_reg_field_set(unit, IBPCELLSETLIMITr, &regVal, CELLSETLIMITf, ibpCellSetLimit);
    soc_reg_field_set(unit, IBPCELLSETLIMITr, &regVal, RESETLIMITSELf,
                      FP_RAPTOR_MMU_RESET_LIMIT_SEL);

    SOC_IF_ERROR_RETURN(WRITE_IBPCELLSETLIMITr(unit, port, regVal));
   
    regVal = 0;
    SOC_IF_ERROR_RETURN(READ_IBPPKTSETLIMITr(unit, port, &regVal));

    soc_reg_field_set(unit, IBPPKTSETLIMITr, &regVal, PKTSETLIMITf, ibpPktSetLimit);
    soc_reg_field_set(unit, IBPPKTSETLIMITr, &regVal, RESETLIMITSELf,
                      FP_RAPTOR_MMU_RESET_LIMIT_SEL);

    SOC_IF_ERROR_RETURN(WRITE_IBPPKTSETLIMITr(unit, port, regVal));
  
    regVal = 0;
    SOC_IF_ERROR_RETURN(READ_IBPDISCARDSETLIMITr(unit, port, &regVal));

    soc_reg_field_set(unit, IBPDISCARDSETLIMITr, &regVal, DISCARDSETLIMITf, 
                      totalNumOfCells-1);

    SOC_IF_ERROR_RETURN(WRITE_IBPDISCARDSETLIMITr(unit, port, regVal));

    for (cos = 0;cos < numCosq; cos++)
    {
      if (IS_CPU_PORT(unit, port))
      {
        lwmCosCellSetLimit = FP_RAPTOR_MMU_LWMLIMIT_PER_COS + cpuExtraCells[cos];
        lwmCosCellResetLimit = (lwmCosCellSetLimit * FP_RAPTOR_MMU_RESET_LIMIT_PERCENT) / (100);

        if (cos >= HAPI_BROAD_INGRESS_HIGH_PRIORITY_COS)
        {
          holCosPktSetLimit = FP_RAPTOR_MMU_HOL_CPU_COS_PKT_LIMIT;
          holCosPktResetLimit = (holCosPktSetLimit * FP_RAPTOR_MMU_RESET_LIMIT_PERCENT) / (100);
        }
      }
      else if (raptor48PortDevice == L7_TRUE)
      {
        /* Only give static cells to control pkt egress queues */
        if ((cos == HAPI_BROAD_EGRESS_HIGH_PRIORITY_COS) ||
            (cos == HAPI_BROAD_EGRESS_NORMAL_PRIORITY_COS))
        {
          lwmCosCellSetLimit = FP_RAPTOR_MMU_LWMLIMIT_PER_COS;
          lwmCosCellResetLimit = (lwmCosCellSetLimit * FP_RAPTOR_MMU_RESET_LIMIT_PERCENT) / (100);
        }
        else
        {
          lwmCosCellSetLimit = 0;
          lwmCosCellResetLimit = 0;
        }
          
      }
      else
      {
        lwmCosCellSetLimit = FP_RAPTOR_MMU_LWMLIMIT_PER_COS;
        lwmCosCellResetLimit = (lwmCosCellSetLimit * FP_RAPTOR_MMU_RESET_LIMIT_PERCENT) / (100);
      }

      regVal = 0;
      SOC_IF_ERROR_RETURN(READ_HOLCOSPKTSETLIMITr(unit, port, cos, &regVal));

      soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &regVal, PKTSETLIMITf,
                        holCosPktSetLimit);
      soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &regVal, RESETLIMITSELf,
                        FP_RAPTOR_MMU_RESET_LIMIT_SEL);

      SOC_IF_ERROR_RETURN(WRITE_HOLCOSPKTSETLIMITr(unit, port, cos, regVal));


      regVal = 0;
      SOC_IF_ERROR_RETURN(READ_LWMCOSCELLSETLIMITr(unit, port, cos, &regVal));

      soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &regVal, CELLSETLIMITf,
                        lwmCosCellSetLimit);
      soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &regVal, CELLRESETLIMITf,
                        lwmCosCellResetLimit);

      SOC_IF_ERROR_RETURN(WRITE_LWMCOSCELLSETLIMITr(unit, port, cos, regVal));

    }

    regVal = 0;
    SOC_IF_ERROR_RETURN(READ_DYNCELLLIMITr(unit, port, &regVal));

    soc_reg_field_set(unit, DYNCELLLIMITr, &regVal, DYNCELLSETLIMITf,
                      dynCellLimit);
    soc_reg_field_set(unit, DYNCELLLIMITr, &regVal, DYNCELLRESETLIMITf,
                      dynCellResetLimit);

    SOC_IF_ERROR_RETURN(WRITE_DYNCELLLIMITr(unit, port, regVal));
  } 

  regVal = 0;
  SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &regVal));
  soc_reg_field_set(unit, MISCCONFIGr, &regVal, HOL_CELL_SOP_DROP_ENf, 1);
  SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, regVal));

  return BCM_E_NONE;
}
#endif



#ifdef BCM_SCORPION_SUPPORT


/*********************************************************************
* @purpose  Calculate the shared limits for scorpion 
*
* @param    unit                    The bcm unit on the local system
* @param    port                    The bcm port on the local system
* @param    total_shared_limit      The ingress shared limit
* @param    op_buffer_shared_limit  The egress shared limit
* @param    portParms               The configured ports parms
*
* @returns  
*
* @comments none
*           
*
* @end
*********************************************************************/
int hapiBroadScorpionSharedLimitsGet(int unit, int port,
                                     int *total_shared_limit, 
                                     int *op_buffer_shared_limit,
                                     usl_bcm_port_mmu_parms_t portParms)
{
    int                 port_iter, i;
    int                 total_cells;
    int                 global_hdrm_limit;
    int                 pg_hdrm = 0;
    int                 pg_min = 0;
    bcm_gport_t         gport;
    usl_bcm_port_mmu_parms_t mmuParms;
    int                 rc = BCM_E_NONE;
    L7_BOOL             pauseMode = L7_FALSE;

    if (L7_NULLPTR == total_shared_limit || L7_NULLPTR == op_buffer_shared_limit)
      return BCM_E_PARAM;

    /* calculate the total shared limit 
     * use the cache to calculate
     * total_cells - global_hdrm - pg_hdrm - port_min
     */
    total_cells = SCORPION_TOTAL_CELLS;
    global_hdrm_limit = NUM_GX_PORT(unit) * SCORPION_MMU_MTU_CELLS;

    for (port_iter = 0;port_iter < NUM_ALL_PORT(unit); port_iter++)
    {
      if (port == port_iter) 
      {
        /* for the port being configured, use the passed in parms 
         * otherwise, must get the parms for the port
         */
        mmuParms = portParms;
      }
      else if ((usl_bcm_unit_port_to_gport(unit, port_iter, &gport) != BCM_E_NONE) ||
          (gport == BCM_GPORT_INVALID))
      {
        continue;
      }
      else
      {
        /* get the settings of the port (pfc,pause,mtu) */
        rc = usl_portdb_mmu_parms_get(USL_CURRENT_DB,gport,&mmuParms);

        if (rc != BCM_E_NONE) continue;
      }

      if (mmuParms.pfc)
      {
        pauseMode = L7_TRUE;
        for (i=0;i < SCORPION_PG_MAX;i++)
        {
          if (mmuParms.pfc_bmp & (1 << i))
          {
            pg_hdrm += (((mmuParms.mtu * MMU_HDRM_MULTIPLIER)/128) + 1);
            pg_min  += SCORPION_PG_MIN_CELLS;
          } 
        }
      }
      else if (mmuParms.pause) 
      {
        pauseMode = L7_TRUE;
        /* only a single pg used when pause is enabled on the interface */
        pg_hdrm += (((mmuParms.mtu * MMU_HDRM_MULTIPLIER)/128) + 1);
      }
      else
      {
        /* No headroom needs to be calculated for this port */
      }
    }

    /* calculate the ingress shared limits */
    *total_shared_limit = total_cells;
    *total_shared_limit -= global_hdrm_limit;
    *total_shared_limit -= pg_hdrm;
    *total_shared_limit -= (NUM_ALL_PORT(unit) * SCORPION_MMU_PORT_MIN);
    *total_shared_limit -= pg_min;

    /* When in ingress drop mode, the ingress shared limit needs to account for
     * egress reservation. Otherwise output shared buffer limit could be hit
     * before the ingress xoff threshold is reached.
     */
    if (pauseMode == L7_TRUE)
    {
      *total_shared_limit -= (SCORPION_OP_QMIN_CELLS * NUM_ALL_PORT(unit) * 
                                SCORPION_MMU_NUM_COS);
    }

    /* calculate the output buffer shared limits */  
    *op_buffer_shared_limit = total_cells;
    *op_buffer_shared_limit -= (SCORPION_OP_QMIN_CELLS * NUM_ALL_PORT(unit) * 
                                SCORPION_MMU_NUM_COS);

    return BCM_E_NONE;
}


/*********************************************************************
* @purpose  Modify any MMU for the port based on the port db
*
* @param    unit      The bcm unit on the local system
* @param    port      The bcm port on the local system
* @param    mmuParms  The pause,pfc,and mtu settings of the port
*
* @returns  defined by BCM calls
*
* @comments none
*           
*
* @end
*********************************************************************/
int hapiBroadMmuScorpionPortUpdate(int unit, int port, 
                                   usl_bcm_port_mmu_parms_t mmuParms)
{
  /* calculate the ports resources and the global resources based on cache */
  int hdrm;
  uint32              rval,rval2,rval3;
  int                 i;
  int                 total_shared_limit,op_buffer_shared_limit;
  int                 num_pg;
  int                 prev_port_pri_grp0=0,prev_port_pri_grp1=0;
  int                 new_port_pri_grp0=0,new_port_pri_grp1=0;
  pbmp_t              pbmp;
  int                 enable=0;
  int                 port_iter;

  SOC_PBMP_CLEAR(pbmp);

  hdrm = ((mmuParms.mtu * MMU_HDRM_MULTIPLIER) / 128) + 1;
  num_pg = SCORPION_PG_MAX;

  /* when a port changes, the shared limits must also change */
  hapiBroadScorpionSharedLimitsGet( unit,port,
                                    &total_shared_limit, 
                                    &op_buffer_shared_limit,
                                    mmuParms);
  
  /* Set the ports config */
  if (mmuParms.pause  && !mmuParms.pfc)
  {
    rval3 = 0;
    SOC_IF_ERROR_RETURN(WRITE_PG_THRESH_SELr(unit, port, rval3));

    SOC_IF_ERROR_RETURN(READ_PORT_PRI_GRP0r(unit, port, &prev_port_pri_grp0));
    SOC_IF_ERROR_RETURN(READ_PORT_PRI_GRP1r(unit, port, &prev_port_pri_grp1));

    /* only a single pg used, pg7 
     * set the headroom for PG7 and zero the rest
     */ 
    new_port_pri_grp0 = 0;
    soc_reg_field_set(unit, PORT_PRI_GRP0r, &new_port_pri_grp0, 
                      PRI0_GRPf, num_pg - 1);
    soc_reg_field_set(unit, PORT_PRI_GRP0r, &new_port_pri_grp0, 
                      PRI1_GRPf, num_pg - 1);
    soc_reg_field_set(unit, PORT_PRI_GRP0r, &new_port_pri_grp0, 
                      PRI2_GRPf, num_pg - 1);
    soc_reg_field_set(unit, PORT_PRI_GRP0r, &new_port_pri_grp0, 
                      PRI3_GRPf, num_pg - 1);
    soc_reg_field_set(unit, PORT_PRI_GRP0r, &new_port_pri_grp0, 
                      PRI4_GRPf, num_pg - 1);
    soc_reg_field_set(unit, PORT_PRI_GRP0r, &new_port_pri_grp0, 
                      PRI5_GRPf, num_pg - 1);
    soc_reg_field_set(unit, PORT_PRI_GRP0r, &new_port_pri_grp0, 
                      PRI6_GRPf, num_pg - 1);

    new_port_pri_grp1 = 0;
    soc_reg_field_set(unit, PORT_PRI_GRP1r, &new_port_pri_grp1, 
                      PRI7_GRPf, num_pg - 1);
    soc_reg_field_set(unit, PORT_PRI_GRP1r, &new_port_pri_grp1, 
                      PRI8_GRPf, num_pg - 1);
    soc_reg_field_set(unit, PORT_PRI_GRP1r, &new_port_pri_grp1, 
                      PRI9_GRPf, num_pg - 1);
    soc_reg_field_set(unit, PORT_PRI_GRP1r, &new_port_pri_grp1, 
                      PRI10_GRPf, num_pg - 1);
    soc_reg_field_set(unit, PORT_PRI_GRP1r, &new_port_pri_grp1, 
                      PRI11_GRPf, num_pg - 1);
    soc_reg_field_set(unit, PORT_PRI_GRP1r, &new_port_pri_grp1, 
                      PRI12_GRPf, num_pg - 1);
    soc_reg_field_set(unit, PORT_PRI_GRP1r, &new_port_pri_grp1, 
                      PRI13_GRPf, num_pg - 1);

    if ((new_port_pri_grp1 != prev_port_pri_grp1) || 
        (new_port_pri_grp0 != prev_port_pri_grp0))
    {
      /* when changing this register, we must insure that traffic is not
       * flowing in the system.  For now, the only way to handle this is via
       * the disable port.  Since the port may already be disabled, we must
       * attempt to set the port back to the previous value by checking the
       * value first.
       */ 
      PBMP_ALL_ITER(unit, port_iter) {
        if (bcm_port_enable_get(unit,port_iter,&enable) == BCM_E_NONE)
        {
          if (enable) 
          {
            SOC_PBMP_PORT_ADD(pbmp, port_iter);
            bcm_port_enable_set(unit,port_iter,FALSE);
          }
        }
      }

      SOC_IF_ERROR_RETURN(WRITE_PORT_PRI_GRP0r(unit, port, new_port_pri_grp0));
      SOC_IF_ERROR_RETURN(WRITE_PORT_PRI_GRP1r(unit, port, new_port_pri_grp1));

      if (SOC_PBMP_NOT_NULL(pbmp))
      {
        PBMP_ITER(pbmp, port_iter) {
          bcm_port_enable_set(unit,port_iter,TRUE);
        }
      }

    }   

    /* Write the HDRM for the port, only last PG will have hdrm */
    rval = rval2 = rval3 = 0;
    soc_reg_field_set(unit, PG_HDRM_LIMITr, &rval, PG_HDRM_LIMITf, 0);
    soc_reg_field_set(unit, PG_HDRM_LIMITr, &rval, PG_GEf, 0);

    /* don't need any pg_min, just rely on the port min */
    soc_reg_field_set(unit, PG_MINr,&rval2,PG_MINf,0);
    
    for (i=0;i < (num_pg - 1);i++)
    {
      SOC_IF_ERROR_RETURN(WRITE_PG_HDRM_LIMITr(unit, port, i, rval));
      SOC_IF_ERROR_RETURN(WRITE_PG_MINr(unit, port, i, rval2));
      SOC_IF_ERROR_RETURN(WRITE_PG_RESET_OFFSETr(unit, port, i, 0));
    }

    soc_reg_field_set(unit, PG_HDRM_LIMITr, &rval, PG_HDRM_LIMITf, hdrm);
    soc_reg_field_set(unit, PG_HDRM_LIMITr, &rval, PG_GEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_PG_HDRM_LIMITr(unit, port, num_pg-1, rval));

    /* PG7 is in use, the field is offset, therefore, 2 is actually 32 cells */
    soc_reg_field_set(unit,PG_RESET_OFFSETr,&rval3,PG_RESET_OFFSETf,2);
    SOC_IF_ERROR_RETURN(WRITE_PG_RESET_OFFSETr(unit, port, num_pg-1, rval3));

    /* Set the ingress dynamic threshold */
    rval = 0;
    soc_reg_field_set(unit, PORT_SHARED_LIMITr, &rval,
                      PORT_SHARED_LIMITf, SCORPION_PORT_SHARED_ALPHA);
    soc_reg_field_set(unit, PORT_SHARED_LIMITr, &rval,
                      PORT_SHARED_DYNAMICf, 1);

    (void)WRITE_PORT_SHARED_LIMITr(unit, port, rval);   

    /* disable the priorities for pause */
    rval = 0;
    soc_reg_field_set(unit, PORT_PRI_XON_ENABLEr, &rval,
                      PORT_PRI_XON_ENABLEf, 
                      0 );   

    soc_reg_field_set(unit, PORT_PRI_XON_ENABLEr, &rval,
                      PORT_PG7PAUSE_DISABLEf, 0);   

    SOC_IF_ERROR_RETURN(WRITE_PORT_PRI_XON_ENABLEr(unit, port, rval));
  }
  else if (mmuParms.pfc)
  {
    rval3 = 0;
    soc_reg_field_set(unit, PG_THRESH_SELr, &rval3, 
                      PG0_THRESH_SELf, 0x5);
    soc_reg_field_set(unit, PG_THRESH_SELr, &rval3, 
                      PG1_THRESH_SELf, 0x6);
    soc_reg_field_set(unit, PG_THRESH_SELr, &rval3, 
                      PG2_THRESH_SELf, 0x7);
    soc_reg_field_set(unit, PG_THRESH_SELr, &rval3, 
                      PG3_THRESH_SELf, 0x7);
    soc_reg_field_set(unit, PG_THRESH_SELr, &rval3, 
                      PG4_THRESH_SELf, 0x7);
    soc_reg_field_set(unit, PG_THRESH_SELr, &rval3, 
                      PG5_THRESH_SELf, 0x8);
    soc_reg_field_set(unit, PG_THRESH_SELr, &rval3, 
                      PG6_THRESH_SELf, 0x9);

    SOC_IF_ERROR_RETURN(WRITE_PG_THRESH_SELr(unit, port, rval3));

    SOC_IF_ERROR_RETURN(READ_PORT_PRI_GRP0r(unit, port, &prev_port_pri_grp0));
    SOC_IF_ERROR_RETURN(READ_PORT_PRI_GRP1r(unit, port, &prev_port_pri_grp1));

    /* Make it one to one for PRI to PG and set the headroom*/
    new_port_pri_grp0 = 0;
    soc_reg_field_set(unit, PORT_PRI_GRP0r, &new_port_pri_grp0, PRI0_GRPf, 0);
    soc_reg_field_set(unit, PORT_PRI_GRP0r, &new_port_pri_grp0, PRI1_GRPf, 1);
    soc_reg_field_set(unit, PORT_PRI_GRP0r, &new_port_pri_grp0, PRI2_GRPf, 2);
    soc_reg_field_set(unit, PORT_PRI_GRP0r, &new_port_pri_grp0, PRI3_GRPf, 3);
    soc_reg_field_set(unit, PORT_PRI_GRP0r, &new_port_pri_grp0, PRI4_GRPf, 4);
    soc_reg_field_set(unit, PORT_PRI_GRP0r, &new_port_pri_grp0, PRI5_GRPf, 5);
    soc_reg_field_set(unit, PORT_PRI_GRP0r, &new_port_pri_grp0, PRI6_GRPf, 6);

    new_port_pri_grp1 = 0;
    soc_reg_field_set(unit, PORT_PRI_GRP1r, &new_port_pri_grp1, PRI7_GRPf, 7);
    soc_reg_field_set(unit, PORT_PRI_GRP1r, &new_port_pri_grp1, PRI8_GRPf, 7);
    soc_reg_field_set(unit, PORT_PRI_GRP1r, &new_port_pri_grp1, PRI9_GRPf, 7);
    soc_reg_field_set(unit, PORT_PRI_GRP1r, &new_port_pri_grp1, PRI10_GRPf,7);
    soc_reg_field_set(unit, PORT_PRI_GRP1r, &new_port_pri_grp1, PRI11_GRPf,7);
    soc_reg_field_set(unit, PORT_PRI_GRP1r, &new_port_pri_grp1, PRI12_GRPf,7);
    soc_reg_field_set(unit, PORT_PRI_GRP1r, &new_port_pri_grp1, PRI13_GRPf,7);

    if ((new_port_pri_grp1 != prev_port_pri_grp1) || 
        (new_port_pri_grp0 != prev_port_pri_grp0))
    {
      /* when changing this register, we must insure that traffic is not
       * flowing in the system.  For now, the only way to handle this is via
       * the disable port.  Since the port may already be disabled, we must
       * attempt to set the port back to the previous value by checking the
       * value first.
       */ 
      PBMP_ALL_ITER(unit, port_iter) {
        if (bcm_port_enable_get(unit,port_iter,&enable) == BCM_E_NONE)
        {
          if (enable) 
          {
            SOC_PBMP_PORT_ADD(pbmp, port_iter);
            bcm_port_enable_set(unit,port_iter,FALSE);
          }
        }
      }

      SOC_IF_ERROR_RETURN(WRITE_PORT_PRI_GRP0r(unit, port, new_port_pri_grp0));
      SOC_IF_ERROR_RETURN(WRITE_PORT_PRI_GRP1r(unit, port, new_port_pri_grp1));

      if (SOC_PBMP_NOT_NULL(pbmp))
      {
        PBMP_ITER(pbmp, port_iter) {
          bcm_port_enable_set(unit,port_iter,TRUE);
        }
      }

    }   

    /* enable the specific priorities for pause */
    rval = 0;
    soc_reg_field_set(unit, PORT_PRI_XON_ENABLEr, &rval,
                      PORT_PRI_XON_ENABLEf, 
                      mmuParms.pfc_bmp & 0xff );   

    soc_reg_field_set(unit, PORT_PRI_XON_ENABLEr, &rval,
                      PORT_PG7PAUSE_DISABLEf, 1);   

    SOC_IF_ERROR_RETURN(WRITE_PORT_PRI_XON_ENABLEr(unit, port, rval));

    /* Set the PG_HDRM for each of the PG */
    for (i=0;i < num_pg;i++)
    {
      rval = rval2 = rval3 = 0;
      if ( (1 << i) & ( mmuParms.pfc_bmp & 0xff))  
      {
        soc_reg_field_set(unit, PG_HDRM_LIMITr, &rval, PG_HDRM_LIMITf, hdrm);
        soc_reg_field_set(unit, PG_HDRM_LIMITr, &rval, PG_GEf, 1);

        /* in order to prevent pfc packets going on an interface that does
         * not have any of the priority traffic, we must set the PG_MIN to 
         * a non-zero value.  The thresholds are calculated for all groups
         * regardless of the existence of traffic for that group.
         */ 
        soc_reg_field_set(unit, PG_MINr,&rval2,PG_MINf,SCORPION_PG_MIN_CELLS);
        soc_reg_field_set(unit,PG_RESET_OFFSETr,&rval3,PG_RESET_OFFSETf,2);
      }
      else
      {
        soc_reg_field_set(unit, PG_HDRM_LIMITr, &rval, PG_HDRM_LIMITf, 0);
        soc_reg_field_set(unit, PG_HDRM_LIMITr, &rval, PG_GEf, 0);
        soc_reg_field_set(unit, PG_MINr,&rval2,PG_MINf,0);
      }
      SOC_IF_ERROR_RETURN(WRITE_PG_HDRM_LIMITr(unit, port, i, rval));
      SOC_IF_ERROR_RETURN(WRITE_PG_MINr(unit, port, i, rval2));
      SOC_IF_ERROR_RETURN(WRITE_PG_RESET_OFFSETr(unit, port, i, rval3));
    }
     
    /* Set the ingress dynamic threshold */
    rval = 0;
    soc_reg_field_set(unit, PORT_SHARED_LIMITr, &rval,
                      PORT_SHARED_LIMITf, SCORPION_PORT_SHARED_ALPHA);
    soc_reg_field_set(unit, PORT_SHARED_LIMITr, &rval,
                      PORT_SHARED_DYNAMICf, 1);

    (void)WRITE_PORT_SHARED_LIMITr(unit, port, rval);     
  }
  else
  {
    /* Assuming that Pause was disabled on the port, just setting mmu */
    /* Disable the headroom in order to free more memory */
    rval = rval2 = rval3 = 0;
    soc_reg_field_set(unit, PG_HDRM_LIMITr, &rval, 
                      PG_HDRM_LIMITf, 0);
    soc_reg_field_set(unit, PG_HDRM_LIMITr, &rval, 
                      PG_GEf, 0);

    soc_reg_field_set(unit, PG_MINr,&rval2,PG_MINf,0);

    for (i=0;i < num_pg;i++)
    {
      SOC_IF_ERROR_RETURN(WRITE_PG_HDRM_LIMITr(unit, port, i, rval));
      SOC_IF_ERROR_RETURN(WRITE_PG_MINr(unit, port, i, rval2));
      SOC_IF_ERROR_RETURN(WRITE_PG_RESET_OFFSETr(unit, port, i, rval3));
    }

    /* disable the priorities for pause */
    rval = rval3 = 0;
    soc_reg_field_set(unit, PORT_PRI_XON_ENABLEr, &rval,
                      PORT_PRI_XON_ENABLEf, 
                      0 );   

    soc_reg_field_set(unit, PORT_PRI_XON_ENABLEr, &rval,
                      PORT_PG7PAUSE_DISABLEf, 0);   

    SOC_IF_ERROR_RETURN(WRITE_PORT_PRI_XON_ENABLEr(unit, port, rval));
  }

  /* mod total share to reflect pg_hdrm cells */
  rval = 0;
  soc_reg_field_set(unit, TOTAL_SHARED_LIMITr, &rval,
                    TOTAL_SHARED_LIMITf, total_shared_limit);
  SOC_IF_ERROR_RETURN(WRITE_TOTAL_SHARED_LIMITr(unit, rval));
  
  return BCM_E_NONE;
}



/*
 * Scorpion
 *
 * Configuration supports only a single priority group (PG7).
 */

/*********************************************************************
* @purpose  Modify MMU for Scorpion devices
*
* @param    unit - bcm unit number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
int hapiBroadScorpionMmuModify(L7_uint32 unit)
{
    uint32              rval, rval2, rval_cpu, wr_val;
    uint32              field_max;
    uint32              pg[SCORPION_PG_MAX];
    int                 port, i;
    int                 num_ports, num_cos;
    int                 total_cells;
    int                 global_hdrm_limit;
    int                 total_shared_limit;
    int                 q_min;
    int                 op_buffer_shared_limit;
    int                 num_pg;
    int                 op_shared_limit_disable;

    num_pg = SCORPION_PG_MAX;
    op_shared_limit_disable = SCORPION_OP_SHARED_LIMIT_DISABLE;

    /* Number of port to configure */
    num_ports = NUM_ALL_PORT(unit);

    /* Number of COS queues */
    num_cos = SCORPION_MMU_NUM_COS;

    /* Total number of cells */
    total_cells = SCORPION_TOTAL_CELLS;

    /*
     * Ingress based threshholds
     */

    /* Enough to absorb in-flight data during PAUSE response */
    /* 3 * Jumbo */

    /* Per-device limit: 1 Ethernet MTU per port */
    global_hdrm_limit = num_ports * SCORPION_MMU_MTU_CELLS;

    /* Use whatever is left over for shared cells */
    total_shared_limit = total_cells;
    total_shared_limit -= global_hdrm_limit;

    /* Not including the CPU port since it is not participating in Pause */
    total_shared_limit -= (NUM_PORT(unit) * SCORPION_PG_HDRM_LIMIT); 

    /* Not including the CPU port in the calculation.  The CPU port will be set
     * to the max in order to prevent flow control on the CPU port
     */
    total_shared_limit -= (NUM_PORT(unit) * SCORPION_MMU_PORT_MIN);  

    /* Set aside some memory for the CPU so that the shared pool can't get to it */
    total_shared_limit -= SCORPION_CPU_RESERVED_CELLS;

    rval = 0;
    soc_reg_field_set(unit, TOTAL_SHARED_LIMITr, &rval,
                      TOTAL_SHARED_LIMITf, total_shared_limit);
    SOC_IF_ERROR_RETURN(WRITE_TOTAL_SHARED_LIMITr(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, GLOBAL_HDRM_LIMITr, &rval, 
                      GLOBAL_HDRM_LIMITf, global_hdrm_limit);
    SOC_IF_ERROR_RETURN(WRITE_GLOBAL_HDRM_LIMITr(unit, rval));


    /* CPU values */
    rval_cpu = 0;
    SOC_IF_ERROR_RETURN(READ_PORT_PRI_XON_ENABLEr(unit, CMIC_PORT(unit), &rval_cpu));
    soc_reg_field_set(unit, PORT_PRI_XON_ENABLEr, &rval_cpu, PORT_PG7PAUSE_DISABLEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_PORT_PRI_XON_ENABLEr(unit, CMIC_PORT(unit), rval_cpu));

    rval = 0;
    rval_cpu = 0; 
    field_max = ( 1 << soc_reg_field_length(unit,PORT_MINr,PORT_MINf)) - 1;
    soc_reg_field_set(unit, PORT_MINr, &rval_cpu, PORT_MINf, field_max);

    /* need to account for the fact that some don't support full range on PORT_MINf */
    if (soc_reg_field_valid(unit, PORT_MINr, PORT_MIN_MSBf)) {
      field_max = ( 1 << soc_reg_field_length(unit,PORT_MINr,PORT_MIN_MSBf)) - 1;
      soc_reg_field_set(unit, PORT_MINr, &rval_cpu, PORT_MIN_MSBf, field_max);
    } 

    soc_reg_field_set(unit, PORT_MINr, &rval, PORT_MINf, SCORPION_MMU_PORT_MIN);

    PBMP_ALL_ITER(unit, port) {
      if (IS_CPU_PORT(unit, port)) 
      {
        SOC_IF_ERROR_RETURN(WRITE_PORT_MINr(unit, port, rval_cpu));
      }
      else
      {
        SOC_IF_ERROR_RETURN(WRITE_PORT_MINr(unit, port, rval));
    }
    }

    /* set the CPU port min to the max in order to prevent Pause on CPU */
    

    rval = 0;
    rval_cpu = 0; 
    /* The following values may change when link is established */
    soc_reg_field_set(unit, PORT_SHARED_LIMITr, &rval, 
                      PORT_SHARED_LIMITf, op_shared_limit_disable);
    soc_reg_field_set(unit, PORT_SHARED_LIMITr, &rval, 
                      PORT_SHARED_DYNAMICf, 0);

    field_max = ( 1 << soc_reg_field_length(unit,PORT_SHARED_LIMITr,PORT_SHARED_LIMITf)) - 1;
    soc_reg_field_set(unit, PORT_SHARED_LIMITr, &rval_cpu, 
                      PORT_SHARED_LIMITf, field_max);
    soc_reg_field_set(unit, PORT_SHARED_LIMITr, &rval_cpu, 
                      PORT_SHARED_DYNAMICf, 0);
    PBMP_ALL_ITER(unit, port) {
      if (IS_CPU_PORT(unit, port)) 
      {
        SOC_IF_ERROR_RETURN(WRITE_PORT_SHARED_LIMITr(unit, port, rval_cpu));
      }
      else
      {
        SOC_IF_ERROR_RETURN(WRITE_PORT_SHARED_LIMITr(unit, port, rval));
      }
    }

    /* Note that only PG max is being used */
    for (i = 0; i < num_pg; i++) {
        pg[i] = 0;
        if (i == (num_pg - 1)) {
            soc_reg_field_set(unit, PG_HDRM_LIMITr, &pg[i], 
                              PG_HDRM_LIMITf, SCORPION_PG_HDRM_LIMIT);
            soc_reg_field_set(unit, PG_HDRM_LIMITr, &pg[i], 
                              PG_GEf, 1);
        } else {
            soc_reg_field_set(unit, PG_HDRM_LIMITr, &pg[i], 
                              PG_HDRM_LIMITf, 0);
            soc_reg_field_set(unit, PG_HDRM_LIMITr, &pg[i], 
                              PG_GEf, 0);
        }
    }

    /* A0 Scorpion requires this change in order to give the CPU extra transmit when 
     * the total_shared_pool is empty
     */
    rval_cpu = 0;
    field_max = ( 1 << soc_reg_field_length(unit,PG_HDRM_LIMITr,PG_HDRM_LIMITf)) - 1;
    soc_reg_field_set(unit, PG_HDRM_LIMITr, &rval_cpu, PG_HDRM_LIMITf, field_max);
    soc_reg_field_set(unit, PG_HDRM_LIMITr, &rval_cpu, PG_GEf, 1);

    PBMP_ALL_ITER(unit, port) {
        for (i = 0; i < num_pg; i++) {
            if (IS_CPU_PORT(unit, port) && (i == (num_pg - 1))) {
                wr_val = rval_cpu;
            } else {
                wr_val = pg[i];
            }
            SOC_IF_ERROR_RETURN
                (WRITE_PG_HDRM_LIMITr(unit, port, i, wr_val));
        }
    }

    /* 
     * Output queue threshold settings
     */

    q_min = SCORPION_OP_QMIN_CELLS;

    op_buffer_shared_limit = total_cells;
    op_buffer_shared_limit -= (q_min * num_ports * num_cos);

    /* leave room for cpu */
    op_buffer_shared_limit -= SCORPION_CPU_RESERVED_CELLS;

    rval = 0;
    soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMITr, &rval, 
                      OP_BUFFER_SHARED_LIMITf, op_buffer_shared_limit);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMITr(unit, rval));

    /*
     * OP_SHARED_LIMIT should be ((op_buffer_shared_limit * 3) / 4)
     * OP_SHARED_RESET_VALUE should be (op_buffer_shared_limit / 4)
     *
     * Since PORT_LIMIT_ENABLE does not work, the above limits
     * can be disabled by configuring values larger than the
     * OP_BUFFER_SHARED_LIMIT value.
     */
    rval = 0;
    soc_reg_field_set(unit, OP_PORT_CONFIGr, &rval, 
                      OP_SHARED_LIMITf, op_shared_limit_disable);
    soc_reg_field_set(unit, OP_PORT_CONFIGr, &rval, 
                      OP_SHARED_RESET_VALUEf, op_shared_limit_disable);
    soc_reg_field_set(unit, OP_PORT_CONFIGr, &rval, 
                      PORT_LIMIT_ENABLEf, 0);
    rval_cpu = 0;
    soc_reg_field_set(unit, OP_PORT_CONFIGr, &rval_cpu, 
                      OP_SHARED_LIMITf, op_shared_limit_disable);
    soc_reg_field_set(unit, OP_PORT_CONFIGr, &rval_cpu, 
                      OP_SHARED_RESET_VALUEf, 0x733);
    soc_reg_field_set(unit, OP_PORT_CONFIGr, &rval_cpu, 
                      PORT_LIMIT_ENABLEf, 0);
    PBMP_MMU_ITER(unit, port) {
        if (IS_CPU_PORT(unit, port)) {
            wr_val = rval_cpu;
        } else {
            wr_val = rval;
        }
        SOC_IF_ERROR_RETURN(WRITE_OP_PORT_CONFIGr(unit, port, wr_val));
    }

    rval = 0;
    rval2 = 0;
    if (soc_reg_field_valid(unit, OP_QUEUE_CONFIGr, Q_RESET_SELf)) {
        /* Make it 87.5 % of original threshold */
        soc_reg_field_set(unit, OP_QUEUE_CONFIGr, &rval, 
                          Q_RESET_SELf, 0x7);
    } else if (soc_reg_field_valid
               (unit, OP_QUEUE_RESET_OFFSETr, Q_RESET_OFFSETf)) {
        
        soc_reg_field_set(unit, OP_QUEUE_RESET_OFFSETr, &rval2, 
                          Q_RESET_OFFSETf,
                SCORPION_OP_QMIN_CELLS >> SCORPION_RESET_GRAN_SHIFT);
    }

    soc_reg_field_set(unit, OP_QUEUE_CONFIGr, &rval, 
                      Q_MINf, q_min);
    soc_reg_field_set(unit, OP_QUEUE_CONFIGr, &rval, 
                      Q_LIMIT_ENABLEf, 0x1);
    /* Use dynamic threshold limits */
    soc_reg_field_set(unit, OP_QUEUE_CONFIGr, &rval, 
                      Q_LIMIT_DYNAMICf, 0x1);
    /* Dynamic threshold limit is alpha of 4 */
    soc_reg_field_set(unit, OP_QUEUE_CONFIGr, &rval, 
                      Q_SHARED_LIMITf, SCORPION_OP_QCFG_ALPHA);

    /* Set the CPU to drop packets early so that it will not caues pause */
    rval_cpu = 0;
    soc_reg_field_set(unit, OP_QUEUE_CONFIGr, &rval_cpu, 
                      Q_SHARED_LIMITf, SCORPION_OP_QCFG_ALPHA_CPU);
    soc_reg_field_set(unit, OP_QUEUE_CONFIGr, &rval_cpu, 
                      Q_MINf, q_min);
    soc_reg_field_set(unit, OP_QUEUE_CONFIGr, &rval_cpu, 
                      Q_LIMIT_ENABLEf, 0x1);
    soc_reg_field_set(unit, OP_QUEUE_CONFIGr, &rval_cpu, 
                      Q_LIMIT_DYNAMICf, 0);

    if (soc_reg_field_valid(unit, OP_QUEUE_CONFIGr, Q_RESET_SELf)) {
        /* Make it 87.5 % of original threshold */
        soc_reg_field_set(unit, OP_QUEUE_CONFIGr, &rval_cpu, 
                          Q_RESET_SELf, 0x7);
    } 

    PBMP_ALL_ITER(unit, port) {
        if (IS_CPU_PORT(unit, port)) {
            wr_val = rval_cpu;
        } else {
            wr_val = rval;
        }
        for (i = 0; i < num_cos; i++) {
            SOC_IF_ERROR_RETURN
                (WRITE_OP_QUEUE_CONFIGr(unit, port, i, wr_val));
            if (soc_reg_field_valid
                (unit, OP_QUEUE_RESET_OFFSETr, Q_RESET_OFFSETf)) {
                SOC_IF_ERROR_RETURN
                    (WRITE_OP_QUEUE_RESET_OFFSETr(unit, port, i, rval2));
            }
        }
    }

    return SOC_E_NONE;
}

/**********************************************************
 * @purpose  Modify MMU Scorpion ingress/egress settings for pause
 *
 * @param    unit - bcm unit number
 *
 * @returns  defined by sdk
 *
 * @comments 
 *
 * @end
 *
 **********************************************************/
int hapiBroadMmuScorpionPauseSet(int unit, int mode)
{
  uint32              rval;
  int                 port, i;
  int                 num_ports, num_cos;
  int                 total_cells;
  int                 global_hdrm_limit;
  int                 total_shared_limit;
  int                 q_min;
  int                 num_pg;

  num_pg = SCORPION_PG_MAX;
  
  /* Number of port to configure */
  num_ports = NUM_ALL_PORT(unit);

  /* Number of COS queues */
  num_cos = SCORPION_MMU_NUM_COS;

  /* Total number of cells */
  total_cells = SCORPION_TOTAL_CELLS;

  /* Per-device limit: 1 Ethernet MTU per port */
  global_hdrm_limit = num_ports * SCORPION_MMU_MTU_CELLS;

  total_shared_limit = total_cells;
  total_shared_limit -= global_hdrm_limit;
  total_shared_limit -= (NUM_PORT(unit) * SCORPION_MMU_PORT_MIN);

  /* leaving some headroom for cpu */
  total_shared_limit -= SCORPION_CPU_RESERVED_CELLS;

  q_min = SCORPION_OP_QMIN_CELLS;

  if (mode)
  {
    /* Must reduce the total shared pool due to allocation in hdrm */
    total_shared_limit -= (NUM_PORT(unit) * SCORPION_PG_HDRM_LIMIT);

    rval = 0;
    soc_reg_field_set(unit, TOTAL_SHARED_LIMITr, &rval,
                      TOTAL_SHARED_LIMITf, total_shared_limit );
    SOC_IF_ERROR_RETURN(WRITE_TOTAL_SHARED_LIMITr(unit, rval));

    /* Setting the port shared limit to use dynamic thresholds */
    rval = 0;
    soc_reg_field_set(unit, PORT_SHARED_LIMITr, &rval, 
                      PORT_SHARED_LIMITf, SCORPION_PORT_SHARED_ALPHA);
    soc_reg_field_set(unit, PORT_SHARED_LIMITr, &rval, 
                      PORT_SHARED_DYNAMICf, 1);
    
    PBMP_ALL_ITER(unit, port) {
        if (IS_CPU_PORT(unit,port)) continue;
        SOC_IF_ERROR_RETURN(WRITE_PORT_SHARED_LIMITr(unit, port, rval));
    }

    /* Setting the hdrm limit to 3*Jumbo for lossless pause */
    soc_reg_field_set(unit, PG_HDRM_LIMITr, &rval, 
                      PG_HDRM_LIMITf, SCORPION_PG_HDRM_LIMIT);
    soc_reg_field_set(unit, PG_HDRM_LIMITr, &rval, 
                      PG_GEf, 1);

    PBMP_ALL_ITER(unit, port) {
      if (IS_CPU_PORT(unit,port)) continue;
      SOC_IF_ERROR_RETURN(WRITE_PG_HDRM_LIMITr(unit, port, 7, rval));
    }

    /* set the queue limit to max */
    rval = 0;
    soc_reg_field_set(unit, OP_QUEUE_CONFIGr, &rval, 
                      Q_MINf, q_min);
    soc_reg_field_set(unit, OP_QUEUE_CONFIGr, &rval, 
                      Q_LIMIT_ENABLEf, 0);
    /* Use dynamic threshold limits */
    soc_reg_field_set(unit, OP_QUEUE_CONFIGr, &rval, 
                      Q_LIMIT_DYNAMICf, 0);
    /* Dynamic threshold limit is alpha of 4 */
    soc_reg_field_set(unit, OP_QUEUE_CONFIGr, &rval, 
                      Q_SHARED_LIMITf, total_cells - SCORPION_CPU_RESERVED_CELLS);

    PBMP_ALL_ITER(unit, port) {
      if (IS_CPU_PORT(unit,port)) continue;
      for (i = 0; i < num_cos; i++) {
        SOC_IF_ERROR_RETURN
            (WRITE_OP_QUEUE_CONFIGr(unit, port, i, rval));
        }
      }
  }
  else
  {
    /* setting the egress again */
    rval = 0;
    soc_reg_field_set(unit, OP_QUEUE_CONFIGr, &rval, 
                      Q_MINf, q_min);
    soc_reg_field_set(unit, OP_QUEUE_CONFIGr, &rval, 
                      Q_LIMIT_ENABLEf, 0x1);
    /* Use dynamic threshold limits */
    soc_reg_field_set(unit, OP_QUEUE_CONFIGr, &rval, 
                      Q_LIMIT_DYNAMICf, 0x1);
    /* Dynamic threshold limit is alpha of 4 */
    soc_reg_field_set(unit, OP_QUEUE_CONFIGr, &rval, 
                      Q_SHARED_LIMITf, SCORPION_OP_QCFG_ALPHA);

    PBMP_ALL_ITER(unit, port) {
        if (IS_CPU_PORT(unit,port)) continue;
        for (i = 0; i < num_cos; i++) {
            SOC_IF_ERROR_RETURN
                (WRITE_OP_QUEUE_CONFIGr(unit, port, i, rval));
            }
    }

    /* Disable the headroom in order to free more memory */
    soc_reg_field_set(unit, PG_HDRM_LIMITr, &rval, 
                      PG_HDRM_LIMITf, 0);
    soc_reg_field_set(unit, PG_HDRM_LIMITr, &rval, 
                      PG_GEf, 0);

    PBMP_ALL_ITER(unit, port) {
      if (IS_CPU_PORT(unit,port)) continue;
      SOC_IF_ERROR_RETURN(WRITE_PG_HDRM_LIMITr(unit, port, 7, rval));
    }

    /* don't use ingress limits when pause disabled */
    rval = 0;
    soc_reg_field_set(unit, PORT_SHARED_LIMITr, &rval, 
                      PORT_SHARED_LIMITf, total_cells - 1);
    soc_reg_field_set(unit, PORT_SHARED_LIMITr, &rval, 
                      PORT_SHARED_DYNAMICf, 0);

    PBMP_ALL_ITER(unit, port) {
      if (IS_CPU_PORT(unit,port)) continue;
        SOC_IF_ERROR_RETURN(WRITE_PORT_SHARED_LIMITr(unit, port, rval));
    }

    /* don't reserve the pg_hdrm cells witout pause */
    rval = 0;
    soc_reg_field_set(unit, TOTAL_SHARED_LIMITr, &rval,
                      TOTAL_SHARED_LIMITf, total_shared_limit);
    SOC_IF_ERROR_RETURN(WRITE_TOTAL_SHARED_LIMITr(unit, rval));
  }
  
  return BCM_E_NONE;
}

/*********************************************************************
* @purpose  Modify any MMU related to egress/ingress congestion control
*
* @param    unit      The bcm unit on the local system
* @param    port      The bcm port on the local system
* @param    mode      0 to disable, 1 to enable
*
* @returns  defined by BCM calls
*
* @comments none
*
*
* @end
*********************************************************************/
int hapiBroadMmuScorpionDropModeSet(int unit,int mode)
{
  int total_shared_limit,op_buffer_shared_limit;
  int rval,i,num_cos;
  int q_min,total_cells;
  int port;
  usl_bcm_port_mmu_parms_t mmuParms;

  /* Number of COS queues */
  num_cos = SCORPION_MMU_NUM_COS;

  /* when a port changes, the shared limits must also change */
  /* using negative one for the port because it doesn't exist */
  hapiBroadScorpionSharedLimitsGet( unit,-1,
                                    &total_shared_limit, 
                                    &op_buffer_shared_limit,
                                    mmuParms);

  total_cells = SCORPION_TOTAL_CELLS; 
  q_min = SCORPION_OP_QMIN_CELLS;

  if (mode == MMU_DROP_MODE_INGRESS)
  {
    /* set the queue limit to max */
    rval = 0;
    soc_reg_field_set(unit, OP_QUEUE_CONFIGr, &rval, 
                      Q_MINf, q_min);
    soc_reg_field_set(unit, OP_QUEUE_CONFIGr, &rval, 
                      Q_LIMIT_ENABLEf, 0);
    /* Use dynamic threshold limits */
    soc_reg_field_set(unit, OP_QUEUE_CONFIGr, &rval, 
                      Q_LIMIT_DYNAMICf, 0);
    /* Dynamic threshold limit is alpha of 4 */
    soc_reg_field_set(unit, OP_QUEUE_CONFIGr, &rval, 
                      Q_SHARED_LIMITf, total_cells - 1);

    PBMP_ALL_ITER(unit, port) {
      if (IS_CPU_PORT(unit,port)) continue;
      for (i = 0; i < num_cos; i++) {
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIGr(unit, port, i, rval));
      }
    }

    /* set each port to use ingress limits */
    /* Setting the port shared limit to use dynamic thresholds */
    rval = 0;
    soc_reg_field_set(unit, PORT_SHARED_LIMITr, &rval, 
                      PORT_SHARED_LIMITf, SCORPION_PORT_SHARED_ALPHA);
    soc_reg_field_set(unit, PORT_SHARED_LIMITr, &rval, 
                      PORT_SHARED_DYNAMICf, 1);
    PBMP_MMU_ITER(unit, port) {
      if (IS_CPU_PORT(unit,port)) continue;
      SOC_IF_ERROR_RETURN(WRITE_PORT_SHARED_LIMITr(unit, port, rval));
    }
  }
  else
  {
    /* setting the egress again */
    rval = 0;
    soc_reg_field_set(unit, OP_QUEUE_CONFIGr, &rval, 
                      Q_MINf, q_min);
    soc_reg_field_set(unit, OP_QUEUE_CONFIGr, &rval, 
                      Q_LIMIT_ENABLEf, 0x1);
    /* Use dynamic threshold limits */
    soc_reg_field_set(unit, OP_QUEUE_CONFIGr, &rval, 
                      Q_LIMIT_DYNAMICf, 0x1);
    /* Dynamic threshold limit is alpha of 4 */
    soc_reg_field_set(unit, OP_QUEUE_CONFIGr, &rval, 
                      Q_SHARED_LIMITf, SCORPION_OP_QCFG_ALPHA);

    PBMP_ALL_ITER(unit, port) {
      if (IS_CPU_PORT(unit,port)) continue;
      for (i = 0; i < num_cos; i++) {
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIGr(unit, port, i, rval));
      }
    }

    /* don't use ingress limits when pause disabled */
    rval = 0;
    soc_reg_field_set(unit, PORT_SHARED_LIMITr, &rval, 
                      PORT_SHARED_LIMITf, total_cells - 1);
    soc_reg_field_set(unit, PORT_SHARED_LIMITr, &rval, 
                      PORT_SHARED_DYNAMICf, 0);

    PBMP_MMU_ITER(unit, port) {
      if (IS_CPU_PORT(unit,port)) continue;
      SOC_IF_ERROR_RETURN(WRITE_PORT_SHARED_LIMITr(unit, port, rval));
    }
  }


  rval = 0;
  soc_reg_field_set(unit, TOTAL_SHARED_LIMITr, &rval,
                    TOTAL_SHARED_LIMITf, total_shared_limit);
  SOC_IF_ERROR_RETURN(WRITE_TOTAL_SHARED_LIMITr(unit, rval));

  return BCM_E_NONE;
}


/*********************************************************************
* @purpose  Modify MMU per FASTPATH requirements
*
* @param    unit - bcm unit number
* @param    port - port number
*
* @returns  none
*
* @comments Required due to the SDK modifying MMU settings when link changes
*
* @end
*********************************************************************/
void hapiBroadPhyMmuScorpionUpdate(int unit, int port, bcm_port_info_t *info)
{
  int rval;
  usl_bcm_port_mmu_parms_t mmuParms;
  int rc = BCM_E_NONE;
  bcm_gport_t   gport = BCM_GPORT_INVALID;

  osapiSemaTake(mmu_lock,L7_WAIT_FOREVER);

  rc = usl_bcm_unit_port_to_gport(unit, port, &gport);

  if ((rc == BCM_E_NONE) && 
      (gport != BCM_GPORT_INVALID))
  {   
    rc = usl_portdb_mmu_parms_get(USL_CURRENT_DB,gport,&mmuParms);
  }
  
  if (rc == BCM_E_NONE)
  {
    if (mmuParms.pause || mmuParms.pfc)
    {
      rc = hapiBroadMmuScorpionPortUpdate(unit, port, mmuParms);
    }
  }

  osapiSemaGive(mmu_lock);

  if (mmuParms.pfc)
  {
    /* 10G ports operating at 1G speed or less must have pause
     * enabled in order to send pfc packets
     */ 
    if (info->speed < 10000)
    {
      (void)bcm_port_pause_set(unit, port, 1,1);
    }
    else
    {
      (void)bcm_port_pause_set(unit, port, 0,0);
    }
  }

  rval = 0;
  soc_reg_field_set(unit, OP_PORT_CONFIGr, &rval, 
                    OP_SHARED_LIMITf, SCORPION_OP_SHARED_LIMIT_DISABLE);
  soc_reg_field_set(unit, OP_PORT_CONFIGr, &rval, 
                    OP_SHARED_RESET_VALUEf, SCORPION_OP_SHARED_LIMIT_DISABLE);
  soc_reg_field_set(unit, OP_PORT_CONFIGr, &rval, 
                    PORT_LIMIT_ENABLEf, 0);
  (void)WRITE_OP_PORT_CONFIGr(unit, port, rval);
}
#endif /* BCM_SCORPION_SUPPORT */


/*********************************************************************
* @purpose  Initialize data for the MMU manager
*
* @returns  BCM_E_MEMORY, BCM_E_NONE
*
* @comments None
*           
*
* @end
*********************************************************************/
int hapiBroadMmuInit(void)
{
  if ( L7_NULLPTR == mmu_lock)
  {
    mmu_lock = osapiSemaMCreate(OSAPI_SEM_Q_FIFO);
  }

  if ( L7_NULLPTR == mmu_lock )
  {
    return BCM_E_MEMORY;
  }

  return BCM_E_NONE;
}


/*********************************************************************
* @purpose  Modify MMU per FASTPATH requirements
*
* @param    unit - bcm unit number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments CPU CoS queues 4-7 will be given more cells
*
* @end
*********************************************************************/
L7_RC_t hapiBroadMmuConfigModify(L7_uint32 unit)
{
  int rc = BCM_E_NONE;

  /* If this platform requires custom MMU settings, call in to the platform's 
   * provided function. 
   * This mechanism allows individual customers to use settings different than 
   * what Fastpath uses. 
   */
  if (cnfgrIsFeaturePresent(L7_DTL_COMPONENT_ID, L7_DTL_CUSTOM_MMU_FEATURE_ID))
  { 
    if (HAPI_BROAD_MMU_CUSTOM_CONFIGURE(unit) == BCM_E_NONE)
    {
      return L7_SUCCESS;
    }
    else
    {
      return L7_ERROR;
    }
  }

#ifdef BCM_ROBO_SUPPORT
  return L7_SUCCESS; /* Presently it doesn't do anything for robo */
#else 
  if (SOC_IS_BRADLEY(unit) || SOC_IS_HAWKEYE(unit))
  {
    /* Dont modify on Raptor & Bradley */
    return L7_SUCCESS;
  }
#ifdef BCM_FIREBOLT_SUPPORT
  else if (SOC_IS_FB(unit) || SOC_IS_HELIX(unit))
  {
    rc = hapiBroadFbHxMmuModify(unit);
  }
#endif /* FIREBOLT/HELIX */
#ifdef BCM_SCORPION_SUPPORT
  else if (SOC_IS_SCORPION(unit))
  {
    rc = hapiBroadScorpionMmuModify(unit); 
  }
#endif /* SCORPION */
#ifdef BCM_TRIUMPH_SUPPORT
  else if (SOC_IS_TR_VL(unit) && !SOC_IS_ENDURO(unit) && !SOC_IS_TRIDENT(unit))  /* PTin modified */
  { 
    rc = hapiBroadTrVlMmuModify(unit);
  }
  else if (SOC_IS_ENDURO(unit) && !SOC_IS_TRIDENT(unit))  /* PTin modified */
  {
    rc = hapiBroadEnduroMmuModify(unit);
  }
  /* PTin added */
  else if (SOC_IS_TRIDENT(unit))
  {
    // TODO
    LOG_WARNING(LOG_CTX_MISC, "hapiBroadTridentMmuModify() is NOT IMPLEMENTED!");
    //rc = hapiBroadTridentMmuModify(unit);
  }
  /* PTin end */
#endif /* TRIUMPH */
#ifdef BCM_RAVEN_SUPPORT
  else if (SOC_IS_RAVEN(unit))
  {
    rc = hapiBroadRavenMmuModify(unit);
  }
#endif /* RAVEN */
#ifdef BCM_RAPTOR_SUPPORT
  else if (SOC_IS_RAPTOR(unit))
  {
    rc = hapiBroadRaptorMmuModify(unit);
  }
#endif
#endif /* !ROBO */

  if (rc != BCM_E_NONE) 
  {
    return L7_ERROR;
  }
  else 
  {
    return L7_SUCCESS;
  }
}

/* defaults are ignored if this becomes set with hapiBroadMmuPauseFunctionSet */
static hapiBroadMmuPauseCb_f hapiBroadCustomMmuPauseCb_g = L7_NULLPTR;

/**********************************************
 * @Purpose Set the custom MMU pause function instead of the default
 * 
 * @param   func  - the function to be registered
 * 
 * @comments  The registered function will be invoked whenever pause is
 *            enabled/disabled.  It should return one of the values from 
 *            bcm_error_t.  Typically BCM_E_NONE or BCM_E_FAIL.  The registered
 *            function should only modify registers that can be modified with
 *            traffic running.  
 * 
 * @returns none
 **********************************************/
void hapiBroadMmuPauseCbSet(hapiBroadMmuPauseCb_f func)
{
    hapiBroadCustomMmuPauseCb_g = func;
}

/*********************************************************************
*
* @purpose Make any MMU related mods when pause is set
* 
* @param  unit  - the bcm unit number
* @param  mode  - 0 to disable or 1 to enable pause
*
* @returns Broadcom driver defined
*
* @end
*
*********************************************************************/
int hapiBroadMmuPauseSet(int unit,int mode)
{
  int rc = BCM_E_NONE;

  system_flowcontrol_mode = mode;

  if (hapiBroadCustomMmuPauseCb_g != L7_NULLPTR)
  {
    rc = hapiBroadCustomMmuPauseCb_g(unit,mode);
  }
#ifdef BCM_TRIUMPH_SUPPORT
  else if (SOC_IS_ENDURO(unit))
  {
    rc=  hapiBroadMmuEnduroPauseSet(unit, mode);
  }
  else if (SOC_IS_TR_VL(unit) && !SOC_IS_TRIDENT(unit)) /* PTin modified: new switch BCM56843 */
  {
    rc = hapiBroadMmuTriumphPauseSet(unit, mode);
  }
  /* PTin added: new switch BCM56843 */
  else if (SOC_IS_TRIDENT(unit))
  {
    LOG_WARNING(LOG_CTX_MISC, "hapiBroadMmuTridentPauseSet() is not implemented!");
  }
  /* PTin end */
#endif
  else
  {
    rc = BCM_E_NONE;
  }

  return rc;
}

/*********************************************************************
*
* @purpose Enable flow control and set the mac if necessary
* 
* @returns Broadcom driver defined
*
* @end
*
*********************************************************************/
int hapiBroadFlowControlModeSet(L7_uint32 mode,L7_uchar8 *mac)
{
  int                        tmpRv=BCM_E_NONE,rv=BCM_E_NONE;
  L7_BOOL                    jam = L7_FALSE;
  int                        unit,port,autoneg, duplex;
  bcm_port_abil_t            ability_mask;
  L7_int32                   mac_pause_tx = 0, mac_pause_rx = 0;
  bcm_phy_config_t	         fiber_config;
  L7_RC_t                    rc=BCM_E_NONE;
  usl_bcm_port_pfc_config_t  pfcConfig;
  bcm_pbmp_t                 linkChangePbmp[L7_MAX_BCM_DEVICES_PER_UNIT];

  
  memset((void *)linkChangePbmp, 0, (L7_MAX_BCM_DEVICES_PER_UNIT) * sizeof(bcm_pbmp_t));

  mode = (mode)?1:0;

  osapiSemaTake(mmu_lock,L7_WAIT_FOREVER);
  for (unit = 0; unit < bde->num_devices(BDE_SWITCH_DEVICES); unit++)
  {
    if (!SOC_IS_XGS_FABRIC(unit))
    {
      /* Set the pause state of each of the ethernet ports on the unit */
      PBMP_E_ITER(unit,port)
      {  
        bcm_gport_t   gport = BCM_GPORT_INVALID;
        autoneg = duplex = 0;
        ability_mask = 0;

        rc = usl_bcm_unit_port_to_gport(unit, port, &gport);

        if ((rc == BCM_E_NONE) && (gport != BCM_GPORT_INVALID))
        {
          memset(&pfcConfig, 0, sizeof(pfcConfig));
          rc = usl_portdb_pfc_config_get(USL_CURRENT_DB, gport, &pfcConfig);
          /* if an error exists, treat the port as pfc disabled */
          if (rc != BCM_E_NONE) pfcConfig.mode = 0;
        }
        else
        {
          /* assuming pfc non-active, set the mode to 0 */
          pfcConfig.mode = 0;
        }

        /* PORT can't be in both flow control and PFC */
        if (!pfcConfig.mode)
        {        
          tmpRv = bcm_port_advert_get(unit, port, &ability_mask);
          if (tmpRv != BCM_E_NONE)
          {
            rv = tmpRv;
          }
          if (mode)
          {
            tmpRv = bcm_port_autoneg_get(unit, port, &autoneg);
            if (tmpRv != BCM_E_NONE)
              rv = tmpRv;

            if (autoneg)
            {
              ability_mask |= (BCM_PORT_ABIL_PAUSE_RX | BCM_PORT_ABIL_PAUSE_TX);
              /* Note: When port link turns UP,
              * If port is FD, it will resolve MAC PAUSE based on local and link partner advertisements
              *      OR
              * If port is HD, it will enable back pressure for port*/
            }
            else
            {
              bcm_port_duplex_get(unit, port, &duplex);
              if (duplex & BCM_PORT_DUPLEX_FULL)
              {
                /* Enable PAUSE and disable back pressure,  as port is full duplex */
                ability_mask |= (BCM_PORT_ABIL_PAUSE_RX|BCM_PORT_ABIL_PAUSE_TX);
                mac_pause_tx = 1;
                mac_pause_rx = 1;
                jam = L7_FALSE;
              }
              else
              {
                /* Disable PAUSE and enable pack pressure, as port is half duplex */
                ability_mask &= ~(BCM_PORT_ABIL_PAUSE_RX|BCM_PORT_ABIL_PAUSE_TX);
                mac_pause_tx = 0;
                mac_pause_rx = 0;
                jam = L7_TRUE;
              }
            }

            rc = bcm_port_medium_config_get (unit, port,
            BCM_PORT_MEDIUM_FIBER, &fiber_config);
            if (rc == BCM_E_NONE)
            {
              mac_pause_tx = 1;
              mac_pause_rx = 1;
              jam = L7_FALSE;
            }

          }/* else disable system flow control */
          else
          {
            /* Disable both PAUSE and back pressure for this port*/
            ability_mask &= ~(BCM_PORT_ABIL_PAUSE_RX | BCM_PORT_ABIL_PAUSE_TX);
            mac_pause_tx = 0;
            mac_pause_rx = 0;
            jam = L7_FALSE;
          }

          /* pause_addr_set is not supported for RoBo */
          if(hapiBroadRoboCheck() != L7_TRUE)
          {
            tmpRv = bcm_port_pause_addr_set(unit, port, mac);
            if (tmpRv != BCM_E_NONE)
              rv = tmpRv;
          }
          tmpRv = bcm_port_advert_set(unit, port, ability_mask);
          if (tmpRv != BCM_E_NONE)
            rv = tmpRv;

          if (!autoneg)
          {

            tmpRv = bcm_port_pause_set(unit, port, mac_pause_tx, mac_pause_rx);
            if (tmpRv != BCM_E_NONE)
            {
              rv = tmpRv;
            }

            tmpRv = bcm_port_jam_set(unit, port, jam);
            if ((tmpRv != BCM_E_NONE) && (tmpRv != BCM_E_UNAVAIL))
            {
              rv = tmpRv;
            }
            
            /* Force a link change on the port so that everything is updated. The
             * linkchange is done after the mmu_lock is given. Otherwise there
             * could be deadlock with bcmLink task.
             */
            BCM_PBMP_PORT_ADD(linkChangePbmp[unit], port);
          }

          tmpRv = hapiBroadMmuPortPauseSet( unit, port, mode );
          if ((tmpRv != BCM_E_NONE) && (tmpRv != BCM_E_UNAVAIL))
            rv = tmpRv;
        }
      }

      tmpRv = hapiBroadMmuPauseSet(unit,mode);
      if ((tmpRv != BCM_E_NONE) && (tmpRv != BCM_E_UNAVAIL))
      {
				rv = tmpRv;
      }

      /* Something on this unit failed, return now */
      if (rv != BCM_E_NONE) break;
    }
  }

  osapiSemaGive(mmu_lock);

  /* Force any link changes needed */
  for (unit = 0; unit < bde->num_devices(BDE_SWITCH_DEVICES); unit++) 
  {
    if (BCM_PBMP_NOT_NULL(linkChangePbmp[unit]))
    {
      (void)bcm_link_change(unit, linkChangePbmp[unit]);
    }
  }

  return rv;
}

/* defaults are ignored if this becomes set with hapiBroadMmuPhyLinkCbSet */
static hapiBroadMmuPhyLinkCb_f hapiBroadCustomMmuPhyLinkCb_g = L7_NULLPTR;

/*********************************************************************
* @purpose  Register a callback to perform MMU operations on Link change
*
* @param    unit - bcm unit number
* @param    port - the port
* @param    info - link information
*
* @returns  none
*
* @comments 
*
* @end
*********************************************************************/
void hapiBroadMmuPhyLinkCbSet(hapiBroadMmuPhyLinkCb_f func)
{
  hapiBroadCustomMmuPhyLinkCb_g = func;
}

/*********************************************************************
* @purpose  Modify the MMU on link callback.  Some chips may have MMU workarounds
*
* @param    unit - bcm unit number
* @param    port - the port
* @param    info - link information
*
* @returns  none
*
* @comments 
*
* @end
*********************************************************************/
void hapiBroadMmuLinkscanCb(int unit, soc_port_t port, bcm_port_info_t *info)
{
  if (hapiBroadCustomMmuPhyLinkCb_g != L7_NULLPTR)
  {
    hapiBroadCustomMmuPhyLinkCb_g(unit,port,info);
  }
#ifdef BCM_SCORPION_SUPPORT
  else if (SOC_IS_SCORPION(unit))
  {
    hapiBroadPhyMmuScorpionUpdate(unit, port,info);
  }
#endif
  return;
}


/*********************************************************************
* @purpose  Modify any MMU settings that rely on the MTU
*
* @param    unit    The bcm unit on the local system
* @param    port    The bcm port on the local system
* @param    mtu     The max frame size for the interface
*
* @returns  defined by BCM calls
*
* @comments none
*           
*
* @end
*********************************************************************/
int hapiBroadMmuPortMtuSet(int unit, int port, int mtu)
{
  int           rc = BCM_E_NONE;
  
#ifdef BCM_SCORPION_SUPPORT
  bcm_gport_t   gport = BCM_GPORT_INVALID;
  usl_bcm_port_mmu_parms_t mmuParms;

  osapiSemaTake(mmu_lock,L7_WAIT_FOREVER);

  if (SOC_IS_SCORPION(unit)) 
  {
    rc = usl_bcm_unit_port_to_gport(unit, port, &gport);

    if ((rc == BCM_E_NONE) && 
        (gport != BCM_GPORT_INVALID))
    {   
      rc = usl_portdb_mmu_parms_get(USL_CURRENT_DB,gport,&mmuParms);
    }
    
    if (rc == BCM_E_NONE)
    {
      /* overide the port db setting for the mtu setting */
      mmuParms.mtu = mtu;

      if (mmuParms.pause || mmuParms.pfc)
      {
        rc = hapiBroadMmuScorpionPortUpdate(unit, port, mmuParms);
      }
    }
  }

  osapiSemaGive(mmu_lock);
#endif

  return rc;
}


/*********************************************************************
* @purpose  Modify any MMU settings that rely on PFC 
*
* @param    unit      The bcm unit on the local system
* @param    port      The bcm port on the local system
* @param    enable    0 to disable, 1 to enable
* @param    pri_bmp   0 bitmap of the priorities particpating in pause
*
* @returns  defined by BCM calls
*
* @comments none
*           
*
* @end
*********************************************************************/
int hapiBroadMmuPortPfcSet(int unit, int port, int enable, int pri_bmp)
{
  int rc = BCM_E_UNAVAIL;

  if (!soc_feature(unit, soc_feature_priority_flow_control)) 
  {
    return BCM_E_UNAVAIL;
  }

#ifdef BCM_SCORPION_SUPPORT
  osapiSemaTake(mmu_lock,L7_WAIT_FOREVER);
  
  if (SOC_IS_SCORPION(unit)) 
  {
    usl_bcm_port_mmu_parms_t mmuParms;
    bcm_gport_t         gport;

    rc = usl_bcm_unit_port_to_gport(unit, port, &gport);

    if ((rc == BCM_E_NONE) && 
        (gport != BCM_GPORT_INVALID))
    {   
      rc = usl_portdb_mmu_parms_get(USL_CURRENT_DB,gport,&mmuParms);
    }
    
    if (rc == BCM_E_NONE)
    {
      /* overide the port db settings for the pfc settings */
      mmuParms.pfc = enable;
      mmuParms.pfc_bmp = pri_bmp;

      rc = hapiBroadMmuScorpionPortUpdate(unit, port, mmuParms);
    }
  }

  osapiSemaGive(mmu_lock);
#endif

  return rc;
}

/*********************************************************************
* @purpose  Modify any MMU settings that rely on Pause 
*
* @param    unit      The bcm unit on the local system
* @param    port      The bcm port on the local system
* @param    enable    0 to disable, 1 to enable
*
* @returns  defined by BCM calls
*
* @comments none
*           
*
* @end
*********************************************************************/
int hapiBroadMmuPortPauseSet(int unit, int port, int enable)
{
  int rc = BCM_E_NONE;
  

#ifdef BCM_SCORPION_SUPPORT
  osapiSemaTake(mmu_lock,L7_WAIT_FOREVER);

  if (SOC_IS_SCORPION(unit)) 
  {
    usl_bcm_port_mmu_parms_t mmuParms;
    bcm_gport_t         gport;

    rc = usl_bcm_unit_port_to_gport(unit, port, &gport);

    if ((rc == BCM_E_NONE) && 
        (gport != BCM_GPORT_INVALID))
    {   
      rc = usl_portdb_mmu_parms_get(USL_CURRENT_DB,gport,&mmuParms);
    }

    
    if (rc == BCM_E_NONE)
    {
      /* overide the port db setting for the pause setting */
      mmuParms.pause = enable;

      rc = hapiBroadMmuScorpionPortUpdate(unit, port, mmuParms);
    }
  }

  osapiSemaGive(mmu_lock);
#endif


  return rc;
}

/*********************************************************************
* @purpose  Modify any MMU related to egress/ingress congestion control
*
* @param    unit      The bcm unit on the local system
* @param    port      The bcm port on the local system
* @param    mode      MMU_DROP_MODE_EGRESS or MMU_DROP_MODE_INGRESS
*
* @returns  defined by BCM calls
*
* @comments none
*           
*
* @end
*********************************************************************/
int hapiBroadMmuDropModeSet(int mode)
{
  int rc = BCM_E_NONE,tmpRc = BCM_E_NONE;
  const bcm_sys_board_t *board_info;
  int num_units,unit;
  
  /* Store the mode, and set the drop mode if different */
  if (mode != mmu_drop_mode)
  {
    mmu_drop_mode = mode;
    board_info = hpcBoardGet();

    if ( L7_NULL == board_info ) 
    {
      return BCM_E_FAIL;
    }

    num_units = board_info->num_units;

    for (unit=0;unit < num_units;unit++)
    { 
#ifdef BCM_SCORPION_SUPPORT
      if (SOC_IS_SCORPION(unit)) 
      {
        tmpRc = hapiBroadMmuScorpionDropModeSet(unit,mode);
      }
#endif
    }
    if (tmpRc != BCM_E_NONE) 
    {
      rc = tmpRc;
    }
  }

  return rc;
}

/*
 * Get the MMU setting for a port or all ports (port = -1)
 DREG_PORT_ALL
 */
#define DREG_PORT_ALL -1
#define DEBUG_DELAY 40
extern DAPI_t *dapi_g;
L7_RC_t hapiBroadDebugMmuGet(int module, int port_num, int format)
{
  uint32                val, param;
  soc_port_t            port;
  pbmp_t                pbmp;
  int                   cos, index, bcos, findex;
  uint32                paramArray[6][8];
  int                   printParams;
  soc_field_t           field[2];
  char                 *fieldName[2];
  int                   GE_found = FALSE, XE_found = FALSE, HG_found = FALSE, CPU_found = FALSE;
  char                  *sepchar;

  if (!SOC_UNIT_VALID(module)){
    printf("\nImproper module specification.\n");
    return(L7_ERROR);
  }
  if (!SOC_IS_FB(module) && !SOC_IS_HELIX(module))
  {
    printf("Unsupported hardware for this MMU debug routine\n");
    return(L7_ERROR);
  }

  if (format) sepchar = ",";
  else sepchar = "";

  if (!IS_ALL_PORT(module, port_num)) {
    port_num = DREG_PORT_ALL;
  }

  printf("\nFlow Control:%s\t\t%s\n", sepchar,dapi_g->system->flowControlEnable == FALSE? "Off":"On");

  osapiSleepMSec(DEBUG_DELAY);
  /* E2E_CONFIG Register */
  val = 0;
  SOC_IF_ERROR_RETURN(READ_E2ECONFIGr(module, &val));
  printf("E2E Config:\n");
  osapiSleepMSec(DEBUG_DELAY);
  param = soc_reg_field_get(module, E2ECONFIGr, val, SEND_RX_E2E_BKP_ENf);
  printf("    Enable:%s\t\t%s\n",sepchar, param == 0? "Off":"On");
  osapiSleepMSec(DEBUG_DELAY);
  param = soc_reg_field_get(module, E2ECONFIGr, val, REMOTE_SRCMODIDf);
  printf("    Remote Module:%s\t%d\n",sepchar, param);
  osapiSleepMSec(DEBUG_DELAY);
  param = soc_reg_field_get(module, E2ECONFIGr, val, E2E_IBP_ENf);
  printf("    IBP Enable:%s\t\t%s\n",sepchar, param == 0? "Off":"On");
  osapiSleepMSec(DEBUG_DELAY);
  param = soc_reg_field_get(module, E2ECONFIGr, val, E2E_HOL_ENf);
  printf("    HOL Enable:%s\t\t%s\n",sepchar, param == 0? "Off":"On");
  osapiSleepMSec(DEBUG_DELAY);

  param = soc_reg_field_get(module, E2ECONFIGr, val, E2E_MAXTIMER_SELf);
  printf("    Max Timer:%s\t\t%d\t",sepchar, param);
  osapiSleepMSec(DEBUG_DELAY);
  if (param == 0) { printf("(diabled)\n"); }
  else if (param < 7) { printf("(%dus)\n", (8<<param)); }
  else { printf("(%dms)\n", (1<<(param-7))); }
  osapiSleepMSec(DEBUG_DELAY);
  param = soc_reg_field_get(module, E2ECONFIGr, val, E2E_MINTIMER_SELf);
  printf("    Min Timer:%s\t\t%d\t",sepchar, param);
  osapiSleepMSec(DEBUG_DELAY);
  if (param == 0) { printf("(diabled)\n"); }
  else if (param < 7) { printf("(%dus)\n", (8<<param)); }
  else { printf("(%dms)\n", (1<<(param-7))); }
  osapiSleepMSec(DEBUG_DELAY);

  param =  soc_reg_field_get( module,E2ECONFIGr,val,XPORT24_SEND_E2E_HOLf);
  if (param == 1) printf("    Port24 Send E2E HOL:%s\t\t%s\n",sepchar, "On");
  param =  soc_reg_field_get( module,E2ECONFIGr,val,XPORT25_SEND_E2E_HOLf);
  if (param == 1) printf("    Port25 Send E2E HOL:%s\t\t%s\n",sepchar, "On");
  param =  soc_reg_field_get( module,E2ECONFIGr,val,XPORT26_SEND_E2E_HOLf);
  if (param == 1) printf("    Port26 Send E2E HOL:%s\t\t%s\n",sepchar, "On");
  param =  soc_reg_field_get( module,E2ECONFIGr,val,XPORT27_SEND_E2E_HOLf);
  if (param == 1) printf("    Port27 Send E2E HOL:%s\t\t%s\n",sepchar, "On");

  param =  soc_reg_field_get( module,E2ECONFIGr,val,XPORT24_SEND_E2E_IBPf);
  if (param == 1) printf("    Port24 Send E2E IBP:%s\t\t%s\n",sepchar, "On");
  param =  soc_reg_field_get( module,E2ECONFIGr,val,XPORT25_SEND_E2E_IBPf);
  if (param == 1) printf("    Port25 Send E2E IBP:%s\t\t%s\n",sepchar, "On");
  param =  soc_reg_field_get( module,E2ECONFIGr,val,XPORT26_SEND_E2E_IBPf);
  if (param == 1) printf("    Port26 Send E2E IBP:%s\t\t%s\n",sepchar, "On");
  param =  soc_reg_field_get( module,E2ECONFIGr,val,XPORT27_SEND_E2E_IBPf);
  if (param == 1) printf("    Port27 Send E2E IBP:%s\t\t%s\n",sepchar, "On");

  /* Total Dynamic Cell Limit */
  if (SOC_IS_FIREBOLT2(module))
  {
    SOC_IF_ERROR_RETURN(READ_TOTALDYNCELLLIMITr(module, &val));
    printf("\nTotal Dyn Cell Limit:\n");
    param = soc_reg_field_get(module, TOTALDYNCELLLIMITr, val, SETLIMITf);
    printf("           Set Limit:%s\t\t%d%s\t\t0x%04X\n",sepchar, param,sepchar, param);
    osapiSleepMSec(DEBUG_DELAY);
    
    SOC_IF_ERROR_RETURN(READ_TOTALDYNCELLRESETLIMITr(module, &val));
    param = soc_reg_field_get(module, TOTALDYNCELLRESETLIMITr, val,RESETLIMITf);
    printf("         Reset Limit:%s\t\t%d%s\t\t0x%04X\n", sepchar,param,sepchar, param);
  }
  else
  {
    SOC_IF_ERROR_RETURN(READ_TOTALDYNCELLLIMITr(module, &val));
    printf("\nTotal Dyn Cell Limit:\n");
    param = soc_reg_field_get(module, TOTALDYNCELLLIMITr, val, TOTALDYNCELLLIMITf);
    printf("           Set Limit:%s\t\t%d%s\t\t0x%04X\n",sepchar, param,sepchar, param);

    if (soc_reg_field_valid(module,TOTALDYNCELLLIMITr , TOTALDYNCELLRESETLIMITSELf))
    {
      param = soc_reg_field_get(module, TOTALDYNCELLLIMITr, val, TOTALDYNCELLRESETLIMITSELf);
      printf("         Reset Limit Sel:%s\t\t%d%s\t\t0x%04X\n",sepchar, param, sepchar,param);
    }
    if (soc_reg_field_valid(module,TOTALDYNCELLLIMITr , TOTALDYNCELLRESETLIMITf))
    {
      param = soc_reg_field_get(module, TOTALDYNCELLLIMITr, val, TOTALDYNCELLRESETLIMITf);
      printf("         Reset Limit:%s\t\t%d%s\t\t0x%04X\n",sepchar, param,sepchar, param);
    }

  }
  osapiSleepMSec(DEBUG_DELAY);

  SOC_PBMP_CLEAR(pbmp);
  SOC_PBMP_OR(pbmp,PBMP_ST_ALL(module));
  if (DREG_PORT_ALL == port_num) {
    PBMP_ALL_ITER(module, port) {
      if ((FALSE == GE_found) && IS_GE_PORT(module, port)) {
        GE_found = TRUE;
        SOC_PBMP_PORT_ADD(pbmp, port);
      }
      if ((FALSE == XE_found) && IS_XE_PORT(module, port)) {
        XE_found = TRUE;
        SOC_PBMP_PORT_ADD(pbmp, port);
      }
      if ((FALSE == HG_found) && IS_HG_PORT(module, port)) {
        HG_found = TRUE;
        SOC_PBMP_PORT_ADD(pbmp, port);
      }
      if ((FALSE == CPU_found) && IS_CPU_PORT(module, port)) {
        CPU_found = TRUE;
        SOC_PBMP_PORT_ADD(pbmp, port);
      }
    }

    printf("\n\t\t\t\t\t%s",sepchar);
    PBMP_ITER(pbmp, port) {
      if (IS_GE_PORT(module, port))         printf("\t GE(%d)%s", port,sepchar);
      if (IS_XE_PORT(module, port))         printf("\tXE(%d)%s", port,sepchar);
      if (IS_HG_PORT(module, port))         printf("\tHG(%d)%s", port,sepchar);
      if (IS_CPU_PORT(module, port))        printf("\t  CPU");
    }
    printf("\n");

    osapiSleepMSec(DEBUG_DELAY);
  } else {
    SOC_PBMP_PORT_SET(pbmp, port_num);
    printf("\n\t\t\t\t\t%sPort:\t%5d\n", sepchar,port_num);
    osapiSleepMSec(DEBUG_DELAY);
  }

  /********** HOLCOSPKTSETLIMIT ************/
  field[1] = PKTSETLIMITf;
  fieldName[1] = "PKTSETLIMIT";
  if (soc_reg_field_valid(module, HOLCOSPKTSETLIMITr, RESETLIMITSELf))
  {
    field[0] = RESETLIMITSELf;
    fieldName[0] = "RESETLIMITSEL";
  } 
  
  if (soc_reg_field_valid(module, HOLCOSPKTSETLIMITr, RESETLIMITf))
  {
    field[0] = RESETLIMITf;
    fieldName[0] = "RESETLIMIT";
  }
  findex = 1;

  do {
    for (printParams = FALSE, bcos = 0, cos = 0; cos < 9; cos++)
    {
      index = 0;
      PBMP_ITER(pbmp, port) {
        val = 0;
        if (cos < 8) {
          SOC_IF_ERROR_RETURN(READ_HOLCOSPKTSETLIMITr(module, port, cos, &val));
          paramArray[index][cos] = soc_reg_field_get(module, HOLCOSPKTSETLIMITr, val, field[findex]);
        }
        if ((paramArray[index][bcos] != paramArray[index][cos]) || (8 == cos))
        {
          printParams = TRUE;
        }
        index += 1;
      }
      if (TRUE == printParams)
      {
        printf("HOLCOSPKTSETLIMIT.%s", fieldName[findex]);
        if (bcos == (cos-1)) 
          printf("(%d):%s\t\t", bcos,sepchar);
        else 
          printf("(%d-%d):%s\t\t",bcos, cos-1,sepchar);
        index = 0;
        PBMP_ITER(pbmp, port) {
          printf("%5d%s\t", paramArray[index][bcos],sepchar);
          index += 1;
        }
        printf("\n");
        osapiSleepMSec(DEBUG_DELAY);
        bcos = cos;
        printParams = FALSE;
      }
    }
    findex -= 1;
  } while (findex >= 0);
  printf("\n");

  /********** LWMCOSCELLSETLIMIT ************/

  field[1] = CELLSETLIMITf;
  fieldName[1] = "CELLSETLIMIT";
  if (soc_reg_field_valid(module, LWMCOSCELLSETLIMITr, CELLRESETLIMITf))
  {
    field[0] = CELLRESETLIMITf;
    fieldName[0] = "CELLRESETLIMIT";
  }
  if (soc_reg_field_valid(module, LWMCOSCELLSETLIMITr, RESETLIMITSELf))
  {
    field[0] = RESETLIMITSELf;
    fieldName[0] = "RESETLIMITSEL";
  }
  findex = 1;

  do {
    for (printParams = FALSE, bcos = 0, cos = 0; cos < 9; cos++)
    {
      index = 0;
      PBMP_ITER(pbmp, port) {
        val = 0;
        if (cos < 8) {
          SOC_IF_ERROR_RETURN(READ_LWMCOSCELLSETLIMITr(module, port, cos, &val));
          paramArray[index][cos] = soc_reg_field_get(module, LWMCOSCELLSETLIMITr, val, field[findex]);
        }
        if ((paramArray[index][bcos] != paramArray[index][cos]) || (8 == cos))
        {
          printParams = TRUE;
        }
        index += 1;
      }
      if (TRUE == printParams)
      {
        printf("LWMCOSCELLSETLIMIT.%s", fieldName[findex]);
        if (bcos == (cos-1)) 
          printf("(%d):%s\t\t", bcos,sepchar);
        else 
          printf("(%d-%d):%s\t\t", bcos, cos-1,sepchar);
        index = 0;
        PBMP_ITER(pbmp, port) {
          printf("%5d%s\t", paramArray[index][bcos],sepchar);
          index += 1;
        }
        printf("\n");
        osapiSleepMSec(DEBUG_DELAY);
        bcos = cos;
        printParams = FALSE;
      }
    }
    findex -= 1;
  } while (findex >= 0);
  printf("\n");

  /********** DYNCELLLIMIT ************/ 
 if (soc_reg_field_valid(module,DYNCELLLIMITr , DYNCELLLIMITf))
 {
  field[1] = DYNCELLLIMITf;
  fieldName[1] = "DYNCELLLIMIT";
 }
 if (soc_reg_field_valid(module,DYNCELLLIMITr , DYNCELLSETLIMITf))
 {
  field[1] = DYNCELLSETLIMITf;
  fieldName[1] = "DYNCELLSETLIMIT";
 }

 if (soc_reg_field_valid(module,DYNCELLLIMITr , DYNCELLRESETLIMITf))
 {
  field[0] = DYNCELLRESETLIMITf;
  fieldName[0] = "DYNCELLRESETLIMIT";
 }
 else if (soc_reg_field_valid(module,DYNCELLLIMITr , DYNCELLRESETLIMITSELf))
 {
  field[0] = DYNCELLRESETLIMITSELf;
  fieldName[0] = "DYNCELLRESETLIMITSEL";
 }


  printf("DYNCELLLIMIT.%s:%s\t\t\t",fieldName[1],sepchar);
  PBMP_ITER(pbmp, port) {
    val = 0;
    SOC_IF_ERROR_RETURN(READ_DYNCELLLIMITr(module, port, &val));
    param = soc_reg_field_get(module, DYNCELLLIMITr, val, field[1]);
  
    printf("%5d%s\t", param,sepchar);
  }
  printf("\n");
  osapiSleepMSec(DEBUG_DELAY);

  printf("DYNCELLLIMIT.%s:%s\t\t\t",fieldName[0],sepchar);
  PBMP_ITER(pbmp, port) {
    val = 0;
    SOC_IF_ERROR_RETURN(READ_DYNCELLLIMITr(module, port, &val));
    param = soc_reg_field_get(module, DYNCELLLIMITr, val, field[0]);
    printf("%5d%s\t", param,sepchar);
  }
  printf("\n\n");
  osapiSleepMSec(DEBUG_DELAY);

  /********** IBP ************/ 
  printf("IBPPKTSETLIMIT.PKTSETLIMIT:%s\t\t\t",sepchar);
  PBMP_ITER(pbmp, port) {
    val = 0;
    SOC_IF_ERROR_RETURN(READ_IBPPKTSETLIMITr(module, port, &val));
    param = soc_reg_field_get(module, IBPPKTSETLIMITr, val, PKTSETLIMITf);
    printf("%5d%s\t", param,sepchar);
  }
  printf("\n");
  osapiSleepMSec(DEBUG_DELAY);

  printf("IBPPKTSETLIMIT.RESETLIMITSEL:%s\t\t\t",sepchar);
  PBMP_ITER(pbmp, port) {
    val = 0;
    SOC_IF_ERROR_RETURN(READ_IBPPKTSETLIMITr(module, port, &val));
    param = soc_reg_field_get(module, IBPPKTSETLIMITr, val, RESETLIMITSELf);
    printf("%5d%s\t", param,sepchar);
  }
  printf("\n");
  osapiSleepMSec(DEBUG_DELAY);

  printf("IBPCELLSETLIMIT.CELLSETLIMIT:%s\t\t\t",sepchar);
  PBMP_ITER(pbmp, port) {
    val = 0;
    SOC_IF_ERROR_RETURN(READ_IBPCELLSETLIMITr(module, port, &val));
    param = soc_reg_field_get(module, IBPCELLSETLIMITr, val, CELLSETLIMITf);
    printf("%5d%s\t", param,sepchar);
  }
  printf("\n");
  osapiSleepMSec(DEBUG_DELAY);

  printf("IBPCELLSETLIMIT.RESETLIMITSEL:%s\t\t\t",sepchar);
  PBMP_ITER(pbmp, port) {
    val = 0;
    SOC_IF_ERROR_RETURN(READ_IBPCELLSETLIMITr(module, port, &val));
    param = soc_reg_field_get(module, IBPCELLSETLIMITr, val, RESETLIMITSELf);
    printf("%5d%s\t", param,sepchar);
  }
  printf("\n");
  osapiSleepMSec(DEBUG_DELAY);

  printf("IBPDISCARDSETLIMIT.DISCARDSETLIMIT:%s\t\t",sepchar);
  PBMP_ITER(pbmp, port) {
    val = 0;
    SOC_IF_ERROR_RETURN(READ_IBPDISCARDSETLIMITr(module, port, &val));
    param = soc_reg_field_get(module, IBPDISCARDSETLIMITr, val, DISCARDSETLIMITf);
    printf("%5d%s\t", param,sepchar);
  }
  printf("\n\n");
  osapiSleepMSec(DEBUG_DELAY);

  /********** E2EIBP ************/ 
  printf("E2EIBPPKTSETLIMIT.PKTSETLIMIT:%s\t\t\t",sepchar);
  PBMP_ITER(pbmp, port) {
    val = 0;
    SOC_IF_ERROR_RETURN(READ_E2EIBPPKTSETLIMITr(module, port, &val));
    param = soc_reg_field_get(module, E2EIBPPKTSETLIMITr, val, PKTSETLIMITf);
    printf("%5d%s\t", param,sepchar);
  }
  printf("\n");
  osapiSleepMSec(DEBUG_DELAY);

  printf("E2EIBPPKTSETLIMIT.RESETLIMITSEL:%s\t\t",sepchar);
  PBMP_ITER(pbmp, port) {
    val = 0;
    SOC_IF_ERROR_RETURN(READ_E2EIBPPKTSETLIMITr(module, port, &val));
    param = soc_reg_field_get(module, E2EIBPPKTSETLIMITr, val, RESETLIMITSELf);
    printf("%5d%s\t", param,sepchar);
  }
  printf("\n");
  osapiSleepMSec(DEBUG_DELAY);

  printf("E2EIBPCELLSETLIMIT.CELLSETLIMIT:%s\t\t",sepchar);
  PBMP_ITER(pbmp, port) {
    val = 0;
    SOC_IF_ERROR_RETURN(READ_E2EIBPCELLSETLIMITr(module, port, &val));
    param = soc_reg_field_get(module, E2EIBPCELLSETLIMITr, val, CELLSETLIMITf);
    printf("%5d%s\t", param,sepchar);
  }
  printf("\n");
  osapiSleepMSec(DEBUG_DELAY);

  printf("E2EIBPCELLSETLIMIT.RESETLIMITSEL:%s\t\t",sepchar);
  PBMP_ITER(pbmp, port) {
    val = 0;
    SOC_IF_ERROR_RETURN(READ_E2EIBPCELLSETLIMITr(module, port, &val));
    param = soc_reg_field_get(module, E2EIBPCELLSETLIMITr, val, RESETLIMITSELf);
    printf("%5d%s\t", param,sepchar);
  }
  printf("\n");
  osapiSleepMSec(DEBUG_DELAY);

  printf("E2EIBPDISCARDSETLIMIT.DISCARDSETLIMIT:%s\t\t",sepchar);
  PBMP_ITER(pbmp, port) {
    val = 0;
    SOC_IF_ERROR_RETURN(READ_E2EIBPDISCARDSETLIMITr(module, port, &val));
    param = soc_reg_field_get(module, E2EIBPDISCARDSETLIMITr, val, DISCARDSETLIMITf);
    printf("%5d%s\t", param,sepchar);
  }
  printf("\n\n");
  osapiSleepMSec(DEBUG_DELAY);

  return L7_SUCCESS;
}
