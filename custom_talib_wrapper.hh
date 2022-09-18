#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <math.h>
#include <ta-lib/ta_libc.h>
#include <unordered_map>
#include <map>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct KLINEf
{
    std::vector<uint> timestamp;
    std::vector<float> open;
    std::vector<float> high;
    std::vector<float> low;
    std::vector<float> close;
    std::unordered_map<std::string, std::vector<float>> indicators;
    uint nb;
};
struct SuperTrend
{
    std::vector<int> supertrend;
    std::vector<float> final_lowerband;
    std::vector<float> final_upperband;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<float> float_range(const float min, const float max, const float step);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<float> TALIB_MIN(const std::vector<float> &vals, const int period);
std::vector<float> TALIB_MAX(const std::vector<float> &vals, const int period);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<float> TALIB_RSI(const std::vector<float> &vals, const int period);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<float> TALIB_EMA(const std::vector<float> &vals, const int period);
std::vector<float> TALIB_SMA(const std::vector<float> &vals, const int period);
std::vector<float> TALIB_ATR(const std::vector<float> &high, const std::vector<float> &low, const std::vector<float> &close, const int period);
std::vector<float> TALIB_TRIX(const std::vector<float> &vals, const int trixLength, const int trixSignal);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<float> TALIB_STOCHRSI_K(const std::vector<float> &vals, const int nb_period_stoch, const int nb_period_rsi, const int k_period, const int d_period);
std::vector<float> TALIB_STOCHRSI_D(const std::vector<float> &vals, const int nb_period_stoch, const int nb_period_rsi, const int k_period, const int d_period);
std::vector<float> TALIB_STOCHRSI_not_averaged(const std::vector<float> &vals, const int nb_period_stoch, const int nb_period_rsi);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SuperTrend TALIB_SuperTrend(const std::vector<float> &high, const std::vector<float> &low, const std::vector<float> &close,
                            const int atr_window, const int atr_multi);
std::vector<float> TALIB_SuperTrend_dir_only(const std::vector<float> &high, const std::vector<float> &low, const std::vector<float> &close,
                                             const int atr_window, const int atr_multi);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<float> TALIB_AO(const std::vector<float> &high, const std::vector<float> &low,
                            const int fast, const int slow);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<float> TALIB_WILLR(const std::vector<float> &high, const std::vector<float> &low, const std::vector<float> &close,
                               const int length);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RESAMPLE_TIMEFRAME(KLINEf &kline_in_in, KLINEf &kline_out, const int tf_in, const int tf_out);
