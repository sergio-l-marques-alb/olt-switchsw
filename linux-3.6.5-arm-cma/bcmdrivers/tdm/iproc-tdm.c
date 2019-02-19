/*
 * Copyright (C) 2013, Broadcom Corporation. All Rights Reserved.
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * TDM interface to Iproc
 */
 
#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <typedefs.h>
#include <osl.h>
#include <bcmutils.h>
#include <siutils.h>
#include <sbhnddma.h>
#include <hnddma.h>
#include <hndsoc.h>
#include "i2s_regs.h"
#include "iproc-tdm.h"
#include <mach/reg_utils.h>
#include <linux/io.h>
#include <mach/io_map.h>

void __iomem *baseAddr;
iproc_tdm_info_t *tdm_info;
iproc_tdm_runtime_data_t *tdm_runtime;

#define DMAREG(a, direction, fifonum)	( \
	(direction == DMA_TX) ? \
                (void *)(uintptr)&(a->regs->dmaregs[fifonum].dmaxmt) : \
                (void *)(uintptr)&(a->regs->dmaregs[fifonum].dmarcv))
				

static void iproc_tdm_pcm_enqueue_tx(iproc_tdm_runtime_data_t *runtime)
{
	dma_addr_t txpos = runtime->dma_txpos;
	dma_addr_t txstart = runtime->dma_txstart;
	//dma_addr_t rxpos = runtime->dma_rxpos;
	//dma_addr_t rxstart = runtime->dma_rxstart;
	int ret;

	unsigned long len;		
	while (runtime->txdma_loaded < runtime->dma_limit) 
	{
		unsigned long len = runtime->dma_period;

		if ((txpos & ~0xFFF) != (((txpos+len - 1) & ~0xFFF))) 
		{
			len = ((txpos+len) & ~0xFFF) - txpos;
		}

		if ((txpos + len) > runtime->dma_txend) 
		{
			len  = runtime->dma_txend - txpos;
		}
								
		ret = dma_txunframed(runtime->tdm_info->di[0], (void *)txpos, len, TRUE);
		
		//DBG("%s txunframed returns: %d\n", __FUNCTION__, ret);

		if (ret == 0) 
		{
			txpos += len;
			runtime->txdma_loaded++;
			if (txpos >= runtime->dma_txend)
				txpos = runtime->dma_txstart;
		} else
			break;
	}

	runtime->dma_txpos = txpos;

	return;
}

static void iproc_tdm_pcm_enqueue_rx(iproc_tdm_runtime_data_t *runtime)
{
	//dma_addr_t txpos = runtime->dma_txpos;
	//dma_addr_t txstart = runtime->dma_txstart;
	dma_addr_t rxpos = runtime->dma_rxpos;
	dma_addr_t rxstart = runtime->dma_rxstart;
	int ret;

	unsigned long len;		

	DBG("%s: before dma_tx devcontrol 0x%x devstatus 0x%x fifocounter 0x%x stxctrl 0x%x stxstatus0 0x%x stxstatus1 0x%x\n", __FUNCTION__,
	    readl(baseAddr + I2S_DEVCONTROL_REG),
	    readl(baseAddr + I2S_DEVSTATUS_REG),
	    readl(baseAddr + I2S_FIFOCOUNTER_REG),	
		readl(baseAddr + I2S_STXCTRL_REG),
		readl(baseAddr + I2S_STXCHSTATUS0_REG),
		readl(baseAddr + I2S_STXCHSTATUS1_REG));		
	
	DBG("%s dma_loaded: %d dma_limit: %d dma_period: %d\n", __FUNCTION__, runtime->rxdma_loaded, runtime->dma_limit, runtime->dma_period);
	DBG("%s: pos %p - dma_start %p - dma_end %p\n", __FUNCTION__, rxpos, runtime->dma_rxstart, runtime->dma_rxend);

	while (runtime->rxdma_loaded < runtime->dma_limit) 
	{
		unsigned long len = runtime->dma_period;

		if ((rxpos & ~0xFFF) != (((rxpos+len - 1) & ~0xFFF))) 
		{
			len = ((rxpos+len) & ~0xFFF) - rxpos;
		}

		if ((rxpos + len) > runtime->dma_rxend) 
		{
			len  = runtime->dma_rxend - rxpos;
		}
								
		ret = dma_rxunframed(runtime->tdm_info->di[0], (void *)rxpos, len, TRUE);
		
		//DBG("%s txunframed returns: %d\n", __FUNCTION__, ret);

		if (ret == 0) 
		{
			rxpos += len;
			runtime->rxdma_loaded++;
			if (rxpos >= runtime->dma_rxend)
				rxpos = runtime->dma_rxstart;
		} else
			break;
	}

	runtime->dma_rxpos = rxpos;

	return;
}

