// ############# addrmap.h ##################
// ######## Nuno Monteiro ###################
// ######## PT Inovacao #####################
// ######## Novembro de 2002 ################
// ######## ADM1 8x100BT Slave ##############

#ifndef __ADDRMAP_H__
#define __ADDRMAP_H__

/**************************************************************/
/* Options...                                                 */
/**************************************************************/

#define _AddrMapDebug    //Comment to hide debug

/**************************************************************/
/*                                                            */
/**************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#ifndef MAP_FAILED
# define MAP_FAILED ((void *)-1)
#endif

/*
#undef  BEGIN_PACKED
#undef  END_PACKED
#define BEGIN_PACKED
#define END_PACKED  __attribute__  ((packed))
*/

typedef struct {
  void  *address;
  int    fd;
  long   length;
} TAddrMap;

/**************************************************************/

#ifdef __cplusplus
extern "C"
#else
extern
#endif
void *AddrAlloc(TAddrMap *AddrMap, int AddrStart, long AddrLen);


#ifdef __cplusplus
extern "C"
#else
extern
#endif
//void *AddrFree(TAddrMap *AddrMap);
int AddrFree(TAddrMap *AddrMap);

/**************************************************************/

#endif
