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

t_fw_shm   *pfw_shm = (void *)0;

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
    PT_LOG_ERR(LOG_CTX_CNFGR, "Failed acquiring new shmid (%d), errno=%d (sizeof=%zu)",
               shmid,err,sizeof(fw_shm));
    return -1;
  }
  PT_LOG_INFO(LOG_CTX_CNFGR, "Success acquiring new shmid: %d", shmid);

  /* Make the attach */
  if ( ( pfw_shm = (t_fw_shm*)shmat( shmid , NULL , 0 ) ) == (t_fw_shm*)(-1) ) {
    pfw_shm = &fw_shm;
    PT_LOG_ERR(LOG_CTX_CNFGR, "Failed attaching shared memory");
    return -1;
  }

  PT_LOG_INFO(LOG_CTX_CNFGR, "Success attaching shared memory\n");

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


void fw_shm_dump(void)
{
  if (pfw_shm == (void *)0)
  {
    printf("Shared memory is not initialized\r\n");
    return;
  }
  
#if (PTIN_BOARD_IS_GPON)
  {
      int i;

      for (i = 0; i < PTIN_SYSTEM_MAX_N_PORTS; i++)
      {
          printf("intf[i] = {admin=%u link=0x%x port_state=0x%x ssm_rx=0x%x ssm_tx=0x%x }\r\n",
                 pfw_shm->intf[i].admin, pfw_shm->intf[i].link, pfw_shm->intf[i].port_state, pfw_shm->intf[i].ssm_rx, pfw_shm->intf[i].ssm_tx);
      }

      printf("SyncE_Recovery_clock[] = { 0x%02x 0x%02x }\r\n", pfw_shm->SyncE_Recovery_clock[0], pfw_shm->SyncE_Recovery_clock[1]);
#if (PTIN_BOARD == PTIN_BOARD_TC16SXG)
      printf("CPLD = { slot_id=0x%02x slot_matrix=0x%02x mx_is_active=0x%02x }\r\n",
             pfw_shm->cpld.slot_id, pfw_shm->cpld.slot_matrix, pfw_shm->cpld.mx_is_active);
#endif
  }
#endif
}
