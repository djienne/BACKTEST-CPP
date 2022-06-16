cd talib
./configure --prefix=/$(pwd)/talib_install/
make
make install
make clean
cd ..
make