default: backtest_double_EMA_float.cpp tools.cpp custom_talib_wrapper.cpp custom_talib_wrapper.hh tools.hh 
	cp -u ./talib/talib_install/lib/libta_lib.so.0 ./
	g++ -O3 -I./talib/talib_install/include/ ./custom_talib_wrapper.hh ./custom_talib_wrapper.cpp ./tools.hh ./tools.cpp ./backtest_double_EMA_float.cpp -L./talib/talib_install/lib -lta_lib -lpthread -o ./backtest_double_EMA_float.exe
	g++ -O3 -I./talib/talib_install/include/ ./custom_talib_wrapper.hh ./custom_talib_wrapper.cpp ./tools.hh ./tools.cpp ./backtest_double_EMA_StochRSI_float.cpp -L./talib/talib_install/lib -lta_lib -lpthread -o ./backtest_double_EMA_StochRSI_float.exe

debug: backtest_double_EMA_float.cpp tools.cpp custom_talib_wrapper.cpp backtest_double_EMA_StochRSI_float.cpp custom_talib_wrapper.hh tools.hh  
	cp -u ./talib/talib_install/lib/libta_lib.so.0 ./
	g++ -g -I./talib/talib_install/include/ ./custom_talib_wrapper.hh ./custom_talib_wrapper.cpp ./tools.hh ./tools.cpp ./backtest_double_EMA_float.cpp -L./talib/talib_install/lib -lta_lib -lpthread -o ./backtest_double_EMA_float.exe
	g++ -g -I./talib/talib_install/include/ ./custom_talib_wrapper.hh ./custom_talib_wrapper.cpp ./tools.hh ./tools.cpp ./backtest_double_EMA_StochRSI_float.cpp -L./talib/talib_install/lib -lta_lib -lpthread -o ./backtest_double_EMA_StochRSI_float.exe

trix: tools.cpp custom_talib_wrapper.cpp backtest_TRIX.cpp custom_talib_wrapper.hh tools.hh  
	cp -u ./talib/talib_install/lib/libta_lib.so.0 ./
	g++ -O3 -I./talib/talib_install/include/ ./custom_talib_wrapper.hh ./custom_talib_wrapper.cpp ./tools.hh ./tools.cpp ./backtest_TRIX.cpp -L./talib/talib_install/lib -lta_lib -lpthread -o ./backtest_TRIX.exe

trix_multi: tools.cpp custom_talib_wrapper.cpp backtest_TRIX_multi_pair.cpp custom_talib_wrapper.hh tools.hh  
	cp -u ./talib/talib_install/lib/libta_lib.so.0 ./
	g++ -O3 -I./talib/talib_install/include/ ./custom_talib_wrapper.hh ./custom_talib_wrapper.cpp ./tools.hh ./tools.cpp ./backtest_TRIX_multi_pair.cpp -L./talib/talib_install/lib -lta_lib -lpthread -o ./backtest_TRIX_multi_pair.exe
