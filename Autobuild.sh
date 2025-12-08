#!/bin/bash

set -e
set -u

if [ ! -d `pwd`/build ];then
    mkdir `pwd`/build
else
    rm -rf `pwd`/build/*
fi

if [ ! -d `pwd`/include ];then
    mkdir `pwd`/include
    cp `pwd`/*.h `pwd`/include
fi

cd `pwd`/build && cmake .. && make 

cd ..