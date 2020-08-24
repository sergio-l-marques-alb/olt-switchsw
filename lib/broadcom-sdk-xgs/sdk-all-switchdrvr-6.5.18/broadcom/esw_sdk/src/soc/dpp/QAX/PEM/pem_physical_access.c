#include "pem_physical_access.h" 
#include <string.h>
#include <assert.h>
#include <stdlib.h>



/* The following are for debug only, and should be replaced by real implementation
  int phy_pem_mem_write(int unit,
    phy_mem_t* mem,
    unsigned int *entry_data) {
  
    return 0;
  }
  
  int phy_pem_mem_read(int            unit,
                   phy_mem_t*     mem,
                   unsigned int*  entry_data){
  
     return 0;
  }
*/

#ifndef DUNE_BCM /*DUMMY*/

int dpp_dsig_read(int unit, int core, char *block, char *from, char *to, char *name, uint32 *value, size);
{
  int array_entry;
   unit = 0;
   core = 0;

  /*randomize all signal as if read from design*/
   for (array_entry = 0; array_entry < DSIG_MAX_SIZE_UINT32; ++array_entry)
     value[array_entry] = rand();
}

#endif /* DUNE_BCM */



