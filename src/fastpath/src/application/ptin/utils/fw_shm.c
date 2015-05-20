//*****************************************************************************
// LEDS
//*****************************************************************************

#define __FW_SHM_C__
#include "fw_shm.h"
#undef __FW_SHM_C__

#include <stdio.h>
#include <errno.h>
#include "logger.h"


//******************************************************************************
//
//******************************************************************************

//Usada se houver problema no acesso a memoria partilhada
t_fw_shm   fw_shm;

t_fw_shm   *pfw_shm;

//******************************************************************************
// Iniciar...
//******************************************************************************

int
fw_shm_open( void )
{
  int  shmid, err;

  /* If not (exists), create it */
  shmid = shmget( FW_SHM_KEY , sizeof( t_fw_shm ) , IPC_CREAT | 0666 );
  err = errno;

  /* Creation successfull? */
  if (shmid < 0)
  {
    pfw_shm = &fw_shm;
    LOG_ERR(LOG_CTX_CNFGR, "Failed acquiring new shmid (%d), errno=%d (sizeof=%u)",shmid,err,sizeof(fw_shm));
    return -1;
  }
  LOG_INFO(LOG_CTX_CNFGR, "Success acquiring new shmid: %d", shmid);

  /* Make the attach */
  if ( ( pfw_shm = (t_fw_shm*)shmat( shmid , NULL , 0 ) ) == (t_fw_shm*)(-1) ) {
    pfw_shm = &fw_shm;
    LOG_ERR(LOG_CTX_CNFGR, "Failed attaching shared memory");
    return -1;
  }

  LOG_INFO(LOG_CTX_CNFGR, "Success attaching shared memory\n");

  return (0);
}

//******************************************************************************
// Terminar...
//******************************************************************************

int
fw_shm_close( void )
{
  //Nada!
  return (0);
}

//******************************************************************************
// FIM
//******************************************************************************

