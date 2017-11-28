/*
 * $Id: iproc_m0ssq.h, jayantan Exp $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        iproc_m0ssq.h
 * Purpose:     iproc M0SSQ read and write tcam and sram from M0 and host.
 */
#ifndef __IPROC_M0SSQ_H__
#define __IPROC_M0SSQ_H__
#ifndef CMICX_FW_BUILD
#include <soc/iproc.h>
#include <sal/core/thread.h>
#endif
#if defined(BCM_CMICX_SUPPORT) || defined(CMICX_FW_BUILD)

#define M0SSQ_RESET_ENABLE               1
#define MIIM_PHY_LINK_STATUS0_OFFSET     0x174

#ifdef CMICX_FW_BUILD
#define IPROC_M0SSQ_SRAM_BASE            0x1340000
#define IPROC_M0SSQ_SRAM_SIZE            (128 * 1024)
#define IPROC_M0SSQ_TCAM_BASE            0x0000
#define IPROC_M0SSQ_TCAM_SIZE            (32 * 1024)
#define IPROC_M0SSQ_CORE_OFFSET          0x10000
#define IPROC_M0SSQ_PERCORE_REGBASE      0x8000
#define IPROC_M0SSQ_PERCORE_PRIPORT_BASE 0x9000
#else
#define IPROC_M0SSQ_SRAM_BASE            0x1340000
#define IPROC_M0SSQ_SRAM_SIZE            (128 * 1024)
#define IPROC_M0SSQ_TCAM_BASE            0x1300000
#define IPROC_M0SSQ_TCAM_SIZE            (32 * 1024)
#define IPROC_M0SSQ_PERCORE_OFFSET       0x10000
#define IPROC_M0SSQ_PERCORE_REGBASE      0x1380000
#define IPROC_M0SSQ_PERCORE_PRIPORT_BASE 0x1390000
#endif /* CMICX_FW_BUILD */

#define IPROC_MDIO_SYS_BASE              0x1319000
#define MAX_UCORES                       4

#define soc_iproc_mbox_base_get(ucnum)   (M0SSQ_SHMEM_MBOX_BASE + (ucnum * M0SSQ_SHMEM_MBOX_SIZE))
#define soc_iproc_mbox_size_get(ucnum)   (M0SSQ_SHMEM_MBOX_SIZE)
static __inline__ uint32 soc_iproc_sysreg_base_get(void)
{
   return IPROC_MDIO_SYS_BASE;
}

#ifndef CMICX_FW_BUILD

typedef struct {
    VOL sal_thread_t thread_pid;         /* Cortex-M0 pid */
    char thread_name[16];
    VOL  sal_usecs_t thread_interval;
    uint32 flags;
    sal_sem_t event_sema;
} soc_iproc_m0ssq_control_t;

#define IPROC_M0SSQ_THREAD_INTERVAL   150
#define IPROC_M0SSQ_THREAD_PRI  50

/* Macros for passing unit + ucore numbers to the threads */
#define M0SSQ_ENCODE_UNIT_UCORE(unit, ucore)    INT_TO_PTR((unit) << 16 | (ucore))
#define M0SSQ_DECODE_UNIT(val)                  ((int)(PTR_TO_INT(val) >> 16))
#define M0SSQ_DECODE_UCORE(val)                 ((int)(PTR_TO_INT(val) & 0xffff))

#define soc_iproc_mbox_write(unit, offset, val)    soc_m0ssq_sram_write32(unit, offset, val)
#define soc_iproc_mbox_read(unit, offset)          soc_m0ssq_sram_read32(unit, offset)

static __inline__ uint32 soc_iproc_percore_membase_get(int ucnum)
{
   return (IPROC_M0SSQ_TCAM_BASE + (ucnum * IPROC_M0SSQ_PERCORE_OFFSET));
}

static __inline__ uint32 soc_iproc_sram_membase_get(void)
{
   return IPROC_M0SSQ_SRAM_BASE;
}

static __inline__ void soc_m0ssq_tcam_write32(int unit, int ucnum, int offset, uint32 val)
{
   uint32 base;

   base = soc_iproc_percore_membase_get(ucnum);
   soc_iproc_setreg(unit, (base + offset), val);
}

