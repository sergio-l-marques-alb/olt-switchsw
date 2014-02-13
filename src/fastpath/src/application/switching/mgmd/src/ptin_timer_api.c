/**
 * ptin_timer_api.h 
 *  
 * Provides a timer API
 *  
 * Created on: 2013/10/16 
 * Author:     Daniel Figueira
 */
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#include "ptin_timer_api.h"
#include "ptin_mgmd_logger.h"

static unsigned short numCBs=0;

#define PTIN_WAKE_UP_CB_TIMER_SIGNAL 40
#define PTIN_LOAD_TIMER_TIMEOUT 100
#define PTIN_CREATE_CONTROL_BLOCK_TIMEOUT 100

#define MGMD_MAX_NUM_CONTROL_BLOCKS 16

#define PTIN_TIMER_STATE_FREE     0
#define PTIN_TIMER_STATE_RESERVED 1
#define PTIN_TIMER_STATE_RUNNING  2

typedef struct {
    unsigned char state;
    unsigned char nLoadedFlag;
    unsigned int  counter;
    void * (*funcPtr)(void *);
    void *funcParam;
    void *controlBlockPtr;
} PTIN_TIMER_STRUCT;

#define PTIN_CONTROL_BLOCK_STATE_FREE     0
#define PTIN_CONTROL_BLOCK_STATE_RESERVED 1
#define PTIN_CONTROL_BLOCK_STATE_RUNNING  2

typedef struct {
    unsigned char state;
    pthread_t thread_id;
    pthread_attr_t attr;
    pthread_mutex_t lock;
    L7_TIMER_GRAN_t tickGranularity;
    unsigned int numTimers;
    size_t timerStackSize;
    PTIN_TIMER_STRUCT *timers;

    unsigned int              lastSleepPeriod, nextSleepPeriod;
} PTIN_CONTROL_BLOCK_STRUCT;

PTIN_CONTROL_BLOCK_STRUCT cbEntry[MGMD_MAX_NUM_CONTROL_BLOCKS];

static void timerSignalHandler (int sig) {

    switch (sig) {
    case PTIN_WAKE_UP_CB_TIMER_SIGNAL:
        break;
    default:
        break;
    }
}

