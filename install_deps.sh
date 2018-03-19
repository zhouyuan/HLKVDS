#!/bin/sh

HOME_DIR=`pwd`
DEPS_DIR=`pwd`/deps
DEPS_SRC_DIR=$DEPS_DIR/src
DEPS_UT_DIR=`pwd`/test
DEPS_UT_SRC_DIR=$DEPS_UT_DIR/include
DEPS_UT_LIB_DIR=$DEPS_UT_DIR/lib

PMDK_DIR=`pwd`/src/pmdk

# Create deps folder
mkdir -p $DEPS_SRC_DIR
mkdir -p $DEPS_UT_SRC_DIR
mkdir -p $DEPS_UT_LIB_DIR

. /etc/os-release

if [ "$ID" = "ubuntu" ] || [ "$ID" = "debian" ]; then
    apt-get install -y libaio-dev libboost-all-dev automake
elif [ "$ID" = "centos" ] || [ "$ID" = "fedora" ]; then
    yum install -y libaio-devel boost-devel automake
fi

# Googletest
rm -fr $DEPS_SRC_DIR/*
cd $DEPS_SRC_DIR
## Fetch the googletest sources
git clone https://github.com/google/googletest.git
cd googletest
g++ -isystem googletest/include/ -I googletest/ -isystem googlemock/include/ -I googlemock/ -pthread -c googletest/src/gtest-all.cc
g++ -isystem googletest/include/ -I googletest/ -isystem googlemock/include/ -I googlemock/ -pthread -c googlemock/src/gmock-all.cc
ar -rv libgmock.a gtest-all.o gmock-all.o

cp googletest/include/gtest $DEPS_UT_SRC_DIR -r
cp googlemock/include/gmock $DEPS_UT_SRC_DIR -r
cp libgmock.a $DEPS_UT_LIB_DIR -r

cd $HOME_DIR
git submodule sync
git submodule update --force --init --recursive
cd $PMDK_DIR
make -j
