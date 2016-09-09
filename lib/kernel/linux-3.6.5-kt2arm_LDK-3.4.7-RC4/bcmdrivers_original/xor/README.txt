SYSTEM SETUP
===================================================================

config files discussed are in bcmdrivers/crypto/setup

to make tools
    cd $LDK_BASE/buildroot
    make iproc-tools_defconfig
    make

to make uboot
    cd $LDK_BASE/bootloader/u-boot-2012.10-rc3
    make O=./build-output distclean
    make O=./build-output northstar_plus_config
    make O=./build-output all   

proper kernel config
    make sure the following modules are enabled:
         Drivers
                        
Cryptographic API
    System Type
        Broadcom IPROC architecture based implementations
            Broadcom iProc Drivers
                NS+ XOR offload support
                FA+ support
                PAE support
    Device Drivers
        Serial ATA and Parallel ATA drivers
            ARASAN CompactFlash PATA Controller Support
        DMA Engine support
            DMA Engine debugging
            DMA Engine verbose debugging
            Async_tx: Offload support for the async_tx api
            DMA Test client
        Multiple devices driver support (RAID and LVM)
            RAID support
            Autodetect RAID arrays during kernel boot
            Linear (append) mode
            RAID-0 (striping) mode
            RAID-1 (mirroring) mode
            RAID-10 (mirrored striping) mode
            RAID-4/RAID-5/RAID-6 mode
    Hardware Hacking
        Self test for hardware accelerated raid6 recovery

to make buildroot
    cd $LDK_BASE/buildroot
    make northstar-plus-initramfs_defconfig
    make

    # do a full build (pae firmware, system, make the image and drop it in the right place)
    buildnsp (described below)  

      
build the buidroot: We use the following macro and export
  (exports are set for my system for reference):

export NSHOME=$P4ROOT/sw/iproc
export NSTARGET=/tftp/uImage_nsp3.img
alias buildnsp="pushd $NSHOME/pae_fw && make && cd $NSHOME/buildroot && make && cd $NSHOME/kernel/linux-3.6.5 && $NSHOME/bootloader/u-boot-2012.10-rc3/build-output/tools/mkimage -A arm -O linux -T kernel -n Image -a 0x61008000 -C none -d arch/arm/boot/Image ./uImage_nsp.img && mv uImage_nsp.img $NSTARGET ; popd ; date"

I just run 'buildnsp'...


USAGE
===========================================================================

    On boot:
        # These commands would enable operation and leave you ready to work.
        mount -t debugfs none /sys/kernel/debug
        cat /lib/firmware/BCM5302x-PAE.srec >/sys/devices/platform/bcmiproc-pae/fw
        echo "1" > /sys/iproc_xor/enable


    Enabling/Disabling debugging
        # Turn on debug logging of execution flow
        echo "1" > /sys/module/iproc_xor/parameters/xor_debug_logging

        # Add delays in the debugging to separate the flows and/or slow down output
        # The delay is in msec units and is per normal line of debug and
        # and at the end of each hex dump. This is rarely used, normally the
        # default value of '0' is fine.
        echo "5" > /sys/module/iproc_xor/parameters/debug_logging_sleep

    Show driver stats
	    # Show number of operations completed in an arrary of numbers.  Currently
        # only the first number will increment as only the XOR operation is
        # supported
        cat /sys/module/iproc_xor/parameters/xor_op_counts

        # Show the number of bytes processed by the driver since system start
        # or module load.
        cat /sys/module/iproc_xor/parameters/xor_bytes_through

    Misc useful commands
        # Setup a raid5 set of ram disks and fail a drive
        mdadm -C /dev/md0 -v -l5 -c16 -x0 -n3 -z256 /dev/ram10 /dev/ram11 /dev/ram12
        mdadm --fail /dev/md0 /dev/ram11
        mdadm -r /dev/md0 /dev/ram11
        mdadm --add /dev/md0 /dev/ram11
        mdadm --detail --scan
        cat /proc/mdstat
