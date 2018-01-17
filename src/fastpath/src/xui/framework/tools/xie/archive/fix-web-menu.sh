#!/bin/sh

OUTPUT_DIR=$1

html_dir=$OUTPUT_DIR/content/html/
MENUFILE=$html_dir/scripts/_xeweb_menu.js

cd $html_dir
html_files=`ls -1 *.html | sort | xargs`
cd -

menu_files=`grep "html'" $MENUFILE | cut -d "'" -f6 | sort | uniq`
extra_in_menu=""
missing_in_menu=""

for i in $menu_files; do
  found=0
  for j in $html_files; do
    if [ "$j" = "$i" ]; then
      found=1
      break;
    fi
  done
  if [ $found -eq 0 ]; then
    extra_in_menu="$extra_in_menu $i"
  fi
done

cp -f $MENUFILE $MENUFILE.1
for i in $extra_in_menu; do
  name=`echo $i | sed "s/\./\\\./g"`
  cat $MENUFILE.1 | sed "s/'$name'/''/g" > $MENUFILE.2
  mv $MENUFILE.2 $MENUFILE.1
done

for i in $html_files; do
  found=0
  for j in $menu_files; do
    if [ "$j" = "$i" ]; then
      found=1
      break;
    fi
  done
  if [ $found -eq 0 ]; then
    missing_in_menu="$missing_in_menu $i"
  fi
done

rm -f extra_in_menu.log
for i in $extra_in_menu; do
  echo $i >> extra_in_menu.log
done

rm -f missing_in_menu.log
for i in $missing_in_menu; do
  echo $i >> missing_in_menu.log
done
  
cp -f $MENUFILE $MENUFILE.original
mv -f $MENUFILE.1 $MENUFILE

