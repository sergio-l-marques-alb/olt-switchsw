#include "pemladrv_physical_access.h" 
#ifdef LINK_TO_SVERILOG
/* The following are for debug only, and should be relplaced by real implementation */
int pemladrv_physical_read(int unit, 
                            unsigned int block_identifier, 
                            unsigned int is_reg, 
                            unsigned int mem_address, 
                            unsigned int mem_width_in_bits, 
                            unsigned int reserve, 
                            void* entry_data){
  return 0;
  }

int pemladrv_physical_write(int unit, 
                            unsigned int block_identifier, 
                            unsigned int is_reg, 
                            unsigned int mem_address, 
                            unsigned int mem_width_in_bits, 
                            unsigned int reserve, 
                            void* entry_data){
  return 0;
}
#endif /*LINK_TO_SVERILOG*/