/* Interrupt Service Routine */
irqreturn_t iproc_tdm_isr(int irq, void *devid)
{
	uint32 intstatus = readl(baseAddr + I2S_INTSTATUS_REG);
	uint32 intmask = readl(baseAddr + I2S_INTMASK_REG);
	uint32 intstatus_new = 0;
	//uint32 int_errmask = I2S_INT_DESCERR | I2S_INT_DATAERR | I2S_INT_DESC_PROTO_ERR |
	//        I2S_INT_RCVFIFO_OFLOW | I2S_INT_XMTFIFO_UFLOW | I2S_INT_SPDIF_PAR_ERR;
	uint32 int_errmask = I2S_INT_DESCERR | I2S_INT_DATAERR | I2S_INT_DESC_PROTO_ERR |
	        I2S_INT_RCVFIFO_OFLOW | I2S_INT_SPDIF_PAR_ERR;
			
	iproc_tdm_runtime_data_t *runtime;

	//DBG("Isr: 0x%x\n", intstatus);
	
	runtime = tdm_runtime;
	
	if (intstatus & I2S_INT_XMT_INT) {						// TX Interrupt
		DBG("T");	

		/* reclaim descriptors that have been TX'd */
		dma_getnexttxp(tdm_info->di[0], HNDDMA_RANGE_TRANSMITTED);

		/* clear this bit by writing a "1" back, we've serviced this */
		intstatus_new |= I2S_INT_XMT_INT;

		// Disable interrupts
		writel(readl(baseAddr + I2S_INTMASK_REG) & ~I2S_INT_XMT_INT, baseAddr + I2S_INTMASK_REG);
	
		spin_lock(&runtime->lock);
		runtime->txdma_loaded--;
		iproc_tdm_pcm_enqueue_tx(runtime);
		spin_unlock(&runtime->lock);
		
		writel(intstatus_new, baseAddr + I2S_INTSTATUS_REG);
	
		/* Enable Tx interrupt */
		writel(intmask | I2S_INT_XMT_INT, baseAddr + I2S_INTMASK_REG);	
	}
	
	if (intstatus & I2S_INT_RCV_INT) {						// RX Interrupt
		DBG("R");	

		/* reclaim descriptors that have been RX'd */
		dma_getnextrxp(tdm_info->di[0], 0);

		/* clear this bit by writing a "1" back, we've serviced this */
		intstatus_new |= I2S_INT_RCV_INT;

		// Disable interrupts
		writel(readl(baseAddr + I2S_INTMASK_REG) & ~I2S_INT_RCV_INT, baseAddr + I2S_INTMASK_REG);
	
		spin_lock(&runtime->lock);
		runtime->rxdma_loaded--;
		//if (brtd->state & BCM_I2S_RUNNING) {
		iproc_tdm_pcm_enqueue_rx(runtime);
		//}
		spin_unlock(&runtime->lock);
		
		writel(intstatus_new, baseAddr + I2S_INTSTATUS_REG);
	
		/* Enable Rx interrupt */
		writel(intmask | I2S_INT_RCV_INT, baseAddr + I2S_INTMASK_REG);
	}
	
	if (intstatus & int_errmask) {
		DBG("\n\n%s: Turning off all interrupts due to error\n", __FUNCTION__);
		DBG("%s: intstatus 0x%x intmask 0x%x\n", __FUNCTION__, intstatus, intmask);

		writel(0, baseAddr + I2S_INTMASK_REG);
	}
	
	return IRQ_RETVAL(intstatus);
}