void* ptin_timer_CB_handle(void *param)
{
  unsigned int              i;
  int                       retCode;
  //unsigned int              cbPtr->lastSleepPeriod,
  //                          cbPtr->nextSleepPeriod;
  PTIN_CONTROL_BLOCK_STRUCT *cbPtr            = param;
  struct timespec           requiredSleepTime, remainingSleepTime;

  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_TIMER, "tickGranularity %d ms", cbPtr->tickGranularity);

  requiredSleepTime.tv_sec = remainingSleepTime.tv_sec = 0;
  requiredSleepTime.tv_nsec = remainingSleepTime.tv_nsec = 0;
  //instalar sinal para os timers
  signal(PTIN_WAKE_UP_CB_TIMER_SIGNAL, timerSignalHandler);

  cbPtr->state = PTIN_CONTROL_BLOCK_STATE_RESERVED;

  while (1)
  {
//  LOG_NOTICE(PTIN_MGMD_LOG_CTX_PTIN_TIMER, "{");
    switch (cbPtr->state)
    {
      case PTIN_CONTROL_BLOCK_STATE_FREE:
        PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_TIMER, "threadId [%u] libertar control block!",  pthread_self());
        pthread_exit(NULL);
        return NULL;
      case PTIN_CONTROL_BLOCK_STATE_RESERVED:
        PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_TIMER, "threadId [%u] nao temos timers!!",  pthread_self());
        if (sleep(10) == 0) break; //dormi ate ao fim...
      case PTIN_CONTROL_BLOCK_STATE_RUNNING:

        cbPtr->nextSleepPeriod = 0;
        PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_TIMER, "threadId [%u] nextSleepPeriod [%d]mS lastSleepPeriod [%d]mS",  pthread_self(), cbPtr->nextSleepPeriod,cbPtr->lastSleepPeriod);
        cbPtr->lastSleepPeriod = (requiredSleepTime.tv_sec - remainingSleepTime.tv_sec) * 1000 + (requiredSleepTime.tv_nsec - remainingSleepTime.tv_nsec) / 1000000; //em ms
        cbPtr->lastSleepPeriod = cbPtr->lastSleepPeriod / cbPtr->tickGranularity; //em timerTicks

        //procurar se algum timer expirou, entretanto guardar o proximo que vai expirar
        for (i = 0; i < cbPtr->numTimers; i++)
        {
          if (cbPtr->timers[i].state == PTIN_TIMER_STATE_RUNNING)
          {
            if (cbPtr->timers[i].nLoadedFlag != 2)
            {
              if (cbPtr->timers[i].counter <= cbPtr->lastSleepPeriod) //expirou!
              {
    
                //tenho de fazer reset ao counter antes de chamar a cb, pois esta cb pode querer reactivar o mesmo timer
                cbPtr->timers[i].counter = 0;
                cbPtr->timers[i].state = PTIN_TIMER_STATE_RESERVED;
    
                if (cbPtr->timerStackSize)
                {
                  pthread_t      thread_id;
                  pthread_attr_t attr;
                  if (0 != pthread_attr_init(&attr))
                  {
                    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_TIMER, "threadId [%u]: Unable to initialize thread attributes", pthread_self());
                  }
                  if (0 != pthread_attr_setstacksize(&attr, PTIN_MGMD_STACK_SIZE / 2))
                  {
                    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_TIMER, "threadId [%u] Unable to set thread stack size to %u", pthread_self(), PTIN_MGMD_STACK_SIZE);
                  }
                  if (0 != pthread_create(&thread_id, &attr, cbPtr->timers[i].funcPtr, cbPtr->timers[i].funcParam))
                  {
                    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_TIMER, "threadId [%u] Unable to start MGMD thread", pthread_self());
                  }
                }
                else
                { 
                  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_TIMER,"threadId [%u] timerPtr [%p]  Expirou!", cbPtr->thread_id, &cbPtr->timers[i]);
                  cbPtr->timers[i].funcPtr(cbPtr->timers[i].funcParam);   
                }
              }
              else
              {
                 cbPtr->timers[i].counter -= cbPtr->lastSleepPeriod;
              }
            } 
            else 
            {
              PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_TIMER, "threadId [%u] timerState [%s]", pthread_self(), (cbPtr->timers[i].state==PTIN_TIMER_STATE_RUNNING)?"RUNNING":"NOT RUNNING");
            }
            if (cbPtr->timers[i].state == PTIN_TIMER_STATE_RUNNING)
            {
              if (!cbPtr->nextSleepPeriod)
              {                
                cbPtr->nextSleepPeriod = cbPtr->timers[i].counter;
                PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_TIMER, "threadId [%u] nextSleepPeriod [%d]mS lastSleepPeriod [%d]mS",  pthread_self(), cbPtr->nextSleepPeriod,cbPtr->lastSleepPeriod);
              }
              else if (cbPtr->nextSleepPeriod > cbPtr->timers[i].counter)
              {                
                cbPtr->nextSleepPeriod = cbPtr->timers[i].counter;
                PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_TIMER, "threadId [%u] nextSleepPeriod [%d]mS lastSleepPeriod [%d]mS",  pthread_self(), cbPtr->nextSleepPeriod,cbPtr->lastSleepPeriod);
              }
            }
          }
          cbPtr->timers[i].nLoadedFlag = 0;
        }
        PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_TIMER, "threadId [%u] nextSleepPeriod [%d]ms lastSleepPeriod [%d]mS",  pthread_self(), cbPtr->nextSleepPeriod,cbPtr->lastSleepPeriod);         
        if (!cbPtr->nextSleepPeriod) //expiraram todos os timers...
        {
          requiredSleepTime.tv_sec = remainingSleepTime.tv_sec = 0;
          requiredSleepTime.tv_nsec = remainingSleepTime.tv_nsec = 0;
          cbPtr->state = PTIN_CONTROL_BLOCK_STATE_RESERVED;
        }
        else
        {
          unsigned long long int nsNextSleep;
          nsNextSleep = cbPtr->nextSleepPeriod;
          nsNextSleep *= cbPtr->tickGranularity;
          nsNextSleep *= 1000;
          nsNextSleep *= 1000;


          requiredSleepTime.tv_sec = nsNextSleep / 1000000000;
          requiredSleepTime.tv_nsec = nsNextSleep % 1000000000;

          PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_TIMER, "threadId [%u] nsNextSleep [%lld]nS requiredSleepTime: [%d]S [%d]nsec", pthread_self(), nsNextSleep, requiredSleepTime.tv_sec, requiredSleepTime.tv_nsec);

          retCode = nanosleep(&requiredSleepTime, &remainingSleepTime);
          if (retCode == 0)
          {
            remainingSleepTime.tv_sec = 0;
            remainingSleepTime.tv_nsec = 0;
          }
          else if (errno != EINTR)
          {
            requiredSleepTime.tv_sec = remainingSleepTime.tv_sec = 0;
            requiredSleepTime.tv_nsec = remainingSleepTime.tv_nsec = 0;
          } else {
            PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_TIMER, "threadId [%u] ErrorCode [%s] remainingSleepTime [%d]sec [%d]nsec", pthread_self(), strerror(errno),remainingSleepTime.tv_sec, remainingSleepTime.tv_nsec);
            cbPtr->state = PTIN_CONTROL_BLOCK_STATE_RUNNING;
          }
        }
        break;
    }
