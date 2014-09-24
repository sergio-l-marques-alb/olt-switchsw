/*
 * net/dsa/mv88e6095.c - Marvell 88e6095/6095f/6131 switch chip support
 * Copyright (c) 2008-2009 Marvell Semiconductor
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/of.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of_platform.h>

#include <linux/list.h>
#include <linux/netdevice.h>
#include <linux/phy.h>
#include <linux/cdev.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>

#include <../net/dsa/dsa_priv.h>
#include <../net/dsa/mv88e6xxx.h>


#define READ_MV_REG_CMD 0x1
#define WRITE_MV_REG_CMD 0x2

//#define SW_CHR_MAJOR 32
//
//static dev_t cdev_devnum;
//static struct device *cdev_sw;
//static struct class *sw_class;
struct miscdevice misc_sw;
static int mvdev_open = 0;

struct mii_bus *global_bus; 
int global_sw_addr;

static int Major;
#define DEVICE_NAME "marvell_sw_dev"

int mv88e6095_reg_read(struct mii_bus *bus, int sw_addr, int addr, int reg)
{
	int ret;

    //printk("Read MV reg %d of port %d\n", reg, addr);

    if (sw_addr != 0) {
        /*
         * Transmit the read command.
         */
        ret = bus->write(bus, sw_addr, 0, 0x9800 | (addr << 5) | reg);
        if (ret < 0)
            return ret;
    
        /*
         * Read the data.
         */
        ret = bus->read(bus, sw_addr, 1);
        if (ret < 0)
            return ret;
    } else {
        ret = bus->read(bus, addr, reg);
        if (ret < 0)
            return ret;
    }

	return ret & 0xffff;
}

int mv88e6095_reg_write(struct mii_bus *bus, int sw_addr, int addr,
			  int reg, u16 val)
{
	int ret;

    //printk("Write MV reg %d of port %d with 0x%04X\n", reg, addr, val);

    if (sw_addr != 0) {
        /*
         * Transmit the data to write.
         */
        ret = bus->write(bus, sw_addr, 1, val);
        if (ret < 0)
            return ret;
    
        /*
         * Transmit the write command.
         */
        ret = bus->write(bus, sw_addr, 0, 0x9400 | (addr << 5) | reg);
        if (ret < 0)
            return ret;
    } else {
        ret = bus->write(bus, addr, reg, val);
        if (ret < 0)
            return ret;
    }

	return 0;
}

static ssize_t read_mv88e6095(struct file *file, char __user *data, size_t size, loff_t *offset)
{
	printk("%s(0x%p, 0x%p, 0x%zu, 0x%p)\n", __FUNCTION__, file, data, size, offset);
    return mv88e6095_reg_read(global_bus, global_sw_addr, data[0], data[1]);
}

static ssize_t write_mv88e6095(struct file *file, const char __user *data, size_t size, loff_t *offset)
{
    u16 val = (data[2]<<8 | data [3]);
	printk("%s(0x%p, 0x%p, 0x%zu, 0x%p)\n", __FUNCTION__, file, data, size, offset);
    mv88e6095_reg_write(global_bus, global_sw_addr, data[0], data[1], val);
	return 0;
}

static int __devinit mv88e6095_probe(struct of_device *ofdev, const struct of_device_id *match)
{
    
    printk ("Probe do driver da PTIN MV88e6095\n");
    return 0;
}

static int mv88e6095_remove(struct of_device *ofdev)
{
    printk ("Remove do driver da PTIN MV88e6095\n");
    
	return 0;
}

static struct of_device_id mv88e6095_match[] = {
	{
		.compatible = "ptin,dsa-switch",
	},
	{},
};
MODULE_DEVICE_TABLE(of, mv88e6095_match);

static int open_mv88e6095(struct inode *inode, struct file *file)
{
	//printk("%s(0x%p, 0x%p)\n", __FUNCTION__, inode, file);
    if (mvdev_open) return -EBUSY;

    mvdev_open=1;
	return 0;
}