int iproc_i2s_configure(void)
{
	uint32_t deviceControl, clkDivider, stxControl, i2sControl, tdmControl;
	uint32_t i2sIntrecelazyDMA0, reg;
		
	// Read the registers
	deviceControl = readl(baseAddr + I2S_DEVCONTROL_REG);
	clkDivider = readl(baseAddr + I2S_CLOCKDIVIDER_REG);
	stxControl = readl(baseAddr + I2S_STXCTRL_REG);
	i2sControl = readl(baseAddr + I2S_CONTROL_REG);
	tdmControl = readl(baseAddr + I2S_TDMCONTROL_REG);

	// Select TDM mode
	deviceControl |= I2S_DC_TDM_SEL;
	// Set to Slave Mode, BITCLK driven from external clock
	deviceControl |= I2S_DC_BCLKD_IN;
	// Set to full-duplex
	deviceControl |= 0x300;						// Full duplex

	i2sIntrecelazyDMA0 |= I2S_INTRECE_LAZY_FC;
	
	// Setup TDM Control Register
	reg = (SCNT_NUM << 8);
	tdmControl |= reg;
	
	tdmControl |= TDM_RDY_CNT;
	tdmControl |= TDM_FRAMESYNDU;
	//tdmControl |= TDM_MODESEL_LIN16;
	DBG("%s: reg: 0x%x\n", __FUNCTION__, reg);
	
	// Set PLAYEN bit to start Frame Sync
	i2sControl |= I2S_CONTROL_PLAYEN;
	
	// Program TxPlayTH to 1
	writel(1, baseAddr + I2S_TXPLAYTH_REG);

	// Write DeviceControl, STX Control registers
	writel(deviceControl, baseAddr + I2S_DEVCONTROL_REG);
	writel(tdmControl, baseAddr + I2S_TDMCONTROL_REG);
	writel(i2sControl, baseAddr + I2S_CONTROL_REG);
	writel(i2sIntrecelazyDMA0, baseAddr + I2S_INTRECELAZY_DMA0_REG);
	
	DBG("%s: deviceControl: 0x%x\n", __FUNCTION__, readl(baseAddr + I2S_DEVCONTROL_REG));
	DBG("%s: stxControl 0x%x\n", __FUNCTION__, readl(baseAddr + I2S_STXCTRL_REG));
	DBG("%s: i2sControl 0x%x\n", __FUNCTION__, readl(baseAddr + I2S_CONTROL_REG));
	DBG("%s: tdmControl 0x%x\n", __FUNCTION__, readl(baseAddr + I2S_TDMCONTROL_REG));	
	DBG("%s: i2sIntrecelazyDMA0 0x%x\n", __FUNCTION__, readl(baseAddr + I2S_INTRECELAZY_DMA0_REG));
	
	return 0;
}

int iproc_i2s_attach(struct platform_device *pdev)
{
	int retVal;
	uint32_t idm_i2s_reset_status_reg, sku_id;
	osl_t *osh = NULL;
	int dma_attach_err = 0;
	
	void __iomem *i2s_m0_idm_io_control_direct    		= IOMEM(HW_IO_PHYS_TO_VIRT(I2S_M0_IDM_IO_CONTROL_DIRECT));
	void __iomem *i2s_m0_idm_io_reset_control    		= IOMEM(HW_IO_PHYS_TO_VIRT(I2S_M0_IDM_RESET_CONTROL));
	void __iomem *i2s_m0_idm_io_reset_control_status    = IOMEM(HW_IO_PHYS_TO_VIRT(I2S_M0_IDM_RESET_STATUS));
	void __iomem *rom_s0_idm_io_status    				= IOMEM(HW_IO_PHYS_TO_VIRT(ROM_S0_IDM_IO_STATUS));
	
	DBG("%s\n", __FUNCTION__);
	
	printk("Checking platform type\n");
	sku_id = readl(rom_s0_idm_io_status);		// 1 - low sku, 2 - medium sku, 0 or 3 - high sku
	printk("sku_id: 0x%x\n", sku_id);
	
	// If platform is not equal to 12, bailing out
    if(sku_id == 1 || sku_id == 2) {
		printk("No I2S block on platform\n");
		return 0;
	}
	
	// Reset I2S block
	printk("Resetting i2s block\n");
	
	// Reset the device
	writel(1, i2s_m0_idm_io_reset_control);
	
	idm_i2s_reset_status_reg = readl(i2s_m0_idm_io_reset_control_status);
	printk("idmResetStatusReg:  %x\n", idm_i2s_reset_status_reg);
	
	printk("Exiting reset\n");
	// Exit Reset the device
	writel(0, i2s_m0_idm_io_reset_control);
	
	baseAddr = IOMEM(IPROC_I2S_REG_VA);
	
	// Create tdm_info structure
	tdm_info = kzalloc(sizeof(iproc_tdm_info_t), GFP_KERNEL);
	if (tdm_info == NULL) {
		DBG("%s: Error allocating tdm_info\n", __FUNCTION__);	
		unregister_chrdev(TDMDRV_MAJOR, TDMDRV_NAME);	
		return -ENOMEM;
	}
			
	tdm_info->irq = BCM5301X_I2S_INTERRUPT;

	DBG("%s: Attaching osl\n", __FUNCTION__);
	osh = osl_attach(pdev, PCI_BUS, FALSE);
	ASSERT(osh);

	tdm_info->osh = osh;
	
	tdm_info->sih = si_attach(0, tdm_info->osh, tdm_info->regsva, PCI_BUS, pdev,
	                        NULL, NULL);
	
	tdm_info->regs = (i2sregs_t *)si_setcore(tdm_info->sih, I2S_CORE_ID, 0);
	//si_core_reset(snd_bcm535x->sih, 0, 0);
	
	DBG("%s: Attaching DMA\n", __FUNCTION__);
									
	tdm_info->di[0] = dma_attach(tdm_info->osh, "i2s_dma", tdm_info->sih,
	                            DMAREG(tdm_info, DMA_TX, 0),
	                            DMAREG(tdm_info, DMA_RX, 0),
	                            64, 64, 512, -1, 0, 0, NULL);

	if (tdm_info->di[0] == NULL)
		DBG("%s: DMA Attach not successfull\n", __FUNCTION__);
	
	dma_attach_err |= (NULL == tdm_info->di[0]);
	
	/* Tell DMA that we're not using framed/packet data */
	dma_ctrlflags(tdm_info->di[0], DMA_CTRL_UNFRAMED /* mask */, DMA_CTRL_UNFRAMED /* value */);
	
	// Configure I2S block
	iproc_i2s_configure();

	return 0;
}