//  LOG_NOTICE(PTIN_MGMD_LOG_CTX_PTIN_TIMER, "[%u] }",  pthread_self());
  }

  return NULL;
}



/**
 * Create a new controlBlock that will manage timer ticks in an 
 * independent thread. 
 * 
 * @param[in]  tickGranularity      : Timer granularity
 * @param[in]  numTimers            : Max number of timers on
 *       the controlBlock
 * @param[in]  timerStackThreadSize : Stack size for callback
 *       fundctions. For disabling threads set to '0'
 * @param[out] controlBlock         : Pointer to the new
 *       controlBlock
 * 
 * @return RC_t 
 */
RC_t ptin_mgmd_timer_createCB(L7_TIMER_GRAN_t tickGranularity, uint32 numTimers, uint32 timerStackThreadSize, PTIN_MGMD_TIMER_CB_t *controlBlock) {
    //void* res;
    int cbIdx;
    unsigned int i;

    PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_TIMER,"Creating %u timers", numTimers);

    if (!numCBs) {//primeira vez que se chama esta funcao. Vou inicializar a memoria
        for (cbIdx=1;cbIdx<MGMD_MAX_NUM_CONTROL_BLOCKS;cbIdx++) {
            cbEntry[cbIdx].state=PTIN_CONTROL_BLOCK_STATE_FREE;
        }
        cbIdx=0;
    } else if (numCBs<MGMD_MAX_NUM_CONTROL_BLOCKS){
        cbIdx=numCBs;
    } else {
        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_TIMER,"Nao existem control blocks disponiveis!!!");
        return TABLE_IS_FULL;
    }

    cbEntry[cbIdx].tickGranularity=tickGranularity;
    cbEntry[cbIdx].numTimers=numTimers;
    cbEntry[cbIdx].timerStackSize=(size_t)timerStackThreadSize;
    //alocar memoria para os timers
    cbEntry[cbIdx].timers=malloc(numTimers*sizeof(PTIN_TIMER_STRUCT));
    if (!cbEntry[cbIdx].timers) {
        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_TIMER,"Out of memory!!!");
        return NO_MEMORY;
    }
    for (i=0;i<numTimers;i++) {
        cbEntry[cbIdx].timers[i].state=PTIN_TIMER_STATE_FREE;
        cbEntry[cbIdx].timers[i].counter=0;
        cbEntry[cbIdx].timers[i].funcPtr=NULL;
        cbEntry[cbIdx].timers[i].funcParam=NULL;
        cbEntry[cbIdx].timers[i].controlBlockPtr=&cbEntry[cbIdx];
    }
    
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_TIMER,"Starting thread initialization");
    if (0 != pthread_attr_init(&cbEntry[cbIdx].attr)) {

        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_TIMER,"Unable to initialize thread attributes");
        return FAILURE;
    }

    if (0 != pthread_attr_setstacksize(&cbEntry[cbIdx].attr, PTIN_MGMD_STACK_SIZE)) {

        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_TIMER,"Unable to set thread stack size to %u", PTIN_MGMD_STACK_SIZE);
        return FAILURE;
    }

    if (0 != pthread_mutex_init(&cbEntry[cbIdx].lock, NULL)) {
        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_TIMER,"Unable to init thread mutex");
        return FAILURE;
    }

    if (0 != pthread_create(&cbEntry[cbIdx].thread_id, &cbEntry[cbIdx].attr, &ptin_timer_CB_handle, &cbEntry[cbIdx])) {

        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_TIMER,"Unable to start MGMD thread");
        return FAILURE;
    }

    for (i=0;i<PTIN_CREATE_CONTROL_BLOCK_TIMEOUT;i++) {
        usleep(10000);
        if (cbEntry[cbIdx].state!=PTIN_CONTROL_BLOCK_STATE_FREE) {
            break;
        }
    }
    if ((i==PTIN_CREATE_CONTROL_BLOCK_TIMEOUT)&&(cbEntry[cbIdx].state==PTIN_CONTROL_BLOCK_STATE_FREE)) {
        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_TIMER,"a pthread nao responde!!!");
        return FAILURE;
    }

    numCBs++;
    *controlBlock=&cbEntry[cbIdx];
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_TIMER,"controlBlock %p", &cbEntry[cbIdx]);
    return SUCCESS;
}


