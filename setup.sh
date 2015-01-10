#!/bin/bash
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

echo "Installation de Boost..."
wget http://sourceforge.net/projects/boost/files/boost/1.57.0/boost_1_57_0.tar.gz
tar -zxvf boost_1_57_0.tar.gz
cd boost_1_57_0
./bootstrap.sh
./b2 --prefix="$DIR" --with-filesystem --with-program_options --with-regex --with-system --with-thread install
cd ..
rm boost_1_57_0.tar.gz
rm -rf boost_1_57_0

echo "Installation de MongoDB..."
wget https://github.com/mongodb/mongo-cxx-driver/archive/legacy-1.0.0-rc3.tar.gz
tar -zxvf legacy-1.0.0-rc3.tar.gz
cd mongo-cxx-driver-legacy-1.0.0-rc3
if [[ "$OSTYPE" == "darwin"* ]]; then
scons --prefix="$DIR" --extrapath="$DIR" --c++11 --libc++ install
else
scons --prefix="$DIR" --extrapath="$DIR" --c++11 install
fi
cd ..
rm legacy-1.0.0-rc3.tar.gz
rm -rf mongo-cxx-driver-legacy-1.0.0-rc3

echo "Installation de PolarSSL..."
wget https://github.com/polarssl/polarssl/archive/polarssl-1.3.9.tar.gz
tar -zxvf polarssl-1.3.9.tar.gz
cd polarssl-polarssl-1.3.9
make
cp -r include/polarssl/ ../include/polarssl
cp library/libpolarssl.a ../lib
cd ..
rm polarssl-1.3.9.tar.gz
rm -rf polarssl-polarssl-1.3.9

echo "Installation de zlib..."
wget https://github.com/madler/zlib/archive/v1.2.8.tar.gz
tar -zxvf v1.2.8.tar.gz
cd zlib-1.2.8
./configure --prefix="$DIR"
make install
cd ..
mkdir include/zlib
mv include/zconf.h include/zlib/
mv include/zlib.h include/zlib/
rm -rf lib/pkgconfig
find lib -mindepth 1 -maxdepth 1 ! -name "*.a" -delete
rm -rf share
rm v1.2.8.tar.gz
rm -rf zlib-1.2.8

echo "Compilation..."
mkdir bin
cd bin
if [[ "$OSTYPE" == "darwin"* ]]; then
cmake -G "Xcode" ..
open Proxy.xcodeproj
else
cmake -G "Unix Makefiles" ..
make
fi