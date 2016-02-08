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

cfg_name="olt7-8ch"
cfg_installdir="/usr/local/ptin/installdir/"
cfg_ptindir="/usr/local/ptin/"

############################################################

do_version()
{
  echo "build   :$file_build"
  if [ ! -z $file_fw      ]; then echo "fw      :$file_fw"     ; fi
  if [ ! -z $file_cli     ]; then echo "cli     :$file_cli"    ; fi
  if [ ! -z $file_dbsync  ]; then echo "dbsync  :$file_dbsync" ; fi
  if [ ! -z $file_manager ]; then echo "manager :$file_manager"; fi
  if [ ! -z $file_snmp    ]; then echo "snmp    :$file_snmp"   ; fi
  if [ ! -z $file_webti   ]; then echo "webti   :$file_webti"  ; fi
}

############################################################

install_setup()
{
  sdir=`pwd`
  cd $fdir
  if [ ! -z $file_setup ]
  then
    echo "Running $file_setup"
    sh $file_setup -v -u
    if [ -e "$file_setup.log" ]
    then
      cat "$file_setup.log" > "$sdir/$file_log.log"
    fi
  fi
  cd $sdir
}

do_install()
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
  file_log=$0

  file_setup=$file_fw     ; install_setup;
  file_setup=$file_cli    ; install_setup; 
  file_setup=$file_dbsync ; install_setup;
  file_setup=$file_manager; install_setup;
  file_setup=$file_snmp   ; install_setup;
  file_setup=$file_webti  ; install_setup;

  #--------------------------------------------
  #Remover arquivos temporarios
  #--------------------------------------------
  echo "$cfg_name: Remove temporary files";
  remove_files;
  /bin/sync;
  echo "$cfg_name: Done!";
}

############################################################

do_restart()
{
  echo "$cfg_name: Reboot!";
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
swap=0
verify=1

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
    "-v")
      version=1;
      ;;
    "-x")
      swap=1;
      ;;
    "-c")
      verify=0;
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
  echo "Usage: $0 [-v] [-c] [-u] [-x] [-s]";
  echo "  -c   Don't check md5sum"
  echo "  -u   Update"
  echo "  -x   Exchange"
  echo "  -s   Restart"
  echo "  -v   Version"
  exit 0;
fi
#-----------------------------------------------------------
if [ $version == "1" ]
then
  do_version;
fi
#-----------------------------------------------------------
if [ $verify == "1" ] && [ $update == "1" ]
then
  if [ ! -e $0.md5sum ]
  then
    echo "Verify failure: Missing md5sum file!" >&2;
    exit 1;
  fi
  md5sum_value=`md5sum $0 | cut -d\  -f1`;
  md5sum_verify=`cat $0.md5sum | cut -d\  -f1`;
  if [ $md5sum_value != $md5sum_verify ]
  then
    echo "Verify failure: $md5sum_value $md5sum_verify" >&2;
    exit 1;
  fi
  echo "Verify: md5sum correct!" >&2;
fi
#-----------------------------------------------------------
rm $0.log > /dev/null 2>&1
touch $0.log
#-----------------------------------------------------------
if [ $update == "1" ]
then
  rm $cfg_installdir/var/build > /dev/null 2>&1
  do_install;
  if [ -s $0.log ]
  then
    echo "Process failure:" >&2;
    cat $0.log >&2;
    exit 1
  fi
  rm $0.log > /dev/null 2>&1
  do_version > $cfg_installdir/var/build
fi

if [ $swap == "1" ]
then
  if [ ! -e $cfg_installdir/var/build ]
  then
    echo "Invalid build!" >&2;
    exit 1
  fi
  /usr/local/ptin/swap;
fi

if [ $restart == "1" ]
then
  if [ ! -e $cfg_ptindir/var/build ]
  then
    echo "Invalid build!" >&2;
    exit 1;
  fi
  do_restart;
fi

exit 0

############################################################