/**
 * Destroy a controlBlock 
 * 
 * @param[in] controlBlock    : Pointer to the new controlBlock
 * 
 * @return RC_t 
 */
RC_t ptin_timer_destroyCB(PTIN_MGMD_TIMER_CB_t controlBlock) {
    PTIN_CONTROL_BLOCK_STRUCT *cbPtr=controlBlock;

    if ( pthread_equal(cbPtr->thread_id, pthread_self()) == 0) { //nao sou esta thread de timers
        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_TIMER,"A auto-destruicao nao e suportada!!!");
        return FAILURE;
    }

    cbPtr->state=PTIN_CONTROL_BLOCK_STATE_FREE;
    pthread_kill(cbPtr->thread_id, PTIN_WAKE_UP_CB_TIMER_SIGNAL);

    pthread_join(cbPtr->thread_id, NULL);

    pthread_mutex_destroy(&cbPtr->lock);
    free(cbPtr->timers);
    return SUCCESS;
}


/**
 * Initialize a new timer, managed by the given controlBlock, 
 * which will call funcPtr upon expiring. 
 * 
 * @param[in]  controlBlock : ControlBlock responsible for this timer's management
 * @param[out] timerPtr     : Pointer to the new timer
 * @param[in]  funcPtr      : Callback to be invoked upon timer's expiral
 * 
 * @return RC_t 
 */
RC_t ptin_mgmd_timer_init(PTIN_MGMD_TIMER_CB_t controlBlock, PTIN_MGMD_TIMER_t *timerPtr, void * (*funcPtr)(void* param)) {
    PTIN_CONTROL_BLOCK_STRUCT *cbPtr=controlBlock;
    unsigned int i;

    pthread_mutex_lock(&cbPtr->lock);
    for (i=0;i<cbPtr->numTimers; i++) {
        if (cbPtr->timers[i].state==PTIN_TIMER_STATE_FREE) {
            *timerPtr=&cbPtr->timers[i];
            break;
        }
    }
    if (i==cbPtr->numTimers) {
        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_TIMER,"Nao existem timers livres!!!");
        pthread_mutex_unlock(&cbPtr->lock);
        return TABLE_IS_FULL;
    }
	PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_TIMER,"threadId [%u] timerPtr [%p]",cbPtr->thread_id, *timerPtr);

    cbPtr->timers[i].counter=0;
    cbPtr->timers[i].funcPtr=funcPtr;
    cbPtr->timers[i].controlBlockPtr=cbPtr;
    cbPtr->timers[i].state=PTIN_TIMER_STATE_RESERVED;
    pthread_mutex_unlock(&cbPtr->lock);

    cbPtr->timers[i].nLoadedFlag=1;
    if ( pthread_equal(cbPtr->thread_id, pthread_self()) == 0 ) { //nao sou esta thread de timers
        //for (i=0;i<PTIN_LOAD_TIMER_TIMEOUT;i++) {
            //acordar a pthread de timers
            pthread_kill(cbPtr->thread_id, PTIN_WAKE_UP_CB_TIMER_SIGNAL);
        //  usleep(1000);
        //  if (cbPtr->timers[i].nLoadedFlag==0) break;
        //  usleep(10000);
        //}
        //if (i==PTIN_LOAD_TIMER_TIMEOUT) {
        //  return FAILURE;
        //}
    }

    return SUCCESS;
}


