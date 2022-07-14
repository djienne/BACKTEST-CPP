*  ***Purpore : run fast and memory effective backtests on trading strategies*** (EMA crossover, supertrends, etc...).
* Technical indicators are provided by TA-lib (Technical Analysis Library)
* The provided strategies to-be-optimized are (so far):
    * `backtest_double_EMA_float.cpp` : finds the two best EMA values for a basic 2-EMA cross-over strategy
    * `backtest_double_EMA_StochRSI_float.cpp` : finds the two best EMA values for a 2-EMA cross-over strategy with Stochastic RSI check (buying if overbought, selling if oversold; was found to be better than the opposite)
    * Other strategies should be added later
* use it like a boiler plate code in order to test other strategies and/or parameter space.
* data is provided in `data` for Binance top 30 coins, several timeframes. If other data is wanted, the user must update it manually and follow the same format: `unix-timestamp(ms);open;high;low;close;volume` (no header).
* A python version of `backtest_double_EMA_StochRSI_float` is provided in the `python` folder for sanity check and performance comparison.

**How to compile and run**
*  Instructions for Linux/Ubuntu (if Windows, you can use Virtual Box that is free)
*  Have a C/C++ compiler installed (recommended g++ and gcc)
*  Compiling TA-lib (Technical Analysis Library):
   *  open terminal in folder `talib` and run `./configure --prefix=/$(pwd)/talib_install/` then  `make`  then  `make install` and then  `make clean` 
* open a terminal in the root folder (=${fileDirname}) (containing `backtest_*.cpp`), and run `make`
* execute the backtest with `./backtest_double_EMA_float.exe` (or other `.exe` for other strategies) in the root folder
* alternatively, run `sh install.sh` in the root folder.

**Example results:** (2-EMA cross simple strategy)
```
-------------------------------------
Strategy to test: 2-EMA crossover simple
DATA FILE TO PROCESS: ./data/BTCUSDT_1HOUR.txt
Initialized TA-Lib !
Initialized calculations.
-------------------------------------
Begin day      : 2017/8/17
End day        : 2022/7/6
OPEN/CLOSE FEE : 0.05 %
FUNDING FEE    : 0 %
LEVERAGE       : 1
Minimum number of trades required: 100
CAN LONG : 1 ; CAN SHORT : 0
-------------------------------------
BEST PARAMETER SET FOUND: 
-------------------------------------
Strategy : 2-EMA crossover simple
EMA      : 72 7
Best Gain: 2803.38%
Win rate : 28.0778%
max DD   : -45.9658%
Gain/DDC : 32.9545
Score    : 925.289
Number of trades: 463
-------------------------------------
Number of backtests performed : 39402
Time taken                    : 14 seconds 
RAM usage                     : 43.6 MB
-------------------------------------
-------------------------------------
-------------------------------------
-------------------------------------
Strategy to test: 2-EMA crossover simple
DATA FILE TO PROCESS: ./data/BTCUSDT_1HOUR.txt
Initialized TA-Lib !
Initialized calculations.
-------------------------------------
Begin day      : 2017/8/17
End day        : 2022/7/6
OPEN/CLOSE FEE : 0.05 %
FUNDING FEE    : 0.01 %
LEVERAGE       : 1
Minimum number of trades required: 100
CAN LONG : 1 ; CAN SHORT : 1
-------------------------------------
BEST PARAMETER SET FOUND: 
-------------------------------------
Strategy : 2-EMA crossover simple
EMA      : 72 7
Best Gain: 2829.81%
Win rate : 27.1058%
max DD   : -40.7833%
Gain/DDC : 41.0884
Score    : 1113.74
Number of trades: 926
-------------------------------------
Number of backtests performed : 39402
Time taken                    : 11 seconds 
RAM usage                     : 43.5 MB
-------------------------------------
```

Performance comparison (naive) Python versus C++ :

```
C++ : backtest_double_EMA_StochRSI_float.exe
-------------------------------------
Number of backtests performed : 9120
Time taken                    : 2 seconds 
RAM usage                     : 14.3 MB
-------------------------------------

Python : backtest_double_EMA_StochRSI_float.py
-------------------------------------
Number of backtests performed :  9120
Time taken                    :  5955  seconds 
RAM usage                     :  86.0  MB
-------------------------------------

The C++ code is ~3000 faster than the (naive) python.
```
