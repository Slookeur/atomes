#!/bin/bash

INNO="/c/Program\ Files\ \(x86\)/Inno\ Setup\ 6/ISCC.exe"

PACKAGE_VERSIONS="win no-win"
GTK_VERSIONS="gtk3 gtk4"
ATOMES_VERSIONS="debug standard"

for GTK in $GTK_VERSIONS
do
  for ATOMES in $ATOMES_VERSIONS
  do
    for PACKAGE in $PACKAGE_VERSIONS
    do
      mkdir -p Setup/$ATOMES/$GTK/
    done
  done
done

for GTK in $GTK_VERSIONS
do
  mv gtk-sources-$GTK gtk-sources
  for ATOMES in $ATOMES_VERSIONS
  do
    mv atomes-$GTK-$ATOMES atomes
    for PACKAGE in $PACKAGE_VERSIONS
    do
      /c/Program\ Files\ \(x86\)/Inno\ Setup\ 6/ISCC.exe ./atomes-latest-$PACKAGE.iss
      mv Setup/*.exe Setup/$ATOMES/$GTK/
    done
    mv atomes atomes-$GTK-$ATOMES
  done
  mv gtk-sources gtk-sources-$GTK
done