/**
 * Delete the given timer.
 * 
 * @param[in] timerPtr : Pointer to the timer
 * 
 * @return RC_t 
 */
RC_t ptin_mgmd_timer_deinit(PTIN_MGMD_TIMER_t timerPtr) {
    PTIN_TIMER_STRUCT *tmrPtr=timerPtr;
    int i;

    if (!tmrPtr) return NOT_EXIST;
	PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_TIMER,"threadId [%u] timerPtr [%p]", ((PTIN_CONTROL_BLOCK_STRUCT *)tmrPtr->controlBlockPtr)->thread_id, tmrPtr);

    pthread_mutex_lock(&((PTIN_CONTROL_BLOCK_STRUCT *)tmrPtr->controlBlockPtr)->lock);
    tmrPtr->state=PTIN_TIMER_STATE_FREE;

    if ( pthread_equal( ((PTIN_CONTROL_BLOCK_STRUCT *)tmrPtr->controlBlockPtr)->thread_id, pthread_self()) == 0) { //nao sou esta thread de timers
    
        //tenho de garantir que a thread CB ja viu que este timer esta livre
        tmrPtr->nLoadedFlag=1;
        for (i=0;i<PTIN_LOAD_TIMER_TIMEOUT;i++) {
            //acordar a pthread de timers
            pthread_kill(((PTIN_CONTROL_BLOCK_STRUCT *)tmrPtr->controlBlockPtr)->thread_id, PTIN_WAKE_UP_CB_TIMER_SIGNAL);
            usleep(1000);
            if (tmrPtr->nLoadedFlag==0) break;
            usleep(10000);
        }
        if (i==PTIN_LOAD_TIMER_TIMEOUT) {
            pthread_mutex_lock(&((PTIN_CONTROL_BLOCK_STRUCT *)tmrPtr->controlBlockPtr)->lock);
            return FAILURE;
        }
    }

    tmrPtr->counter=0;
    tmrPtr->funcPtr=NULL;
    pthread_mutex_unlock(&((PTIN_CONTROL_BLOCK_STRUCT *)tmrPtr->controlBlockPtr)->lock);
    //tmrPtr->controlBlockPtr=NULL;
    return SUCCESS;
}


/**
 * Start the given timer, which will expire after timeout, 
 * calling the registered funcPtr with param. 
 * 
 * @param[in] timerPtr : Pointer to the timer
 * @param[in] timeout  : Timer's timeout (expressed accordingly to the controlBlock granularity)
 * @param[in] param    : Callback argument
 * 
 * @return RC_t 
 */
