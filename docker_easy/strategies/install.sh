#!/bin/sh
cd talib
./configure --prefix=/$(pwd)/talib_install/
make
make install
make clean
cd ..
make
cp -u ../talib/talib_install/lib/libta_lib.so.0 ./