static int release_mv88e6095(struct inode *inode, struct file *file)
{
	//printk("%s(0x%p, 0x%p)\n", __FUNCTION__, inode, file);
    mvdev_open=0;
	return 0;
}

static long ioctl_mv88e6095(struct file *file, unsigned int cmd, unsigned long arg) {
	int retval = 0;
	switch ( cmd ) {
        case READ_MV_REG_CMD:
			return (mv88e6095_reg_read(global_bus, global_sw_addr, (unsigned char)(arg>>8), (unsigned char)arg));
			break;
        case WRITE_MV_REG_CMD:
            mv88e6095_reg_write(global_bus, global_sw_addr, (unsigned char)(arg>>24), (unsigned char)(arg>>16), (unsigned short)arg);
			break;
        
		default:
			retval = -EINVAL;
	}
	return retval;
}

static struct file_operations mv_fops = {
    .open = open_mv88e6095,
    .release = release_mv88e6095,
    .read = read_mv88e6095,
    .write = write_mv88e6095,
    .unlocked_ioctl = ioctl_mv88e6095,
};

static struct of_platform_driver mv88e6095_driver = {
	.driver = {
		.name = "mv88e6095",
		.owner = THIS_MODULE,
		.of_match_table = mv88e6095_match,
	},
	.probe = mv88e6095_probe,
	.remove = mv88e6095_remove,
};

static int __init mv88e6095_init(void)
{
    printk ("Init do driver da PTIN MV88e6095\n");

	return of_register_platform_driver(&mv88e6095_driver);
}
module_init(mv88e6095_init);

void mv88e6095_exit(void)
{
    unregister_chrdev(Major, DEVICE_NAME);
	of_unregister_platform_driver(&mv88e6095_driver);
}
module_exit(mv88e6095_exit);

//static struct dsa_switch_driver mv88e6095_switch_driver = {
//	.tag_protocol		= cpu_to_be16(ETH_P_DSA),
//	.priv_size		= sizeof(struct mv88e6xxx_priv_state),
//	.probe			= mv88e6095_probe,
//	.setup			= mv88e6095_setup,
//	.set_addr		= mv88e6xxx_set_addr_direct,
//	.phy_read		= mv88e6095_phy_read,
//	.phy_write		= mv88e6095_phy_write,
//	.poll_link		= mv88e6xxx_poll_link,
//	.get_strings		= mv88e6095_get_strings,
//	.get_ethtool_stats	= mv88e6095_get_ethtool_stats,
//	.get_sset_count		= mv88e6095_get_sset_count,
//};