RC_t ptin_mgmd_timer_start(PTIN_MGMD_TIMER_t timerPtr, uint32 timeout, void *param) {
    PTIN_TIMER_STRUCT *tmrPtr=timerPtr;
    unsigned int i;

//  LOG_NOTICE(PTIN_MGMD_LOG_CTX_PTIN_TIMER, "{");

    if (!tmrPtr) return NOT_EXIST;

	PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_TIMER,"threadId [%u] timerPtr [%p] timeout [%d]mS",((PTIN_CONTROL_BLOCK_STRUCT *)tmrPtr->controlBlockPtr)->thread_id, tmrPtr, timeout);

    switch (tmrPtr->state) {
    case PTIN_TIMER_STATE_RESERVED:
        tmrPtr->funcParam=param;
        tmrPtr->counter=timeout;
        //tmrPtr->state=PTIN_TIMER_STATE_RUNNING;

        if ( pthread_equal( ((PTIN_CONTROL_BLOCK_STRUCT *)tmrPtr->controlBlockPtr)->thread_id, pthread_self()) == 0) { //nao sou esta thread de timers
            ////sse nao houver um timer menor que este...
            //for (i=0;i<((PTIN_CONTROL_BLOCK_STRUCT *)tmrPtr->controlBlockPtr)->numTimers;i++) {
            //    if ((&((PTIN_CONTROL_BLOCK_STRUCT *)tmrPtr->controlBlockPtr)->timers[i]!=timerPtr                     ) &&//nao e este timer
            //        (((PTIN_CONTROL_BLOCK_STRUCT *)tmrPtr->controlBlockPtr)->timers[i].state==PTIN_TIMER_STATE_RUNNING) &&//esta a correr
            //        (((PTIN_CONTROL_BLOCK_STRUCT *)tmrPtr->controlBlockPtr)->timers[i].counter<=timeout               ) ){
            //
            //        break;
            //    }
            //}
            //if (i==((PTIN_CONTROL_BLOCK_STRUCT *)tmrPtr->controlBlockPtr)->numTimers) {

                PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_TIMER,"Nao ha nenhum timer que acorde antes deste, garantir que a thread CB carrega este timer!");

                if(timeout == 0)
                {
                  tmrPtr->nLoadedFlag=1;
                }
                else
                {
                  tmrPtr->nLoadedFlag=2;
                }
                tmrPtr->state=PTIN_TIMER_STATE_RUNNING;
                for (i=0;i<PTIN_LOAD_TIMER_TIMEOUT;i++) {
                    //acordar a pthread de timers
                    pthread_kill(((PTIN_CONTROL_BLOCK_STRUCT *)tmrPtr->controlBlockPtr)->thread_id, PTIN_WAKE_UP_CB_TIMER_SIGNAL);
                    usleep(1000);
                    if (tmrPtr->nLoadedFlag==0) break;
                    usleep(10000);
                }
                if (i==PTIN_LOAD_TIMER_TIMEOUT) {
                    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_TIMER,"ERRO! PTIN_LOAD_TIMER_TIMEOUT!!!");
                    return FAILURE;
                }
            //}
        } else {
          tmrPtr->state=PTIN_TIMER_STATE_RUNNING;
        }

        ((PTIN_CONTROL_BLOCK_STRUCT *)tmrPtr->controlBlockPtr)->state=PTIN_CONTROL_BLOCK_STATE_RUNNING;
        break;
    case PTIN_TIMER_STATE_FREE:
    case PTIN_TIMER_STATE_RUNNING:
    default:
        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_TIMER,"O timer tem de estar criado ou a correr");
        return REQUEST_DENIED;
    }

//  LOG_NOTICE(PTIN_MGMD_LOG_CTX_PTIN_TIMER, "}");
    return SUCCESS;
}

/**
 * Stop the given timer.
 * 
 * @param[in] timerPtr : Pointer to the timer
 * 
 * @return RC_t 
 */
