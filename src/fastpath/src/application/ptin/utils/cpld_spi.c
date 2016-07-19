
//////////////////////////////////////////////////////////////////////////
//                                                                      //
//              ||||||\   ||||||||   ||||||   ||    ||                  //
//              ||   ||      ||        ||     ||||  ||                  //
//              |||||||      ||        ||     || || ||                  //
//              ||           ||        ||     ||  ||||                  //
//              ||           ||        ||     ||   |||                  //
//              ||           ||      ||||||   ||    ||                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FileName:                                                            //
//            cpld_spi.c                                                //
//                                                                      //
//----------------------------------------------------------------------//
// Projecto:                                                            //
//            OLTSWITCH                                                 //
//                                                                      //
//----------------------------------------------------------------------//
// Descricao: Funcoes para acesso 'a CPLD via SPI                       //
//            Estas rotinas apenas sao aplicadas a uProcessadores ARM   //
//                                                                      //
//----------------------------------------------------------------------//
//                                                                      //
// Hist�rico:                                                         //
//            2 Marco de 2010                                           //
//                                                                      //
//----------------------------------------------------------------------//
// Autor: Celso Lemos                                                   //
//////////////////////////////////////////////////////////////////////////

#include "cpld_spi.h"

#if (PTIN_BOARD == PTIN_BOARD_TG16GF || PTIN_BOARD == PTIN_BOARD_OLT1T0F || PTIN_BOARD == PTIN_BOARD_OLT1T0)

#include <linux/spi/spidev.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>

#define SEMPT_ERROR  (-1)
//#include "utils.h"

#include "addrmap.h"
#include "logger.h"

//----------------------------------------------------------------------
// Declaracao de constantes
//----------------------------------------------------------------------
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))


static const unsigned char BitReverseTable256[] = 
{
  0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0, 
  0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8, 
  0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4, 
  0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC, 
  0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2, 
  0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
  0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6, 
  0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
  0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
  0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9, 
  0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5, 0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
  0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
  0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3, 
  0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
  0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7, 
  0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
};


#define GPIO_SEL_CS_MASK      0x00000010
#define SEL_CS_FLASH_BOOT     0x00000010
#define SEL_CS_SPI_BOARD      0x00000000

#define GPIO_SEL_SPI_MUX_MASK 0x00000021

#define SEL_SPI_LIU           0x00000000
#define SEL_SPI_PLL           0x00000020
#define SEL_SPI_PLD           0x00000030


#define SELECT_RELEASE_BUS    0
#define SELECT_BUS_PLL        2
#define SELECT_BUS_PLD        4

#define SPI_WR_MODE 0
#define SPI_RD_MODE 1

#define SEM_KEY 20021

//----------------------------------------------------------------------
// Declaracao de variaveis
//----------------------------------------------------------------------
int fd; //SPI descriptor 
static const char *device = "/dev/spidev1.0";
static unsigned char  mode;
static unsigned char  bits = 8;
static unsigned int   speed = 60000000;
static unsigned short delay = 0;

unsigned int reverse_int(unsigned int x)
{
  x = (((x & 0xaaaaaaaa) >> 1) | ((x & 0x55555555) << 1));
  x = (((x & 0xcccccccc) >> 2) | ((x & 0x33333333) << 2));
  x = (((x & 0xf0f0f0f0) >> 4) | ((x & 0x0f0f0f0f) << 4));
  x = (((x & 0xff00ff00) >> 8) | ((x & 0x00ff00ff) << 8));
  return((x >> 16) | (x << 16));

}

static int semid_SPI;

static int init_called=0;
//----------------------------------------------------------------------



//----------------------------------------------------------------------
// Declaracao de funcoes para semaforos
//----------------------------------------------------------------------


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
    return semid;
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














//----------------------------------------------------------------------
// Declaracao de funcoes para controlo de BUS
//----------------------------------------------------------------------


static unsigned int  *gpio_base;  
static unsigned int  *gpio_control;  
static TAddrMap       gpio_AddrMap;


/**
 * 
 * 
 * @author clemos (27-11-2015)
 * 
 * @param bus_type 
 * 
 * @return int 
 */
