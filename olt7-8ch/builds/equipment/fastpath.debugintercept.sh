packet="help"
enable=1

if [ ! -z $1 ]; then
  packet=$1
fi
if [ ! -z $2 ]; then
  enable=$2
fi

if [ $packet = "help" ]; then
  echo "This script is used to know if Fastpath is intercepting packets to CPU."
  echo "Syntax: $0 <type> <enable>"
  echo "   type   => cpu : any packet intercepted;"
  echo "             pdu : shows more information about packets;"
  echo "             igmp: shows information about intercepted igmp packets."
  echo "   enable => 1 : show information;"
  echo "             0 : hide information."
  echo "done!"
  exit
fi

echo "Activating interception info..."
if [ $packet = "cpu" ]; then
  ./fastpath.shell dev "CpuInterceptDebug($enable)"
elif [ $packet = "pdu" ]; then
  ./fastpath.shell dev "PduInterceptDebug($enable)"
elif [ $packet = "igmp" ]; then 
  ./fastpath.shell dev "IgmpInterceptDebug($enable)"
else
  echo "Invalid parameter: $packet"
  echo "Type '$0 help' for more information about this script."
  exit
fi

echo "Redirecting stdout to this console..."
./fastpath.cli m 1000

echo "Done!!!"