static __inline__ uint32 soc_m0ssq_tcam_read32(int unit, int ucnum, int offset)
{
   uint32 base;
   uint32 val;

   base = soc_iproc_percore_membase_get(ucnum);
   soc_iproc_getreg(unit, (base + offset), &val);
   return val;
}

static __inline__ void soc_m0ssq_sram_write32(int unit, int offset, uint32 val)
{
   uint32 base;

   base = soc_iproc_sram_membase_get();
   soc_iproc_setreg(unit, (base + offset), val);
}

static __inline__ uint32 soc_m0ssq_sram_read32(int unit, int offset)
{
   uint32 base;
   uint32 val;

   base = soc_iproc_sram_membase_get();
   soc_iproc_getreg(unit, (base + offset), &val);
   return val;
}

static __inline__ uint32 soc_cmicx_reg_read(int unit, int offset)
{
   uint32 base;
   uint32 val;

   base = soc_iproc_sysreg_base_get();
   soc_iproc_getreg(unit, (base + offset), &val);
   return val;
}

static __inline__ void soc_cmicx_reg_write(int unit, int offset, uint32 data)
{
   uint32 base;

   base = soc_iproc_sysreg_base_get();
   soc_iproc_setreg(unit, (base + offset), data);
}

static __inline__ uint32 soc_cmicx_miim_phy_reg_read(int unit, int index)
{
    uint32 base, val;
    uint32 offset = MIIM_PHY_LINK_STATUS0_OFFSET + (index * 4);

    base = soc_iproc_sysreg_base_get();
    soc_iproc_getreg(unit, (base + offset), &val);

    return val;
}

static __inline__ void soc_cmicx_miim_phy_reg_write(int unit, int index, uint32 data)
{
    uint32 base;
    uint32 offset = MIIM_PHY_LINK_STATUS0_OFFSET + (index * 4);

    base = soc_iproc_sysreg_base_get();
    soc_iproc_setreg(unit, (base + offset), data);
}

/* APIs */
int soc_iproc_m0ssq_init(int unit);
int soc_iproc_m0ssq_exit(int unit);
void soc_iproc_m0ssq_reset(int unit, int enable);
int soc_iproc_m0_init(int unit);
int soc_iproc_m0_exit(int unit);

#else
static __inline__ uint32 soc_iproc_percore_membase_get(void)
{
   return IPROC_M0SSQ_TCAM_BASE;
}

static __inline__ uint32 soc_iproc_sram_membase_get(void)
{
   return IPROC_M0SSQ_SRAM_BASE;
}

static __inline__ void soc_m0ssq_tcam_write32(int offset, uint32 val)
{
   uint32 base;

   base = soc_iproc_percore_membase_get();
   *(volatile uint32 *)(base + offset) = val;
}

static __inline__ uint32 soc_m0ssq_tcam_read32(int offset)
{
   uint32 base;

   base = soc_iproc_percore_membase_get();
   return *(volatile uint32 *)(base + offset);
}

static __inline__ void soc_m0ssq_sram_write32(int offset, uint32 val)
{
   uint32 base;

   base = soc_iproc_sram_membase_get();
   *(volatile uint32 *)(base + offset) = val;
}

static __inline__ uint32 soc_m0ssq_sram_read32(int offset)
{
   uint32 base;

   base = soc_iproc_sram_membase_get();
   return *(volatile uint32 *)(base + offset);
}

static __inline__ uint32 soc_cmicx_reg_read(int offset)
{
   uint32 base;

   base = soc_iproc_sysreg_base_get();
   return *(volatile uint32 *)(base + offset);
}

static __inline__ void soc_cmicx_reg_write(int offset, uint32 data)
{
   uint32 base;

   base = soc_iproc_sysreg_base_get();
   *(volatile uint32 *)(base + offset) = data;
}
#endif /* !CMICX_FW_BUILD */
#endif /*defined(BCM_CMICX_SUPPORT) | defined(CMICX_FW_BUILD)*/
#endif /*__IPROC_M0SSQ_H__ */
