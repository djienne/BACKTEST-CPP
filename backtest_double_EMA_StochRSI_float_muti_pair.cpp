#include <iostream>
#include <vector>
#include <iterator>
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
using uint = unsigned int;

static const uint NB_PAIRS = 4;
const uint NB_POSITION_MAX = 4;
const vector<string> COINS = {"BTC", "ETH", "BNB", "XRP"};
const string timeframe = "1h";

const vector<string> DATAFILES = {"./data/Binance/" + timeframe + "/" + COINS[0] + "-USDT.csv",
                                  "./data/Binance/" + timeframe + "/" + COINS[1] + "-USDT.csv",
                                  "./data/Binance/" + timeframe + "/" + COINS[2] + "-USDT.csv",
                                  "./data/Binance/" + timeframe + "/" + COINS[3] + "-USDT.csv"};
const string STRAT_NAME = "2-EMA crossover with Stoch RSI Multi Pair";

const float start_year = 2017; // forced year to start (applies if data below is available)
const float FEE = 0.07f;       // FEES in %
const float USDT_amount_initial = 1000.0f;
const uint MIN_NUMBER_OF_TRADES = 100;         // minimum number of trades required (to avoid some noise / lucky circunstances)
const float MIN_ALLOWED_MAX_DRAWBACK = -50.0f; // %
const float STOCH_RSI_UPPER = 0.800;
const float STOCH_RSI_LOWER = 0.200;
uint start_indexes[NB_PAIRS];

// RANGE OF EMA PERIDOS TO TESTs
const int period_max_EMA = 600;
// const int range_step = 2;
// vector<int> range_EMA = {180};
vector<int> range_EMA_short = integer_range(2, 200 + 4, 1);
vector<int> range_EMA_long = integer_range(70, period_max_EMA + 4, 1);
//////////////////////////
array<std::unordered_map<string, vector<float>>, NB_PAIRS> EMA_LISTS{};
array<vector<float>, NB_PAIRS> StochRSI{};

array<long int, NB_PAIRS> last_times{};

uint i_print = 0;
uint nb_tested = 0;

