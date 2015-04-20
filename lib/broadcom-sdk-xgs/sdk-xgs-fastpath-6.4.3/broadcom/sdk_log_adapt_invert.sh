TARGET=esw_sdk

echo "Apply this script before creating a diff file... It will revert modified logs to the original ones."
read -p "Proceed? (type 'yes') "

if [ $ans != "yes" ]; then
  echo "Leaving..."
  exit 0;
fi

echo "Replacing *.c files..."
echo "Replacing LOG_BSL_FATAL..."
find $TARGET -name '*.c' -type f | xargs perl -pi -e 's/LOG_BSL_FATAL\(/LOG_FATAL\(/g'
echo "Replacing LOG_BSL_ERROR..."
find $TARGET -name '*.c' -type f | xargs perl -pi -e 's/LOG_BSL_ERROR\(/LOG_ERROR\(/g'
echo "Replacing LOG_BSL_WARN..."
find $TARGET -name '*.c' -type f | xargs perl -pi -e 's/LOG_BSL_WARN\(/LOG_WARN\(/g'
echo "Replacing LOG_BSL_INFO..."
find $TARGET -name '*.c' -type f | xargs perl -pi -e 's/LOG_BSL_INFO\(/LOG_INFO\(/g'
echo "Replacing LOG_BSL_VERBOSE..."
find $TARGET -name '*.c' -type f | xargs perl -pi -e 's/LOG_BSL_VERBOSE\(/LOG_VERBOSE\(/g'
echo "Replacing LOG_BSL_DEBUG..."
find $TARGET -name '*.c' -type f | xargs perl -pi -e 's/LOG_BSL_DEBUG\(/LOG_DEBUG\(/g'
echo "Replacing LOG_BSL_CHECK..."
find $TARGET -name '*.c' -type f | xargs perl -pi -e 's/LOG_BSL_CHECK\(/LOG_CHECK\(/g'

echo "Replacing *.h files..."
find $TARGET -name '*.h' -type f | xargs perl -pi -e 's/LOG_BSL_FATAL\(/LOG_FATAL\(/g'
find $TARGET -name '*.h' -type f | xargs perl -pi -e 's/LOG_BSL_ERROR\(/LOG_ERROR\(/g'
find $TARGET -name '*.h' -type f | xargs perl -pi -e 's/LOG_BSL_WARN\(/LOG_WARN\(/g'
find $TARGET -name '*.h' -type f | xargs perl -pi -e 's/LOG_BSL_INFO\(/LOG_INFO\(/g'
find $TARGET -name '*.h' -type f | xargs perl -pi -e 's/LOG_BSL_VERBOSE\(/LOG_VERBOSE\(/g'
find $TARGET -name '*.h' -type f | xargs perl -pi -e 's/LOG_BSL_DEBUG\(/LOG_DEBUG\(/g'
find $TARGET -name '*.h' -type f | xargs perl -pi -e 's/LOG_BSL_CHECK\(/LOG_CHECK\(/g'

echo "Done!"
