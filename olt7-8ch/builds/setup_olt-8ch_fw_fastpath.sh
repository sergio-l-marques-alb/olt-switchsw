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

cfg_name="olt7-8ch.fw.fastpath"
cfg_installdir="/usr/local/ptin/installdir/"
cfg_ptindir="/usr/local/ptin/"

ver()
{
  echo "Build `basename $0`";
}

show_release_notes()
{
  cat $dir/log/fastpath/fastpath.releasenotes.txt
}

verify()
{
  ###### INICIO DE TESTE ###############

  #echo "$cfg_name: verifying user name"
  #if [ "$USER" != "root" ]; then
  #  echo "$cfg_name: User is not root" >> $0.log
  #  exit 0;
  #fi

  #echo "SETUP_FASTPATH: Verificacao da integridade do ficheiro de setup...";
  #echo "Espere uns segundos...";

  #ckfile=`cat $0.md5sum | awk '{print $1}'`
  #ckcalculado=`md5sum $0 | awk '{print $1}'`
  #echo ckfile $ckfile
  #echo ckcalculado $ckcalculado
  #if [ $ckfile = $ckcalculado ]; then
  #   echo MD5SUM OK;
  #else
  #   echo MD5SUM ERROR;
  #   exit 0;
  #fi
  ###### FIM DE TESTE ##################

  #######################################################
  # VERIFICACAO DE ESPACO EM DISCO
  #######################################################
  echo "$cfg_name: Checking disk space"
  ESPACO_NO_DISCO=`df -m | grep /dev/ | awk '{print $4}'`
	
  if [ $ESPACO_NO_DISCO -lt 40 ]; then
      echo "$cfg_name: No sufficient space on disk" >> $0.log
      exit 0;
  fi
  #######################################################
  # FIM DE VERIFICACAO DE ESPEACO EM DISCO
  #######################################################
  
  echo "$cfg_name: startup verifications concluded successfuly"
}


############################################################
pre()
{
  echo "$cfg_name: pre-install"

  if [ $dir == $cfg_ptindir ]; then
    # Kill fastpath script process
    ps > proc.tmp
    pid_proc=`cat proc.tmp | grep 'fastpath boot' | awk '{print $1}'`
    if [ ! -z $pid_proc ]; then
      kill $pid_proc
    fi
    rm proc.tmp;
    # Kill switchdrvr process
    sh /usr/local/ptin/sbin/fastpath stop
    sleep 2;
  fi

  rm -rf /fastpath*				2>/dev/null
  rm -rf /root/fastpath*			2>/dev/null
  rm -rf /mnt/application			2>/dev/null
  rm -rf /mnt/fastpath*				2>/dev/null
  rm -rf /var/log/fastpath*			2>/dev/null
  rm -rf /usr/local/ptin/fastpath*		2>/dev/null
  rm -rf /usr/local/ptin/scripts/rc.soc		2>/dev/null
  rm -rf /usr/local/ptin/scripts/insmods.sh	2>/dev/null
  rm -rf /usr/local/ptin/sbin/bootlog		2>/dev/null
  rm -rf /usr/local/ptin/sbin/debugtrace.sh	2>/dev/null
  rm -rf /usr/local/ptin/sbin/debugintercept.sh	2>/dev/null
}


############################################################
install()
{
  #--------------------------------------------
  #Descomprimir arquivos temporarios
  #--------------------------------------------
  echo "$cfg_name: Extracting files";
  extract_files > /dev/null 2>&1;

  echo "$cfg_name: Installing files";
  mkdir $dir/log/fastpath 2>/dev/null
  mv -f $fdir/fastpath.releasenotes.txt		$dir/log/fastpath	2>>$0.log
  mv -f $fdir/startup-config			$dir/log/fastpath	2>>$0.log
  mv -f $fdir/rcS1				$dir/scripts		2>>$0.log
  mv -f $fdir/fastpath.insmods.sh		$dir/scripts		2>>$0.log
  mv -f $fdir/fastpath.debugintercept.sh	$dir/sbin		2>>$0.log
  mv -f $fdir/fastpath.debugtrace.sh		$dir/sbin		2>>$0.log
  mv -f $fdir/linux-kernel-bde.ko		$dir/sbin		2>>$0.log
  mv -f $fdir/linux-user-bde.ko			$dir/sbin		2>>$0.log
  mv -f $fdir/fastpath				$dir/sbin		2>>$0.log
  mv -f $fdir/fastpath.ver			$dir/sbin		2>>$0.log
  mv -f $fdir/fastpath.cli			$dir/sbin		2>>$0.log
  mv -f $fdir/fastpath.shell			$dir/sbin		2>>$0.log
  mv -f $fdir/switchdrvr			$dir/sbin		2>>$0.log
  mv -f $fdir/devshell_symbols.gz		$dir/sbin		2>>$0.log

  #--------------------------------------------
  #Remover arquivos temporarios
  #--------------------------------------------
  echo "$cfg_name: Removing temporary files";
  remove_files;
  /bin/sync;
  echo "$cfg_name: Done!";
}

post()
{
  echo "$cfg_name: Rebooting system!";
  sleep 2;
  /sbin/reboot;
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
    "-d")
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
rm $0.log > /dev/null 2>&1
touch $0.log
echo "$cfg_name: setup started"
#-----------------------------------------------------------
if [ $version == "1" ]
then
  ver;
fi
#-----------------------------------------------------------
verify;
pre;
#-----------------------------------------------------------
if [ $update == "1" ]
then
  install;
fi
#-----------------------------------------------------------
#if [ $swap == "1" ]
#then
  #if [ ! -e $cfg_installdir/var/build ]
  #then
  #  echo "$cfg_name: Invalid build!" >&2
  #  echo "$cfg_name: install directory does not exist" >> $0.log
  #  exit 1
  #fi
  #/usr/local/ptin/swap;
#fi
#-----------------------------------------------------------
if [ -s $0.log ]
then
  echo "$cfg_name: setup finished with errors:" >&2;
  cat $0.log >&2;
  exit 1
else
  rm $0.log > /dev/null 2>&1
  echo -e "$cfg_name: setup finished successfully at \c"; date
fi
#-----------------------------------------------------------
if [ $version == "1" ]
then
  show_release_notes;
fi
#-----------------------------------------------------------
if [ $restart == "1" ] && [ $dir == $cfg_ptindir ]
then
  post;
fi
#-----------------------------------------------------------

exit 0
############################################################

