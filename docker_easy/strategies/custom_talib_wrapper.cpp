#include "custom_talib_wrapper.hh"
using namespace std;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<float> TALIB_MIN(const std::vector<float> &vals, const int period)
{
    std::vector<float> OUT;
    OUT.reserve(vals.size());

    TA_Integer outBeg;
    TA_Integer outNbElement;
    TA_RetCode retCode;
    TA_Real out_val[vals.size()];

    int min_idx = 0;
    int max_idx = vals.size() - 1;

    retCode = TA_S_MIN(min_idx, max_idx,
                       &vals[0],
                       period,
                       &outBeg,
                       &outNbElement,
                       &out_val[0]);

    for (int ii = 0; ii < outBeg; ii++)
    {
        OUT.push_back(0.0);
    }

    for (int ii = 0; ii < outNbElement; ii++)
    {
        OUT.push_back(out_val[ii]);
    }

    if (OUT.size() != vals.size())
        abort();

    return OUT;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<float> TALIB_MAX(const std::vector<float> &vals, const int period)
{
    std::vector<float> OUT;
    OUT.reserve(vals.size());

    TA_Integer outBeg;
    TA_Integer outNbElement;
    TA_RetCode retCode;
    TA_Real out_val[vals.size()];

    int min_idx = 0;
    int max_idx = vals.size() - 1;

    retCode = TA_S_MAX(min_idx, max_idx,
                       &vals[0],
                       period,
                       &outBeg,
                       &outNbElement,
                       &out_val[0]);

    for (int ii = 0; ii < outBeg; ii++)
    {
        OUT.push_back(0.0);
    }

    for (int ii = 0; ii < outNbElement; ii++)
    {
        OUT.push_back(out_val[ii]);
    }

    if (OUT.size() != vals.size())
        abort();

    return OUT;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<float> TALIB_RSI(const std::vector<float> &vals, const int period)
{
    std::vector<float> OUT;
    OUT.reserve(vals.size());

    TA_Integer outBeg;
    TA_Integer outNbElement;
    TA_RetCode retCode;
    TA_Real out_val[vals.size()];

    int min_idx = 0;
    int max_idx = vals.size() - 1;

    retCode = TA_S_RSI(min_idx, max_idx,
                       &vals[0],
                       period,
                       &outBeg,
                       &outNbElement,
                       &out_val[0]);

    for (int ii = 0; ii < outBeg; ii++)
    {
        OUT.push_back(0.0);
    }

    for (int ii = 0; ii < outNbElement; ii++)
    {
        OUT.push_back(out_val[ii]);
    }

    if (OUT.size() != vals.size())
        abort();

    return OUT;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<float> TALIB_STOCHRSI_K(const std::vector<float> &vals, const int period, const int k_period, const int d_period)
{
    std::vector<float> OUT;
    OUT.reserve(vals.size());

    TA_Integer outBeg;
    TA_Integer outNbElement;
    TA_RetCode retCode;
    TA_Real out_val[vals.size()];
    TA_Real out_val_notused[vals.size()];

    int min_idx = 0;
    int max_idx = vals.size() - 1;

    retCode = TA_S_STOCHRSI(min_idx, max_idx,
                            &vals[0],
                            period, k_period, d_period,
                            TA_MAType_SMA,
                            &outBeg,
                            &outNbElement,
                            &out_val[0], &out_val_notused[0]);

    for (int ii = 0; ii < outBeg; ii++)
    {
        OUT.push_back(0.0);
    }

    for (int ii = 0; ii < outNbElement; ii++)
    {
        OUT.push_back(out_val[ii]);
    }

    if (OUT.size() != vals.size())
        abort();

    return OUT;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<float> TALIB_EMA(const std::vector<float> &vals, const int period)
{
    std::vector<float> OUT;
    OUT.reserve(vals.size());

    TA_Integer outBeg;
    TA_Integer outNbElement;
    TA_RetCode retCode;
    TA_Real out_val[vals.size()];

    int min_idx = 0;
    int max_idx = vals.size() - 1;

    retCode = TA_S_EMA(min_idx, max_idx,
                       &vals[0],
                       period,
                       &outBeg,
                       &outNbElement,
                       &out_val[0]);

    for (int ii = 0; ii < outBeg; ii++)
    {
        OUT.push_back(0.0);
    }

    for (int ii = 0; ii < outNbElement; ii++)
    {
        OUT.push_back(out_val[ii]);
    }

    if (OUT.size() != vals.size())
        abort();

    return OUT;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<float> TALIB_SMA(const std::vector<float> &vals, const int period)
{
    std::vector<float> OUT;
    OUT.reserve(vals.size());

    TA_Integer outBeg;
    TA_Integer outNbElement;
    TA_RetCode retCode;
    TA_Real out_val[vals.size()];

    int min_idx = 0;
    int max_idx = vals.size() - 1;

    retCode = TA_S_SMA(min_idx, max_idx,
                       &vals[0],
                       period,
                       &outBeg,
                       &outNbElement,
                       &out_val[0]);

    for (int ii = 0; ii < outBeg; ii++)
    {
        OUT.push_back(0.0);
    }

    for (int ii = 0; ii < outNbElement; ii++)
    {
        OUT.push_back(out_val[ii]);
    }

    if (OUT.size() != vals.size())
        abort();

    return OUT;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<float> TALIB_ATR(const std::vector<float> &high, const std::vector<float> &low, const std::vector<float> &close, const int period)
{
    std::vector<float> OUT;
    OUT.reserve(high.size());

    TA_Integer outBeg;
    TA_Integer outNbElement;
    TA_RetCode retCode;
    TA_Real out_val[high.size()];

    int min_idx = 0;
    int max_idx = high.size() - 1;

    retCode = TA_S_ATR(min_idx, max_idx,
                       &high[0], &low[0], &close[0],
                       period,
                       &outBeg,
                       &outNbElement,
                       &out_val[0]);

    for (int ii = 0; ii < outBeg; ii++)
    {
        OUT.push_back(0.0);
    }

    for (int ii = 0; ii < outNbElement; ii++)
    {
        OUT.push_back(out_val[ii]);
    }

    if (OUT.size() != high.size() || OUT.size() != low.size() || OUT.size() != close.size())
        abort();

    return OUT;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<float> TALIB_STOCHRSI_not_averaged(const std::vector<float> &vals, const int nb_period_stoch, const int nb_period_rsi)
{
    std::vector<float> stochrsi{};
    stochrsi.reserve(vals.size());
    // lowest_rsi = rsi.rolling(length).min()
    // highest_rsi = rsi.rolling(length).max()
    // stochrsi = (rsi - lowest_rsi) / (highest_rsi - lowest_rsi)
    std::vector<float> rsi = TALIB_RSI(vals, nb_period_rsi);
    // std::cout << "Calculated RSI." << std::endl;
    std::vector<float> highest_rsi = TALIB_MAX(rsi, nb_period_stoch);
    std::vector<float> lowest_rsi = TALIB_MIN(rsi, nb_period_stoch);

    for (uint i = 0; i < rsi.size(); i++)
    {
        float val = (rsi[i] - lowest_rsi[i]) / (highest_rsi[i] - lowest_rsi[i]);
        if (std::isnan(val) | std::isinf(val))
        {
            val = 0.0;
        }
        stochrsi.push_back(std::round(val * 1000.0) / 1000.0);
    }

    return stochrsi;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<float> TALIB_TRIX(const std::vector<float> &vals, const int trixLength, const int trixSignal)
{
    std::vector<float> TRIX;
    std::vector<float> TRIX_PCT;
    std::vector<float> TRIX_SIGNAL;
    std::vector<float> TRIX_HISTO;

    TRIX = TALIB_EMA(vals, trixLength);
    TRIX = TALIB_EMA(TRIX, trixLength);
    TRIX = TALIB_EMA(TRIX, trixLength);

    TRIX_PCT.reserve(vals.size());
    TRIX_HISTO.reserve(vals.size());

    TRIX_PCT.push_back(0.0);
    for (uint i = 1; i < TRIX.size(); i++)
    {
        float val = (TRIX[i] - TRIX[i - 1]) / TRIX[i - 1] * 100.0;
        if (std::isinf(val) | std::isnan(val))
        {
            val = 0.0;
        }
        TRIX_PCT.push_back(val);
    }

    if (TRIX_PCT.size() != TRIX.size())
    {
        std::cout << "ERROR TRIX_PCT.size()!=TRIX.size()" << std::endl;
        std::abort();
    }

    TRIX_SIGNAL = TALIB_SMA(TRIX_PCT, trixSignal);

    for (uint i = 0; i < TRIX_PCT.size(); i++)
    {
        TRIX_HISTO.push_back(TRIX_PCT[i] - TRIX_SIGNAL[i]);
    }

    return TRIX_HISTO;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SuperTrend TALIB_SuperTrend(const std::vector<float> &high, const std::vector<float> &low, const std::vector<float> &close,
                            const int atr_window, const int atr_multi)
{
    vector<float> hl2{};
    vector<float> upperband{};
    vector<float> lowerband{};
    vector<int> dir{};
    vector<float> trend{};
    vector<float> longg{};
    vector<float> shortt{};
    vector<float> matr{};
    hl2.reserve(high.size());
    upperband.reserve(high.size());
    lowerband.reserve(high.size());
    dir.reserve(high.size());
    trend.reserve(high.size());
    longg.reserve(high.size());
    shortt.reserve(high.size());
    matr.reserve(high.size());

    const vector<float> ATR = TALIB_ATR(high, low, close, atr_window);

    const uint m = close.size();

    for (uint ii = 0; ii < m; ii++)
    {
        dir.push_back(1);
        trend.push_back(0.0);
        longg.push_back(NAN);
        shortt.push_back(NAN);
    }

    for (uint ii = 0; ii < m; ii++)
    {
        // HL2 is the average of high and low prices
        hl2.push_back((high[ii] + low[ii]) / 2.0f);
        matr.push_back(float(atr_multi) * ATR[ii]);
        upperband.push_back(hl2.back() + matr.back());
        lowerband.push_back(hl2.back() - matr.back());
    }

    for (uint ii = 1; ii < m; ii++)
    {
        if (close[ii] > upperband[ii - 1])
        {
            dir[ii] = 1;
        }
        else if (close[ii] < lowerband[ii - 1])
        {
            dir[ii] = -1;
        }
        else
        {
            dir[ii] = dir[ii - 1];
            if (dir[ii] > 0 && lowerband[ii] < lowerband[ii - 1])
            {
                lowerband[ii] = lowerband[ii - 1];
            }
            if (dir[ii] < 0 && upperband[ii] > upperband[ii - 1])
            {
                upperband[ii] = upperband[ii - 1];
            }
        }

        if (dir[ii] > 0)
        {
            trend[ii] = lowerband[ii];
            longg[ii] = lowerband[ii];
        }
        else
        {
            trend[ii] = upperband[ii];
            shortt[ii] = upperband[ii];
        }
    }

    return {dir, lowerband, upperband};
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void swap(int *first, int *second)
{
    int temp;
    temp = *first;
    *first = *second;
    *second = temp;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<float> TALIB_AO(const std::vector<float> &high, const std::vector<float> &low,
                            const int fast, const int slow)
{
    int fastt = fast;
    int sloww = slow;
    if (sloww < fastt)
    {
        swap(&fastt, &sloww);
    }

    const uint m = high.size();

    vector<float> median_price{};
    vector<float> fast_sma{};
    vector<float> slow_sma{};
    vector<float> ao{};
    median_price.reserve(m);
    ao.reserve(m);

    for (uint ii = 0; ii < m; ii++)
    {
        median_price.push_back(0.5f * (high[ii] + low[ii]));
    }

    fast_sma = TALIB_SMA(median_price, fastt);
    slow_sma = TALIB_SMA(median_price, sloww);
    for (uint ii = 0; ii < m; ii++)
    {
        ao.push_back(fast_sma[ii] - slow_sma[ii]);
    }

    return ao;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<float> TALIB_WILLR(const std::vector<float> &high, const std::vector<float> &low, const std::vector<float> &close,
                               const int length)
{
    std::vector<float> OUT;
    OUT.reserve(high.size());

    TA_Integer outBeg;
    TA_Integer outNbElement;
    TA_RetCode retCode;
    TA_Real out_val[high.size()];

    int min_idx = 0;
    int max_idx = high.size() - 1;

    retCode = TA_S_WILLR(min_idx, max_idx,
                         &high[0], &low[0], &close[0],
                         length,
                         &outBeg,
                         &outNbElement,
                         &out_val[0]);

    for (int ii = 0; ii < outBeg; ii++)
    {
        OUT.push_back(0.0);
    }

    for (int ii = 0; ii < outNbElement; ii++)
    {
        OUT.push_back(out_val[ii]);
    }

    if (OUT.size() != high.size() || OUT.size() != low.size() || OUT.size() != close.size())
        abort();

    return OUT;
}
