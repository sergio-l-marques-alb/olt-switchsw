#ifndef __MEMCHK_H__
#define __MEMCHK_H__

//*****************************************************************************

#ifdef __cplusplus
extern "C"
#else
extern
#endif
void memfill(char *start, unsigned int dim, char *with);


#ifdef __cplusplus
extern "C"
#else
extern
#endif
int memchk(char *start, unsigned int dim, char *with);


#ifdef __cplusplus
extern "C"
#else
extern
#endif
void printmem(char *start, unsigned int dim);

//*****************************************************************************

#endif
