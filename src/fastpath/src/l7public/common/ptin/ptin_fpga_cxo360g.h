#ifndef _PTIN_FPGA_CXO360G_H
#define _PTIN_FPGA_CXO360G_H

/* FPGA AND CPLD BASE ADDRESS */
# define MAP_CPLD
# define CPLD_BASE_ADDR                0xFF500000

/* PLD map registers */
# define CPLD_ID                       0x1165
# define CPLD_ID0_REG                  0x0000
# define CPLD_ID1_REG                  0x0001
# define CPLD_VER_REG                  0x0002
# define CPLD_HW_ID_REG                0x0008
# define CPLD_CHASSIS_ID_REG           0x000C
# define CPLD_SLOT_ID_REG              0x000F

typedef union
{
  L7_uint8 map[PTIN_CPLD_MAP_SIZE];
  struct
  {
    L7_uint16 id;               /* 0x00 (2 bytes) */
    L7_uint8  ver;              /* 0x02 (1 byte)  */
    L7_uint8  empty0[0x08 - 0x03];
    L7_uint8  hw_id;            /* 0x08 (1 byte)  */
    L7_uint8  hw_ver;           /* 0x09 (1 byte)  */
    L7_uint8  empty1[0x0C - 0x0A];
    L7_uint8  chassis_id;       /* 0x0C (1 byte)  */
    L7_uint8  empty2[0x0F - 0x0D];
    L7_uint8  slot_id;          /* 0x0F (1 byte)  */
    L7_uint8  empty3[0x1E - 0x10];
    L7_uint8  mx_is_active;     /* 0x1E (1 byte)  */
    L7_uint8  empty4[PTIN_CPLD_MAP_SIZE - 0x1F];
  } reg;
} st_cpld_map_t;

#ifdef MAP_CPLD
extern volatile st_cpld_map_t *cpld_map;
#endif

#ifdef MAP_CPLD
#define CPLD_INIT()
#define CPLD_DEINIT()
#define CPLD_REG_GET(addr)        (cpld_map->map[addr])
#define CPLD_REG_SET(addr, val)   {cpld_map->map[addr]=(unsigned char)(val);}
#define CPLD_ID_GET()             (cpld_map->reg.id)
#define CPLD_SLOT_ID_GET()        (cpld_map->reg.slot_id)
#define CPLD_SLOT_MATRIX_GET()    (cpld_map->reg.mx_is_active & 1)
#else
#define CPLD_INIT()
#define CPLD_DEINIT()
#define CPLD_REG_GET(addr)        0
#define CPLD_REG_SET(addr, val)
#define CPLD_ID_GET()             0
#define CPLD_SLOT_ID_GET()        0
#define CPLD_SLOT_MATRIX_GET()    1
#endif


/* FPGA map registers */
# define FPGA_ID                       0x1256
# define FPGA_ID0_REG                  0x0000
# define FPGA_ID1_REG                  0x0001
# define FPGA_VER_REG                  0x0002

typedef union
{
  L7_uint8 map[PTIN_FPGA_MAP_SIZE];
  struct
  {
    L7_uint8  empty0[0xffff - 0x0000];
  } reg;
} st_fpga_map_t;

#ifdef MAP_FPGA
extern volatile st_fpga_map_t *fpga_map;
#endif

#ifdef MAP_FPGA
#define FPGA_INIT()
#define FPGA_DEINIT()
#define FPGA_REG_GET(addr)        (fpga_map->map[addr])
#define FPGA_REG_SET(addr, val)   {fpga_map->map[addr]=(unsigned char)(val);}
#define FPGA_ID_GET()             (((unsigned short) fpga_map->map[FPGA_ID0_REG]<<8) | ((unsigned short) fpga_map->map[FPGA_ID1_REG]))
#else
#define FPGA_INIT()
#define FPGA_DEINIT()
#define FPGA_REG_GET(addr)        0
#define FPGA_REG_SET(addr, val)
#define FPGA_ID_GET()             0
#endif

#endif /* _PTIN_FPGA_CXO360G_H */

