#!/bin/bash
set -e
rm -f *.o
rm -f ../lib/libmlcpp.a
g++  -fopenmp  -O3 -I   ../include   -c   *.cpp
ar  rcs   ../lib/libmlcpp.a  *.o
rm -f *.o
echo "../lib/libmlcpp.a created"
