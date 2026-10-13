#!/bin/sh

lib_dir=${MESON_INSTALL_PREFIX}/Contents/Resources/lib
bin_dir=${MESON_INSTALL_PREFIX}/Contents/Resources/bin

function inst_tool
{
  libf=`echo $1|sed 's/\// /g'|awk '{printf $NF}'`
#  install_name_tool -change "$1" "@loader_path/../Resources/lib/$libf" ${MESON_INSTALL_PREFIX}/Contents/MacOS/atomes-bin
}

function lib_copy_dependency
{
  local lib_path=$1
  local lib=`echo $1|sed 's/\// /g'|awk '{printf $NF}'`
  if [ ! -f $lib_dir/$lib ]; then
    echo "Library: "$lib", in : "$lib_path
    cp -R -L $lib_path $lib_dir
#    install_name_tool -id "@loader_path/../Resources/lib/"$lib $lib_dir/$lib 
#    install_name_tool -id "@rpath/"$lib $lib_dir/$lib 
    local liste_dolib=`otool -L $lib_dir/$lib|grep 'dylib'|grep 'opt'|awk '{printf $1" "}'`    
    local liste_dopath=`otool -L $lib_path|grep 'rpath'|grep 'dylib'|sed 's/@rpath//g'|awk '{printf $1" "}'`
    local rpath=`otool -L $lib_path|head -n 1|sed 's/'$lib'//1'|sed 's/\/://g'`
    #if [ $lib == "libjxl.0.10.dylib" ]; then
    #  echo "liste_dolib  = "$liste_dolib
    #  echo "liste_dopath = "$liste_dopath
    #  echo "rpath        = "$rpath 
    #fi
    for dolib in $liste_dolib
    do 
      lib_copy_dependency $dolib
    done
    # liste_dolib=`otool -L $lib_dir/$lib|grep '/usr'|awk '{printf $1" "}'`
    # for dolib in $liste_dolib
    # do 
    #   lib_copy_dependency $dolib
    # done
    for dopath in $liste_dopath
    do
      lib_copy_dependency $rpath$dopath
    done
  fi
}

liste_lib=`otool -L atomes-bin|grep 'dylib'|grep 'opt'|awk '{printf $1" "}'`
mkdir -p $lib_dir
for llib in $liste_lib
do
  lib_copy_dependency $llib
  inst_tool $llib  
done

liste_lib=`otool -L atomes-bin|grep 'dylib'|grep 'rpath'|awk '{printf $1" "}'`
for llib in $liste_lib
do
  lib_copy_dependency $llib
  inst_tool $llib  
done

# d-bus utility
mkdir -p $bin_dir
cp /opt/homebrew/bin/dbus-daemon $bin_dir

# Extra stuff because you'll never know
cp -R -L /opt/homebrew/lib/gtk-4.0 $lib_dir
cp -R -L /opt/homebrew/lib/gdk-pixbuf-2.0 $lib_dir

install_name_tool -add_rpath "@loader_path/../Resources/lib" ${MESON_INSTALL_PREFIX}/Contents/MacOS/atomes-bin 