//static int mv88e6095_switch_reset(struct mii_bus *bus, int sw_addr)
//{
//	int i;
//	int ret;
//
//	/*
//	 * Set all ports to the disabled state.
//	 */
//	for (i = 0; i < 11; i++) {
//		ret = mv88e6095_reg_read(bus, sw_addr, REG_PORT(i), 0x04);
//        mv88e6095_reg_write(bus, sw_addr, REG_PORT(i), 0x04, ret & 0xfffc);
//	}
//
//	/*
//	 * Wait for transmit queues to drain.
//	 */
//	msleep(2);
//
//	/*
//	 * Reset the switch.
//	 */
//    mv88e6095_reg_write(bus, sw_addr, REG_GLOBAL, 0x04, 0xC400);
//
//	/*
//	 * Wait up to one second for reset to complete.
//	 */
//	for (i = 0; i < 1000; i++) {
//		ret = mv88e6095_reg_read(bus, sw_addr, REG_GLOBAL, 0x00);
//		if ((ret & 0xc800) == 0xc800)
//			break;
//
//		msleep(1);
//	}
//	if (i == 1000)
//		return -ETIMEDOUT;
//
//	return 0;
//}
//
//static int mv88e6095_setup_global(struct mii_bus *bus, int sw_addr)
//{
////	int ret;
////	int i;
//
//	/*
//	 * Enable the PHY polling unit, don't discard packets with
//	 * excessive collisions, use a weighted fair queueing scheme
//	 * to arbitrate between packet queues, set the maximum frame
//	 * size to 1632, and mask all interrupt sources.
//	 */
//    mv88e6095_reg_write(bus, sw_addr, REG_GLOBAL, 0x04, 0x4400);
//
//	/*
//	 * Set the default address aging time to 5 minutes, and
//	 * enable address learn messages to be sent to all message
//	 * ports.
//	 */
//    mv88e6095_reg_write(bus, sw_addr, REG_GLOBAL, 0x0A, 0x0148);
//
////	/*
////	 * Configure the priority mapping registers.
////	 */
////	ret = mv88e6xxx_config_prio(ds);
////	if (ret < 0)
////		return ret;
//
//	/*
//	 * Set the VLAN ethertype to 0x8100.
//	 */
//    mv88e6095_reg_write(bus, sw_addr, REG_GLOBAL, 0x19, 0x8100);
//
//	/*
//	 * Disable ARP mirroring, and configure the upstream port as
//	 * the port to which ingress and egress monitor frames are to
//	 * be sent.
//	 */
////	REG_WRITE(REG_GLOBAL, 0x1a, (dsa_upstream_port(ds) * 0x1100) | 0x00f0);
//
//	/*
//	 * Disable cascade port functionality, and set the switch's
//	 * DSA device number.
//	 */
//    mv88e6095_reg_write(bus, sw_addr, REG_GLOBAL, 0x1C, 0xE000);
//
//	/*
//	 * Send all frames with destination addresses matching
//	 * 01:80:c2:00:00:0x to the CPU port.
//	 */
//    mv88e6095_reg_write(bus, sw_addr, REG_GLOBAL2, 0x3, 0xFFFF);
//	//REG_WRITE(REG_GLOBAL2, 0x03, 0xffff);
//
//	/*
//	 * Ignore removed tag data on doubly tagged packets, disable
//	 * flow control messages, force flow control priority to the
//	 * highest, and send all special multicast frames to the CPU
//	 * port at the higest priority.
//	 */
//    mv88e6095_reg_write(bus, sw_addr, REG_GLOBAL2, 0x05, 0x00FF);
//	//REG_WRITE(REG_GLOBAL2, 0x05, 0x00ff);
//
//	/*
//	 * Program the DSA routing table.
//	 */
////	for (i = 0; i < 32; i++) {
////		int nexthop;
////
////		nexthop = 0x1f;
////		if (i != ds->index && i < ds->dst->pd->nr_chips)
////			nexthop = ds->pd->rtable[i] & 0x1f;
////
////		REG_WRITE(REG_GLOBAL2, 0x06, 0x8000 | (i << 8) | nexthop);
////	}
//
//	/*
//	 * Clear all trunk masks.
//	 */
////	for (i = 0; i < 8; i++)
////		REG_WRITE(REG_GLOBAL2, 0x07, 0x8000 | (i << 12) | 0x7ff);
////
////	/*
////	 * Clear all trunk mappings.
////	 */
////	for (i = 0; i < 16; i++)
////		REG_WRITE(REG_GLOBAL2, 0x08, 0x8000 | (i << 11));
//
//	/*
//	 * Force the priority of IGMP/MLD snoop frames and ARP frames
//	 * to the highest setting.
//	 */
//    mv88e6095_reg_write(bus, sw_addr, REG_GLOBAL2, 0x0F, 0x00FF);
//	//REG_WRITE(REG_GLOBAL2, 0x0f, 0x00ff);
//
//	return 0;
//}
//
//static int mv88e6095_setup_port(struct mii_bus *bus, int sw_addr, int p)
//{
//	int addr = REG_PORT(p);
//	u16 val;
//
//	/*
//	 * MAC Forcing register: don't force link, speed, duplex
//	 * or flow control state to any particular values on physical
//	 * ports, but force the CPU port and all DSA ports to 1000 Mb/s
//	 * full duplex.
//	 */
//	//if (dsa_is_cpu_port(ds, p) || ds->dsa_port_mask & (1 << p))
//    if (p == 9 || p == 10)
//        mv88e6095_reg_write(bus, sw_addr, addr, 0x01, 0x003E);
//        //REG_WRITE(addr, 0x01, 0x003e);
//    else
//        mv88e6095_reg_write(bus, sw_addr, addr, 0x01, 0x0003);
//		//REG_WRITE(addr, 0x01, 0x0003);
//
//	/*
//	 * Port Control: disable Core Tag, disable Drop-on-Lock,
//	 * transmit frames unmodified, disable Header mode,
//	 * enable IGMP/MLD snoop, disable DoubleTag, disable VLAN
//	 * tunneling, determine priority by looking at 802.1p and
//	 * IP priority fields (IP prio has precedence), and set STP
//	 * state to Forwarding.
//	 *
//	 * If this is the upstream port for this switch, enable
//	 * forwarding of unknown unicasts, and enable DSA tagging
//	 * mode.
//	 *
//	 * If this is the link to another switch, use DSA tagging
//	 * mode, but do not enable forwarding of unknown unicasts.
//	 */
//	val = 0x0433;
//	if (p == 9 || p == 10)
//		val |= 0x0104;
////	if (ds->dsa_port_mask & (1 << p))
////		val |= 0x0100;
//    mv88e6095_reg_write(bus, sw_addr, addr, 0x04, val);
////	REG_WRITE(addr, 0x04, val);
//
//	/*
//	 * Port Control 1: disable trunking.  Also, if this is the
//	 * CPU port, enable learn messages to be sent to this port.
//	 */
//    if (p==9 || p==10) mv88e6095_reg_write(bus, sw_addr, addr, 0x05, 0x8000);
//    else mv88e6095_reg_write(bus, sw_addr, addr, 0x05, 0x0000);
//	//REG_WRITE(addr, 0x05, dsa_is_cpu_port(ds, p) ? 0x8000 : 0x0000);
//
//	/*
//	 * Port based VLAN map: give each port its own address
//	 * database, allow the CPU port to talk to each of the 'real'
//	 * ports, and allow each of the 'real' ports to only talk to
//	 * the upstream port.
//	 */
////	val = (p & 0xf) << 12;
////	if (dsa_is_cpu_port(ds, p))
////		val |= ds->phys_port_mask;
////	else
////		val |= 1 << dsa_upstream_port(ds);
////	REG_WRITE(addr, 0x06, val);
//
//	/*
//	 * Default VLAN ID and priority: don't set a default VLAN
//	 * ID, and set the default packet priority to zero.
//	 */
//    mv88e6095_reg_write(bus, sw_addr, addr, 0x07, 0x0000);
//	//REG_WRITE(addr, 0x07, 0x0000);
//
//	/*
//	 * Port Control 2: don't force a good FCS, don't use
//	 * VLAN-based, source address-based or destination
//	 * address-based priority overrides, don't let the switch
//	 * add or strip 802.1q tags, don't discard tagged or
//	 * untagged frames on this port, do a destination address
//	 * lookup on received packets as usual, don't send a copy
//	 * of all transmitted/received frames on this port to the
//	 * CPU, and configure the upstream port number.
//	 *
//	 * If this is the upstream port for this switch, enable
//	 * forwarding of unknown multicast addresses.
//	 */
//	val = 0x0080 | 0x9;
//	if (p == 9)
//		val |= 0x0040;
//    mv88e6095_reg_write(bus, sw_addr, addr, 0x08, val);
//	//REG_WRITE(addr, 0x08, val);
//
//	/*
//	 * Rate Control: disable ingress rate limiting.
//	 */
//    mv88e6095_reg_write(bus, sw_addr, addr, 0x09, 0x0000);
//	//REG_WRITE(addr, 0x09, 0x0000);
//
//	/*
//	 * Rate Control 2: disable egress rate limiting.
//	 */
//    mv88e6095_reg_write(bus, sw_addr, addr, 0x0A, 0x0000);
//	//REG_WRITE(addr, 0x0a, 0x0000);
//
//	/*
//	 * Port Association Vector: when learning source addresses
//	 * of packets, add the address to the address database using
//	 * a port bitmap that has only the bit for this port set and
//	 * the other bits clear.
//	 */
//	//REG_WRITE(addr, 0x0b, 1 << p);
//
//	/*
//	 * Tag Remap: use an identity 802.1p prio -> switch prio
//	 * mapping.
//	 */
//	//REG_WRITE(addr, 0x18, 0x3210);
//
//	/*
//	 * Tag Remap 2: use an identity 802.1p prio -> switch prio
//	 * mapping.
//	 */
//	//REG_WRITE(addr, 0x19, 0x7654);
//
//	return 0;
//}
//
//static int mv88e6095_setup(struct mii_bus *bus, int sw_addr)
//{
//	int i;
//	int ret;
//
////	mv88e6xxx_ppu_state_init(ds);
//
//	ret = mv88e6095_switch_reset(bus, sw_addr);
//	if (ret < 0)
//		return ret;
//
//	/* @@@ initialise vtu and atu */
//
//	ret = mv88e6095_setup_global(bus, sw_addr);
//	if (ret < 0)
//		return ret;
//
//	for (i = 0; i < 11; i++) {
//		ret = mv88e6095_setup_port(bus, sw_addr, i);
//		if (ret < 0)
//			return ret;
//	}
//
//	return 0;
//}
//
//static int mv88e6095_port_to_phy_addr(int port)
//{
//	if (port >= 0 && port <= 11)
//		return port;
//	return -1;
//}
//
//static int
//mv88e6095_phy_read(struct dsa_switch *ds, int port, int regnum)
//{
//	int addr = mv88e6095_port_to_phy_addr(port);
//	return mv88e6xxx_phy_read_ppu(ds, addr, regnum);
//}
//
//static int
//mv88e6095_phy_write(struct dsa_switch *ds,
//			      int port, int regnum, u16 val)
//{
//	int addr = mv88e6095_port_to_phy_addr(port);
//	return mv88e6xxx_phy_write_ppu(ds, addr, regnum, val);
//}
//
//static struct mv88e6xxx_hw_stat mv88e6095_hw_stats[] = {
//	{ "in_good_octets", 8, 0x00, },
//	{ "in_bad_octets", 4, 0x02, },
//	{ "in_unicast", 4, 0x04, },
//	{ "in_broadcasts", 4, 0x06, },
//	{ "in_multicasts", 4, 0x07, },
//	{ "in_pause", 4, 0x16, },
//	{ "in_undersize", 4, 0x18, },
//	{ "in_fragments", 4, 0x19, },
//	{ "in_oversize", 4, 0x1a, },
//	{ "in_jabber", 4, 0x1b, },
//	{ "in_rx_error", 4, 0x1c, },
//	{ "in_fcs_error", 4, 0x1d, },
//	{ "out_octets", 8, 0x0e, },
//	{ "out_unicast", 4, 0x10, },
//	{ "out_broadcasts", 4, 0x13, },
//	{ "out_multicasts", 4, 0x12, },
//	{ "out_pause", 4, 0x15, },
//	{ "excessive", 4, 0x11, },
//	{ "collisions", 4, 0x1e, },
//	{ "deferred", 4, 0x05, },
//	{ "single", 4, 0x14, },
//	{ "multiple", 4, 0x17, },
//	{ "out_fcs_error", 4, 0x03, },
//	{ "late", 4, 0x1f, },
//	{ "hist_64bytes", 4, 0x08, },
//	{ "hist_65_127bytes", 4, 0x09, },
//	{ "hist_128_255bytes", 4, 0x0a, },
//	{ "hist_256_511bytes", 4, 0x0b, },
//	{ "hist_512_1023bytes", 4, 0x0c, },
//	{ "hist_1024_max_bytes", 4, 0x0d, },
//};
//
//static void
//mv88e6095_get_strings(struct dsa_switch *ds, int port, uint8_t *data)
//{
//	mv88e6xxx_get_strings(ds, ARRAY_SIZE(mv88e6095_hw_stats),
//			      mv88e6095_hw_stats, port, data);
//}
//
//static void
//mv88e6095_get_ethtool_stats(struct dsa_switch *ds,
//				  int port, uint64_t *data)
//{
//	mv88e6xxx_get_ethtool_stats(ds, ARRAY_SIZE(mv88e6095_hw_stats),
//				    mv88e6095_hw_stats, port, data);
//}
//
//static int mv88e6095_get_sset_count(struct dsa_switch *ds)
//{
//	return ARRAY_SIZE(mv88e6095_hw_stats);
//}
//