static int iproc_tdm_pcm_start(iproc_tdm_runtime_data_t *runtime)
{
	uint32 intmask = readl(baseAddr + I2S_INTMASK_REG);
	uint32_t tdm0ControlReg = readl(baseAddr + I2S_TDMCH0_CTRL_REG);
	uint32_t tdm1ControlReg = readl(baseAddr + I2S_TDMCH1_CTRL_REG);
	
	spin_lock_irq(&runtime->lock);
	
	runtime->dma_period = DMA_BYTES;
	runtime->dma_limit = DMA_LIMIT;
	
	/* Virtual address of our runtime buffer */
	/* TX */
	runtime->dma_txstart = (dma_addr_t)runtime->tdm_info->dmaBuf;
	runtime->txdma_loaded = 0;
	runtime->dma_txpos = runtime->dma_txstart;
	runtime->dma_txend = runtime->dma_txstart + DMABUF_SIZE;
	/* RX */
	runtime->dma_rxstart = runtime->dma_txend;
	runtime->rxdma_loaded = 0;	
	runtime->dma_rxpos = runtime->dma_rxstart;	
	runtime->dma_rxend = runtime->dma_rxstart + DMABUF_SIZE;
	
	DBG("%s: dma_period 0x%x\n", __FUNCTION__, runtime->dma_period);
	DBG("%s: txdma_loaded 0x%x\n", __FUNCTION__, runtime->txdma_loaded);
	DBG("%s: rxdma_loaded 0x%x\n", __FUNCTION__, runtime->rxdma_loaded);
	DBG("%s: dma_limit 0x%x\n", __FUNCTION__, runtime->dma_limit);
	DBG("%s: dma_txpos 0x%p\n", __FUNCTION__, runtime->dma_txpos);
	DBG("%s: dma_txstart 0x%p\n", __FUNCTION__, runtime->dma_txstart);
	DBG("%s: dma_txend 0x%p\n", __FUNCTION__, runtime->dma_txend);
	DBG("%s: dma_rxpos 0x%p\n", __FUNCTION__, runtime->dma_rxpos);
	DBG("%s: dma_rxstart 0x%p\n", __FUNCTION__, runtime->dma_rxstart);
	DBG("%s: dma_rxend 0x%p\n", __FUNCTION__, runtime->dma_rxend);
	
	spin_unlock_irq(&runtime->lock);

	// Clear all interrupt masks
	writel(0, baseAddr + I2S_INTMASK_REG);
	
	// Enable TDM Channel 0
	tdm0ControlReg = 1;
	tdm1ControlReg = 1;
	writel(tdm0ControlReg, baseAddr + I2S_TDMCH0_CTRL_REG);	
	writel(tdm1ControlReg, baseAddr + I2S_TDMCH1_CTRL_REG);	
	
	/* Turn on Tx interrupt */
	writel(intmask | I2S_INT_XMT_INT, baseAddr + I2S_INTMASK_REG);

	/* Turn on RX interrupt */
	writel(intmask | I2S_INT_RCV_INT, baseAddr + I2S_INTMASK_REG);
	
	// Enqueue
	iproc_tdm_pcm_enqueue_tx(runtime);
	iproc_tdm_pcm_enqueue_rx(runtime);

	return 0;
}

