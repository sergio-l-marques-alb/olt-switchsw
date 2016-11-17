/***************************************************************************/

/*
 *	m527x.c  -- platform support for ColdFire 527x based boards
 *
 *	Sub-architcture dependent initialization code for the Freescale
 *	5270/5271 and 5274/5275 CPUs.
 *
 *	Copyright (C) 1999-2004, Greg Ungerer (gerg@snapgear.com)
 *	Copyright (C) 2001-2004, SnapGear Inc. (www.snapgear.com)
 */

/***************************************************************************/

#include <linux/kernel.h>
#include <linux/param.h>
#include <linux/init.h>
#include <linux/io.h>
#include <asm/machdep.h>
#include <asm/coldfire.h>
#include <asm/mcfsim.h>
#include <asm/mcfuart.h>
#include <asm/mcfclk.h>

/***************************************************************************/

DEFINE_CLK(pll, "pll.0", MCF_CLK);
DEFINE_CLK(sys, "sys.0", MCF_BUSCLK);
DEFINE_CLK(mcfpit0, "mcfpit.0", MCF_CLK);
DEFINE_CLK(mcfpit1, "mcfpit.1", MCF_CLK);
DEFINE_CLK(mcfpit2, "mcfpit.2", MCF_CLK);
DEFINE_CLK(mcfpit3, "mcfpit.3", MCF_CLK);
DEFINE_CLK(mcfuart0, "mcfuart.0", MCF_BUSCLK);
DEFINE_CLK(mcfuart1, "mcfuart.1", MCF_BUSCLK);
DEFINE_CLK(mcfuart2, "mcfuart.2", MCF_BUSCLK);
DEFINE_CLK(mcfqspi0, "mcfqspi.0", MCF_BUSCLK);
DEFINE_CLK(fec0, "fec.0", MCF_BUSCLK);
DEFINE_CLK(fec1, "fec.1", MCF_BUSCLK);

struct clk *mcf_clks[] = {
	&clk_pll,
	&clk_sys,
	&clk_mcfpit0,
	&clk_mcfpit1,
	&clk_mcfpit2,
	&clk_mcfpit3,
	&clk_mcfuart0,
	&clk_mcfuart1,
	&clk_mcfuart2,
	&clk_mcfqspi0,
	&clk_fec0,
	&clk_fec1,
	NULL
};

/***************************************************************************/

static void __init m527x_qspi_init(void)
{
#if IS_ENABLED(CONFIG_SPI_COLDFIRE_QSPI)
#if defined(CONFIG_M5271)
	u16 par;

	/* setup QSPS pins for QSPI with gpio CS control */
	writeb(0x1f, MCFGPIO_PAR_QSPI);
	/* and CS2 & CS3 as gpio */
	par = readw(MCFGPIO_PAR_TIMER);
	par &= 0x3f3f;
	writew(par, MCFGPIO_PAR_TIMER);
#elif defined(CONFIG_M5275)
	/* setup QSPS pins for QSPI with gpio CS control */
	writew(0x003e, MCFGPIO_PAR_QSPI);
#endif
#endif /* IS_ENABLED(CONFIG_SPI_COLDFIRE_QSPI) */
}

/***************************************************************************/

static void __init m527x_uarts_init(void)
{
	u16 sepmask;

	/*
	 * External Pin Mask Setting & Enable External Pin for Interface
	 */
	sepmask = readw(MCFGPIO_PAR_UART);
	sepmask |= UART0_ENABLE_MASK | UART1_ENABLE_MASK | UART2_ENABLE_MASK;
	writew(sepmask, MCFGPIO_PAR_UART);
}

/***************************************************************************/

static void __init m527x_fec_init(void)
{
	u16 par;
	u8 v;

	/* Set multi-function pins to ethernet mode for fec0 */
#if defined(CONFIG_M5271)
	v = readb(MCFGPIO_PAR_FECI2C);
	writeb(v | 0xf0, MCFGPIO_PAR_FECI2C);
#else
	par = readw(MCFGPIO_PAR_FECI2C);
	writew(par | 0xf00, MCFGPIO_PAR_FECI2C);
	v = readb(MCFGPIO_PAR_FEC0HL);
	writeb(v | 0xc0, MCFGPIO_PAR_FEC0HL);

	/* Set multi-function pins to ethernet mode for fec1 */
	par = readw(MCFGPIO_PAR_FECI2C);
	writew(par | 0xa0, MCFGPIO_PAR_FECI2C);
	v = readb(MCFGPIO_PAR_FEC1HL);
	writeb(v | 0xc0, MCFGPIO_PAR_FEC1HL);
#endif
}

