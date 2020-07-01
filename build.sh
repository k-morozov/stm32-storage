#! /bin/bash

mkdir bin
mkdir build
make clean
clear
make
valgrind --leak-check=full ./build/main.out
#make clean
#doxygen
#opera doxygen/html/index.html
