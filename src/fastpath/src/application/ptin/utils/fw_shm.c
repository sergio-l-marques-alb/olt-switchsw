//*****************************************************************************
// LEDS
//*****************************************************************************

#define __FW_SHM_C__
#include "fw_shm.h"
#undef __FW_SHM_C__

#include <stdio.h>

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
  int  shmid;

  /* Check if shared memory key already exists */
  if ( ( shmid = shmget( FW_SHM_KEY , sizeof( t_fw_shm ) , /*IPC_CREAT |*/ 0666 ) ) >= 0 ) {
    printf("%s(%d) shmid is already in use: %d\n", __FUNCTION__, __LINE__,shmid);
  }
  else
  {
    /* If not (exists), create it */
    shmid = shmget( FW_SHM_KEY , sizeof( t_fw_shm ) , IPC_CREAT | 0666 );

    /* Creation successfull? */
    if (shmid < 0)
    {
      pfw_shm = &fw_shm;
      printf("%s(%d) New shmid: %d\n", __FUNCTION__, __LINE__,shmid);
      return -1;
    }
  }

  /* Make the attach */
  if ( ( pfw_shm = (t_fw_shm*)shmat( shmid , NULL , 0 ) ) == (t_fw_shm*)(-1) ) {
    pfw_shm = &fw_shm;
    return -1;
  }

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

