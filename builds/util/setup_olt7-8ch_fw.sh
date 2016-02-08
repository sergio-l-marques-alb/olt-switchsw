############################################################
## Instalacao de software                                 ##
############################################################
## fdir            - directorio temporario                ##
## extract_files() - cria fdir com respectivos ficheiros  ##
## remove_files()  - remove fdir                          ##
############################################################

############################################################
## Configuracao                                           ##
############################################################

cfg_name="olt7-8ch.fw"
cfg_bin="olt7_8ch"
cfg_installdir="/usr/local/ptin/installdir/"
cfg_ptindir="/usr/local/ptin/"

cfg_kernel="Linux OLT7_8CH 2.6.12.3-PQ2 #1 Fri Apr 23 14:07:04 WEST 2010 ppc GNU/Linux"
cfg_busybox="BusyBox v1.15.2 (2010-03-01 18:02:58 WET) multi-call binary"

############################################################

ver()
{
  echo "Build `basename $0`";
}

############################################################

pre()
{
  echo "$cfg_name: Stop!";
  #/usr/local/ptin/sbin/;
  #sleep 2;
}

############################################################

install()
{
  #--------------------------------------------
  #Arquivos ou dados a salvaguardar
  #--------------------------------------------
  #echo "$cfg_name: Config?"

  #--------------------------------------------
  #Descomprimir arquivos temporarios
  #--------------------------------------------
  echo "$cfg_name: Extract files";
  extract_files > /dev/null 2>&1;

  #--------------------------------------------
  #Instalar
  #--------------------------------------------
  echo "$cfg_name: Install files!";

  #1-FPGAS
  echo -n "  Fpgas:";
  mv -f $fdir/FPGA1009.fp7  $dir/sbin/fpgas/.  2>>$0.log;
  mv -f $fdir/FPGA1011.fp7  $dir/sbin/fpgas/.  2>>$0.log;
  echo " done!";
  
  #2-Scripts
  echo -n "  Scripts:";
  mv -f $fdir/insmods.sh    $dir/scripts/. 2>>$0.log;
  mv -f $fdir/mbx_load.sh   $dir/scripts/. 2>>$0.log;
  mv -f $fdir/bashrc_ptin   $dir/scripts/. 2>>$0.log;
  mv -f $fdir/rcS_ptin      $dir/scripts/. 2>>$0.log;
  mv -f $fdir/rc.soc        $dir/scripts/. 2>>$0.log;
  #mv -f $fdir/config.bcm    $dir/scripts/. 2>>$0.log;
  echo " done!";
  
  #3-Drivers
  echo -n "  Drivers:";
  mv -f $fdir/linux-kernel-bde.ko  $dir/sbin/. 2>>$0.log;
  mv -f $fdir/linux-user-bde.ko    $dir/sbin/. 2>>$0.log;
  mv -f $fdir/mbx_drv.ko           $dir/sbin/. 2>>$0.log;
  echo " done!";
  
  #4-Code
  echo -n "  Code:";
  mv -f $fdir/buga                 $dir/sbin/.         2>>$0.log;
  mv -f $fdir/$cfg_bin\_v*         $dir/sbin/$cfg_bin  2>>$0.log;
  mv -f $fdir/bl3458_firmware.bin  $dir/bin/.          2>>$0.log;
  mv -f $fdir/bl3458_bootrom.bin   $dir/bin/.          2>>$0.log;
  mv -f $fdir/fastpath             $dir/sbin/.         2>>$0.log;
  mv -f $fdir/fastpath.cli         $dir/sbin/.         2>>$0.log;
  mv -f $fdir/fastpath.shell       $dir/sbin/.         2>>$0.log;
  mv -f $fdir/switchdrvr           $dir/sbin/.         2>>$0.log;
  mv -f $fdir/devshell_symbols.gz  $dir/sbin/.         2>>$0.log;
  mv -f $fdir/debugtrace.sh        $dir/sbin/.         2>>$0.log;
  mv -f $fdir/debugintercept.sh    $dir/sbin/.         2>>$0.log;
  mv -f $fdir/startup-config       /var/log/fastpath/. 2>>$0.log;
  echo " done!";
  
  #5-Kernel
  echo -n "  Kernel:";
  if [ "`uname -a`" == "$cfg_kernel" ]
  then
    echo " updated!"
  else
    dd if=$fdir/kernelImage.z of=/dev/hda1 2>&1 | grep -v records >> $0.log
    echo " done!"
  fi
  
  #6-Busybox
  echo -n "  BusyBox:";
  if [ "`busybox | head -n 1`" == "$cfg_busybox" ]
  then
    echo " updated!"
  else
    tar zxf $fdir/busybox-*.tgz -C / 2>>$0.log;
    chmod 4755 /bin/busybox          2>>$0.log;         
    echo " done!"
  fi

  #--------------------------------------------
  #Repor arquivos ou dados salvaguardados
  #--------------------------------------------
  #echo "$cfg_name: Config!"

  #--------------------------------------------
  #Remover arquivos temporarios
  #--------------------------------------------
  echo "$cfg_name: Remove temporary files";
  remove_files;
  /bin/sync;
  echo "$cfg_name: Done!";
}

############################################################

post()
{
  echo "$cfg_name: Run!";
  #sleep 2;
  #/sbin/reboot;
}

############################################################
# MAIN

#-----------------------------------------------------------
# Por omissao
update=0
restart=0
version=0
help=0
args=0
dir=$cfg_installdir

#-----------------------------------------------------------
# Opcoes
until [ -z  "$1" ]
do
  args=1
  case $1 in
    "-u")
      update=1;
      ;;
    "-s")
      restart=1;
      ;;
    "-f")
      dir=$cfg_ptindir
      ;;
    "-v")
      version=1;
      ;;
    *)
      help=1;
      ;;
  esac
  shift
done

#-----------------------------------------------------------
# Executar
if [ $help == "1" ] || [ $args == "0" ]
then
  echo "Usage: $0 [-v] [-u]";
  echo "  or:  $0 -f [-v] [-u] [-s]" 
  echo "  -u   Update"
  echo "  -s   Restart"
  echo "  -f   Don't use swap process"
  echo "  -v   Version"
  exit 0;
fi
#-----------------------------------------------------------
if [ $version == "1" ]
then
  ver;
fi
#-----------------------------------------------------------
rm $0.log > /dev/null 2>&1
touch $0.log
#-----------------------------------------------------------
if [ $dir == $cfg_ptindir ]
then
  pre;
fi
#-----------------------------------------------------------
if [ $update == "1" ]
then
  install;
fi
#-----------------------------------------------------------
if [ $dir == $cfg_ptindir ] && [ $restart == "1" ]
then    
  post;
fi
#-----------------------------------------------------------
if [ -s $0.log ]
then
  echo "Process with erros:" >&2;
  cat $0.log >&2;
  exit 1
else
  rm $0.log > /dev/null 2>&1
fi
#-----------------------------------------------------------
exit 0

############################################################

