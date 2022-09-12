#!/bin/bash
cd talib
./configure --prefix=/$(pwd)/talib_install/
make
make install
make clean
cd ..
make
cp -u ./talib/talib_install/lib/libta_lib.so.0 ./
echo "  "  >> ~/.bashrc
echo "export LD_LIBRARY_PATH=$(pwd)/talib/talib_install/lib/:$LD_LIBRARY_PATH" >> ~/.bashrc
echo "  "  >> ~/.bashrc
source ~/.bashrc