/***************************************************************************/

void __init config_BSP(char *commandp, int size)
{
	mach_sched_init = hw_timer_init;
	m527x_uarts_init();
	m527x_fec_init();
	m527x_qspi_init();
}

/***************************************************************************/

/*
 *	linux/arch/m68knommu/platform/527x/config.c
 *
 *	Sub-architcture dependent initialization code for the Freescale
 *	5270/5271 CPUs.
 *
 *	Copyright (C) 1999-2004, Greg Ungerer (gerg@snapgear.com)
 *	Copyright (C) 2001-2004, SnapGear Inc. (www.snapgear.com)
 */

/***************************************************************************/

#include <linux/kernel.h>
#include <linux/param.h>
#include <linux/init.h>
#include <linux/io.h>
#include <asm/machdep.h>
#include <asm/coldfire.h>
#include <asm/mcfsim.h>
#include <asm/mcfuart.h>

/***************************************************************************/

<<<<<<< HEAD
void coldfire_reset(void);
#if defined(CONFIG_UBOOT)
void parse_uboot_commandline(char *commandp, int size);
#endif

/***************************************************************************/

static struct mcf_platform_uart m527x_uart_platform[] = {
	{
		.mapbase	= MCFUART_BASE1,
		.irq		= MCFINT_VECBASE + MCFINT_UART0,
	},
	{
		.mapbase 	= MCFUART_BASE2,
		.irq		= MCFINT_VECBASE + MCFINT_UART1,
	},
	{
		.mapbase 	= MCFUART_BASE3,
		.irq		= MCFINT_VECBASE + MCFINT_UART2,
	},
	{ },
};

static struct platform_device m527x_uart = {
	.name			= "mcfuart",
	.id			= 0,
	.dev.platform_data	= m527x_uart_platform,
};

static struct resource m527x_fec0_resources[] = {
	{
		.start		= MCFFEC_BASE0,
		.end		= MCFFEC_BASE0 + MCFFEC_SIZE0 - 1,
		.flags		= IORESOURCE_MEM,
	},
	{
		.start		= 64 + 23,
		.end		= 64 + 23,
		.flags		= IORESOURCE_IRQ,
	},
	{
		.start		= 64 + 27,
		.end		= 64 + 27,
		.flags		= IORESOURCE_IRQ,
	},
	{
		.start		= 64 + 29,
		.end		= 64 + 29,
		.flags		= IORESOURCE_IRQ,
	},
};

static struct resource m527x_fec1_resources[] = {
	{
		.start		= MCFFEC_BASE1,
		.end		= MCFFEC_BASE1 + MCFFEC_SIZE1 - 1,
		.flags		= IORESOURCE_MEM,
	},
	{
		.start		= 128 + 23,
		.end		= 128 + 23,
		.flags		= IORESOURCE_IRQ,
	},
	{
		.start		= 128 + 27,
		.end		= 128 + 27,
		.flags		= IORESOURCE_IRQ,
	},
	{
		.start		= 128 + 29,
		.end		= 128 + 29,
		.flags		= IORESOURCE_IRQ,
	},
};

static struct platform_device m527x_fec[] = {
	{
		.name		= "fec",
		.id		= 0,
		.num_resources	= ARRAY_SIZE(m527x_fec0_resources),
		.resource	= m527x_fec0_resources,
	},
	{
		.name		= "fec",
		.id		= 1,
		.num_resources	= ARRAY_SIZE(m527x_fec1_resources),
		.resource	= m527x_fec1_resources,
	},
};

