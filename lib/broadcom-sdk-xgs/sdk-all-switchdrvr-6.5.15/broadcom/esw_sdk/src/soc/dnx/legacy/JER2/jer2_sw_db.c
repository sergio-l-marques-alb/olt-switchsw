#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_SWDB

#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_ingress_congestion_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_scheduler_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ipq.h>
#include <soc/dnx/legacy/TMC/tmc_api_ingress_traffic_mgmt.h>

#include <soc/drv.h> 

int 
jer2_sw_db_rate_class_ref_count_get(
    DNX_SAND_IN  int                         unit, 
    DNX_SAND_IN  uint32                      rate_class, 
    DNX_SAND_OUT uint32*                     ref_count)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_NUM_VALID(unit)) {
        SHR_ERR_EXIT(_SHR_E_PARAM,"Invalid unit: %d", unit);
    }

    SHR_NULL_CHECK(ref_count, _SHR_E_PARAM, "ref_count");

    
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.voq_rate_class.ref_count.get(unit, rate_class, ref_count));

exit:
    SHR_FUNC_EXIT;
}

int 
jer2_sw_db_tm_queue_to_rate_class_mapping_ref_count_exchange(
    DNX_SAND_IN  int                         unit,
    DNX_SAND_IN  uint32                      old_rate_class,
    DNX_SAND_IN  uint32                      new_rate_class,
    DNX_SAND_IN  int                         nof_additions) 
{
    uint32 new_ref_count, old_ref_count;

    int nof_queue_remaped = nof_additions;
    SHR_FUNC_INIT_VARS(unit);

    if (old_rate_class != new_rate_class) {

        if (old_rate_class  >  SOC_DNX_ITM_NOF_RATE_CLASSES ) 
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Rate class (%d) is above max (%d)", old_rate_class, SOC_DNX_ITM_NOF_RATE_CLASSES);
        }
        if (new_rate_class  >  SOC_DNX_ITM_NOF_RATE_CLASSES ) 
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Rate class (%d) is above max (%d)", new_rate_class, SOC_DNX_ITM_NOF_RATE_CLASSES);
        }
        
        if (new_rate_class != SOC_DNX_ITM_NOF_RATE_CLASSES) {
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.voq_rate_class.ref_count.get(unit, new_rate_class, &new_ref_count));
            
            if (new_ref_count >  dnx_data_ipq.queues.nof_queues_get(unit))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Number of mapped queues (%d) is above max (%d)", new_ref_count, dnx_data_ipq.queues.nof_queues_get(unit));
            }
            
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.voq_rate_class.ref_count.set(unit, new_rate_class, new_ref_count + nof_queue_remaped));
            
        }
        
        if (old_rate_class != SOC_DNX_ITM_NOF_RATE_CLASSES) {
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.voq_rate_class.ref_count.get(unit, old_rate_class, &old_ref_count));
            
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.voq_rate_class.ref_count.set(unit, old_rate_class, old_ref_count - nof_queue_remaped));
            
        }
    }

exit:
  SHR_FUNC_EXIT;
}

int
  jer2_sw_db_sch_port_rate_get(
   DNX_SAND_IN  int               unit,
   DNX_SAND_IN  int               core,
   DNX_SAND_IN  uint32            tm_port,
   DNX_SAND_OUT uint32            *rate, 
   DNX_SAND_OUT int               *valid
   
   )
{
    bcm_port_t logical_port;
  int use_core = core;

  SHR_FUNC_INIT_VARS(unit);

  if(core == MEM_BLOCK_ALL) {
      use_core = 0;
  }
  if(use_core < 0 || use_core > dnx_data_device.general.nof_cores_get(unit)){
      SHR_ERR_EXIT(_SHR_E_UNAVAIL, "_SHR_E_INTERNAL");
  }
  
  SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, use_core, tm_port, &logical_port));

  SHR_IF_ERR_EXIT(dnx_scheduler_db.port.rate.get(unit, logical_port, (int*)rate));
  SHR_IF_ERR_EXIT(dnx_scheduler_db.port.valid.get(unit, logical_port, valid));

exit:
  SHR_FUNC_EXIT;
}

int
  jer2_sw_db_sch_port_rate_set(
   DNX_SAND_IN  int               unit,
   DNX_SAND_IN  int               core,
   DNX_SAND_IN  uint32            tm_port,
   DNX_SAND_IN  uint32            rate,
   DNX_SAND_IN  int               valid
   )
{
    bcm_port_t logical_port;
  int use_core = core;

  int nof_cores = 1, i;

  SHR_FUNC_INIT_VARS(unit);

  if(core == MEM_BLOCK_ALL) {
      use_core = 0;
      nof_cores = dnx_data_device.general.nof_cores_get(unit);
  }

  if(use_core < 0 || use_core > dnx_data_device.general.nof_cores_get(unit)){
      SHR_ERR_EXIT(_SHR_E_UNAVAIL, "_SHR_E_INTERNAL");
  }
  
  for(i=0 ; i < nof_cores ; i++) { 
      SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, use_core+i, tm_port, &logical_port));

      SHR_IF_ERR_EXIT(dnx_scheduler_db.port.rate.set(unit, logical_port, rate));
      SHR_IF_ERR_EXIT(dnx_scheduler_db.port.valid.set(unit, logical_port, valid));
  }

exit:
  SHR_FUNC_EXIT;
}

