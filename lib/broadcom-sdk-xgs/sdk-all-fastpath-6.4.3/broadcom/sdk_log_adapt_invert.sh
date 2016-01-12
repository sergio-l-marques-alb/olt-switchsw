TARGET=$1

if [ -z "$TARGET" ]; then
  echo "No target specified!"
  exit 0;
fi

echo "Apply this script before creating a diff file... It will revert modified logs to the original ones."
read -p "Proceed patch to $TARGET? (type 'yes') "

if [ "$ans" != "yes" ]; then
  echo "Leaving..."
  exit 0;
fi

echo "-------- Applying patch --------"

echo "Replacing *.c files:"
echo "Replacing LOG_BSL_FATAL..."
find $TARGET -name '*.c' -type f | xargs perl -pi -e 's/LOG_BSL_FATAL\(/LOG_FATAL\(/g'
find $TARGET -name '*.c' -type f | xargs perl -pi -e 's/LOG_BSL_FATAL /LOG_FATAL /g'
echo "Replacing LOG_BSL_ERROR..."
find $TARGET -name '*.c' -type f | xargs perl -pi -e 's/LOG_BSL_ERROR\(/LOG_ERROR\(/g'
find $TARGET -name '*.c' -type f | xargs perl -pi -e 's/LOG_BSL_ERROR /LOG_ERROR /g'
echo "Replacing LOG_BSL_WARN..."
find $TARGET -name '*.c' -type f | xargs perl -pi -e 's/LOG_BSL_WARN\(/LOG_WARN\(/g'
find $TARGET -name '*.c' -type f | xargs perl -pi -e 's/LOG_BSL_WARN /LOG_WARN /g'
echo "Replacing LOG_BSL_INFO..."
find $TARGET -name '*.c' -type f | xargs perl -pi -e 's/LOG_BSL_INFO\(/LOG_INFO\(/g'
find $TARGET -name '*.c' -type f | xargs perl -pi -e 's/LOG_BSL_INFO /LOG_INFO /g'
echo "Replacing LOG_BSL_VERBOSE..."
find $TARGET -name '*.c' -type f | xargs perl -pi -e 's/LOG_BSL_VERBOSE\(/LOG_VERBOSE\(/g'
find $TARGET -name '*.c' -type f | xargs perl -pi -e 's/LOG_BSL_VERBOSE /LOG_VERBOSE /g'
echo "Replacing LOG_BSL_DEBUG..."
find $TARGET -name '*.c' -type f | xargs perl -pi -e 's/LOG_BSL_DEBUG\(/LOG_DEBUG\(/g'
find $TARGET -name '*.c' -type f | xargs perl -pi -e 's/LOG_BSL_DEBUG /LOG_DEBUG /g'
echo "Replacing LOG_BSL_CHECK..."
find $TARGET -name '*.c' -type f | xargs perl -pi -e 's/LOG_BSL_CHECK\(/LOG_CHECK\(/g'
find $TARGET -name '*.c' -type f | xargs perl -pi -e 's/LOG_BSL_CHECK /LOG_CHECK /g'

echo "Replacing *.h files:"
echo "Replacing LOG_BSL_FATAL..."
find $TARGET -name '*.h' -type f | xargs perl -pi -e 's/LOG_BSL_FATAL\(/LOG_FATAL\(/g'
find $TARGET -name '*.c' -type f | xargs perl -pi -e 's/LOG_BSL_FATAL /LOG_FATAL /g'
echo "Replacing LOG_BSL_ERROR..."
find $TARGET -name '*.h' -type f | xargs perl -pi -e 's/LOG_BSL_ERROR\(/LOG_ERROR\(/g'
find $TARGET -name '*.c' -type f | xargs perl -pi -e 's/LOG_BSL_ERROR /LOG_ERROR /g'
echo "Replacing LOG_BSL_WARN..."
find $TARGET -name '*.h' -type f | xargs perl -pi -e 's/LOG_BSL_WARN\(/LOG_WARN\(/g'
find $TARGET -name '*.c' -type f | xargs perl -pi -e 's/LOG_BSL_WARN /LOG_WARN /g'
echo "Replacing LOG_BSL_INFO..."
find $TARGET -name '*.h' -type f | xargs perl -pi -e 's/LOG_BSL_INFO\(/LOG_INFO\(/g'
find $TARGET -name '*.c' -type f | xargs perl -pi -e 's/LOG_BSL_INFO /LOG_INFO /g'
echo "Replacing LOG_BSL_VERBOSE..."
find $TARGET -name '*.h' -type f | xargs perl -pi -e 's/LOG_BSL_VERBOSE\(/LOG_VERBOSE\(/g'
find $TARGET -name '*.h' -type f | xargs perl -pi -e 's/LOG_BSL_VERBOSE /LOG_VERBOSE /g'
echo "Replacing LOG_BSL_DEBUG..."
find $TARGET -name '*.h' -type f | xargs perl -pi -e 's/LOG_BSL_DEBUG\(/LOG_DEBUG\(/g'
find $TARGET -name '*.h' -type f | xargs perl -pi -e 's/LOG_BSL_DEBUG /LOG_DEBUG /g'
echo "Replacing LOG_BSL_CHECK..."
find $TARGET -name '*.h' -type f | xargs perl -pi -e 's/LOG_BSL_CHECK\(/LOG_CHECK\(/g'
find $TARGET -name '*.h' -type f | xargs perl -pi -e 's/LOG_BSL_CHECK /LOG_CHECK /g'

echo "Done!"
