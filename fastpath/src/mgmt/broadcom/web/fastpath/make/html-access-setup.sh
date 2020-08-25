#!/bin/bash

PLATFORM=$1; 

FP_BASE=$FP_ROOT
DST_DIR=$FP_ROOT/$OUT_OBJECTS/xui

DBG=

WEB_CONTENT_DIRS=`cat $DST_DIR/web_content.txt`
JAVA_CONTENT_DIRS=`cat $DST_DIR/java_content.txt`

CONTENT=$DST_DIR/content
C_CONTENT=$CONTENT/obj.tmp
H_CONTENT=$CONTENT/html
JS_CONTENT=$CONTENT/html/scripts
IMG_CONTENT=$CONTENT/html/images
CSS_CONTENT=$CONTENT/html/css

## TEMP FIX FOR NOEOL
echo "" >> $C_CONTENT/_xeweb_run_if.c
## TEMP FIX FOR NOEOL

copy_content()
{
  src=$1;shift
  dst=$1;shift
  types=$*
  for t in $types; do
    files=`find $FP_BASE/$src -name "*.$t"`
    for x in $files; do
      cp -f $x $dst
    done
  done
}

TEMP_ACCESS_FILE=access.tmp
rm -f $TEMP_ACCESS_FILE
$DBG mkdir -p $H_CONTENT $JS_CONTENT $CSS_CONTENT $IMG_CONTENT $CONTENT/html/java/base $CONTENT/html/java/nv
for d in $WEB_CONTENT_DIRS; do
  copy_content $d $H_CONTENT "html" "htm"
  copy_content $d $JS_CONTENT "js"
  copy_content $d $CSS_CONTENT "css"
  copy_content $d $IMG_CONTENT "jpeg" "gif" "png" "tiff" "jpg"
  copy_content $d $CONTENT/html/java/base "class"
  copy_content $d $CONTENT/html/java/nv "jar"
  [ -f $FP_BASE/$d/_ACCESS ] && cat $FP_BASE/$d/_ACCESS >> $TEMP_ACCESS_FILE
done

echo "" >> $H_CONTENT/_ACCESS
cat $TEMP_ACCESS_FILE | sort | uniq >> $H_CONTENT/_ACCESS
echo "" >> $H_CONTENT/_ACCESS
rm -f $TEMP_ACCESS_FILE

$DBG mkdir -p $CONTENT/html/java/$PLATFORM
for d in $JAVA_CONTENT_DIRS; do
  copy_content $d $CONTENT/html/java/$PLATFORM "jpeg" "gif" "png" "jpg" "html" "htm"
done

$DBG touch $JS_CONTENT/index.html
$DBG touch $CSS_CONTENT/index.html
$DBG touch $IMG_CONTENT/index.html

if [ "$XIE_ENV_NO_ACCESS_FILE" != "" ]; then
  echo "XIE_ENV_NO_ACCESS_FILE :: IGNORING WEB ACCESS PERMISSIONS"
  rm -f $H_CONTENT/_ACCESS 
  touch $H_CONTENT/_ACCESS
fi

if [ "$XIE_NO_OPTIMIZE_JAVASCRIPT" == "" ]; then
echo "====================Start Compressing JS Content ==============================="
for d in $JS_CONTENT/*.js; do
  chmod 777 $d
  java -jar $CAT_DIR/app/yuicompressor-2.4.2.jar --type js $d > $JS_CONTENT/temp.js 2>/dev/null
if [[ -s $JS_CONTENT/temp.js ]]; then
  cp -rf $JS_CONTENT/temp.js $d
fi
  rm -f $JS_CONTENT/temp.js
done
#rm -f $JS_CONTENT/temp.js
echo "====================Finished Compressing JS Content ==============================="
fi

exit 0;
