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

const string STRAT_NAME = "SuperReversal";

const vector<uint> MAX_OPEN_TRADES_TO_TEST{5, 6, 7, 8, 9, 10};
const vector<string> COINS = {"BTC", "ETH", "BNB", "XRP", "ADA", "SOL",
                              "AVAX", "MATIC", "DOT", "FTT", "CHZ", "EGLD",
                              "ATOM", "ALGO", "NEAR", "LTC", "MANA", "SAND", "APE", "ETC"};
static const uint NB_PAIRS = 20;

const string timeframe_1 = "1h";
const string timeframe_2 = "15m";
vector<string> DATAFILES_1h{};
vector<string> DATAFILES_15m{};

const float start_year = 2017; // forced year to start (applies if data below is available)
const float FEE = 0.07f;       // FEES in %
const float USDT_amount_initial = 1000.0f;
const uint MIN_NUMBER_OF_TRADES = 100;         // minimum number of trades required (to avoid some noise / lucky circunstances)
const float MIN_ALLOWED_MAX_DRAWBACK = -36.0f; // %
vector<KLINEf> PAIRS;
uint start_indexes[NB_PAIRS];

// RANGE OF EMA PERIDOS TO TESTs
const int period_max = 600;
// const int range_step = 2;
// vector<int> range_EMA = {180};
vector<int> range_ema_fast = integer_range(2, 200 + 4, 2);
vector<int> range_ema_slow = integer_range(70, period_max + 4, 3);
//////////////////////////

uint last_times[NB_PAIRS];

uint i_print = 0;
uint nb_tested = 0;

RUN_RESULTf best{};