RC_t ptin_mgmd_timer_stop(PTIN_MGMD_TIMER_t timerPtr) {
    PTIN_TIMER_STRUCT *tmrPtr=timerPtr;
    //int i;

    if (!tmrPtr) return NOT_EXIST;

	PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_TIMER,"threadId [%u] timerPtr [%p] ", ((PTIN_CONTROL_BLOCK_STRUCT *)tmrPtr->controlBlockPtr)->thread_id, tmrPtr);

    switch (tmrPtr->state) {
    case PTIN_TIMER_STATE_RUNNING:

        tmrPtr->state=PTIN_TIMER_STATE_RESERVED;

        //if ( pthread_equal( ((PTIN_CONTROL_BLOCK_STRUCT *)tmrPtr->controlBlockPtr)->thread_id, pthread_self()) == 0) { //nao sou esta thread de timers
        //  tmrPtr->nLoadedFlag=1;
        //  for (i=0;i<PTIN_LOAD_TIMER_TIMEOUT;i++) {
        //    //acordar a pthread de timers
        //    pthread_kill(((PTIN_CONTROL_BLOCK_STRUCT *)tmrPtr->controlBlockPtr)->thread_id, PTIN_WAKE_UP_CB_TIMER_SIGNAL);
        //    usleep(1000);
        //    if (tmrPtr->nLoadedFlag==0) break;
        //    usleep(10000);
        //  }
        //  if (i==PTIN_LOAD_TIMER_TIMEOUT) {
        //    return FAILURE;
        //  }
        //}
        break;
    case PTIN_TIMER_STATE_RESERVED:
    case PTIN_TIMER_STATE_FREE:
        break;
    default:
        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_TIMER,"O timer tem de estar criado ou a correr");
        return REQUEST_DENIED;
    }

    return SUCCESS;
}

/**
 * Return how much time the given timer has before expiring.
 * 
 * @param[in] timerPtr : Pointer to the timer 
 * 
 * @return uint32
 * 
 * @warning This function can't be called in any callback timer
 *        function
 */
uint32 ptin_mgmd_timer_timeLeft(PTIN_MGMD_TIMER_t timerPtr) {
    int i; 
    PTIN_TIMER_STRUCT *tmrPtr=timerPtr;

    if (!tmrPtr) return((uint32)-1);
//  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_TIMER,"%s: timer %p [%u]\n\r", __FUNCTION__, tmrPtr, ((PTIN_CONTROL_BLOCK_STRUCT *)tmrPtr->controlBlockPtr)->thread_id);
    if ( pthread_equal( ((PTIN_CONTROL_BLOCK_STRUCT *)tmrPtr->controlBlockPtr)->thread_id, pthread_self()) ) { //sou a thread de timers
        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_TIMER,"This function can't be called in any callback timer function");
        return ((uint32)-1);
    }

    tmrPtr->nLoadedFlag=1;

    for (i=0;i<PTIN_LOAD_TIMER_TIMEOUT;i++) {
        //acordar a pthread de timers
        pthread_kill(((PTIN_CONTROL_BLOCK_STRUCT *)tmrPtr->controlBlockPtr)->thread_id, PTIN_WAKE_UP_CB_TIMER_SIGNAL);
        usleep(1000);
        if (tmrPtr->nLoadedFlag==0) {
            return ((uint32) tmrPtr->counter);
        }
        usleep(10000);
    }

    return ((uint32)-1);
}


/**
 * Check if the given timer is running.
 * 
 * @param[in] timerPtr : Pointer to the timer 
 * 
 * @return BOOL 
 */
BOOL ptin_mgmd_timer_isRunning(PTIN_MGMD_TIMER_t timerPtr) {
    PTIN_TIMER_STRUCT *tmrPtr=timerPtr;

    if (!tmrPtr) return FALSE;
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_TIMER,"threadId [%u] timerPtr [%p] State [%s]", ((PTIN_CONTROL_BLOCK_STRUCT *)tmrPtr->controlBlockPtr)->thread_id, tmrPtr, (tmrPtr->state==PTIN_TIMER_STATE_RUNNING)?"RUNNING":"NOT RUNNING");
    if (tmrPtr->state==PTIN_TIMER_STATE_RUNNING) return TRUE;
    else                                         return FALSE;
}


/**
 * Check if the given timer exists.
 * 
 * @param[in] timerPtr : Pointer to the timer 
 * 
 * @return BOOL 
 */
BOOL ptin_mgmd_timer_exist(PTIN_MGMD_TIMER_t timerPtr) {
    PTIN_TIMER_STRUCT *tmrPtr=timerPtr;

    if (!tmrPtr) return FALSE;

    if (tmrPtr->state==PTIN_TIMER_STATE_FREE) return FALSE;
    else                                      return TRUE;
}