RUN_RESULTf best{};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void print_best_res(const RUN_RESULTf &bestt)
{
    std::cout << "\n-------------------------------------" << endl;
    std::cout << "BEST PARAMETER SET FOUND: " << endl;
    std::cout << "-------------------------------------" << endl;
    std::cout << "Strategy : " << STRAT_NAME << endl;
    std::cout << "EMAs : " << bestt.ema1 << " " << bestt.ema2 << endl;
    std::cout << "Best Gain: " << bestt.gain_pc << "%" << endl;
    std::cout << "Porfolio : " << bestt.WALLET_VAL_USDT << "$ (started with 1000$)" << endl;
    std::cout << "Win rate : " << bestt.win_rate << "%" << endl;
    std::cout << "max DD   : " << bestt.max_DD << "%" << endl;
    std::cout << "Gain/DDC : " << bestt.gain_over_DDC << endl;
    std::cout << "Score    : " << bestt.score << endl;
    std::cout << "Calmar ratio : " << bestt.calmar_ratio << endl;
    std::cout << "Number of trades: " << bestt.nb_posi_entered << endl;
    std::cout << "Total fees paid: " << round(bestt.total_fees_paid * 100.0f) / 100.0f << "$ (started with 1000$)" << endl;

    std::cout << "-------------------------------------" << endl;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RUN_RESULTf PROCESS(const vector<KLINEf> &PAIRS, const int ema_s, const int ema_l)
{
    nb_tested++;

    RUN_RESULTf result{};

    vector<float> USDT_tracking{};
    vector<int> USDT_tracking_ts{};

    const uint nb_max = PAIRS[0].nb;

    bool LAST_ITERATION = false;
    bool OPEN_LONG_CONDI = false;
    bool CLOSE_LONG_CONDI = false;
    uint nb_profit = 0;
    uint nb_loss = 0;
    uint NB_POSI_ENTERED = 0;
    float pc_change_with_max = 0, max_drawdown = 0;
    float USDT_amount = USDT_amount_initial;
    float MAX_WALLET_VAL_USDT = USDT_amount_initial;
    float total_fees_paid_USDT = 0.0f;
    float WALLET_VAL_USDT = USDT_amount_initial;
    array<float, NB_PAIRS> price_position_open{};
    array<float, NB_PAIRS> COIN_AMOUNTS{};
    array<float, NB_PAIRS> take_profit{};
    array<float, NB_PAIRS> stop_loss{};
    array<float, NB_PAIRS> stop_loss_at_open{};
    for (uint ic = 0; ic < NB_PAIRS; ic++)
    {
        COIN_AMOUNTS[ic] = 0.0f;
    }
    uint ACTIVE_POSITIONS = 0;

    const uint ii_begin = start_indexes[0];

    for (uint ii = ii_begin; ii < nb_max; ii++)
    {
        if (ii == nb_max - 1)
            LAST_ITERATION = true;

        bool closed = false;
        // For all pairs, check to close / open positions
        for (uint ic = 0; ic < NB_PAIRS; ic++)
        {
            if (ii < start_indexes[ic])
                continue;

            // conditions for open / close position
            
            OPEN_LONG_CONDI = EMA_LISTS[ic]["EMA_" + std::to_string(ema_s)][ii] >= EMA_LISTS[ic]["EMA_" + std::to_string(ema_l)][ii] && StochRSI[ic][ii] < STOCH_RSI_UPPER ;
            CLOSE_LONG_CONDI = EMA_LISTS[ic]["EMA_" + std::to_string(ema_s)][ii] <= EMA_LISTS[ic]["EMA_" + std::to_string(ema_l)][ii] && StochRSI[ic][ii] > STOCH_RSI_LOWER ;

            // IT IS IMPORTANT TO CHECK FIRST FOR CLOSING POSITION AND ONLY THEN FOR OPENING POSITION

            // CLOSE LONG
            if (COIN_AMOUNTS[ic] > 0.0f && (CLOSE_LONG_CONDI || LAST_ITERATION))
            {
                const float to_add = COIN_AMOUNTS[ic] * PAIRS[ic].close[ii];
                USDT_amount += to_add;
                COIN_AMOUNTS[ic] = 0.0f;

                ACTIVE_POSITIONS--;

                // apply FEEs
                const float fe = to_add * FEE / 100.0f;
                USDT_amount -= fe;
                total_fees_paid_USDT += fe;
                //
                if (PAIRS[ic].close[ii] >= price_position_open[ic])
                {
                    nb_profit++;
                }
                else
                {
                    nb_loss++;
                }
                closed = true;
            }

            // OPEN LONG
            if (COIN_AMOUNTS[ic] == 0.0f && OPEN_LONG_CONDI && LAST_ITERATION == false && ACTIVE_POSITIONS < NB_POSITION_MAX)
            {
                price_position_open[ic] = PAIRS[ic].close[ii];

                const float usdMultiplier = 1.0f / float(NB_POSITION_MAX - ACTIVE_POSITIONS);

                COIN_AMOUNTS[ic] = USDT_amount * usdMultiplier / PAIRS[ic].close[ii];
                USDT_amount -= USDT_amount * usdMultiplier;

                // apply FEEs
                const float fe = COIN_AMOUNTS[ic] * FEE / 100.0f;
                COIN_AMOUNTS[ic] -= fe;
                total_fees_paid_USDT += fe * PAIRS[ic].close[ii];
                //

                ACTIVE_POSITIONS++;
                NB_POSI_ENTERED++;
            }
        }

        // check wallet status
        if (closed || LAST_ITERATION)
        {
            array<float, NB_PAIRS> closes{};
            for (uint ic = 0; ic < NB_PAIRS; ic++)
                closes[ic] = PAIRS[ic].close[ii];

            WALLET_VAL_USDT = USDT_amount + vector_product(COIN_AMOUNTS, closes);
            if (WALLET_VAL_USDT > MAX_WALLET_VAL_USDT)
                MAX_WALLET_VAL_USDT = WALLET_VAL_USDT;

            pc_change_with_max = (WALLET_VAL_USDT - MAX_WALLET_VAL_USDT) / MAX_WALLET_VAL_USDT * 100.0f;
            if (pc_change_with_max < max_drawdown)
                max_drawdown = pc_change_with_max;

            USDT_tracking.push_back(WALLET_VAL_USDT);
            USDT_tracking_ts.push_back(PAIRS[0].timestamp[ii]);
        }
    }

    array<float, NB_PAIRS> last_closes{};
    for (uint ic = 0; ic < NB_PAIRS; ic++)
        last_closes[ic] = PAIRS[ic].close[nb_max - 1];

    WALLET_VAL_USDT = USDT_amount + vector_product(COIN_AMOUNTS, last_closes);

    const float gain = (WALLET_VAL_USDT - USDT_amount_initial) / USDT_amount_initial * 100.0f;
    const float WR = float(nb_profit) / float(NB_POSI_ENTERED) * 100.0f;
    const float DDC = (1.0f / (1.0f + max_drawdown / 100.0f) - 1.0f) * 100.0f;
    const float score = gain / DDC * WR;

    i_print++;
    if (i_print == 1000)
    {
        i_print = 0;
        std::cout << "DONE: EMAs: " << ema_s << " " << ema_l << endl;
        std::cout << "NB tested = " << nb_tested << "/" << range_EMA_long.size() * range_EMA_short.size() << endl;
        std::cout << "Done " << std::round(float(nb_tested) / float(range_EMA_long.size() * range_EMA_short.size()) * 100.0 * 100.0) / 100.0 << " %" << endl;
        print_best_res(best);
    }

    result.WALLET_VAL_USDT = USDT_amount;
    result.gain_over_DDC = gain / DDC;
    result.gain_pc = gain;
    result.max_DD = max_drawdown;
    result.nb_posi_entered = NB_POSI_ENTERED;
    result.win_rate = WR;
    result.score = score;
    result.calmar_ratio = calculate_calmar_ratio(USDT_tracking_ts, USDT_tracking, DDC);
    result.ema1 = ema_s;
    result.ema2 = ema_l;
    result.total_fees_paid = total_fees_paid_USDT;

    return result;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int super_index = 0;
KLINEf read_input_data(const string &input_file_path)
{
    KLINEf kline;

    ifstream myfile(input_file_path);
    string line;
    long int ts;
    float op, hi, lo, cl, vol;

    ifstream file(input_file_path);
    string value;
    vector<string> getInform;
    while (file.good())
    {
        getline(file, value);
        value = ReplaceAll(value, ";", " ");
        getInform.push_back(value);
        // getInform.erase(getInform.begin()+1);
        stringstream ss{};
        ss << value;
        ss >> ts >> op >> hi >> lo >> cl >> vol;
        kline.timestamp.push_back(ts / 1000);
        kline.open.push_back(op);
        kline.high.push_back(hi);
        kline.low.push_back(lo);
        kline.close.push_back(cl);
    }

    last_times[super_index] = kline.timestamp.back();

    // cout << last_times[super_index] << endl;
    // cout << super_index << endl;

    if (super_index >= 1)
    {
        if (last_times[super_index] != last_times[super_index - 1])
        {
            cout << "Error: inconsistent last times between different pairs." << endl;
            std::abort();
        }
    }

    super_index++;

    myfile.close();

    kline.nb = int(kline.close.size());

    std::cout << "Loaded data file." << endl;
    return kline;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void INITIALIZE_DATA(vector<KLINEf> &PAIRS)
// will modify PAIRS since it is passed as reference
{
    std::cout << "Running INITIALIZE_DATA..." << endl;

    start_indexes[0] = find_max(range_EMA_long) + 2;

    // find initial indexes (different starting times)
    for (uint i = 0; i < PAIRS[0].timestamp.size(); i++)
    {
        for (uint ic = 1; ic < NB_PAIRS; ic++)
        {
            if (PAIRS[ic].timestamp[0] == PAIRS[0].timestamp[i])
            {
                start_indexes[ic] = start_indexes[0] + i;
                std::cout << "Start for " + COINS[ic] << " : " << i << endl;
            }
        }
    }

    for (uint ic = 1; ic < NB_PAIRS; ic++)
    {
        while (PAIRS[ic].close.size() < PAIRS[0].close.size())
        {
            PAIRS[ic].timestamp.insert(PAIRS[ic].timestamp.begin(), 0);
            PAIRS[ic].open.insert(PAIRS[ic].open.begin(), 0.0f);
            PAIRS[ic].high.insert(PAIRS[ic].high.begin(), 0.0f);
            PAIRS[ic].low.insert(PAIRS[ic].low.begin(), 0.0f);
            PAIRS[ic].close.insert(PAIRS[ic].close.begin(), 0.0f);
            PAIRS[ic].nb++;
        }

        for (uint ii = 0; ii < start_indexes[ic] + 5; ii++)
        {
            PAIRS[ic].timestamp[ii] = PAIRS[0].timestamp[ii];
        }
    }

    for (uint ic = 1; ic < NB_PAIRS; ic++)
    {
        if (PAIRS[ic].nb != PAIRS[0].nb || PAIRS[ic].open.size() != PAIRS[0].open.size())
        {
            std::cout << "ERROR: incosistent size" << endl;
            abort();
        }
    }

    //
    for (uint ic = 0; ic < NB_PAIRS; ic++)
    {
        std::cout << "Calculating for " << COINS[ic] << endl;

        // std::cout << "Calculated STOCHRSI." << endl;
        
        vector<int> merged(range_EMA_long.size() + range_EMA_short.size());
        merge(range_EMA_long.begin(),
              range_EMA_long.end(),
              range_EMA_short.begin(),
              range_EMA_short.end(),
              merged.begin());
        sort(merged.begin(), merged.end());
        merged.erase(unique(merged.begin(), merged.end()), merged.end());
        for (const int ema_per : merged)
        {
            EMA_LISTS[ic]["EMA_" + std::to_string(ema_per)] = TALIB_EMA(PAIRS[ic].close, ema_per);
        }
        // std::cout << "Calculated EMAs." << endl;
    }
    cout << "Calculated EMAs." << endl;
    
    for (uint ic = 0; ic < NB_PAIRS; ic++)
    {
    	StochRSI[ic] = TALIB_STOCHRSI_not_averaged(PAIRS[ic].close, 14, 14);
    }
    cout << "Calculated STOCHRSI." << endl;


    std::cout << "Initialized calculations." << endl;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    const double t_begin = get_wall_time();
    std::cout << "\n-------------------------------------" << endl;
    std::cout << "Strategy to test: " << STRAT_NAME << endl;
    std::cout << "DATA FILES TO PROCESS: " << endl;
    for (const string &dataf : DATAFILES)
    {
        std::cout << dataf << endl;
    }

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

    vector<KLINEf> PAIRS;
    PAIRS.reserve(NB_PAIRS);
    for (const string &dataf : DATAFILES)
    {
        PAIRS.push_back(read_input_data(dataf));
    }

    random_shuffle_vector(range_EMA_long);

    INITIALIZE_DATA(PAIRS); // this function modifies PAIRS

    best.gain_over_DDC = -100.0f;
    best.calmar_ratio = -100.0f;

    const uint last_idx = PAIRS[0].nb - 1;

    const int year = get_year_from_timestamp(PAIRS[0].timestamp[0]);
    const int month = get_month_from_timestamp(PAIRS[0].timestamp[0]);
    const int day = get_day_from_timestamp(PAIRS[0].timestamp[0]);

    const int last_year = get_year_from_timestamp(PAIRS[0].timestamp[last_idx]);
    const int last_month = get_month_from_timestamp(PAIRS[0].timestamp[last_idx]);
    const int last_day = get_day_from_timestamp(PAIRS[0].timestamp[last_idx]);

    // Display info
    std::cout << "Begin day      : " << year << "/" << month << "/" << day << endl;
    std::cout << "End day        : " << last_year << "/" << last_month << "/" << last_day << endl;
    std::cout << "OPEN/CLOSE FEE : " << FEE << " %" << endl;
    std::cout << "Maximum number open trades: " << NB_POSITION_MAX << endl;
    std::cout << "Minimum number of trades required    : " << MIN_NUMBER_OF_TRADES << endl;
    std::cout << "Maximum drawback (=drawdown) allowed : " << MIN_ALLOWED_MAX_DRAWBACK << " %" << endl;
    std::cout << "StochRSI Upper Band   : " << STOCH_RSI_UPPER << std::endl;
    std::cout << "StochRSI Lower Band   : " << STOCH_RSI_LOWER << std::endl;
    std::cout << "EMA short period max tested : " << find_max(range_EMA_short) << endl;
    std::cout << "EMA long period max tested : " << find_max(range_EMA_long) << endl;
    std::cout << "-------------------------------------" << endl;

    // MAIN LOOP

    for (const int ema_l : range_EMA_long)
    {
        for (const int ema_s : range_EMA_short)
        {
            const RUN_RESULTf res = PROCESS(PAIRS, ema_s, ema_l);

            if (res.calmar_ratio > best.calmar_ratio && res.gain_pc < 1000000.0f && res.nb_posi_entered >= MIN_NUMBER_OF_TRADES && res.max_DD > MIN_ALLOWED_MAX_DRAWBACK)
            {
                best = res;
            }
        }
    }

    print_best_res(best);

    const double t_end = get_wall_time();

    std::cout << "Number of backtests performed : " << nb_tested << endl;
    std::cout << "Time taken                    : " << t_end - t_begin << " seconds " << endl;
    const double ram_usage = process_mem_usage();
    std::cout << "RAM usage                     : " << std::round(ram_usage * 10.0) / 10.0 << " MB" << endl;
    std::cout << "-------------------------------------" << endl;

    TA_Shutdown();

    return 0;
}
