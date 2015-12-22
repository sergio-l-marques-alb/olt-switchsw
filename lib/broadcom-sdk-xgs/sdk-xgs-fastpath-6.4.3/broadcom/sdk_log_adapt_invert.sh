TARGET=esw_sdk

if [ $# -ge 1 ]; then
  TARGET=$1
fi

echo "Apply this script before creating a diff file... It will revert modified logs to the original ones."
read -p "Proceed to $TARGET? (type 'yes') " ans

if [ "$ans" != "yes" ]; then
  echo "Leaving..."
  exit 0;
fi

echo "Replacing *.c/h files..."
echo "Replacing LOG_BSL_FATAL..."
find $TARGET -name '*.[c|h]' -type f | xargs perl -pi -e 's/LOG_BSL_FATAL\(/LOG_FATAL\(/g'
echo "Replacing LOG_BSL_ERROR..."
find $TARGET -name '*.[c|h]' -type f | xargs perl -pi -e 's/LOG_BSL_ERROR\(/LOG_ERROR\(/g'
echo "Replacing LOG_BSL_WARN..."
find $TARGET -name '*.[c|h]' -type f | xargs perl -pi -e 's/LOG_BSL_WARN\(/LOG_WARN\(/g'
echo "Replacing LOG_BSL_INFO..."
find $TARGET -name '*.[c|h]' -type f | xargs perl -pi -e 's/LOG_BSL_INFO\(/LOG_INFO\(/g'
echo "Replacing LOG_BSL_VERBOSE..."
find $TARGET -name '*.[c|h]' -type f | xargs perl -pi -e 's/LOG_BSL_VERBOSE\(/LOG_VERBOSE\(/g'
echo "Replacing LOG_BSL_DEBUG..."
find $TARGET -name '*.[c|h]' -type f | xargs perl -pi -e 's/LOG_BSL_DEBUG\(/LOG_DEBUG\(/g'
echo "Replacing LOG_BSL_CHECK..."
find $TARGET -name '*.[c|h]' -type f | xargs perl -pi -e 's/LOG_BSL_CHECK\(/LOG_CHECK\(/g'

echo "Done!"
