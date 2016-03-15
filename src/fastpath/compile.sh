#$/bin/sh

if [ $# -ge 1 ]; then
  if [ $1 == "all" ]; then
    options="all"
  else
    options="-f $1.make"
  fi
else
  options="all"
fi

make $options > output.txt &
echo "Compiling in background (check results at output.txt file)."