static int arm_spi_select_bus(int bus_type)
{
    *gpio_control &= ((~GPIO_SEL_CS_MASK) & (~GPIO_SEL_SPI_MUX_MASK));
    
    switch (bus_type) {
        case SELECT_BUS_PLL:
            *gpio_control |= (SEL_CS_SPI_BOARD | SEL_SPI_PLL);
            break;

        case SELECT_BUS_PLD:
        default:
            *gpio_control |= (SEL_CS_SPI_BOARD | SEL_SPI_PLD);
            break;
    }
    return(0);
}



/**
 * ----------------------------------------------------------------------
 * Function name: arm_spi_init_serial_buses
 * ----------------------------------------------------------------------
 * Description:     
 *   
 * ----------------------------------------------------------------------
 * ----------------------------------------------------------------------
 * 
 * @return int: 
 * ----------------------------------------------------------------------
 * @author clemos (6/6/2014)
 * ----------------------------------------------------------------------
 */
static int arm_spi_init_serial_buses(void)
{    
    //init controlo de barramentos SPI
    if ((gpio_base = (unsigned int *)AddrAlloc(&gpio_AddrMap, 0x18000000, 0x100000)) == MAP_FAILED) {
        printf("Erro de alocacao\n\r");
    }
    gpio_base[0x00068/4] = 0x00000035; //GPIO_0, GPIO_2, GPIO_4 e GPIO_5: output

    gpio_control = &gpio_base[0x00064/4]; //Control pins

    arm_spi_select_bus(SELECT_RELEASE_BUS);

    return(0);
}




/**
 * ----------------------------------------------------------------------
 * Function name: arm_spi_close_serial_buses
 * ----------------------------------------------------------------------
 * Description:     
 *   
 * ----------------------------------------------------------------------
 * 
 * @param slot: 
 * ----------------------------------------------------------------------
 * 
 * @return int: 
 * ----------------------------------------------------------------------
 * @author clemos (6/7/2014)
 * ----------------------------------------------------------------------
 */
static int arm_spi_close_serial_buses(void) 
{
  AddrFree(&gpio_AddrMap);
  return(0);
}



/**
 * ----------------------------------------------------------------------
 * Function name: pabort
 * ----------------------------------------------------------------------
 * Description:   
 *   
 * ----------------------------------------------------------------------
 * 
 * @param s: 
 * ----------------------------------------------------------------------
 * ----------------------------------------------------------------------
 * @author clemos (6/7/2016)
 * ----------------------------------------------------------------------
 */
static void pabort(const char *s)
{
  perror(s);
  abort();
}


/**
 * ----------------------------------------------------------------------
 * Function name: cpld_transfer
 * ----------------------------------------------------------------------
 * Description:   
 *   
 * ----------------------------------------------------------------------
 * 
 * @param fd: 
 * @param addr0: 
 * @param addr1: 
 * @param addr2: 
 * @param val: 
 * @param op: 
 * ----------------------------------------------------------------------
 * 
 * @return unsigned char: 
 * ----------------------------------------------------------------------
 * @author clemos (6/7/2016)
 * ----------------------------------------------------------------------
 */
static unsigned char cpld_transfer(int fd, unsigned char addr, unsigned char *val, unsigned char op)
{

  int ret;

  if (op==SPI_RD_MODE) {
    addr |= 0x80;
    *val=0;
  }

  uint8_t tx[] = {
    addr,
  };

  uint8_t tx_w[] = {
    addr, *val,
  };


  uint8_t rx[1] = {0,};

  struct spi_ioc_transfer tr_w[] = {
    {
      .tx_buf = (unsigned long)tx_w,
      .rx_buf = (unsigned long)NULL,
      .len = ARRAY_SIZE(tx_w),
       .delay_usecs = delay,
      .speed_hz = speed,
      .bits_per_word = bits,
    }
  };


  struct spi_ioc_transfer tr[] = {
    {
      .tx_buf = (unsigned long)tx,
      .rx_buf = (unsigned long)NULL,
      .len = ARRAY_SIZE(tx),
      .delay_usecs = delay,
      .speed_hz = speed,
      .bits_per_word = bits,
    },
    {
      .tx_buf = (unsigned long)NULL,
      .rx_buf = (unsigned long)rx,
      .len = ARRAY_SIZE(rx),
      .delay_usecs = delay,
      .speed_hz = speed,
      .bits_per_word = bits,
    }
  };

  if (op==SPI_RD_MODE) {
    ret = ioctl(fd, SPI_IOC_MESSAGE(2), tr); 
    if (ret < 1)
      perror("spi_driver: can't send spi message");
  } else {
    ret = ioctl(fd, SPI_IOC_MESSAGE(1), tr_w); 
    if (ret < 1)
      perror("spi_driver: can't send spi message");
  }

  if (op==SPI_RD_MODE) {
    *val = rx[0];
  }

  return(*val);
}


