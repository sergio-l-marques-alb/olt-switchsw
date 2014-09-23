#include <stdio.h>
#include "semlib.h"

//*****************************************************************************

#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
/* union semun is defined by including <sys/sem.h> */
#else
/* according to X/OPEN we have to define it ourselves */
union semun {
  int val;                    /* value for SETVAL */
  struct semid_ds *buf;       /* buffer for IPC_STAT, IPC_SET */
  unsigned short int *array;  /* array for GETALL, SETALL */
  struct seminfo *__buf;      /* buffer for IPC_INFO */
} __attribute__ ((packed));
#endif

//*****************************************************************************

int sempt_open(key_t key, int units){
  int semid;
  int value;
  union semun arg;
  unsigned short int initv[1];

  // Create semaphore
  semid = semget(key, 1, IPC_CREAT|IPC_EXCL|0666);
  if (semid < 0) {
    if (errno == EEXIST) {
      semid = semget(key, 0, 0);
      if (semid < 0 ) {
        #ifdef _SemLibDebub
        fprintf(stderr,"SEM_ERROR: sempt_open(exist)\n\r");
        #endif
        return -1;
      }
    }
    else {
      #ifdef _SemLibDebub
      fprintf(stderr,"SEM_ERROR: sempt_open\n\r");
      #endif
      return -1;
    }
  }
  // Initialize the semaphore set
  initv[0]  = units;
  arg.array = initv;
  value = semctl(semid, 0, SETALL, arg);
  if (value < 0) {
    #ifdef _SemLibDebub
    fprintf(stderr,"SEM_ERROR: sempt_open(init)\n\r");
    #endif
    return -1;
  }
  return semid;
}

//*****************************************************************************

int sempt_setunits(int semid, int units){
  union semun arg;
  unsigned short int initv[1];
  int value;

  initv[0]  = units;
  arg.array = initv;
  value= semctl(semid, 0, SETALL, arg);
  #ifdef _SemLibDebub
  if (value < 0) fprintf(stderr,"SEM_ERROR: sempt_setunits\n\r");
  #endif
  return value;
}

//*****************************************************************************

int sempt_getunits(int semid){
  union semun arg;
  unsigned short int initv[1];
  int value;

  arg.array = initv;
  value= semctl(semid, 0, GETALL, arg);
  if (value < 0) {
    #ifdef _SemLibDebub
    fprintf(stderr,"SEM_ERROR: sempt_getunits\n\r");
    #endif
    return -1;
  }
  return (initv[0]);
}

//*****************************************************************************

int sempt_getid(key_t key){
  int value;

  value = semget(key, 0, 0);
  #ifdef _SemLibDebub
  if (value < 0) fprintf(stderr,"SEM_ERROR: sempt_getid\n\r");
  #endif
  return value;
}

//*****************************************************************************

int sempt_close(int semid) {
  int value;
  union semun arg;

  value= semctl(semid, 0, IPC_RMID, arg);
  #ifdef _SemLibDebub
  if (value < 0) fprintf(stderr,"SEM_ERROR: sempt_close\n\r");
  #endif
  return value;
}

//*****************************************************************************

int sempt_wait(int semid) {
  int value;
  struct sembuf sops[1];

  // Enter critical area
  sops[0].sem_num =  0;
  sops[0].sem_op  = -1;
  sops[0].sem_flg =  0;
  do {
    value = semop (semid, sops, 1);
  } while ( (value == -1) && (errno == EINTR) );
  
  #ifdef _SemLibDebub
  if (value < 0) fprintf(stderr,"SEM_ERROR: sempt_wait\n\r");
  #endif
  return value;
}

//*****************************************************************************

int sempt_trywait(int semid) {
  int value;
  struct sembuf sops[1];

  // Enter critical area
  sops[0].sem_num =  0;
  sops[0].sem_op  = -1;
  sops[0].sem_flg =  IPC_NOWAIT;
  do {
    value = semop (semid, sops, 1);
  } while ( (value == -1) && (errno == EINTR) );
  
  #ifdef _SemLibDebub
  if (value < 0) fprintf(stderr,"SEM_ERROR: sempt_nowait\n\r");
  #endif
  return value;
}

//*****************************************************************************

int sempt_post(int semid) {
  int value;
  struct sembuf sops[1];

  // Enter critical area
  sops[0].sem_num = 0;
  sops[0].sem_op = +1;
  sops[0].sem_flg = 0;
  value = semop (semid, sops, 1);
  #ifdef _SemLibDebub
  if (value < 0) fprintf(stderr,"SEM_ERROR: sempt_post\n\r");
  #endif
  return value;
}

//*****************************************************************************
