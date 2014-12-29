TARGET=esw_sdk

echo "Apply this script before patching."
read -p "Proceed? (type 'yes') " ans

if [ $ans != "yes" ]; then
  echo "Leaving..."
  exit 0;
fi

echo "Replacing *.c files..."
echo "Replacing LOG_FATAL..."
find $TARGET -name '*.c' -type f | xargs perl -pi -e 's/LOG_FATAL\(/LOG_BSL_FATAL\(/g'
echo "Replacing LOG_ERROR..."
find $TARGET -name '*.c' -type f | xargs perl -pi -e 's/LOG_ERROR\(/LOG_BSL_ERROR\(/g'
echo "Replacing LOG_WARN..."
find $TARGET -name '*.c' -type f | xargs perl -pi -e 's/LOG_WARN\(/LOG_BSL_WARN\(/g'
echo "Replacing LOG_INFO..."
find $TARGET -name '*.c' -type f | xargs perl -pi -e 's/LOG_INFO\(/LOG_BSL_INFO\(/g'
echo "Replacing LOG_VERBOSE..."
find $TARGET -name '*.c' -type f | xargs perl -pi -e 's/LOG_VERBOSE\(/LOG_BSL_VERBOSE\(/g'
echo "Replacing LOG_DEBUG..."
find $TARGET -name '*.c' -type f | xargs perl -pi -e 's/LOG_DEBUG\(/LOG_BSL_DEBUG\(/g'
echo "Replacing LOG_CHECK..."
find $TARGET -name '*.c' -type f | xargs perl -pi -e 's/LOG_CHECK\(/LOG_BSL_CHECK\(/g'

echo "Replacing *.h files..."
find $TARGET -name '*.h' -type f | xargs perl -pi -e 's/LOG_FATAL\(/LOG_BSL_FATAL\(/g'
find $TARGET -name '*.h' -type f | xargs perl -pi -e 's/LOG_ERROR\(/LOG_BSL_ERROR\(/g'
find $TARGET -name '*.h' -type f | xargs perl -pi -e 's/LOG_WARN\(/LOG_BSL_WARN\(/g'
find $TARGET -name '*.h' -type f | xargs perl -pi -e 's/LOG_INFO\(/LOG_BSL_INFO\(/g'
find $TARGET -name '*.h' -type f | xargs perl -pi -e 's/LOG_VERBOSE\(/LOG_BSL_VERBOSE\(/g'
find $TARGET -name '*.h' -type f | xargs perl -pi -e 's/LOG_DEBUG\(/LOG_BSL_DEBUG\(/g'
find $TARGET -name '*.h' -type f | xargs perl -pi -e 's/LOG_CHECK\(/LOG_BSL_CHECK\(/g'

echo "Done!"
