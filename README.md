*  ***Purpore : run fast and memory effective backtests on trading stratregies*** (EMA crossover, supertrends, etc...).
* Technical indicators are provided by TA-lib (Technical Analysis Library)
* The provided code `backtest_double_EMA_float.cpp` performs backtests in order to find the two best EMA values for a basic EMA cross-over strategy
* use it at a boiler plate code in order to test other strategies and/or parameter space.
* data is provided in `data` for BTC, 1 HOUR candles. If other data is wanted, the user must update it manually and follow the same format: `unix-timestamp open high low close` (no header).
* you can use the python script `harvest_data_1HOUR_BINANCE.py` to get latest data and/or change it to get data for another PAIR or timeframe.

**How to compile and run**
*  Instructions for Linux/Ubuntu
*  Have a C/C++ compiler installed (recommended g++ and gcc)
*  Compiling TA-lib (Technical Analysis Library):
   *  open terminal in folder `talib` and run `./configure --prefix=/$(pwd)/talib_install/` then  `make`  then  `make install` and then  `make clean` 
* open a terminal in the root folder (=${fileDirname}) (containing `backtest_*.cpp`), and run `make`
* execute the backtest with `./backtest_double_EMA_float.exe` in the root folder