static int iproc_tdm_pcm_stop(iproc_tdm_runtime_data_t *runtime)
{
	uint32_t tdm0ControlReg = readl(baseAddr + I2S_TDMCH0_CTRL_REG);
	uint32_t tdm1ControlReg = readl(baseAddr + I2S_TDMCH1_CTRL_REG);

	// Disable TDM Channel 0
	tdm0ControlReg = 0;
	tdm1ControlReg = 0;
	writel(tdm0ControlReg, baseAddr + I2S_TDMCH0_CTRL_REG);
	writel(tdm0ControlReg, baseAddr + I2S_TDMCH1_CTRL_REG);

	/* Turn off interrupts... */
	writel(readl(baseAddr + I2S_INTMASK_REG) & ~I2S_INT_RCV_INT, baseAddr + I2S_INTMASK_REG);
	writel(readl(baseAddr + I2S_INTMASK_REG) & ~I2S_INT_XMT_INT, baseAddr + I2S_INTMASK_REG);

	/* reclaim all descriptors */
	dma_txreclaim(tdm_info->di[0], HNDDMA_RANGE_ALL);
	dma_getnextrxp(tdm_info->di[0], HNDDMA_RANGE_ALL);

	return 0;
}

static int slic_ring(void)
{
	unsigned char hookStatus;
	unsigned char channel;
	int i;

	channel = 0;
	
	hookStatus = PROSLIC_ONHOOK;
	

	while (slic_readHookStatus(channel) == PROSLIC_ONHOOK)
		msleep(250);
		
	//DBG("Off Hook\n");
	
	slic_toneSetup(channel, DEFAULT_TONE);
	
	slic_PCMSetup(channel, 0);	
	
	// Start PCM on tdm block
	iproc_tdm_pcm_start(tdm_runtime);
	
	// Start PCM on slic
	slic_PCMStart(channel);
	
	slic_toneStart(channel, 1);
	
	while (slic_readHookStatus(channel) == PROSLIC_OFFHOOK)
		msleep(250);
	
	//msleep(5000);
	
	slic_toneStop(channel);
	
	// Stop PCM	
	slic_PCMStop(channel);
	iproc_tdm_pcm_stop(tdm_runtime);
	return 0;
}

static int dial_toneGen(void)
{
	unsigned char channel;
	channel = 0;
	
	DBG("Please pick up the phone\n");
	// Wait for phone to be picked up
	while (slic_readHookStatus(channel) == PROSLIC_ONHOOK)
		msleep(250);

	DBG("Generating dial tone\n");
	
	slic_toneSetup(channel, TONEGEN_FCC_DIAL);
	
	slic_toneStart(channel, 1);
	
	while (slic_readHookStatus(channel) == PROSLIC_OFFHOOK)
		msleep(250);

	slic_toneStop(channel);
		
	return 0;
}

static int busy_toneGen(void)
{
	unsigned char channel;
	channel = 0;

	DBG("Please pick up the phone\n");
	// Wait for phone to be picked up
	while (slic_readHookStatus(channel) == PROSLIC_ONHOOK)
		msleep(250);
		
	DBG("Generating busy tone\n");
	
	slic_toneSetup(channel, TONEGEN_FCC_BUSY);
	
	while (slic_readHookStatus(channel) == PROSLIC_OFFHOOK)
	{
		slic_toneStart(channel, 0);
		msleep(500);
		slic_toneStop(channel);
		msleep(500);
	}

	return 0;
}