#if defined(CONFIG_SPI_COLDFIRE_QSPI) || defined(CONFIG_SPI_COLDFIRE_QSPI_MODULE)
static struct resource m527x_qspi_resources[] = {
	{
		.start		= MCFQSPI_IOBASE,
		.end		= MCFQSPI_IOBASE + MCFQSPI_IOSIZE - 1,
		.flags		= IORESOURCE_MEM,
	},
	{
		.start		= MCFINT_VECBASE + MCFINT_QSPI,
		.end		= MCFINT_VECBASE + MCFINT_QSPI,
		.flags		= IORESOURCE_IRQ,
	},
};

#if defined(CONFIG_M5271)
#define MCFQSPI_CS0    91
#define MCFQSPI_CS1    92
#define MCFQSPI_CS2    99
#define MCFQSPI_CS3    103
#elif defined(CONFIG_M5275)
#define MCFQSPI_CS0    59
#define MCFQSPI_CS1    60
#define MCFQSPI_CS2    61
#define MCFQSPI_CS3    62
#endif

static int m527x_cs_setup(struct mcfqspi_cs_control *cs_control)
{
	int status;

	status = gpio_request(MCFQSPI_CS0, "MCFQSPI_CS0");
	if (status) {
		pr_debug("gpio_request for MCFQSPI_CS0 failed\n");
		goto fail0;
	}
	status = gpio_direction_output(MCFQSPI_CS0, 1);
	if (status) {
		pr_debug("gpio_direction_output for MCFQSPI_CS0 failed\n");
		goto fail1;
	}

	status = gpio_request(MCFQSPI_CS1, "MCFQSPI_CS1");
	if (status) {
		pr_debug("gpio_request for MCFQSPI_CS1 failed\n");
		goto fail1;
	}
	status = gpio_direction_output(MCFQSPI_CS1, 1);
	if (status) {
		pr_debug("gpio_direction_output for MCFQSPI_CS1 failed\n");
		goto fail2;
	}

	status = gpio_request(MCFQSPI_CS2, "MCFQSPI_CS2");
	if (status) {
		pr_debug("gpio_request for MCFQSPI_CS2 failed\n");
		goto fail2;
	}
	status = gpio_direction_output(MCFQSPI_CS2, 1);
	if (status) {
		pr_debug("gpio_direction_output for MCFQSPI_CS2 failed\n");
		goto fail3;
	}

	status = gpio_request(MCFQSPI_CS3, "MCFQSPI_CS3");
	if (status) {
		pr_debug("gpio_request for MCFQSPI_CS3 failed\n");
		goto fail3;
	}
	status = gpio_direction_output(MCFQSPI_CS3, 1);
	if (status) {
		pr_debug("gpio_direction_output for MCFQSPI_CS3 failed\n");
		goto fail4;
	}

	return 0;

fail4:
	gpio_free(MCFQSPI_CS3);
fail3:
	gpio_free(MCFQSPI_CS2);
fail2:
	gpio_free(MCFQSPI_CS1);
fail1:
	gpio_free(MCFQSPI_CS0);
fail0:
	return status;
}

static void m527x_cs_teardown(struct mcfqspi_cs_control *cs_control)
{
	gpio_free(MCFQSPI_CS3);
	gpio_free(MCFQSPI_CS2);
	gpio_free(MCFQSPI_CS1);
	gpio_free(MCFQSPI_CS0);
}

static void m527x_cs_select(struct mcfqspi_cs_control *cs_control,
			    u8 chip_select, bool cs_high)
{
	switch (chip_select) {
	case 0:
		gpio_set_value(MCFQSPI_CS0, cs_high);
		break;
	case 1:
		gpio_set_value(MCFQSPI_CS1, cs_high);
		break;
	case 2:
		gpio_set_value(MCFQSPI_CS2, cs_high);
		break;
	case 3:
		gpio_set_value(MCFQSPI_CS3, cs_high);
		break;
	}
}

static void m527x_cs_deselect(struct mcfqspi_cs_control *cs_control,
			      u8 chip_select, bool cs_high)
{
	switch (chip_select) {
	case 0:
		gpio_set_value(MCFQSPI_CS0, !cs_high);
		break;
	case 1:
		gpio_set_value(MCFQSPI_CS1, !cs_high);
		break;
	case 2:
		gpio_set_value(MCFQSPI_CS2, !cs_high);
		break;
	case 3:
		gpio_set_value(MCFQSPI_CS3, !cs_high);
		break;
	}
}

