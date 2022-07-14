#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <math.h>
#include <unordered_map>
#include "tools.hh"
#include "custom_talib_wrapper.hh"
#include <ta-lib/ta_libc.h>
using namespace std;

const std::string DATAFILE = "./data/Binance/30m/BTC-USDT.csv";
const std::string STRAT_NAME = "2-EMA crossover with Stoch RSI";

const float start_year = 2017; // forced year to start (applies if data below is available)
const float FEE = 0.05;        // FEES in %
const float USDT_amount_initial = 1000.0;
const int MIN_NUMBER_OF_TRADES_PER_YEAR = 14; // minimum number of trades required (to avoid some noise / lucky circunstances)
int MIN_NUMBER_OF_TRADES = -1;                // minimum number of trades required (to avoid some noise / lucky circunstances)
const float MIN_ALLOWED_MAX_DRAWBACK = -50.0; // %
const float MAX_ALLOWED_DAYS_BETWEEN_PORTFOLIO_ATH = 365;
const float STOCH_RSI_UPPER = 0.800;
const float STOCH_RSI_LOWER = 0.200;
const float minimum_yearly_gain_pc = -100.0; // pc
int i_start_year = 0;

// RANGE OF EMA PERIDOS TO TESTs
const int period_max_EMA = 350;
const int range_step = 1;
std::vector<int> range1 = integer_range(2, period_max_EMA, range_step);
std::vector<int> range2 = integer_range(2, period_max_EMA, range_step);
//////////////////////////

uint i_print = 0;