static int ringback_toneGen(void)
{
	unsigned char channel;
	channel = 0;

	DBG("Please pick up the phone\n");
	// Wait for phone to be picked up
	while (slic_readHookStatus(channel) == PROSLIC_ONHOOK)
		msleep(250);
		
	DBG("Generating Ringback tone\n");
	
	slic_toneSetup(channel, TONEGEN_FCC_RINGBACK);
	
	while (slic_readHookStatus(channel) == PROSLIC_OFFHOOK)
	{
		slic_toneStart(channel, 0);
		msleep(2000);
		slic_toneStop(channel);
		msleep(4000);
	}
	
	return 0;
}

static int iproc_tdm_allocate_dma_buffer(iproc_tdm_info_t *tdminfo)
{

	size_t size = DMABUF_MAX_SIZE;
	
	// Test
	int i, j;
	int8_t sin_1000[8] = {0x0, 0x5a, 0x7f, 0x5a, 0x0, 0xa5, 0x80, 0xa5};
	int8_t sin_500[16] = {0x0, 0x30, 0x5a, 0x76, 0x7f, 0x76, 0x5a, 0x30, 0x0, 0xcf, 0xa5, 0x89, 0x80, 0x89, 0xa5, 0xcf};
	int8_t sin_500_shift[16] = {0x7f, 0x76, 0x5a, 0x30, 0x0, 0xcf, 0xa5, 0x89, 0x80, 0x89, 0xa5, 0xcf, 0x0, 0x30, 0x5a, 0x76};

	int16_t *buffer;
	double max_phase;

	//DBG("%s\n", __FUNCTION__);
	
	tdminfo->dmaBuf = kmalloc(size, GFP_ATOMIC);
	DBG("%s: size %d @ 0x%p\n", __FUNCTION__, size, tdminfo->dmaBuf);

	if (!tdminfo->dmaBuf) {
		DBG("%s: dma_alloc failed\n", __FUNCTION__);
		return -ENOMEM;
	}

	// Test, populate buffer with 500Hz sine wave
	// Assume 8-bit PCM format and 8000Hz sampling frequency
	i = 0;
	j = 0;
	
	while (i < size/4)  // No of words per channel
	{
		for (j = 0; j < 16; j++) {
#if defined TX_TEST_SINEWAVE		
			buffer[i++] = sin_500[j];
			//buffer[i++] = sin_500_shift[j];
#else
			buffer[i++] = 0;
			//buffer[i++] = 0;
#endif			
		}
	}

	return 0;
}

/*
 * Function Name: tdm_open
 * Description  : Called when an user application opens this device.
 * Returns      : 0 - success
 */
static int tdm_open(struct inode *inode, struct file *fp)
{
	//DBG("%s\n", __FUNCTION__);
	
	iproc_tdm_allocate_dma_buffer(tdm_info);
	
	/* probably should put this somewhere else, after setting up isr ??? */
	dma_txreset(tdm_info->di[0]);
	dma_txinit(tdm_info->di[0]);
	dma_rxreset(tdm_info->di[0]);
	dma_rxinit(tdm_info->di[0]);

	return 0;
}

/*
 * Function Name: tdm_open
 * Description  : Called when an user application opens this device.
 * Returns      : 0 - success
 */
static int tdm_release(struct inode *inode, struct file *fp)
{
	//DBG("%s\n", __FUNCTION__);
	
	/* Turn off interrupts... */
	writel(readl(baseAddr + I2S_INTMASK_REG) & ~I2S_INT_RCV_INT, baseAddr + I2S_INTMASK_REG);
	writel(readl(baseAddr + I2S_INTMASK_REG) & ~I2S_INT_XMT_INT, baseAddr + I2S_INTMASK_REG);
	
	if (tdm_info->dmaBuf) 
	{	
		kfree(tdm_info->dmaBuf);
		tdm_info->dmaBuf = NULL;
	}

	return 0;
}

/*
 * Function Name: tdm_handle_ioctl
 * Description  : Main entry point to handle user applications IOCTL requests
 * Returns      : 0 - success or error
 */
static int tdm_handle_ioctl(struct file *filep,
				unsigned int command, unsigned long arg)
{
	//DBG("%s cmd: %d\n", __FUNCTION__, command);
		
	switch (command)
	{		
	case CMD_RING:
		DBG("Ringing the Phone\n");
		slic_ring();
		//DBG("Slic Test\n");
		//slic_test();
		break;
		
	case CMD_TONE:
		switch (arg)
		{
		case 0:
			DBG("Dial Tone\n");
			dial_toneGen();
			break;
		case 1:
			DBG("Busy Tone\n");		
			busy_toneGen();
			break;
		case 2:
			DBG("Ringback Tone\n");			
			ringback_toneGen();
			break;
		}
		break;
		
	case CMD_DTMFDET:
		DBG("CMD_DTMFDET\n");
		break;
		
	default:
		DBG("Unknown IOCTL\n");			
		break;
	}

	return 0;
}

