default: backtest_double_EMA_float.cpp tools.cpp custom_talib_wrapper.cpp custom_talib_wrapper.hh tools.hh 
	g++ -O3 -I./talib/talib_install/include/ ./custom_talib_wrapper.hh ./custom_talib_wrapper.cpp ./tools.hh ./tools.cpp ./backtest_double_EMA_float.cpp -L./talib/talib_install/lib -lta_lib -lpthread -o ./backtest_double_EMA_float.exe
	g++ -O3 -I./talib/talib_install/include/ ./custom_talib_wrapper.hh ./custom_talib_wrapper.cpp ./tools.hh ./tools.cpp ./backtest_double_EMA_StochRSI_float.cpp -L./talib/talib_install/lib -lta_lib -lpthread -o ./backtest_double_EMA_StochRSI_float.exe

debug: backtest_double_EMA_float.cpp tools.cpp custom_talib_wrapper.cpp backtest_double_EMA_StochRSI_float.cpp custom_talib_wrapper.hh tools.hh  
	g++ -g -I./talib/talib_install/include/ ./custom_talib_wrapper.hh ./custom_talib_wrapper.cpp ./tools.hh ./tools.cpp ./backtest_double_EMA_float.cpp -L./talib/talib_install/lib -lta_lib -lpthread -o ./backtest_double_EMA_float.exe
	g++ -g -I./talib/talib_install/include/ ./custom_talib_wrapper.hh ./custom_talib_wrapper.cpp ./tools.hh ./tools.cpp ./backtest_double_EMA_StochRSI_float.cpp -L./talib/talib_install/lib -lta_lib -lpthread -o ./backtest_double_EMA_StochRSI_float.exe

trix: tools.cpp custom_talib_wrapper.cpp backtest_TRIX.cpp custom_talib_wrapper.hh tools.hh  
	g++ -O3 -I./talib/talib_install/include/ ./custom_talib_wrapper.hh ./custom_talib_wrapper.cpp ./tools.hh ./tools.cpp ./backtest_TRIX.cpp -L./talib/talib_install/lib -lta_lib -lpthread -o ./backtest_TRIX.exe

trix_multi: tools.cpp custom_talib_wrapper.cpp backtest_TRIX_multi_pair.cpp custom_talib_wrapper.hh tools.hh  
	g++ -O3 -I./talib/talib_install/include/ ./custom_talib_wrapper.hh ./custom_talib_wrapper.cpp ./tools.hh ./tools.cpp ./backtest_TRIX_multi_pair.cpp -L./talib/talib_install/lib -lta_lib -lpthread -o ./backtest_TRIX_multi_pair.exe

STEMAATR: tools.cpp custom_talib_wrapper.cpp SuperTrend_EMA_ATR.cpp custom_talib_wrapper.hh tools.hh  
	g++ -O3 -I./talib/talib_install/include/ ./custom_talib_wrapper.hh ./custom_talib_wrapper.cpp ./tools.hh ./tools.cpp ./SuperTrend_EMA_ATR.cpp -L./talib/talib_install/lib -lta_lib -lpthread -o ./SuperTrend_EMA_ATR.exe

SR: tools.cpp custom_talib_wrapper.cpp SuperReversal.cpp custom_talib_wrapper.hh tools.hh  
	g++ -O3 -I./talib/talib_install/include/ ./custom_talib_wrapper.hh ./custom_talib_wrapper.cpp ./tools.hh ./tools.cpp ./SuperReversal.cpp -L./talib/talib_install/lib -lta_lib -lpthread -o ./SuperReversal.exe
	
EMA2SOTCHRSIMULTI: tools.cpp custom_talib_wrapper.cpp backtest_double_EMA_StochRSI_float_muti_pair.cpp custom_talib_wrapper.hh tools.hh  
	g++ -O3 -I./talib/talib_install/include/ ./custom_talib_wrapper.hh ./custom_talib_wrapper.cpp ./tools.hh ./tools.cpp ./backtest_double_EMA_StochRSI_float_muti_pair.cpp -L./talib/talib_install/lib -lta_lib -lpthread -o ./backtest_double_EMA_StochRSI_float_muti_pair.exe

BigWill: tools.cpp custom_talib_wrapper.cpp BigWill.cpp custom_talib_wrapper.hh tools.hh  
	g++ -Ofast -I./talib/talib_install/include/ ./custom_talib_wrapper.hh ./custom_talib_wrapper.cpp ./tools.hh ./tools.cpp ./BigWill.cpp -L./talib/talib_install/lib -lta_lib -lpthread -o ./BigWill.exe

BigWill_d: tools.cpp custom_talib_wrapper.cpp BigWill.cpp custom_talib_wrapper.hh tools.hh  
	g++ -g -fsanitize=address -I./talib/talib_install/include/ ./custom_talib_wrapper.hh ./custom_talib_wrapper.cpp ./tools.hh ./tools.cpp ./BigWill.cpp -L./talib/talib_install/lib -lta_lib -lpthread -o ./BigWill.exe

SR_mtf :  tools.cpp custom_talib_wrapper.cpp SuperReversal_mtf.cpp custom_talib_wrapper.hh tools.hh  
	g++ -Ofast -I./talib/talib_install/include/ ./custom_talib_wrapper.hh ./custom_talib_wrapper.cpp ./tools.hh ./tools.cpp ./SuperReversal_mtf.cpp -L./talib/talib_install/lib -lta_lib -lpthread -o ./SuperReversal_mtf.exe
	
SR_mtf_d :  tools.cpp custom_talib_wrapper.cpp SuperReversal_mtf.cpp custom_talib_wrapper.hh tools.hh  
	g++ -g -fsanitize=address -I./talib/talib_install/include/ ./custom_talib_wrapper.hh ./custom_talib_wrapper.cpp ./tools.hh ./tools.cpp ./SuperReversal_mtf.cpp -L./talib/talib_install/lib -lta_lib -lpthread -o ./SuperReversal_mtf.exe

3EMA_SRSI_ATR : tools.cpp custom_talib_wrapper.cpp 3EMA_SRSI_ATR.cpp custom_talib_wrapper.hh tools.hh  
	g++ -Ofast -I./talib/talib_install/include/ ./custom_talib_wrapper.hh ./custom_talib_wrapper.cpp ./tools.hh ./tools.cpp ./3EMA_SRSI_ATR.cpp -L./talib/talib_install/lib -lta_lib -lpthread -o ./3EMA_SRSI_ATR.exe 