struct phy_device * mv88e6095_detect(struct mii_bus *bus, int sw_addr)
{
    struct phy_device *dev = NULL;
	u32 phy_id;
    int i, ret=0;

    /*volatile u32 *util_regs;*/

    Major = register_chrdev(0, DEVICE_NAME, &mv_fops);

	if (Major < 0) {
	    printk(KERN_ALERT "Registering marvell char device failed with %d\n", Major);
	}else {
        printk(KERN_ALERT "Registering marvell char device with %d \n", Major);
    }

	/*util_regs = ioremap(0xE00E0040, sizeof(u32));
    printk ("Release MV88e6095 reset (gpio state: 0x%08X)\n", *util_regs);
    *util_regs |= 0x04000000;
    msleep(50);
    printk ("Result (gpio state: 0x%08X)\n", *util_regs);*/

    printk ("Initializing MV88e6095 (addr %d): \n", sw_addr);

	ret = mv88e6095_reg_read(bus, sw_addr, REG_PORT(0), 0x03);
	if (ret >= 0 && ret != 0xFFFF) {
		phy_id = ret & 0xfff0;
		if (phy_id == 0x0950)
			printk("Marvell 88E6095/88E6095F Detected\n");
		else if (phy_id == 0x1060)
			printk("Marvell 88E6131 Detected\n");
        else{
            printk("Unknown Device Detected - but will use it anyway\n");
        }
	}else{
        printk("Marvell 88E6095/88E6095F not Detected - but will use it anyway\n");
        phy_id = 0x0950;
    }

