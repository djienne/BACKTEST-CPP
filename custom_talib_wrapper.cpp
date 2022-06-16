#include "custom_talib_wrapper.hh"
using namespace std;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<float> TALIB_MIN(const std::vector<float> vals, const int period)
{
    std::vector<float> OUT;

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
    
    for (uint ii = 0; ii < outBeg; ii++)
    {
        OUT.push_back(0.0);
    }

    for (uint ii = 0; ii < outNbElement; ii++)
    {
        OUT.push_back(out_val[ii]);
    }

    if (OUT.size()!=vals.size()) abort();

    return OUT;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<float> TALIB_MAX(const std::vector<float> vals, const int period)
{
    std::vector<float> OUT;

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
    
    for (uint ii = 0; ii < outBeg; ii++)
    {
        OUT.push_back(0.0);
    }

    for (uint ii = 0; ii < outNbElement; ii++)
    {
        OUT.push_back(out_val[ii]);
    }

    if (OUT.size()!=vals.size()) abort();

    return OUT;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<float> TALIB_RSI(const std::vector<float> vals, const int period)
{
    std::vector<float> OUT;

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
    
    for (uint ii = 0; ii < outBeg; ii++)
    {
        OUT.push_back(0.0);
    }

    for (uint ii = 0; ii < outNbElement; ii++)
    {
        OUT.push_back(out_val[ii]);
    }

    if (OUT.size()!=vals.size()) abort();

    return OUT;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<float> TALIB_STOCHRSI_K(std::vector<float> vals, const int period, const int k_period, const int d_period)
{
    std::vector<float> OUT;

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
    
    for (uint ii = 0; ii < outBeg; ii++)
    {
        OUT.push_back(0.0);
    }

    for (uint ii = 0; ii < outNbElement; ii++)
    {
        OUT.push_back(out_val[ii]);
    }

    if (OUT.size()!=vals.size()) abort();

    return OUT;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<float> TALIB_EMA(std::vector<float> vals, const int period)
{
    std::vector<float> OUT;

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
    
    for (uint ii = 0; ii < outBeg; ii++)
    {
        OUT.push_back(0.0);
    }

    for (uint ii = 0; ii < outNbElement; ii++)
    {
        OUT.push_back(out_val[ii]);
    }

    if (OUT.size()!=vals.size()) abort();

    return OUT;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<float> TALIB_STOCHRSI_not_averaged(const std::vector<float> vals, const int nb_period_stoch, const int nb_period_rsi)
{
    std::vector<float> stochrsi{};
    stochrsi.reserve(vals.size());
    // lowest_rsi = rsi.rolling(length).min()
    // highest_rsi = rsi.rolling(length).max()
    // stochrsi = (rsi - lowest_rsi) / (highest_rsi - lowest_rsi)
    std::vector<float> rsi = TALIB_RSI(vals, nb_period_rsi);
    std::cout << "Calculated RSI." << std::endl;
    std::vector<float> highest_rsi = TALIB_MAX(rsi, nb_period_stoch);
    std::vector<float> lowest_rsi = TALIB_MIN(rsi, nb_period_stoch);

    for (uint i = 0; i < rsi.size(); i++)
    {
        float val = (rsi[i] - lowest_rsi[i]) / (highest_rsi[i] - lowest_rsi[i]);
        if (std::isnan(val))
        {
            val = 0.0;
        }
        stochrsi.push_back(std::round(val * 1000.0) / 1000.0);
    }

    return stochrsi;
}