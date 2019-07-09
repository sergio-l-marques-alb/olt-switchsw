#include <linux/init.h>
#include <linux/module.h>
#include <linux/spi/spi.h>

static struct spi_device *spi_device_0;
static struct spi_device *spi_device_1;
static struct spi_device *spi_device_2;

static int __init spi_init(void) {
    int ret;
    struct spi_master *master;


    //Register information about your slave device:
    struct spi_board_info spi_device_info_0 = {                     //CPLD
        .modalias = "spidev",
        .max_speed_hz = 400000, //speed your device (slave) can handle
        .bus_num = 1,
        .chip_select = 0,
        .mode = 0,
    };
    struct spi_board_info spi_device_info_1 = {                     //FPGA
        .modalias = "spidev",
        .max_speed_hz = 400000, //speed your device (slave) can handle
        .bus_num = 1,
        .chip_select = 1,
        .mode = 0,
    };
    struct spi_board_info spi_device_info_2 = {                     //LOAD FPGA
        .modalias = "spidev",
        .max_speed_hz = 400000, //speed your device (slave) can handle
        .bus_num = 1,
        .chip_select = 2,
        .mode = 0,
    };

    /*To send data we have to know what spi port/pins should be used. This information
      can be found in the device-tree. */
    master = spi_busnum_to_master( spi_device_info_0.bus_num );
    if( !master ){
        printk("MASTER not found.\n");
            return -ENODEV;
    }
    
    // create a new slave device, given the master and device info
    spi_device_0 = spi_new_device( master, &spi_device_info_0);
    if( !spi_device_0 ) {
        printk("FAILED to create slave device 0.\n");
        //return -ENODEV;
    }

    // create a new slave device, given the master and device info
    spi_device_1 = spi_new_device( master, &spi_device_info_1);
    if( !spi_device_1 ) {
        printk("FAILED to create slave device 1.\n");
        //return -ENODEV;
    }

    // create a new slave device, given the master and device info
    spi_device_2 = spi_new_device( master, &spi_device_info_2);
    if( !spi_device_2 ) {
        printk("FAILED to create slave device 2.\n");
        //return -ENODEV;
    }

    spi_device_0->bits_per_word = 16;
    spi_device_1->bits_per_word = 16;
    spi_device_2->bits_per_word = 8;

    ret = spi_setup( spi_device_0 );
    if( ret ){
        printk("FAILED to setup slave 0.\n");
        spi_unregister_device( spi_device_0 );
        //return -ENODEV;
    }
    ret = spi_setup( spi_device_1 );
    if( ret ){
        printk("FAILED to setup slave 1.\n");
        spi_unregister_device( spi_device_1 );
        //return -ENODEV;
    }
    ret = spi_setup( spi_device_2 );
    if( ret ){
        printk("FAILED to setup slave 2.\n");
        spi_unregister_device( spi_device_2 );
        //return -ENODEV;
   }
     
    printk("All SPI devices created!");
    return 0;
}


static void __exit spi_exit(void)
{
    if( spi_device_0 ){
        spi_unregister_device( spi_device_0 );
    }
    if( spi_device_1 ){
        spi_unregister_device( spi_device_1 );
    }
    if( spi_device_2 ){
        spi_unregister_device( spi_device_2 );
    }

}

module_init(spi_init);
module_exit(spi_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sergio Marques <email protected>");
MODULE_DESCRIPTION("PTIN SPI module");