std::unordered_map<std::string, std::vector<float>> EMA_LISTS{};
std::vector<float> StochRSI{};
std::vector<float> year{};
std::vector<float> hour{};
std::vector<float> month{};
std::vector<float> day{};
uint nb_tested = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void print_res(const RUN_RESULTf best)
{
    std::cout << "-------------------------------------" << std::endl;
    std::cout << "EMA: " << best.ema1 << " " << best.ema2 << std::endl;
    std::cout << "Gain: " << best.gain_pc << "%" << std::endl;
    std::cout << "Win rate: " << best.win_rate << "%" << std::endl;
    std::cout << "max DD: " << best.max_DD << "%" << std::endl;
    std::cout << "Gain over DDC: " << best.gain_over_DDC << std::endl;
    std::cout << "Score: " << best.score << std::endl;
    std::cout << "Number of trades: " << best.nb_posi_entered << std::endl;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void print_best_res(const RUN_RESULTf best)
{
    std::cout << "\n-------------------------------------" << std::endl;
    std::cout << "BEST PARAMETER SET FOUND: " << std::endl;
    std::cout << "-------------------------------------" << std::endl;
    std::cout << "Strategy : " << STRAT_NAME << std::endl;
    std::cout << "EMAs     : " << best.ema1 << " " << best.ema2 << std::endl;
    std::cout << "Best Gain: " << best.gain_pc << "%" << std::endl;
    std::cout << "Porfolio : " << best.WALLET_VAL_USDT << "$ (started with 1000$)" << std::endl;
    std::cout << "Win rate : " << best.win_rate << "%" << std::endl;
    std::cout << "max DD   : " << best.max_DD << "%" << std::endl;
    std::cout << "Gain/DDC : " << best.gain_over_DDC << std::endl;
    std::cout << "Score    : " << best.score << std::endl;
    std::cout << "Number of trades: " << best.nb_posi_entered << std::endl;
    std::cout << "Max days between porfolio ATH: " << int(float(best.max_delta_t_new_ATH) / 3600.0 / 24.0) << std::endl;
    std::cout << "Yearly gains: " << std::endl;
    for (uint i = 0; i < best.yearly_gains.size(); i++)
    {
        std::cout << best.years_yearly_gains[i] << " :: " << best.yearly_gains[i] << "%" << std::endl;
    }
    std::cout << "Total fees paid: " << round(best.total_fees_paid*100.0)/100.0 << "$ (started with 1000$)" << std::endl;

    std::cout << "-------------------------------------" << std::endl;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void INITIALIZE_DATA(const KLINEf &kline)
{
    std::vector<int> list_ema = {};

    for (uint i = 2; i <= std::max(find_max(range1), find_max(range2)) + 5; i++)
    {
        list_ema.push_back(i);
    }

    for (const uint i : list_ema)
    {
        EMA_LISTS["EMA" + std::to_string(i)] = TALIB_EMA(kline.d_close, i);
    }
    cout << "Calculated EMAs." << endl;
    //StochRSI = TALIB_STOCHRSI_K(kline.d_close,14,3,3);
    StochRSI = TALIB_STOCHRSI_not_averaged(kline.d_close, 14, 14);
    cout << "Calculated STOCHRSI." << endl;

    float yy_b = 1990;
    for (uint ii = 0; ii < kline.nb; ii++)
    {
        const float yy = get_year_from_timestamp(kline.d_time[ii]);
        year.push_back(yy);
        hour.push_back(get_hour_from_timestamp(kline.d_time[ii]));
        month.push_back(get_month_from_timestamp(kline.d_time[ii]));
        day.push_back(get_day_from_timestamp(kline.d_time[ii]));
        // if (ii<100) cout << StochRSI[ii] << endl;
        if (yy >= start_year && yy_b < start_year)
        {
            i_start_year = ii;
        }
        yy_b = yy;
    }

    MIN_NUMBER_OF_TRADES = MIN_NUMBER_OF_TRADES_PER_YEAR * int(find_max(year) - find_min(year) + 1);

    cout << "Initialized calculations." << endl;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RUN_RESULTf PROCESS(const KLINEf &KLINEf, const int ema1_v, const int ema2_v)
{
    nb_tested++;

    RUN_RESULTf result{};

    std::vector<float> close = KLINEf.d_close;
    std::vector<int> timestamp = KLINEf.d_time;
    std::vector<float> EMA1 = EMA_LISTS["EMA" + std::to_string(ema1_v)];
    std::vector<float> EMA2 = EMA_LISTS["EMA" + std::to_string(ema2_v)];

    const int nb_max = KLINEf.nb;

    bool LAST_ITERATION = false;
    bool OPEN_LONG_CONDI = false, CLOSE_LONG_CONDI = false;
    int nb_profit = 0, nb_loss = 0, NB_POSI_ENTERED = 0;
    float pc_change_with_max = 0, max_drawdown = 0, price_position_open = 0;
    float USDT_amount = USDT_amount_initial;
    float WALLET_VAL_begin_year = USDT_amount_initial;
    float MAX_WALLET_VAL_USDT = USDT_amount_initial;
    float COIN_AMOUNT = 0.0;
    float total_fees_paid_USDT = 0.0;
    float WALLET_VAL_USDT = USDT_amount_initial;

    int t_new_ATH = 0;
    int delta_t_new_ATH = 0;
    int max_delta_t_new_ATH = 0;

    int ii_begin = period_max_EMA + 2;
    ii_begin = std::max(i_start_year, ii_begin);

    for (uint ii = ii_begin; ii < nb_max; ii++)
    {
        if (ii == nb_max - 1) LAST_ITERATION = true;

        // condition for open / close position
        OPEN_LONG_CONDI = EMA2[ii] >= EMA1[ii] && StochRSI[ii] > STOCH_RSI_UPPER;
        CLOSE_LONG_CONDI = EMA2[ii] <= EMA1[ii] && StochRSI[ii] < STOCH_RSI_LOWER;

        // IT IS IMPORTANT TO CHECK FIRST FOR CLOSING POSITION AND THEN FOR OPENING POSITION

        // CLOSE LONG
        if (COIN_AMOUNT>0.0 && (CLOSE_LONG_CONDI || LAST_ITERATION))
        {
            USDT_amount = COIN_AMOUNT * close[ii];
            COIN_AMOUNT = 0.0;

            // apply FEEs
            const float fe = USDT_amount * FEE / 100.0;
            USDT_amount -= fe;
            total_fees_paid_USDT += fe;
            //
            if (close[ii] >= price_position_open)
            {
                nb_profit++;
            }
            else
            {
                nb_loss++;
            }
        }

        // OPEN LONG
        if (COIN_AMOUNT==0.0 && OPEN_LONG_CONDI && LAST_ITERATION==false)
        {
            price_position_open = close[ii];

            COIN_AMOUNT = USDT_amount / close[ii];
            USDT_amount = 0.0;

            // apply FEEs
            const float fe = COIN_AMOUNT * FEE / 100.0;
            COIN_AMOUNT -= fe;
            total_fees_paid_USDT += fe * close[ii];
            //

            NB_POSI_ENTERED++;
        }

        // check yealy gains
        if (year[ii - 1] != year[ii] || ii == KLINEf.nb - 1)
        {
            result.years_yearly_gains.push_back(year[ii - 1]);
            WALLET_VAL_USDT = USDT_amount + COIN_AMOUNT * close[ii];
            const float yg = (WALLET_VAL_USDT - WALLET_VAL_begin_year) / WALLET_VAL_begin_year * 100.0;
            result.yearly_gains.push_back(std::round(yg * 100.0) / 100.0);
            WALLET_VAL_begin_year = WALLET_VAL_USDT;
        }

        // check wallet status
        if (CLOSE_LONG_CONDI || LAST_ITERATION) {
            WALLET_VAL_USDT = USDT_amount + COIN_AMOUNT * close[ii];
            if (WALLET_VAL_USDT > MAX_WALLET_VAL_USDT) MAX_WALLET_VAL_USDT = WALLET_VAL_USDT;
            pc_change_with_max = (WALLET_VAL_USDT - MAX_WALLET_VAL_USDT) / MAX_WALLET_VAL_USDT * 100.0;
            if (pc_change_with_max < max_drawdown) max_drawdown = pc_change_with_max;
        
            if (t_new_ATH != 0)
                {
                    delta_t_new_ATH = timestamp[ii] - t_new_ATH;
                    if (delta_t_new_ATH > max_delta_t_new_ATH)
                    {
                        max_delta_t_new_ATH = delta_t_new_ATH;
                    }
                }
                t_new_ATH = timestamp[ii];
        }
    }

    WALLET_VAL_USDT = USDT_amount + COIN_AMOUNT * close[close.size() - 1];

    const float gain = (WALLET_VAL_USDT - USDT_amount_initial) / USDT_amount_initial * 100.0;
    const float WR = float(nb_profit) / float(NB_POSI_ENTERED) * 100.0;
    const float DDC = (1.0 / (1.0 + max_drawdown / 100.0) - 1.0) * 100.0;
    const float score = gain / DDC * WR;

    i_print++;
    if (i_print == 30000)
    {
        i_print = 0;
        std::cout << "DONE: EMA: " << ema1_v << " and EMA: " << ema2_v << endl;
    }

    result.WALLET_VAL_USDT = USDT_amount;
    result.gain_over_DDC = gain / DDC;
    result.gain_pc = gain;
    result.max_DD = max_drawdown;
    result.nb_posi_entered = NB_POSI_ENTERED;
    result.win_rate = WR;
    result.score = score;
    result.ema1 = ema1_v;
    result.ema2 = ema2_v;
    result.total_fees_paid = total_fees_paid_USDT;
    result.max_delta_t_new_ATH = max_delta_t_new_ATH;

    return result;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

KLINEf read_input_data(const std::string &input_file_path)
{
    KLINEf kline;

    ifstream myfile(input_file_path);
    std::string line;
    long int ts;
    float op, hi, lo, cl, vol;
    int ho;
    char comma = ';';
    bool first_line = true;

    ifstream file(input_file_path);
    string value;
    vector<string> getInform;
    while (file.good())
    {
        getline(file, value);
        const auto pos = value.find(';');
        value = ReplaceAll(value, ";", " ");
        getInform.push_back(value);
        // getInform.erase(getInform.begin()+1);
        stringstream ss{};
        ss << value;
        ss >> ts >> op >> hi >> lo >> cl >> vol;
        kline.d_time.push_back(ts / 1000);
        kline.d_open.push_back(op);
        kline.d_high.push_back(hi);
        kline.d_low.push_back(lo);
        kline.d_close.push_back(cl);
        // cout << kline.d_open.back()<<endl;
    }

    myfile.close();

    kline.nb = int(kline.d_close.size());

    std::cout << "Loaded data file." << std::endl;
    return kline;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    // std::cout << range_step << std::endl;
    // std::cout << period_max_EMA << std::endl; 
    // std::vector<float> test{1.,2.,3.,4.,5.,6.,7.,8.,9.,10.,11.,12.,13.,14.,15.,16.,17.,5.,4.,3.,2.,1.};
    // std::vector<float> EMA = TALIB_EMA(test,14);
    // std::vector<float> RSI = TALIB_RSI(test,14);
    // std::vector<float> MIN = TALIB_MIN(test,14);
    // std::cout << MIN.size() << std::endl;
    // for (uint i = 0; i < RSI.size(); i++)
    // {
    //     cout << RSI[i] << endl;
    // }
    // std::abort();   

    double t_begin = get_wall_time();
    std::cout << "\n-------------------------------------" << std::endl;
    std::cout << "Strategy to test: " << STRAT_NAME << std::endl;
    std::cout << "DATA FILE TO PROCESS: " << DATAFILE << std::endl;

    TA_RetCode retCode;
    retCode = TA_Initialize();
    if (retCode != TA_SUCCESS)
    {
        std::cout << "Cannot initialize TA-Lib !\n"
                  << retCode << "\n";
    }
    else
    {
        std::cout << "Initialized TA-Lib !\n";
    }

    const KLINEf kline = read_input_data(DATAFILE);

    INITIALIZE_DATA(kline);

    RUN_RESULTf best{};
    best.gain_over_DDC = -100.0;

    int i_print2 = 0;

    // Display info
    std::cout << "Begin day      : " << year[i_start_year] << "/" << month[i_start_year] << "/" << day[i_start_year] << std::endl;
    std::cout << "End day        : " << year.back() << "/" << month.back() << "/" << day.back() << std::endl;
    std::cout << "OPEN/CLOSE FEE : " << FEE << " %" << std::endl;
    std::cout << "Minimum number of trades required    : " << MIN_NUMBER_OF_TRADES << std::endl;
    std::cout << "Maximum drawback (=drawdown) allowed : " << MIN_ALLOWED_MAX_DRAWBACK << " %" << std::endl;
    std::cout << "StochRSI Upper Band   : " << STOCH_RSI_UPPER << std::endl;
    std::cout << "StochRSI Lower Band   : " << STOCH_RSI_LOWER << std::endl;
    std::cout << "EMA period max tested : " << period_max_EMA << std::endl;
    std::cout << "EMA range step        : " << range_step << std::endl;
    std::cout << "-------------------------------------" << std::endl;

    // MAIN LOOP

    for (int ema1 : range1)
    {
        for (int ema2 : range2)
        {
            if (std::abs(ema1-ema2)<3) continue;

            RUN_RESULTf res = PROCESS(kline, ema1, ema2);

            if (res.gain_over_DDC > best.gain_over_DDC 
                && res.gain_pc < 100000.0 
                && res.nb_posi_entered >= MIN_NUMBER_OF_TRADES // should do at least 100 trades
                && res.max_DD > MIN_ALLOWED_MAX_DRAWBACK 
                && find_min(res.yearly_gains) > minimum_yearly_gain_pc)
            {
                best = res;
            }
        }
    }

    print_best_res(best);
    double t_end = get_wall_time();

    std::cout << "Number of backtests performed : " << nb_tested << std::endl;
    std::cout << "Time taken                    : " << t_end - t_begin << " seconds " << std::endl;
    const double ram_usage = process_mem_usage();
    std::cout << "RAM usage                     : " << std::round(ram_usage * 10.0) / 10.0 << " MB" << std::endl;
    std::cout << "-------------------------------------" << std::endl;

    TA_Shutdown();
}
