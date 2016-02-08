filter1=0xffff
filter2=0xffff0000

if [ ! -z $1 ]; then
  filter1=$1
fi
if [ ! -z $2 ]; then
  filter2=$2
fi

echo "This command defines the tracing parameters in FastPath application"
echo "Syntax: $0 0x???? 0x????0000"
echo ""

echo "Redefining tracing filters for ($filter1,$filter2)"
./fastpath.shell dev "SetDebugFilters($filter1,$filter2)"

echo "Redirecting stdout to this console..."
./fastpath.cli m 1000

echo "Done!!!"

