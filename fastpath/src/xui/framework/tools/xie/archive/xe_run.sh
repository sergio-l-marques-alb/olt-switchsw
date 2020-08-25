#!/bin/bash

export XIE_ENV_REMOVE_UNUSED_OBJECTS=1

FP_BASE=$1;shift
DST_DIR=$1;shift
SEP=$1;shift
XE_OPTS=$1;shift
JAR_FILE=$1;shift
JAR_CP=$1;shift
JAR_CMP_FILE=$1;shift
PLATFORM=$1

CUR_DIR=$FP_BASE/src/xui
test -d $FP_BASE/products && CUR_DIR=$FP_BASE
DBG=
XML_DIRS=`cat $XE_OPTS | grep XML_ | cut -d ":" -f5 | xargs`
WEB_CONTENT_DIRS=`cat $XE_OPTS | grep WEB_ | cut -d ":" -f5 | xargs`
JAVA_CONTENT_DIRS=`cat $XE_OPTS | grep JAVA_ | cut -d ":" -f5 | xargs`

echo WEB_CONTENT_DIRS = $WEB_CONTENT_DIRS

CAT_NAME="Default"
CAT_DIR=$DST_DIR/workspace
CAT_FILE=$CAT_DIR/catlog.txt
CAT_IMPORT=$CAT_DIR/catlog.import
CONTENT=$DST_DIR/content
C_CONTENT=$CONTENT/obj.tmp
H_CONTENT=$CONTENT/html
JS_CONTENT=$CONTENT/html/scripts
IMG_CONTENT=$CONTENT/html/images
CSS_CONTENT=$CONTENT/html/css

$DBG mkdir -p $DST_DIR
$DBG mkdir -p $CAT_DIR
$DBG mkdir -p $CAT_DIR/$CAT_NAME
$DBG mkdir -p $CAT_DIR/app
$DBG rm -f $CAT_FILE
$DBG touch -a $CAT_FILE
for d in $XML_DIRS; do
  find $FP_BASE/$d -maxdepth 1 -name '*.xml' 1>> $CAT_FILE 2>/dev/null ;\
  find $d -maxdepth 1 -name '*.xml' 1>> $CAT_FILE.import 2>/dev/null ;\
done
HERE=`pwd`
cd $FP_BASE
for d in $XML_DIRS; do
  find $d -maxdepth 1 -name '*.xml' 1>> $CAT_IMPORT 2>/dev/null ;\
done
for d in $WEB_CONTENT_DIRS; do
  find $d -maxdepth 1 -name '*.css' 1>> $CAT_IMPORT 2>/dev/null ;\
done
for d in $WEB_CONTENT_DIRS; do
  find $d -maxdepth 1 -name '*.js' 1>> $CAT_IMPORT 2>/dev/null ;\
done
cd $HERE

grep "\-object.xml" $CAT_FILE > $CAT_FILE.1
grep -v "\-object.xml" $CAT_FILE > $CAT_FILE.2
cat $CAT_FILE.2 $CAT_FILE.1 > $CAT_FILE

grep "\-web.xml" $CAT_FILE > $CAT_FILE.1
grep -v "\-web.xml" $CAT_FILE > $CAT_FILE.2
if [ "$XWEB_OPT" = "noweb" ]; then
  cat $CAT_FILE.2 > $CAT_FILE
else
  cat $CAT_FILE.2 $CAT_FILE.1 > $CAT_FILE
fi

# Copy the files from jar
rm -rf $CAT_DIR/jarfiles; 
mkdir $CAT_DIR/jarfiles; 
cd $CAT_DIR/jarfiles
jar -xf $JAR_FILE lib 2>/dev/null
cd -

touch $CAT_DIR/$CAT_NAME/.no_xie_import

# copy the jar and libs to catalog directory
cp -f $JAR_FILE $CAT_DIR/app/xie.jar
cp -r -f $JAR_CP $CAT_DIR/app/lib
cp -f $JAR_CMP_FILE $CAT_DIR/app/yuicompressor-2.4.2.jar
#cp -rf $CAT_DIR/jarfiles/lib $CAT_DIR/app

# run the application
echo "====================Starting XUI : `date`==============================="
$DBG java -mx1024m -ms100m -Xss10m -jar $CAT_DIR/app/xie.jar -xemode $CAT_NAME $CAT_FILE $DST_DIR $FP_BASE
retval=$?
find $CAT_DIR/$CAT_NAME -name '*.dtd' | xargs rm -f
echo "====================Completed XUI : `date`=============================="

dos2unix $H_CONTENT/*.* 2>/dev/null
dos2unix $JS_CONTENT/*.* 2>/dev/null
dos2unix $C_CONTENT/*.* 2>/dev/null
dos2unix $C_CONTENT/stubs/*.* 2>/dev/null
indent -l100 -bli0 -sob -nut -bap -npsl $C_CONTENT/*.[ch] 2>/dev/null
indent -l100 -bli0 -sob -nut -bap -npsl $C_CONTENT/stubs/*.[ch] 2>/dev/null
rm -f $C_CONTENT/stubs/*~
rm -f $C_CONTENT/*~

## TEMP FIX FOR NOEOL
## TEMP FIX FOR NOEOL
## TEMP FIX FOR NOEOL
## TEMP FIX FOR NOEOL
## TEMP FIX FOR NOEOL
## TEMP FIX FOR NOEOL
echo "" >> $C_CONTENT/_xeweb_run_if.c
## TEMP FIX FOR NOEOL
## TEMP FIX FOR NOEOL
## TEMP FIX FOR NOEOL
## TEMP FIX FOR NOEOL
## TEMP FIX FOR NOEOL

copy_content()
{
  src=$1;shift
  dst=$1;shift
  types=$*
  for t in $types; do
    cp -f `find $FP_BASE/$src -name "*.$t"` $dst 2>/dev/null
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

if [ "$XWEB_OPT" = "noweb" ]; then
  rm -f $C_CONTENT/_xeweb_*.*
fi

if [ "$PRODUCT" = "xuitest" ] ; then
  rm -f $H_CONTENT/_ACCESS 
  touch $H_CONTENT/_ACCESS
fi

if [ "$XIE_ENV_NO_ACCESS_FILE" != "" ]; then
  echo "XIE_ENV_NO_ACCESS_FILE :: IGNORING WEB ACCESS PERMISSIONS"
  rm -f $H_CONTENT/_ACCESS 
  touch $H_CONTENT/_ACCESS
fi

if [ "$XIE_ENV_IGNORE_BLDERR" != "" ]; then
  echo "XIE_ENV_IGNORE_BLDERR :: IGNORING BUILD ERRORS"
  retval=0
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

############### TEMP ####################
/bin/sh $FP_BASE/src/xui/framework/tools/xie/fix-web-menu.sh $DST_DIR
############### TEMP ####################

exit $retval;
