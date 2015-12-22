TARGET=esw_sdk

if [ $# -ge 1 ]; then
  TARGET=$1
fi

echo "Apply this script before patching."
read -p "Proceed to $TARGET? (type 'yes') " ans

if [ "$ans" != "yes" ]; then
  echo "Leaving..."
  exit 0;
fi

echo "Replacing *.c/h files..."
echo "Replacing LOG_FATAL..."
find $TARGET -name '*.[c|h]' -type f | xargs perl -pi -e 's/LOG_FATAL\(/LOG_BSL_FATAL\(/g'
echo "Replacing LOG_ERROR..."
find $TARGET -name '*.[c|h]' -type f | xargs perl -pi -e 's/LOG_ERROR\(/LOG_BSL_ERROR\(/g'
echo "Replacing LOG_WARN..."
find $TARGET -name '*.[c|h]' -type f | xargs perl -pi -e 's/LOG_WARN\(/LOG_BSL_WARN\(/g'
echo "Replacing LOG_INFO..."
find $TARGET -name '*.[c|h]' -type f | xargs perl -pi -e 's/LOG_INFO\(/LOG_BSL_INFO\(/g'
echo "Replacing LOG_VERBOSE..."
find $TARGET -name '*.[c|h]' -type f | xargs perl -pi -e 's/LOG_VERBOSE\(/LOG_BSL_VERBOSE\(/g'
echo "Replacing LOG_DEBUG..."
find $TARGET -name '*.[c|h]' -type f | xargs perl -pi -e 's/LOG_DEBUG\(/LOG_BSL_DEBUG\(/g'
echo "Replacing LOG_CHECK..."
find $TARGET -name '*.[c|h]' -type f | xargs perl -pi -e 's/LOG_CHECK\(/LOG_BSL_CHECK\(/g'

echo "Done!"