/**
 * ----------------------------------------------------------------------
 * Function name: cpld_spi_open
 * ----------------------------------------------------------------------
 * Description:   
 *   
 * ----------------------------------------------------------------------
 * ----------------------------------------------------------------------
 * ----------------------------------------------------------------------
 * @author clemos (6/7/2016)
 * ----------------------------------------------------------------------
 */
static void cpld_spi_open(void)
{
  int ret = 0;
  errno=0;

  fd = open(device, O_RDWR);
  if (fd < 0)
    pabort("can't open device\n\r");
  /*
   * spi mode
   */
  ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
  if (ret == -1)
    pabort("can't set spi mode\n\r");

  ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
  if (ret == -1)
    pabort("can't get spi mode\n\r");

  /*
   * bits per word
   */
  ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
  if (ret == -1)
    pabort("can't set bits per word\n\r");

  ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
  if (ret == -1)
    pabort("can't get bits per word\n\r");

  /*
   * max speed hz
   */
  ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
  if (ret == -1)
    pabort("can't set max speed hz\n\r");

  ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
  if (ret == -1)
    pabort("can't get max speed hz\n\r");

  printf("spi mode: %d  -", mode);
  printf("  bits per word: %d  -", bits);
  printf("  max speed: %d Hz (%d KHz)\n\r", speed, speed/1000);
}


/**
 * ----------------------------------------------------------------------
 * Function name: cpld_spi_transfer
 * ----------------------------------------------------------------------
 * Description:   
 *   
 * ----------------------------------------------------------------------
 * 
 * @param addr: 
 * @param val: 
 * @param op: 
 * ----------------------------------------------------------------------
 * 
 * @return unsigned char: 
 * ----------------------------------------------------------------------
 * @author clemos (6/7/2016)
 * ----------------------------------------------------------------------
 */
static unsigned char cpld_spi_transfer(unsigned int addr, unsigned char val, unsigned char op)
{
  unsigned char return_val_spi;
  errno=0;

  sempt_wait(semid_SPI);
  arm_spi_select_bus(SELECT_BUS_PLD);

  addr = (BitReverseTable256[addr & 0xFF] >> 1) & 0x7F;
  val  =  BitReverseTable256[val & 0xFF];

  return_val_spi=cpld_transfer(fd,(unsigned char)addr, &val, op);

  arm_spi_select_bus(SELECT_RELEASE_BUS);
  sempt_post(semid_SPI);
  return BitReverseTable256[return_val_spi];
}


/**
 * ----------------------------------------------------------------------
 * Function name: cpld_spi_close
 * ----------------------------------------------------------------------
 * Description:   
 *   
 * ----------------------------------------------------------------------
 * ----------------------------------------------------------------------
 * ----------------------------------------------------------------------
 * @author clemos (6/7/2016)
 * ----------------------------------------------------------------------
 */
static void cpld_spi_close(void)
{
  close(fd);
}



//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
// FUNCOES A EXPORTAR
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------


/**
 * ----------------------------------------------------------------------
 * Function name: OLT1T0_CPLD_create
 * ----------------------------------------------------------------------
 * Description:   
 *   
 * ----------------------------------------------------------------------
 * ----------------------------------------------------------------------
 * 
 * @return int: 
 * ----------------------------------------------------------------------
 * @author clemos (6/7/2016)
 * ----------------------------------------------------------------------
 */
