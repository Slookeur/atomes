#!/bin/bash

function prepare_deps
{
  mkdir $1
  cp atomes.exe $1/
  cp atomes_startup_testing.exe $1/
  cp /mingw64/bin/gspawn-win64*exe $1/
  liste_deps=`cat $2|wc -l`
  let ldeps=$liste_deps
  for ((num = 1 ; num <= ldeps ; num ++))
  do
    dep=`tac $2|tail --lines=$num|tac|tail --lines=1|awk '{printf $3}'`
    cp $dep $1/
  done
}

# Win10
ldd ./atomes | grep -v "Win" > no-win-deps.dat
ldd ./atomes | grep "Win" > win-deps.dat
# Win11
# ldd ./atomes | grep -v "WIN" > no-win-deps.dat
# ldd ./atomes | grep "WIN" > win-deps.dat

ldd ./atomes > deps.dat
rm -rf bin bin-no-win
prepare_deps bin deps.dat
prepare_deps bin-no-win no-win-deps.dat
