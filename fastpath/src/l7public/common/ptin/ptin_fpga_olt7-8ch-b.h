#ifndef _PTIN_FPGA_OLT78CHB_H
#define _PTIN_FPGA_OLT78CHB_H

/* FPGA AND CPLD BASE ADDRESS */
# define MAP_FPGA
# define MAP_CPLD
# define FPGA_BASE_ADDR                0xFF200000
# define CPLD_BASE_ADDR                0xFF500000

/* PLD map registers */
//# define CPLD_ID                       0x0849
//# define CPLD_ID0_REG                  0x0005
//# define CPLD_ID1_REG                  0x0004
//# define CPLD_VER_REG                  0x0006
//# define CPLD_HW_ID_REG                0x0008
//# define CPLD_CHASSIS_ID_REG           0x0006
//# define CPLD_SLOT_ID_REG              0x0050

# define CPLD_ID                       0x1195
# define CPLD_ID0_REG                  0x0000
# define CPLD_ID1_REG                  0x0001
# define CPLD_VER_REG                  0x0002
# define CPLD_HW_ID_REG                0x0008
# define CPLD_CHASSIS_ID_REG           0x0006
# define CPLD_SLOT_ID_REG              0x0050

typedef union
{
  L7_uint8 map[PTIN_CPLD_MAP_SIZE];
  struct
  {
    L7_uint16 id;               /* 0x00 (2 bytes) */
    L7_uint8  ver;              /* 0x02 (1 byte)  */
    L7_uint8  empty0[0x06 - 0x03];
    L7_uint8  chassis_id;       /* 0x06 (1 byte)  */
    L7_uint8  empty1[0x08 - 0x07];
    L7_uint8  hw_id;            /* 0x08 (1 byte)  */
    L7_uint8  hw_ver;           /* 0x09 (1 byte)  */
    L7_uint8  empty2[0x1E - 0x0A];
    L7_uint8  slot_matrix;      /* 0x1e (1 byte) */
    L7_uint8  empty3[0x50 - 0x1F];
    L7_uint8  slot_id;          /* 0x50 (1 byte)  */
    L7_uint8  empty4[PTIN_CPLD_MAP_SIZE - 0x51];
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
#else
#define CPLD_INIT()
#define CPLD_DEINIT()
#define CPLD_REG_GET(addr)        0
#define CPLD_REG_SET(addr, val)
#define CPLD_ID_GET()             0
#endif

/* FPGA map registers */
# define FPGA_ID                       0x1112
# define FPGA_ID0_REG                  0x0005 /* Address is swapped! */
# define FPGA_ID1_REG                  0x0004 /* Address is swapped! */
# define FPGA_VER_REG                  0x0006
# define FPGA_TXDISABLE_REG            0xF061
# define FPGA_EXTPHY_RESET             0xF080

typedef union
{
  L7_uint8 map[PTIN_FPGA_MAP_SIZE];
  struct
  {
    L7_uint8  empty0[0x0004 - 0x0000];
    L7_uint8  id1;                  /* 0x0004 (1 bytes) */
    L7_uint8  id0;                  /* 0x0005 (1 bytes) */
    L7_uint8  ver;                  /* 0x0006 (1 byte)  */
    L7_uint8  empty1[0xF061 - 0x0007];
    L7_uint8  tx_disable;           /* 0xF061 (1 byte)  */
    L7_uint8  empty2[0xF080 - 0xF062];
    L7_uint8  extPhy_reset;         /* 0xF080 (1 byte)  */
    L7_uint8  empty3[PTIN_FPGA_MAP_SIZE - 0xF081];
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

#endif /* _PTIN_FPGA_OLT78CHB_H */

