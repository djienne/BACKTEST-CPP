#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <math.h>
#include <ta-lib/ta_libc.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct KLINEf
{
    std::vector<int> d_time;
    std::vector<float> d_open;
    std::vector<float> d_high;
    std::vector<float> d_low;
    std::vector<float> d_close;
    std::vector<float> d_K;
    std::vector<float> d_D;
    std::vector<float> ATR;
    std::vector<float> RSI;
    int nb;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct StochRSI
{
    std::vector<double> K;
    std::vector<double> D;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<int> integer_range(const int min, const int max);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<double> double_range(const double min, const double max, const double step);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<float> float_range(const float min, const float max, const float step);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<float> ATR(const KLINEf &kline, const int period);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<float> RSI(const KLINEf &kline, const int period);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<float> EMA(const KLINEf &kline, const int period);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

KLINEf kline_sub_set(const KLINEf &kline, const int imin, const int imax);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<float> Calculate_ATR(const KLINEf &kline, const int nb_period);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<float> Calculate_EMA(const KLINEf &kline, const int nb_period);
