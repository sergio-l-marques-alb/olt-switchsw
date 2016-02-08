#ifndef __SEMLIB_H__
#define __SEMLIB_H__

//*****************************************************************************

//define _SemLibDebug    //Comment to hide debug

//*****************************************************************************

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <errno.h>

#define SEMPT_ERROR  (-1)

//*****************************************************************************

#ifdef __cplusplus
extern "C"
#else
extern
#endif
int sempt_open(key_t key, int units);

//*****************************************************************************

#ifdef __cplusplus
extern "C"
#else
extern
#endif
int sempt_close(int semid);

//*****************************************************************************

#ifdef __cplusplus
extern "C"
#else
extern
#endif
int sempt_getunits(int semid);

//*****************************************************************************

#ifdef __cplusplus
extern "C"
#else
extern
#endif
int sempt_setunits(int semid, int units);

//*****************************************************************************

#ifdef __cplusplus
extern "C"
#else
extern
#endif
int sempt_getid(key_t key);

//*****************************************************************************

#ifdef __cplusplus
extern "C"
#else
extern
#endif
int sempt_wait(int semid);

//*****************************************************************************

#ifdef __cplusplus
extern "C"
#else
extern
#endif
int sempt_trywait(int semid);

//*****************************************************************************

#ifdef __cplusplus
extern "C"
#else
extern
#endif
int sempt_post(int semid);

//*****************************************************************************

#endif