	dev = phy_device_create(bus, sw_addr, phy_id);

 //   printk ("Set MV88e6095 port bitmap...\n");
 //   //FE Ports
 //   for (i=0;i<8;i++){
 //       mv88e6095_reg_write(bus, sw_addr, REG_PORT(i), 0x06, 0x0600);
 //   }
 //
 //   //GbE Ports
 //   mv88e6095_reg_write(bus, sw_addr, REG_PORT(8),  0x06, 0x0600);    //HUGO porto G0 liga ao switch FPGA CXP360G
 //   mv88e6095_reg_write(bus, sw_addr, REG_PORT(9),  0x06, 0x05ff);
 //   mv88e6095_reg_write(bus, sw_addr, REG_PORT(10), 0x06, 0x03ff);
    
    printk ("Activate MV88e6095 ports\n");
    //GbE Ports
    mv88e6095_reg_write(bus, sw_addr, REG_PORT(8), 0x01, 0x003E); //HUGO porto G0 liga ao switch FPGA CXP360G
    mv88e6095_reg_write(bus, sw_addr, REG_PORT(9), 0x01, 0x003E);
    mv88e6095_reg_write(bus, sw_addr, REG_PORT(10), 0x01, 0x003E);

    //FE Ports
    for (i=0;i<8;i++){
        printk ("Switch Port %d Control reg: 0x%X\r\n", i, mv88e6095_reg_read(bus, sw_addr, REG_PORT(i), 0x4));
        mv88e6095_reg_write(bus, sw_addr, REG_PORT(i), 0x04, 0x0077);
        msleep(100);
        printk ("Switch Port %d Control reg: 0x%X\r\n", i, mv88e6095_reg_read(bus, sw_addr, REG_PORT(i), 0x4));
    }

    global_bus = bus;
    global_sw_addr = sw_addr;

	return dev;

}

MODULE_LICENSE("GPL");

