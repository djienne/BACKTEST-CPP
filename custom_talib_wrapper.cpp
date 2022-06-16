#include "custom_talib_wrapper.hh"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<int> integer_range(const int min, const int max)
{
    std::vector<int> the_range;

    if (min > max)
        std::abort();

    for (size_t i = min; i <= max; i++)
    {
        the_range.push_back(i);
    }

    return the_range;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<double> double_range(const double min, const double max, const double step)
{
    std::vector<double> the_range;

    if (min > max)
        std::abort();

    int nb = int(max - min) / step;

    for (size_t i = 0; i <= nb; i++)
    {
        the_range.push_back(min + double(i) * step);
    }

    return the_range;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<float> float_range(const float min, const float max, const float step)
{
    std::vector<float> the_range;

    if (min > max)
        std::abort();

    const int nb = int(max - min) / step;

    for (size_t i = 0; i <= nb; i++)
    {
        the_range.push_back(min + float(i) * step);
    }

    return the_range;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<float> ATR(const KLINEf &kline, const int period)
{
    TA_Integer outBeg;
    TA_Integer outNbElement;
    TA_RetCode retCode;
    TA_Real ATR1_tmp[kline.nb], ATR1[kline.nb];

    retCode = TA_S_ATR(0, kline.nb - 1,
                       &kline.d_high[0], &kline.d_low[0], &kline.d_close[0],
                       period,
                       &outBeg, &outNbElement, &ATR1_tmp[0]);

    for (int i = 0; i < kline.nb; i++)
    {
        if (i < period - 1)
        {
            ATR1[i] = 0;
        }
        else
        {

            ATR1[i] = ATR1_tmp[i - period + 1];
        }
    }

    std::vector<float> ATR11;
    ATR11.reserve(kline.nb);

    for (const float &val : ATR1)
    {
        ATR11.push_back(val);
    }

    return ATR11;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<float> RSI(const KLINEf &kline, const int period)
{
    TA_Integer outBeg;
    TA_Integer outNbElement;
    TA_RetCode retCode;
    TA_Real RSI1_tmp[kline.nb], RSI1[kline.nb];

    retCode = TA_S_RSI(0, kline.nb - 1,
                     &kline.d_close[0],
                     period,
                     &outBeg, &outNbElement, &RSI1_tmp[0]);

    for (int i = 0; i < kline.nb; i++)
    {
        if (i < period - 1)
        {
            RSI1[i] = 0;
        }
        else
        {

            RSI1[i] = RSI1_tmp[i - period + 1];
        }
    }

    std::vector<float> RSI11;
    RSI11.reserve(kline.nb);

    for (const float &val : RSI1)
    {
        RSI11.push_back(val);
    }

    return RSI11;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<float> EMA(const KLINEf &kline, const int period)
{
    TA_Integer outBeg;
    TA_Integer outNbElement;
    TA_RetCode retCode;
    TA_Real EMA1_tmp[kline.nb], EMA1[kline.nb];

    retCode = TA_S_EMA(0, kline.nb - 1,
                       &kline.d_close[0],
                       period,
                       &outBeg, &outNbElement, &EMA1_tmp[0]);

    for (int i = 0; i < kline.nb; i++)
    {
        if (i < period - 1)
        {
            EMA1[i] = 0;
        }
        else
        {

            EMA1[i] = EMA1_tmp[i - period + 1];
        }
    }

    std::vector<float> EMA11;
    EMA11.reserve(kline.nb);

    for (const float &val : EMA1)
    {
        EMA11.push_back(val);
    }

    return EMA11;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

KLINEf kline_sub_set(const KLINEf &kline, const int imin, const int imax)
{
    KLINEf kline_sub{};

    kline_sub.nb = imax - imin + 1;

    kline_sub.d_open.reserve(kline_sub.nb);
    kline_sub.d_high.reserve(kline_sub.nb);
    kline_sub.d_low.reserve(kline_sub.nb);
    kline_sub.d_close.reserve(kline_sub.nb);

    for (int i = imin; i < imax; i++)
    {
        kline_sub.d_open.push_back(kline.d_open[i]);
        kline_sub.d_high.push_back(kline.d_high[i]);
        kline_sub.d_low.push_back(kline.d_low[i]);
        kline_sub.d_close.push_back(kline.d_close[i]);
    }

    return kline_sub;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<float> Calculate_ATR(const KLINEf &kline, const int nb_period)
{
    KLINEf kline_sub1 = kline_sub_set(kline, 0, kline.nb / 2);
    KLINEf kline_sub2 = kline_sub_set(kline, kline.nb / 2 - 205, kline.nb);
    std::vector<float> ATR_p1 = ATR(kline_sub1, nb_period);
    std::vector<float> ATR_p2 = ATR(kline_sub2, nb_period);
    std::vector<float> ATR_out{};
    ATR_out.reserve(kline.nb);

    for (int i = 0; i < ATR_p1.size(); i++)
    {
        ATR_out.push_back(float(ATR_p1[i]));
    }
    for (int i = 205; i < ATR_p2.size() - 205; i++)
    {
        ATR_out.push_back(float(ATR_p2[i]));
    }

    return ATR_out;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<float> Calculate_EMA(const KLINEf &kline, const int nb_period)
{
    KLINEf kline_sub1 = kline_sub_set(kline, 0, kline.nb / 2);
    KLINEf kline_sub2 = kline_sub_set(kline, kline.nb / 2 - 205, kline.nb);
    std::vector<float> EMA1_p1 = EMA(kline_sub1, nb_period);
    std::vector<float> EMA1_p2 = EMA(kline_sub2, nb_period);
    std::vector<float> EMA_out{};
    EMA_out.reserve(kline.nb);

    for (int i = 0; i < EMA1_p1.size(); i++)
    {
        EMA_out.push_back(float(EMA1_p1[i]));
    }
    for (int i = 205; i < EMA1_p2.size() - 205; i++)
    {
        EMA_out.push_back(float(EMA1_p2[i]));
    }

    return EMA_out;
}