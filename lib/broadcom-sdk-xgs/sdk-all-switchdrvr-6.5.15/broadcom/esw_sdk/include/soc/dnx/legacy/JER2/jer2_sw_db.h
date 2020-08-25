#ifndef _JER2_SW_DB_H_
/* { */
#define _JER2_SW_DB_H_

#ifndef BCM_DNX_SUPPORT 
#error "This file is for use by DNX (Jer2) family only!" 
#endif

int jer2_sw_db_rate_class_ref_count_get(
    DNX_SAND_IN  int                         unit, 
    DNX_SAND_IN  uint32                      rate_class, 
    DNX_SAND_OUT uint32*                     ref_count);

int jer2_sw_db_tm_queue_to_rate_class_mapping_ref_count_exchange(
    DNX_SAND_IN  int                         unit,
    DNX_SAND_IN  uint32                      old_rate_class,
    DNX_SAND_IN  uint32                      new_rate_class,
    DNX_SAND_IN  int                         nof_additions);

int
  jer2_sw_db_sch_port_rate_set(
   DNX_SAND_IN  int               unit,
   DNX_SAND_IN  int               core,
   DNX_SAND_IN  uint32            tm_port,
   DNX_SAND_IN  uint32            rate,
   DNX_SAND_IN  int               valid
   );
int
  jer2_sw_db_sch_port_rate_get(
   DNX_SAND_IN  int               unit,
   DNX_SAND_IN  int               core,
   DNX_SAND_IN  uint32            tm_port,
   DNX_SAND_OUT uint32            *rate,
   DNX_SAND_OUT int               *valid
   );

#endif /**_JER2_SW_DB_H_ */  
