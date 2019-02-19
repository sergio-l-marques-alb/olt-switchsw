/*
 * $Id: $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#include <soc/drv.h>
#include <soc/monterey.h>
#include <bcm/error.h>
#include <bcm_int/esw/port.h>
#include <bcm/types.h>
#if defined(BCM_PREEMPTION_SUPPORT)
#include <bcm_int/esw/preemption.h>
#endif
#if defined(BCM_MONTEREY_SUPPORT)

int bcm_mn_preemption_edb_init(int unit) { 


int preemption_start[128]={4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
                           6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
                           4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
                           6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
                           4,4,4,5,4,4,4,5,4,4,4,5,4,4,4,5,
                           6,6,6,6,6,6,6,6,6,6,6,5,6,6,6,6,
                           5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
                           6,6,6,6,6,6,6,6,6,6,6,5,5,5,5,6
}; 
int preemption_terminate[512]={
                               8,8,8,8,8,8,8,8,8,8,8,0xd,8,8,8,0xd,  
                               8,8,8,8,8,8,8,8,8,8,8,0xd,8,8,8,0xd,  
                               8,8,8,8,8,8,8,8,8,8,8,0xd,8,8,8,0xd,  
                               8,8,8,8,8,8,8,8,8,8,8,0xd,8,8,8,0xd,  
                               8,8,8,8,8,8,8,8,8,0xd,8,0xd,8,0xd,8,0xd,  
                               8,8,8,8,8,8,8,8,8,0xa,8,0xd,8,0xa,8,0xd,  
                               8,8,8,8,8,8,8,8,8,0xa,8,0xd,8,0xa,8,0xd,  
                               8,8,8,8,8,8,8,8,8,0xa,8,0xd,8,0xa,8,0xd,  
                               8,8,8,8,8,8,8,8,8,8,8,0xd,8,8,8,0xd,  
                               8,8,8,8,8,8,8,8,8,8,8,0xd,8,8,8,0xd,  
                               8,8,8,8,8,8,8,8,8,8,8,0xd,8,8,8,0xd,  
                               8,8,8,8,8,8,8,8,8,8,8,0xd,8,8,8,0xd,  
                               8,8,8,8,8,8,8,8,8,0xd,8,0xd,8,0xd,8,0xd,  
                               8,8,8,8,8,8,8,8,8,0xa,8,0xd,8,0xa,8,0xd,  
                               8,8,8,8,8,8,8,8,8,0xa,8,0xd,8,0xa,8,0xd,  
                               8,8,8,8,8,8,8,8,8,0xa,8,0xd,8,0xa,8,0xd,  
                               8,8,8,8,8,8,8,8,8,8,8,0xd,8,8,8,0xd,  
                               8,8,8,8,8,8,8,8,8,8,8,0xd,8,8,8,0xd,  
                               8,8,8,8,8,8,8,8,8,8,8,0xd,8,8,8,0xd,  
                               8,8,8,8,8,8,8,8,8,8,8,0xd,8,8,8,0xd,  
                               8,8,8,8,8,8,8,8,8,0xd,8,0xd,8,0xd,8,0xd,  
                               8,8,8,8,8,8,8,8,8,0xa,8,0xd,8,0xa,8,0xd,  
                               8,8,8,8,8,8,8,8,8,0xa,8,0xd,8,0xa,8,0xd,  
                               8,8,8,8,8,8,8,8,8,0xa,8,0xd,8,0xa,8,0xd,  
                               8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,  
                               8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,  
                               8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,  
                               8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,  
                               8,8,8,8,8,8,8,8,8,0xd,8,0xd,8,0xd,8,0xd,  
                               8,8,8,8,8,8,8,8,8,0xa,8,0xd,8,0xa,8,0xd,  
                               8,8,8,8,8,8,8,8,8,0xa,8,0xd,8,0xa,8,0xd,  
                               8,8,8,8,8,8,8,8,8,0xa,8,8,8,0xa,8,8,  
};

 static const soc_mem_t preemption_start_mem[16]={ 
                                                  EGR_FLEX_PREEMPT_START_LOOKUP0m,    
                                                  EGR_FLEX_PREEMPT_START_LOOKUP1m,    
                                                  EGR_FLEX_PREEMPT_START_LOOKUP2m,    
                                                  EGR_FLEX_PREEMPT_START_LOOKUP3m,    
                                                  EGR_FLEX_PREEMPT_START_LOOKUP4m,    
                                                  EGR_FLEX_PREEMPT_START_LOOKUP5m,    
                                                  EGR_FLEX_PREEMPT_START_LOOKUP6m,    
                                                  EGR_FLEX_PREEMPT_START_LOOKUP7m,    
                                                  EGR_FLEX_PREEMPT_START_LOOKUP8m,    
                                                  EGR_FLEX_PREEMPT_START_LOOKUP9m,    
                                                  EGR_FLEX_PREEMPT_START_LOOKUP10m,    
                                                  EGR_FLEX_PREEMPT_START_LOOKUP11m,    
                                                  EGR_FLEX_PREEMPT_START_LOOKUP12m,    
                                                  EGR_FLEX_PREEMPT_START_LOOKUP13m,    
                                                  EGR_FLEX_PREEMPT_START_LOOKUP14m,    
                                                  EGR_FLEX_PREEMPT_START_LOOKUP15m,
                                                 };
 static const soc_mem_t preemption_terminate_mem[16]={
                                                  EGR_FLEX_PREEMPT_TERMINATE_LOOKUP0m,
                                                  EGR_FLEX_PREEMPT_TERMINATE_LOOKUP1m,
                                                  EGR_FLEX_PREEMPT_TERMINATE_LOOKUP2m,
                                                  EGR_FLEX_PREEMPT_TERMINATE_LOOKUP3m,
                                                  EGR_FLEX_PREEMPT_TERMINATE_LOOKUP4m,
                                                  EGR_FLEX_PREEMPT_TERMINATE_LOOKUP5m,
                                                  EGR_FLEX_PREEMPT_TERMINATE_LOOKUP6m,
                                                  EGR_FLEX_PREEMPT_TERMINATE_LOOKUP7m,
                                                  EGR_FLEX_PREEMPT_TERMINATE_LOOKUP8m,
                                                  EGR_FLEX_PREEMPT_TERMINATE_LOOKUP9m,
                                                  EGR_FLEX_PREEMPT_TERMINATE_LOOKUP10m,
                                                  EGR_FLEX_PREEMPT_TERMINATE_LOOKUP11m,
                                                  EGR_FLEX_PREEMPT_TERMINATE_LOOKUP12m,
                                                  EGR_FLEX_PREEMPT_TERMINATE_LOOKUP13m,
                                                  EGR_FLEX_PREEMPT_TERMINATE_LOOKUP14m,
                                                  EGR_FLEX_PREEMPT_TERMINATE_LOOKUP15m,
};
uint32 entry0[SOC_MAX_MEM_WORDS];
uint32 entry1[SOC_MAX_MEM_WORDS];
int i,j;                                

  for(i = 0 ; i < 16 ; i++) {
      for (j = 0 ; j <= 127 ; j++) { 
           SOC_IF_ERROR_RETURN
                (soc_mem_read(unit,preemption_start_mem[i] , 
                         MEM_BLOCK_ALL, j, &entry0));
           soc_mem_field32_set(unit,preemption_start_mem[i] , 
                          entry0, STARTf,preemption_start[j]); 
           SOC_IF_ERROR_RETURN(
              soc_mem_write(unit,preemption_start_mem[i] ,
                  MEM_BLOCK_ANY, j, &entry0));
      }
      for (j = 0 ; j < 512 ; j++) { 
           SOC_IF_ERROR_RETURN
                (soc_mem_read(unit,preemption_terminate_mem[i] , 
                         MEM_BLOCK_ALL, j, &entry1));
           soc_mem_field32_set(unit,preemption_terminate_mem[i] , 
                          entry1, TERMINATEf,preemption_terminate[j]); 
           SOC_IF_ERROR_RETURN(
              soc_mem_write(unit,preemption_terminate_mem[i] ,
                  MEM_BLOCK_ANY, j, &entry1));
      }
   } 
   return BCM_E_NONE;  
}

int 
bcmi_mn_preemption_init(int unit) { 

    BCM_IF_ERROR_RETURN(bcm_mn_preemption_edb_init(unit));
    return BCM_E_NONE; 
}

STATIC int
bcmi_mn_preemption_port_check(int unit, bcm_port_t port)
{
  return TRUE; 
}

int
bcmi_mn_preemption_capability_set(int unit, bcm_port_t port,
                                   uint32 arg)
{
    soc_info_t *si = &SOC_INFO(unit);
    int phy_port = si->port_l2p_mapping[port];
    int mmu_port;
    
     mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];

    BCM_IF_ERROR_RETURN(bcmi_mn_preemption_port_check(unit, port));
    BCM_IF_ERROR_RETURN(
        soc_reg_field32_modify(unit,
                               HSP_SCHED_PORT_PREEMPT_CONFIGr,
                               mmu_port,
                               PORT_PREEMPTABLEf,
                               arg ? 1 : 0));
    return BCM_E_NONE;
}

int
bcmi_mn_preemption_queue_bitmap_set(int unit, bcm_port_t port,
                                     uint32 arg)
{
    soc_info_t *si = &SOC_INFO(unit);
    int phy_port = si->port_l2p_mapping[port];
    int mmu_port;
    mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];
    BCM_IF_ERROR_RETURN(bcmi_mn_preemption_port_check(unit, port));
    BCM_IF_ERROR_RETURN(
        soc_reg_field_validate(unit,
                              HSP_SCHED_PORT_PREEMPT_CONFIGr,
                               COS_PREEMPTABLEf,
                               arg));
    BCM_IF_ERROR_RETURN(
        soc_reg_field32_modify(unit,
                               HSP_SCHED_PORT_PREEMPT_CONFIGr,
                               mmu_port,
                               COS_PREEMPTABLEf,
                               arg));
    return BCM_E_NONE;
}



int
bcmi_mn_preemption_capability_get(int unit, bcm_port_t port,
                                   uint32* arg)
{
    uint32 regval;
    soc_info_t *si = &SOC_INFO(unit);
    int phy_port = si->port_l2p_mapping[port];
    int mmu_port;

    mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];
    if (NULL == arg) {
        return SOC_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(bcmi_mn_preemption_port_check(unit, port));
    BCM_IF_ERROR_RETURN(
        READ_HSP_SCHED_PORT_PREEMPT_CONFIGr(unit, mmu_port,
                                         &regval));
    *arg = soc_reg_field_get(unit,
                             HSP_SCHED_PORT_PREEMPT_CONFIGr,
                             regval, PORT_PREEMPTABLEf);
    return BCM_E_NONE;
}

int
bcmi_mn_preemption_queue_bitmap_get(int unit, bcm_port_t port,
                                     uint32* arg)
{
    uint32 regval;
    soc_info_t *si = &SOC_INFO(unit);
    int phy_port = si->port_l2p_mapping[port];
    int mmu_port;

    mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];
    if (NULL == arg) {
        return SOC_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(bcmi_mn_preemption_port_check(unit, port));
    BCM_IF_ERROR_RETURN(
       READ_HSP_SCHED_PORT_PREEMPT_CONFIGr(unit, mmu_port,
                                         &regval));
    *arg = soc_reg_field_get(unit,
                             HSP_SCHED_PORT_PREEMPT_CONFIGr,
                             regval, COS_PREEMPTABLEf);
    return BCM_E_NONE;
}

#endif /* BCM_MONTEREY_SUPPORT */
