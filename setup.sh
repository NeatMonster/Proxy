#!/bin/bash
cd lib
wget http://sourceforge.net/projects/boost/files/boost/1.57.0/boost_1_57_0.tar.gz
tar -zxvf boost_1_57_0.tar.gz
rm boost_1_57_0.tar.gz
mv boost_1_57_0 boost-tmp
cd boost-tmp
./bootstrap.sh
./b2 install
cd ..
wget https://github.com/mongodb/mongo-cxx-driver/archive/legacy-1.0.0-rc3.tar.gz
tar -zxvf legacy-1.0.0-rc3.tar.gz
rm legacy-1.0.0-rc3.tar.gz
mv mongo-cxx-driver-legacy-1.0.0-rc3 mongo-tmp
cd mongo-tmp
if [[ "$OSTYPE" == "darwin"* ]]; then
scons install --c++11 --libc++
else
scons install --c++11
fi
cd ..
mkdir boost
cp -r boost-tmp/boost/* boost
cp boost-tmp/stage/lib/libboost_filesystem.a .
cp boost-tmp/stage/lib/libboost_program_options.a .
cp boost-tmp/stage/lib/libboost_regex.a .
cp boost-tmp/stage/lib/libboost_system.a .
cp boost-tmp/stage/lib/libboost_thread.a .
rm -rf boost-tmp/*
rmdir boost-tmp
mkdir mongo
cp -r mongo-tmp/build/install/include/mongo/* mongo
cp mongo-tmp/build/install/lib/libmongoclient.a .
rm -rf mongo-tmp/*
rm -rf mongo-tmp/.*
rmdir mongo-tmp