static struct mcfqspi_cs_control m527x_cs_control = {
	.setup                  = m527x_cs_setup,
	.teardown               = m527x_cs_teardown,
	.select                 = m527x_cs_select,
	.deselect               = m527x_cs_deselect,
};

static struct mcfqspi_platform_data m527x_qspi_data = {
	.bus_num		= 0,
	.num_chipselect		= 4,
	.cs_control		= &m527x_cs_control,
};

static struct platform_device m527x_qspi = {
	.name			= "mcfqspi",
	.id			= 0,
	.num_resources		= ARRAY_SIZE(m527x_qspi_resources),
	.resource		= m527x_qspi_resources,
	.dev.platform_data	= &m527x_qspi_data,
};
=======
#ifdef CONFIG_SPI_COLDFIRE_QSPI
>>>>>>> 437538267b672f9320833907f1b5acbb2605f4be

static void __init m527x_qspi_init(void)
{
#if defined(CONFIG_M5271)
	u16 par;

	/* setup QSPS pins for QSPI with gpio CS control */
	writeb(0x1f, MCFGPIO_PAR_QSPI);
	/* and CS2 & CS3 as gpio */
	par = readw(MCFGPIO_PAR_TIMER);
	par &= 0x3f3f;
	writew(par, MCFGPIO_PAR_TIMER);
#elif defined(CONFIG_M5275)
	/* setup QSPS pins for QSPI with gpio CS control */
	writew(0x003e, MCFGPIO_PAR_QSPI);
#endif
}

#endif /* CONFIG_SPI_COLDFIRE_QSPI */

/***************************************************************************/

static void __init m527x_uarts_init(void)
{
	u16 sepmask;

	/*
	 * External Pin Mask Setting & Enable External Pin for Interface
	 */
	sepmask = readw(MCF_IPSBAR + MCF_GPIO_PAR_UART);
	sepmask |= UART0_ENABLE_MASK | UART1_ENABLE_MASK | UART2_ENABLE_MASK;
	writew(sepmask, MCF_IPSBAR + MCF_GPIO_PAR_UART);
}

/***************************************************************************/

static void __init m527x_fec_init(void)
{
	u16 par;
	u8 v;

	/* Set multi-function pins to ethernet mode for fec0 */
#if defined(CONFIG_M5271)
	v = readb(MCF_IPSBAR + 0x100047);
	writeb(v | 0xf0, MCF_IPSBAR + 0x100047);
#else
	par = readw(MCF_IPSBAR + 0x100082);
	writew(par | 0xf00, MCF_IPSBAR + 0x100082);
	v = readb(MCF_IPSBAR + 0x100078);
	writeb(v | 0xc0, MCF_IPSBAR + 0x100078);
#endif

#ifdef CONFIG_FEC2
	/* Set multi-function pins to ethernet mode for fec1 */
	par = readw(MCF_IPSBAR + 0x100082);
	writew(par | 0xa0, MCF_IPSBAR + 0x100082);
	v = readb(MCF_IPSBAR + 0x100079);
	writeb(v | 0xc0, MCF_IPSBAR + 0x100079);
#endif
}

/***************************************************************************/

void __init config_BSP(char *commandp, int size)
{
<<<<<<< HEAD
	mcf_disableall();

#ifdef CONFIG_BOOTPARAM
	strncpy(commandp, CONFIG_BOOTPARAM_STRING, size);
	commandp[size-1] = 0;
#elif defined(CONFIG_UBOOT)
	parse_uboot_commandline(commandp, size);
#else
	memset(commandp, 0, size);
#endif

	mach_reset = m527x_cpu_reset;
=======
	mach_sched_init = hw_timer_init;
>>>>>>> 437538267b672f9320833907f1b5acbb2605f4be
	m527x_uarts_init();
	m527x_fec_init();
#ifdef CONFIG_SPI_COLDFIRE_QSPI
	m527x_qspi_init();
#endif
}

/***************************************************************************/