/* tdm driver file ops */
static struct file_operations tdm_fops =
{
	.unlocked_ioctl  = (void *) tdm_handle_ioctl,
	.open   	= tdm_open,
	.release 	= tdm_release,
};

static int __devinit iproc_tdm_probe(struct platform_device *pdev)
{
	DBG("%s\n", __FUNCTION__);
	
	// Initialize I2S
	iproc_i2s_attach(pdev);
	
	// Setup ISR
	if ((request_irq(tdm_info->irq, iproc_tdm_isr, IRQF_SHARED, "tdm", tdm_info)) < 0)
		DBG("%s: request_irq failure\n", __FUNCTION__);
		
	tdm_runtime = kzalloc(sizeof(iproc_tdm_runtime_data_t), GFP_KERNEL);
	if (tdm_runtime == NULL) {
		return -ENOMEM;
	}
	tdm_runtime->tdm_info = tdm_info;
	spin_lock_init(&tdm_runtime->lock);
	
	// Initialize slic	
	DBG("%s ******************* Initializing SLIC *********************\n", __FUNCTION__);
	slic_init();
	
	return 0;
}

static int iproc_tdm_remove(struct platform_device *pdev)
{
	DBG("%s\n", __FUNCTION__);
	return 0;
}


static int iproc_tdm_suspend(struct platform_device *pdev, pm_message_t state)
{
	DBG("%s\n", __FUNCTION__);
	return 0;
}

static int iproc_tdm_resume(struct platform_device *pdev)
{
	DBG("%s\n", __FUNCTION__);
	return 0;
}

static u64 iproc_tdm_dmamask = DMA_BIT_MASK(32);

static struct platform_driver iproc_tdm_driver =
{
    .driver = {
        .name = "IPROC_TDM",
        .owner = THIS_MODULE,
    },
	.probe      = iproc_tdm_probe,
	.remove     = iproc_tdm_remove,
	.suspend    = iproc_tdm_suspend,
	.resume     = iproc_tdm_resume,
};

static struct platform_device iproc_tdm_device = {
	.name		=	"IPROC_TDM",
	.id		    =	-1,
	.dev =  {
		.init_name = "IPROC_TDM",
		.dma_mask = &iproc_tdm_dmamask,
		.coherent_dma_mask = DMA_BIT_MASK(32),
	},
};

static int __init iproc_tdm_init(void)
{
	int ret;
	
	DBG("%s\n", __FUNCTION__);
	
	DBG("%s  Registering platform driver\n", __FUNCTION__);	
	ret = platform_driver_register(&iproc_tdm_driver);
	if (ret) {
		DBG("Error registering iproc_tdm_driver %d .\n", ret);
		return ret;
	}
	
	DBG("%s  Registering platform device\n", __FUNCTION__);	
    ret = platform_device_register(&iproc_tdm_device);
	if (ret) {
		DBG("Error registering iproc_tdm_device %d .\n", ret);
	    platform_driver_unregister(&iproc_tdm_driver);
	}
	
	DBG("%s  Registering TDM character device\n", __FUNCTION__);
	ret = register_chrdev(TDMDRV_MAJOR, TDMDRV_NAME, &tdm_fops);
	if (ret) {
		DBG("%s Unable to get major number <%d>\n", __FUNCTION__, TDMDRV_MAJOR);
		return -ENOMEM;
	}
	
	return ret;
}

static void __exit iproc_tdm_exit(void)
{
	DBG("%s\n", __FUNCTION__);
	
	if (tdm_info)
		kfree(tdm_info);
		
	if (tdm_runtime)
		kfree(tdm_runtime);
		
	unregister_chrdev(TDMDRV_MAJOR, TDMDRV_NAME);
	platform_device_unregister(&iproc_tdm_device);	
	platform_driver_unregister(&iproc_tdm_driver);	
}

module_init(iproc_tdm_init);
module_exit(iproc_tdm_exit);

/* Module information */
MODULE_DESCRIPTION("IPROC TDM Driver");
MODULE_LICENSE("GPL");
