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
        Cryptographic API
            Software Async Daemon
            Authenc Support
            [M] Testing Module (Note: this needs to be a module)
            CCM, GCM, and Sequence Number IV Generator
            CBC, CTR, and ECB modes
            HMAC mode
            CRC32c, GHASH, MD5, SHA1, SHA224/SHA256, AES, ARC4, DES/3DES, and Pseudo Random Generator
        System Type -> Broadcom IPROC... -> Broadcom iProc Drivers -> Broadcom HND Network Devices
            ET ALL PASSIVE Mode
            BCMCTF Support
            BCM GMAC_ACP
            BCM GMAC prefetching
            - BCM GMAC TX-ON-CPU1 (Note: will slow down system)
            BCM GMAC SKB-RECYCLING
            - BCM GMAC LOCK OPTIMIZATION (Note: will break system)
            BCM GMAC R/W_REG OPTIMIZATION
        System Type -> Broadcom IPROC... -> Broadcom iProc Drivers
            FA+ Support
        System Type -> Broadcom IPROC... -> Broadcom iProc Drivers -> FA+ Support
            NS+ symmetric crypto/hash acceleration (Note: Can be a module for ease of on/off testing)

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


    In uboot:
        Make sure there is an env entry for eth3addr=XX:XX:XX:XX:XX:XX
        (pick a macaddr that makes sense)

        For reference, this is mine:
            DIAGS_BOARD=22SVK
            baudrate=115200
            bootargs=console=ttyS0,115200n8 maxcpus=2 mem=496M
            bootcmd=tftpboot 0x61007fc0 192.168.1.57:uImage_nsp3.img ; bootm
            bootdelay=3
            bootfile=uImage
            brcmtag=1
            clr_env=sf probe 0;sf erase 0xa0000 +1;
            console=ttyS0
            eth1addr=00:10:18:AF:FA:CD
            eth2addr=00:10:18:AF:FA:CE
            eth3addr=00:10:18:AF:FA:CF
            ethact=bcmiproc_eth-0
            ethaddr=00:10:18:AF:FA:D0
            fileaddr=61007FC0
            hostname=northstar3
            ipaddr=192.168.1.67
            linux36naetenv=linux36nandbootargs
            loadaddr=0x90000000
            loglevel=7
            machid=bb8
            serverip=192.168.1.57
            stderr=serial
            stdin=serial
            stdout=serial
            tftpblocksize=512
            vlan1ports=0 1 2 3 8*
            vlan2ports=4 8*
            wanport=4

USAGE
===========================================================================

    On boot:
        ifconfig eth2 up
        ifconfig eth3 up

        mount -t debugfs none /sys/kernel/debug
        cat /lib/firmware/BCM5302x-PAE.srec >/sys/devices/platform/bcmiproc-pae/fw

        modprobe ocf
        modprobe cryptodev
        modprobe cryptosoft

    Enabling/Disabling debugging
        # Turn on debug logging of execution flow
        echo "1" > /sys/module/iproc_crypto/parameters/flow_debug_logging
        # Turn on debug logging of packet data in/out
        echo "1" > /sys/module/iproc_crypto/parameters/packet_debug_logging

        # Add delays in the debugging to separate the flows and/or slow down output
        # The delay is in msec units and is per normal line of debug and
        # and at the end of each hex dump.
        echo "5" > /sys/module/iproc_crypto/parameters/debug_logging_sleep

    Misc useful commands
        # print out usage info
        cryptotest --help
        # do a paired cbc-aes encrypt/decryp with verbose output
        cryptotest -v -a aes
        # same operation and check the final cleartext with original
        cryptotest -c -v -a aes
        # same operation but on a 1024 byte datablock (random).
        cryptotest -c -v -a aes 1 1024
        # same operation but performed 1000 times, new random data each time.
        cryptotest -c -v -a aes 1000 1024
        # same operation but all key/iv/data is 0x00. Quiet for timings
        cryptotest -Z -c -a aes 1000 1024
        # do 1 encrypt and then 1000 decrypt operations - (U)nencrypt only, quiet
        cryptotest -U -a aes 1000 1024
        # do 1000 encrypt operations and then 1 decrypt operation - (E)ncrypt only, quiet
        cryptotest -E -a aes 1000 1024

        # generate a sha1 hash of a 160 byte datablock
        cryptotest -v -a sha1 1 160
        # generate an md5 hash of a 2k byte datablock 2000 times - quiet
        cryptotest -a md5 2000 2048

        # use openssl to benchmark openssl->ocf->cryptoAPI->iproc-crypto module
        openssl speed -evp aes-128-cbc -engine cryptodev -elapsed
        # use openssl to run a single encrypt operation
        echo -n "@@@@@@@@@@@@@@@" | \
        openssl aes-128-ecb -K 00000000000000000000000000000000 -iv 00000000000000000000000000000000 | \
        hexdump -C

    Commands for self-tests
        # NOTE: The module will _always_ error out.  This is to allow it to be immediately rerun with
        # another test. The error will be:
        # modprobe: can't load module tcrypt (kernel/crypto/tcrypt.ko): Resource temporarily unavailable
        # Any other errors are actual failures of some kind (possibly expected, i.e. for unsupported things).
        # Other than the above error, silence is a positive result.

        # NOTE: Need Authenc + "Testing Module" as stated above in "proper kernel config"

        modprobe tcrypt mode=1   # md5
        modprobe tcrypt mode=2   # sha1
        modprobe tcrypt mode=6   # sha256
        modprobe tcrypt mode=3   # ecb(des), cbc(des)
        modprobe tcrypt mode=4   # ecb(3des_ede), cbc(3des_ede)
        modprobe tcrypt mode=10  # ecb(aes), cbc(aes), ctr(aes)
        modprobe tcrypt mode=100 # hmac(md5)
        modprobe tcrypt mode=101 # hmac(sha1)
        modprobe tcrypt mode=102 # hmac(sha256)

        modprobe tcrypt mode=46  # authenc(cbc(aes),hmac(sha1|sha256))

        modprobe tcrypt mode=402 sec=1 # md5 speed tests
        modprobe tcrypt mode=403 sec=1 # sha1 speed tests
        modprobe tcrypt mode=404 sec=1 # sha256 speed tests

        modprobe tcrypt mode=500 sec=1 # aes speed tests
        modprobe tcrypt mode=501 sec=1 # 3des_ede speed tests
        modprobe tcrypt mode=504 sec=1 # des speed tests