int cpld_spi_create(void)
{
#if !defined(LVL7_KATANAARM) && !defined(LVL7_HELIXARM)
  PT_LOG_ERR(LOG_CTX_STARTUP,"This routine can only be executed on ARM uProcessors!");
  return (-1);
#endif

  semid_SPI = sempt_open(SEM_KEY,1);

  arm_spi_init_serial_buses();
  cpld_spi_open();

  init_called=1;

  PT_LOG_INFO(LOG_CTX_STARTUP, "CPLD SPI initialized: CPLD id 0x%02x%02x", cpld_spi_read(0), cpld_spi_read(1));
  printf("CPLD SPI initialized: CPLD id 0x%02x%02x\r\n", cpld_spi_read(0), cpld_spi_read(1));

  return(0);
}


/**
 * ----------------------------------------------------------------------
 * Function name: cpld_spi_destroy
 * ----------------------------------------------------------------------
 * Description:   
 *   
 * ----------------------------------------------------------------------
 * ----------------------------------------------------------------------
 * 
 * @return int: 
 * ----------------------------------------------------------------------
 * @author clemos (6/7/2016)
 * ----------------------------------------------------------------------
 */
int cpld_spi_destroy(void)
{   
  init_called=0;

#if !defined(LVL7_KATANAARM) && !defined(LVL7_HELIXARM)
  PT_LOG_ERR(LOG_CTX_STARTUP,"This routine can only be executed on ARM uProcessors!");
  return (-1);
#endif

  cpld_spi_close();
  arm_spi_close_serial_buses();
  sempt_close(semid_SPI);

  PT_LOG_INFO(LOG_CTX_STARTUP, "CPLD SPI deinitialized!");

  return(0);    
}




/**
 * ----------------------------------------------------------------------
 * Function name: cpld_spi_read
 * ----------------------------------------------------------------------
 * Description:   
 *   
 * ----------------------------------------------------------------------
 * 
 * @param addr: 
 * ----------------------------------------------------------------------
 * 
 * @return unsigned char: 
 * ----------------------------------------------------------------------
 * @author clemos (7/5/2016)
 * ----------------------------------------------------------------------
 */
unsigned char cpld_spi_read(unsigned int addr)
{
  if (init_called==0)
  {
    PT_LOG_ERR(LOG_CTX_STARTUP, "Error reading CPLD register through SPI!");
    return(0xFF);
  }

  return(cpld_spi_transfer(addr,0,SPI_RD_MODE));
}



/**
 * ----------------------------------------------------------------------
 * Function name: cpld_spi_write
 * ----------------------------------------------------------------------
 * Description:   
 *   
 * ----------------------------------------------------------------------
 * 
 * @param addr: 
 * ----------------------------------------------------------------------
 * 
 * @return unsigned char: 
 * ----------------------------------------------------------------------
 * @author clemos (7/5/2016)
 * ----------------------------------------------------------------------
 */
unsigned char cpld_spi_write(unsigned int addr, unsigned char value)
{
  if (init_called==0) 
  {
    PT_LOG_ERR(LOG_CTX_STARTUP, "Error writing to CPLD register through SPI!");
    return(0xFF);
  }

  return(cpld_spi_transfer(addr,value,SPI_WR_MODE));
}


/**
 * Debug routines
 * 
 * @param addr 
 */
int cpld_spi_read_debug(unsigned int addr)
{
  unsigned char val;

  val = cpld_spi_read(addr);

  printf("CPLD reg 0x%04x: 0x%02x\r\n", addr, val);

  return 0;
}

/**
 * Debug routines
 * 
 * @param addr 
 */
int cpld_spi_write_debug(unsigned int addr, unsigned char val)
{
  cpld_spi_write(addr, val);

  printf("CPLD reg 0x%04x written with value 0x%02x\r\n", addr, val);

  return 0;
}

#endif /* PTIN_BOARD == PTIN_BOARD_TG16GF || PTIN_BOARD == PTIN_BOARD_OLT1T0F */