void fill_datafile_paths()
{
    for (uint i = 0; i < COINS.size(); i++)
    {
        DATAFILES_1h.push_back("./data/Binance/" + timeframe_1 + "/" + COINS[i] + "-USDT.csv");
        DATAFILES_15m.push_back("./data/Binance/" + timeframe_2 + "/" + COINS[i] + "-USDT.csv");
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void print_best_res(const RUN_RESULTf &bestt)
{
    std::cout << "\n-------------------------------------" << endl;
    std::cout << "BEST PARAMETER SET FOUND: " << endl;
    std::cout << "-------------------------------------" << endl;
    std::cout << "Strategy : " << STRAT_NAME << endl;
    std::cout << "EMAs : " << bestt.ema1 << " " << bestt.ema2 << endl;
    std::cout << "Max Open Trades : " << bestt.max_open_trades << endl;
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

RUN_RESULTf PROCESS(vector<KLINEf> &PAIRS, const int ema_f, const int ema_s, const uint MAX_OPEN_TRADES)
{
    nb_tested++;

    RUN_RESULTf result{};

    vector<float> USDT_tracking{};
    USDT_tracking.reserve(1000);
    vector<int> USDT_tracking_ts{};
    USDT_tracking_ts.reserve(1000);

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
            const std::string ema_f_str = "EMA_" + std::to_string(ema_f) + "_1h";
            const std::string ema_s_str = "EMA_" + std::to_string(ema_s) + "_1h";
            OPEN_LONG_CONDI = PAIRS[ic].indicators[ema_f_str][ii] > PAIRS[ic].indicators[ema_s_str][ii] 
                                && PAIRS[ic].indicators["supertrend_1h"][ii] == 1 
                                && PAIRS[ic].close[ii] > PAIRS[ic].indicators[ema_f_str][ii] 
                                && PAIRS[ic].low[ii] < PAIRS[ic].indicators[ema_f_str][ii];

            CLOSE_LONG_CONDI = (PAIRS[ic].indicators[ema_f_str][ii] < PAIRS[ic].indicators[ema_s_str][ii] || PAIRS[ic].indicators["supertrend_1h"][ii] == -1) 
                                && PAIRS[ic].close[ii] < PAIRS[ic].indicators[ema_f_str][ii] 
                                && PAIRS[ic].high[ii] > PAIRS[ic].indicators[ema_f_str][ii];

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
            if (COIN_AMOUNTS[ic] == 0.0f && OPEN_LONG_CONDI && LAST_ITERATION == false && ACTIVE_POSITIONS < MAX_OPEN_TRADES)
            {
                price_position_open[ic] = PAIRS[ic].close[ii];

                const float usdMultiplier = 1.0f / float(MAX_OPEN_TRADES - ACTIVE_POSITIONS);

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
    if (i_print == 100)
    {
        i_print = 0;
        std::cout << "DONE: EMAs: " << ema_f << " " << ema_s << endl;
        std::cout << "NB tested = " << nb_tested << "/" << range_ema_slow.size() * range_ema_fast.size() * MAX_OPEN_TRADES_TO_TEST.size() << endl;
        std::cout << "Done " << std::round(float(nb_tested) / float(range_ema_slow.size() * range_ema_fast.size() * MAX_OPEN_TRADES_TO_TEST.size()) * 100.0 * 100.0) / 100.0 << " %" << endl;
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
    result.ema1 = ema_f;
    result.ema2 = ema_s;
    result.total_fees_paid = total_fees_paid_USDT;
    result.max_open_trades = MAX_OPEN_TRADES;

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

vector<uint> add_zeros(const vector<uint> &vec_in, const int nb_to_add)
{
    vector<uint> vec_to_add(nb_to_add, 0);
    vec_to_add.insert(vec_to_add.end(), vec_in.begin(), vec_in.end());
    return vec_to_add;
}
vector<float> add_zeros(const vector<float> &vec_in, const int nb_to_add)
{
    vector<float> vec_to_add(nb_to_add, 0.0);
    vec_to_add.insert(vec_to_add.end(), vec_in.begin(), vec_in.end());
    return vec_to_add;
}

vector<KLINEf> LOAD_LTF_DATA()
{
    // calculate MTF 1h data from 15 min data

    vector<KLINEf> PAIRS{};
    PAIRS.reserve(NB_PAIRS);

    for (const string &dataf : DATAFILES_15m)
    {
        PAIRS.push_back(read_input_data(dataf));
    }
    super_index = 0;

    start_indexes[0] = find_max(range_ema_slow) + 2;

    // find initial indexes (different starting times)
    for (uint i = 0; i < PAIRS[0].timestamp.size(); i++)
    {
        for (uint ic = 1; ic < NB_PAIRS; ic++)
        {
            if (PAIRS[ic].timestamp[0] == PAIRS[0].timestamp[i])
            {
                start_indexes[ic] = start_indexes[0] + i;
                std::cout << "Start for 15m " + COINS[ic] << " : " << i << endl;
            }
        }
    }

    for (uint ic = 1; ic < NB_PAIRS; ic++)
    {
        const int nb_to_add = PAIRS[0].close.size() - PAIRS[ic].close.size();

        PAIRS[ic].timestamp = add_zeros(PAIRS[ic].timestamp, nb_to_add);
        PAIRS[ic].open = add_zeros(PAIRS[ic].open, nb_to_add);
        PAIRS[ic].high = add_zeros(PAIRS[ic].high, nb_to_add);
        PAIRS[ic].low = add_zeros(PAIRS[ic].low, nb_to_add);
        PAIRS[ic].close = add_zeros(PAIRS[ic].close, nb_to_add);

        PAIRS[ic].nb = PAIRS[0].close.size();

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
            std::cout << PAIRS[ic].nb << " " << PAIRS[0].nb << " " << PAIRS[ic].open.size() << " " << PAIRS[0].open.size() << std::endl;
            abort();
        }
    }

    return PAIRS;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

vector<KLINEf> LOAD_H1_DATA()
{
    // calculation indicators in the 1h timeframe

    vector<KLINEf> PAIRS_1h{};
    uint start_indexes_1h[NB_PAIRS];

    for (const string &dataf : DATAFILES_1h)
    {
        PAIRS_1h.push_back(read_input_data(dataf));
    }
    start_indexes_1h[0] = find_max(range_ema_slow) + 2;

    // find initial indexes (different starting times)
    for (uint i = 0; i < PAIRS_1h[0].timestamp.size(); i++)
    {
        for (uint ic = 1; ic < NB_PAIRS; ic++)
        {
            if (PAIRS_1h[ic].timestamp[0] == PAIRS_1h[0].timestamp[i])
            {
                start_indexes_1h[ic] = start_indexes_1h[0] + i;
                std::cout << "Start for 1h " + COINS[ic] << " : " << i << endl;
            }
        }
    }

    for (uint ic = 1; ic < NB_PAIRS; ic++)
    {
        const int nb_to_add = PAIRS_1h[0].close.size() - PAIRS_1h[ic].close.size();

        PAIRS_1h[ic].timestamp = add_zeros(PAIRS_1h[ic].timestamp, nb_to_add);
        PAIRS_1h[ic].open = add_zeros(PAIRS_1h[ic].open, nb_to_add);
        PAIRS_1h[ic].high = add_zeros(PAIRS_1h[ic].high, nb_to_add);
        PAIRS_1h[ic].low = add_zeros(PAIRS_1h[ic].low, nb_to_add);
        PAIRS_1h[ic].close = add_zeros(PAIRS_1h[ic].close, nb_to_add);

        PAIRS_1h[ic].nb = PAIRS_1h[0].close.size();

        for (uint ii = 0; ii < start_indexes_1h[ic] + 5; ii++)
        {
            PAIRS_1h[ic].timestamp[ii] = PAIRS_1h[0].timestamp[ii];
        }
    }

    for (uint ic = 1; ic < NB_PAIRS; ic++)
    {
        if (PAIRS_1h[ic].nb != PAIRS_1h[0].nb || PAIRS_1h[ic].open.size() != PAIRS_1h[0].open.size())
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
        PAIRS_1h[ic].indicators["supertrend"] = TALIB_SuperTrend_dir_only(PAIRS_1h[ic].high, PAIRS_1h[ic].low, PAIRS_1h[ic].close, 15, 5);

        // Calculate EMAs
        vector<int> merged(range_ema_slow.size() + range_ema_fast.size());
        merge(range_ema_slow.begin(),
              range_ema_slow.end(),
              range_ema_fast.begin(),
              range_ema_fast.end(),
              merged.begin());
        sort(merged.begin(), merged.end());
        merged.erase(unique(merged.begin(), merged.end()), merged.end());

        for (const int ema_per : merged)
        {
            PAIRS_1h[ic].indicators["EMA_" + std::to_string(ema_per)] = TALIB_EMA(PAIRS_1h[ic].close, ema_per);
        }
    }

    return PAIRS_1h;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void INITIALIZE_DATA()
// will modify PAIRS since it is passed as reference
{
    std::cout << "Running INITIALIZE_DATA..." << endl;

    PAIRS = LOAD_LTF_DATA();
    vector<KLINEf> PAIRS_1h = LOAD_H1_DATA();

    // resample 1h to 15min data
    for (uint ic = 0; ic < NB_PAIRS; ic++)
    {
        RESAMPLE_TIMEFRAME(PAIRS_1h[ic], PAIRS[ic], 60, 15);
    }

    std::cout << "Initialized calculations." << endl;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    const double t_begin = get_wall_time();
    std::cout << "\n-------------------------------------" << endl;
    std::cout << "Strategy to test: " << STRAT_NAME << endl;
    std::cout << "DATA FILES TO PROCESS: " << endl;

    fill_datafile_paths();

    for (const string &dataf : DATAFILES_1h)
    {
        std::cout << dataf << endl;
    }
    for (const string &dataf : DATAFILES_15m)
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

    INITIALIZE_DATA();

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
    std::cout << "Minimum number of trades required    : " << MIN_NUMBER_OF_TRADES << endl;
    std::cout << "Maximum drawback (=drawdown) allowed : " << MIN_ALLOWED_MAX_DRAWBACK << " %" << endl;
    std::cout << "EMA short period max tested : " << find_max(range_ema_fast) << endl;
    std::cout << "EMA long period max tested : " << find_max(range_ema_slow) << endl;
    std::cout << "-------------------------------------" << endl;

    // MAIN LOOP

    std::vector<SR_params> param_list{};
    param_list.reserve(range_ema_slow.size() * range_ema_fast.size() * MAX_OPEN_TRADES_TO_TEST.size());

    random_shuffle_vector(range_ema_slow);

    for (const uint MAX_OPEN_TRADES : MAX_OPEN_TRADES_TO_TEST)
    {
        for (const int ema_s : range_ema_slow)
        {
            for (const int ema_f : range_ema_fast)
            {
                SR_params to_add{ema_f, ema_s, MAX_OPEN_TRADES};
                param_list.push_back(to_add);
            }
        }
    }
    std::cout << "Saved parameter list to test." << std::endl;
    std::cout << "Running all backtests..." << std::endl;

    random_shuffle_vector_params(param_list);

    for (const SR_params par : param_list)
    {
        const RUN_RESULTf res = PROCESS(PAIRS, par.ema_fast, par.ema_slow, par.max_open_trades);

        if (res.calmar_ratio > best.calmar_ratio && res.gain_pc < 1000000.0f && res.nb_posi_entered >= MIN_NUMBER_OF_TRADES && res.max_DD > MIN_ALLOWED_MAX_DRAWBACK)
        {
            best = res;
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
