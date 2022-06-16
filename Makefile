default: backtest_double_EMA_float.cpp tools.cpp
	cp -u ./talib/talib_install/lib/libta_lib.so.0 ./
	g++ -Ofast -I./talib/talib_install/include/ ./custom_talib_wrapper.hh ./custom_talib_wrapper.cpp ./tools.hh ./tools.cpp ./backtest_double_EMA_float.cpp -L./talib/talib_install/lib -lta_lib -lpthread -o ./backtest_double_EMA_float.exe
