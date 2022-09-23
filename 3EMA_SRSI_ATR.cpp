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

const string STRAT_NAME = "EMA3_SRSI_ATR";

const vector<uint> MAX_OPEN_TRADES_TO_TEST{7, 8, 9, 10, 11, 12};
const vector<string> COINS = {"BTC",
                              "ETH",
                              "BNB",
                              "XRP",
                              "ADA",
                              "SOL",
                              "DOGE",
                              "DOT",
                              "TRX",
                              "AVAX",
                              "MATIC",
                              "LTC",
                              "FTT",
                              "LINK",
                              "XMR",
                              "XLM",
                              "NEAR",
                              "ALGO",
                              "ATOM",
                              "VET",
                              "MANA",
                              "APE",
                              "XTZ",
                              "SAND",
                              "THETA",
                              "EGLD",
                              "EOS",
                              "AAVE",
                              "FTM",
                              "ETC",
                              "BCH",
                              "FLOW",
                              "CHZ"};
static const uint NB_PAIRS = 33;
static const int NB_THREADS = 4;
string timeframe = "5m";
vector<string> DATAFILES{};

const float start_year = 2017; // forced year to start (applies if data below is available)
const float FEE = 0.03f;       // FEES in %
const float USDT_amount_initial = 1000.0f;
const uint MIN_NUMBER_OF_TRADES = 1000;         // minimum number of trades required (to avoid some noise / lucky circunstances)
const float MIN_ALLOWED_MAX_DRAWBACK = -50.0f; // %
const float STOCH_RSI_UPPER = 0.800f;
const float STOCH_RSI_LOWER = 0.200f;
uint start_indexes[NB_PAIRS];

// RANGE OF PARAMETERS TO TEST
// const int range_step = 2;
// vector<int> range_EMA = {180};
vector<int> range_EMA1 = integer_range(2, 62, 2);
vector<int> range_EMA2 = integer_range(2, 200, 5);
vector<int> range_EMA3 = integer_range(25, 350, 10);
vector<float> range_UP{3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0};
vector<float> range_DOWN{3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0};
//////////////////////////
array<std::unordered_map<string, vector<float>>, NB_PAIRS> INDICATORS{};

array<long int, NB_PAIRS> last_times{};

uint i_print = 0;
uint nb_tested = 0;

RUN_RESULTf best{};

uint end_timestamp_datasets = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void fill_datafile_paths()
{
    for (uint i = 0; i < COINS.size(); i++)
    {
        DATAFILES.push_back("./data/Binance/" + timeframe + "/" + COINS[i] + "-USDT.csv");
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool read_best(const RUN_RESULTf &bestt)
{

    int ema1, ema2, ema3, max_open_trades;
    float calmar_ratio_monthly, up, down;

    std::ifstream in("best_result.txt", std::ios::in); // input

    if (!in)
    {
        std::cout << "Cannot open test.txt file." << std::endl;
        ;
        return false;
    }

    in >> calmar_ratio_monthly >> ema1 >> ema1 >> ema3 >> up >> down >> max_open_trades;

    std::cout << "Global best: " << calmar_ratio_monthly << std::endl;
    std::cout << "Local best : " << bestt.calmar_ratio_monthly << std::endl;
    std::cout << "(yearly : " << bestt.calmar_ratio << ")" << std::endl;

    in.close();

    if ((bestt.calmar_ratio_monthly - calmar_ratio_monthly) / bestt.calmar_ratio_monthly * 100.0 > 0.1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void write_best_to_file(const RUN_RESULTf &bestt)
{

    if (read_best(bestt))
    {
        ofstream out("best_result.txt", std::ios::trunc);
        if (out.is_open())
        {
            out << bestt.calmar_ratio_monthly
                << " " << bestt.ema1
                << " " << bestt.ema2
                << " " << bestt.ema3
                << " " << bestt.up
                << " " << bestt.down
                << " " << bestt.max_open_trades
                << std::endl;
            out.close();
        }
        else
        {
            cout << "Unable to open file";
        }
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void print_best_res(const RUN_RESULTf &bestt)
{
    std::cout << "\n-------------------------------------" << endl;
    std::cout << "BEST PARAMETER SET FOUND: " << endl;
    std::cout << "-------------------------------------" << endl;
    std::cout << "Strategy : " << STRAT_NAME << endl;
    std::cout << "EMAs : " << bestt.ema1 << " - " << bestt.ema2 << " - " << bestt.ema3 << endl;
    std::cout << "UP - DOWN : " << bestt.up << " - " << bestt.down << endl;
    std::cout << "Max Open Trades : " << bestt.max_open_trades << endl;
    std::cout << "Best Gain: " << bestt.gain_pc << "%" << endl;
    std::cout << "Porfolio : " << bestt.WALLET_VAL_USDT << "$ (started with 1000$)" << endl;
    std::cout << "Win rate : " << bestt.win_rate << "%" << endl;
    std::cout << "max DD   : " << bestt.max_DD << "%" << endl;
    std::cout << "Gain/DDC : " << bestt.gain_over_DDC << endl;
    std::cout << "Score    : " << bestt.score << endl;
    std::cout << "Calmar ratio monthly: " << bestt.calmar_ratio_monthly << endl;
    std::cout << "Calmar ratio : " << bestt.calmar_ratio << endl;
    std::cout << "Number of trades: " << bestt.nb_posi_entered << endl;
    std::cout << "Total fees paid: " << round(bestt.total_fees_paid * 100.0f) / 100.0f << "$ (started with 1000$)" << endl;
    std::cout << "-------------------------------------" << endl;
    write_best_to_file(bestt);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool key_exists(const std::unordered_map<string, vector<float>> &m, const string &ch)
{
    if (m.find(ch) != m.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RUN_RESULTf PROCESS(const vector<KLINEf> &PAIRS, const int ema1, const int ema2, int ema3, const float up, const float down, const uint MAX_OPEN_TRADES)
{
    nb_tested++;

    for (int ic = 0; ic < NB_PAIRS; ic++)
    {
        const string k1 = "EMA_" + std::to_string(ema1);
        const string k2 = "EMA_" + std::to_string(ema2);
        const string k3 = "EMA_" + std::to_string(ema3);
        if (!key_exists(INDICATORS[ic], k1))
        {
            INDICATORS[ic][k1] = TALIB_EMA(PAIRS[ic].close, ema1);
        }
        if (!key_exists(INDICATORS[ic], k2))
        {
            INDICATORS[ic][k2] = TALIB_EMA(PAIRS[ic].close, ema2);
        }
        if (!key_exists(INDICATORS[ic], k3))
        {
            INDICATORS[ic][k3] = TALIB_EMA(PAIRS[ic].close, ema3);
        }
        if (!key_exists(INDICATORS[ic], "StochRSI_K"))
        {
            INDICATORS[ic]["StochRSI_K"] = TALIB_STOCHRSI_K(PAIRS[ic].close, 14, 14, 3, 3);
        }
        if (!key_exists(INDICATORS[ic], "StochRSI_D"))
        {
            INDICATORS[ic]["StochRSI_D"] = TALIB_STOCHRSI_D(PAIRS[ic].close, 14, 14, 3, 3);
        }
        if (!key_exists(INDICATORS[ic], "ATR"))
        {
            INDICATORS[ic]["ATR"] = TALIB_ATR(PAIRS[ic].high, PAIRS[ic].low, PAIRS[ic].close, 14);
        }
    }

    RUN_RESULTf result{};

    vector<float> USDT_tracking{};
    vector<int> USDT_tracking_ts{};

    const uint nb_max = PAIRS[0].nb;

    bool LAST_ITERATION = false;
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
    array<float, NB_PAIRS> ATR_AT_OPEN{};
    array<uint, NB_PAIRS> OPEN_TS{};
    
    uint ACTIVE_POSITIONS = 0;

    const uint ii_begin = start_indexes[0];

    bool NEW_MONTH = false;

    for (uint ii = ii_begin + 1; ii < nb_max; ii++)
    {
        if (ii == nb_max - 1)
            LAST_ITERATION = true;

        const int month_b = get_month_from_timestamp(PAIRS[0].timestamp[ii-1]);
        const int month = get_month_from_timestamp(PAIRS[0].timestamp[ii]);

        if (month_b != month)
        {
            NEW_MONTH = true;
        }
        else
        {
            NEW_MONTH = false;
        }

        bool closed = false;
        // For all pairs, check to close / open positions
        for (uint ic = 0; ic < NB_PAIRS; ic++)
        {
            if (ii < start_indexes[ic])
                continue;

            // conditions for open / close position

            bool OPEN_LONG_CONDI = INDICATORS[ic]["EMA_" + std::to_string(ema1)][ii] >= INDICATORS[ic]["EMA_" + std::to_string(ema2)][ii] 
                                && INDICATORS[ic]["EMA_" + std::to_string(ema2)][ii] >= INDICATORS[ic]["EMA_" + std::to_string(ema3)][ii] 
                                && PAIRS[ic].close[ii] >= INDICATORS[ic]["EMA_" + std::to_string(ema1)][ii] 
                                && INDICATORS[ic]["StochRSI_K"][ii] < STOCH_RSI_LOWER && INDICATORS[ic]["StochRSI_D"][ii] < STOCH_RSI_LOWER
                                && INDICATORS[ic]["StochRSI_K"][ii-1] > INDICATORS[ic]["StochRSI_D"][ii-1] && INDICATORS[ic]["StochRSI_K"][ii] <= INDICATORS[ic]["StochRSI_D"][ii];
            
            bool timeout = false;
            bool hard_TP_condition = false;
            if (COIN_AMOUNTS[ic] != 0.0f)
            {
                timeout = (PAIRS[ic].timestamp[ii] - OPEN_TS[ic]) >= 2 * 24 * 3600;
                const float pc_gain = (PAIRS[ic].close[ii] - price_position_open[ic]) / price_position_open[ic] * 100.0f;
                hard_TP_condition = pc_gain > 15.0f;
            }
            else
            {
                timeout = false;
            }

            bool CLOSE_LONG_CONDI = PAIRS[ic].close[ii] > price_position_open[ic] + up*ATR_AT_OPEN[ic] 
                                || PAIRS[ic].close[ii] < price_position_open[ic] - down*ATR_AT_OPEN[ic] 
                                || timeout || hard_TP_condition;

            // IT IS IMPORTANT TO CHECK FIRST FOR CLOSING POSITION AND ONLY THEN FOR OPENING POSITION

            // CLOSE LONG
            if (COIN_AMOUNTS[ic] > 0.0f && (CLOSE_LONG_CONDI || LAST_ITERATION))
            {
                const float to_add = COIN_AMOUNTS[ic] * PAIRS[ic].close[ii];
                USDT_amount += to_add;
                COIN_AMOUNTS[ic] = 0.0f;
                ATR_AT_OPEN[ic] = 0.0f;
                OPEN_TS[ic] = -10;

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
                ATR_AT_OPEN[ic] = INDICATORS[ic]["ATR"][ii];
                OPEN_TS[ic] = PAIRS[ic].timestamp[ii];

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
        if (closed || LAST_ITERATION || NEW_MONTH)
        {
            array<float, NB_PAIRS> closes{};
            for (uint ic = 0; ic < NB_PAIRS; ic++) 
            {
                closes[ic] = PAIRS[ic].close[ii];
            }

            WALLET_VAL_USDT = USDT_amount + vector_product(COIN_AMOUNTS, closes);
            if (WALLET_VAL_USDT > MAX_WALLET_VAL_USDT) 
            {
                MAX_WALLET_VAL_USDT = WALLET_VAL_USDT;
            }

            pc_change_with_max = (WALLET_VAL_USDT - MAX_WALLET_VAL_USDT) / MAX_WALLET_VAL_USDT * 100.0f;
            if (pc_change_with_max < max_drawdown)
            {
                max_drawdown = pc_change_with_max;
            }

            USDT_tracking.push_back(WALLET_VAL_USDT);
            USDT_tracking_ts.push_back(PAIRS[0].timestamp[ii]);
        }
    }

    array<float, NB_PAIRS> last_closes{};
    for (uint ic = 0; ic < NB_PAIRS; ic++)
    {
        last_closes[ic] = PAIRS[ic].close[nb_max - 1];
    }

    WALLET_VAL_USDT = USDT_amount + vector_product(COIN_AMOUNTS, last_closes);

    const float gain = (WALLET_VAL_USDT - USDT_amount_initial) / USDT_amount_initial * 100.0f;
    const float WR = float(nb_profit) / float(NB_POSI_ENTERED) * 100.0f;
    const float DDC = (1.0f / (1.0f + max_drawdown / 100.0f) - 1.0f) * 100.0f;
    const float score = gain / DDC * WR;

    i_print++;
    if (i_print == 10)
    {
        i_print = 0;
        std::cout << "DONE: EMAs : " << ema1 << " - " << ema2 << " - " << ema3 << endl;
        const int nb_total = range_EMA2.size() * range_EMA1.size() * MAX_OPEN_TRADES_TO_TEST.size() * range_EMA3.size() * range_UP.size() * range_DOWN.size();
        std::cout << "NB tested = " << nb_tested << "/" << nb_total << endl;
        std::cout << "Done " << std::round(float(nb_tested) / float(nb_total) * 100.0f * 100.0f) / 100.0f << " %" << endl;
        print_best_res(best);
    }

    if (gain > 0.0)
    {
        result.WALLET_VAL_USDT = USDT_amount;
        result.gain_over_DDC = gain / DDC;
        result.gain_pc = gain;
        result.max_DD = max_drawdown;
        result.nb_posi_entered = NB_POSI_ENTERED;
        result.win_rate = WR;
        result.score = score;
        result.calmar_ratio_monthly = calculate_calmar_ratio_monthly(USDT_tracking_ts, USDT_tracking, DDC);
        result.calmar_ratio = calculate_calmar_ratio(USDT_tracking_ts, USDT_tracking, DDC);
        result.ema1 = ema1;
        result.ema2 = ema2;
        result.ema3 = ema3;
        result.up = up;
        result.down = down;
        result.total_fees_paid = total_fees_paid_USDT;
        result.max_open_trades = MAX_OPEN_TRADES;
    }
    else
    {
        result.WALLET_VAL_USDT = 0.0;
        result.gain_over_DDC = 0.0;
        result.gain_pc = 0.0;
        result.max_DD = -100;
        result.nb_posi_entered = 0;
        result.win_rate = 0;
        result.score = 0;
        result.calmar_ratio_monthly = 0;
        result.calmar_ratio = 0;
        result.ema1 = ema1;
        result.ema2 = ema2;
        result.ema3 = ema3;
        result.up = up;
        result.down = down;
        result.total_fees_paid = 0;
        result.max_open_trades = MAX_OPEN_TRADES;
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int super_index = 0;
long int previous_ts = 0;
int nb_read = 0;
KLINEf read_input_data(const string &input_file_path)
{
    KLINEf kline;

    ifstream myfile(input_file_path);
    string line;
    long int ts;
    float op, hi, lo, cl, vol;
    previous_ts = 0;
    ifstream file(input_file_path);
    string value;
    vector<string> getInform;
    nb_read = 0;
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
        if (previous_ts == ts)
        {
            std::cout << "FOUND DUPLICATE TS at index " << nb_read << "; IGNORING. " << std::endl;
            previous_ts = ts / 1000;
            continue;
        }
        previous_ts = ts;
        kline.open.push_back(op);
        kline.high.push_back(hi);
        kline.low.push_back(lo);
        kline.close.push_back(cl);
        nb_read++;
    }

    last_times[super_index] = kline.timestamp.back();

    if (super_index >= 1)
    {
        while (last_times[super_index] != last_times[super_index - 1])
        {
            cout << "warning: inconsistent last times between different pairs, fixing it." << endl;

            kline.timestamp.pop_back();
            kline.open.pop_back();
            kline.high.pop_back();
            kline.low.pop_back();
            kline.close.pop_back();

            last_times[super_index] = kline.timestamp.back();
        }
    }

    super_index++;

    myfile.close();

    kline.nb = int(kline.close.size());

    std::cout << "Loaded data file. " << input_file_path << endl;

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
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void INITIALIZE_DATA(vector<KLINEf> &PAIRS)
// will modify PAIRS since it is passed as reference
{
    std::cout << "Running INITIALIZE_DATA..." << endl;

    start_indexes[0] = 400;

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

    std::cout << "Checking consistency of sizes of datasets..." << std::endl;
    for (uint ic = 1; ic < NB_PAIRS; ic++)
    {
        if (PAIRS[ic].nb != PAIRS[0].nb || PAIRS[ic].open.size() != PAIRS[0].open.size())
        {
            std::cout << "ERROR: incosistent size" << endl;
            abort();
        }
    }
    std::cout << "Done." << std::endl;

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    const double t_begin = get_wall_time();
    std::cout << "\n-------------------------------------" << endl;
    std::cout << "Strategy to test: " << STRAT_NAME << endl;
    std::cout << "DATA FILES TO PROCESS: " << endl;

    fill_datafile_paths();

    for (const string &dataf : DATAFILES)
    {
        std::cout << "  " << dataf << endl;
    }

    TA_RetCode retCode;
    retCode = TA_Initialize();
    if (retCode != TA_SUCCESS)
    {
        std::cout << "Cannot initialize TA-Lib !\n" << retCode << "\n";
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

    INITIALIZE_DATA(PAIRS); // this function modifies PAIRS

    best.gain_over_DDC = -100.0f;
    best.calmar_ratio = -100.0f;
    best.calmar_ratio_monthly = -100.0f;

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
    std::cout << "EMA1 max tested : " << find_max(range_EMA1) << endl;
    std::cout << "EMA2 max tested : " << find_max(range_EMA2) << endl;
    std::cout << "EMA3 max tested : " << find_max(range_EMA3) << endl;
    std::cout << "StochRSI Upper Band   : " << STOCH_RSI_UPPER << endl;
    std::cout << "StochRSI Lower Band   : " << STOCH_RSI_LOWER << endl;
    std::cout << "-------------------------------------" << endl;

    // MAIN LOOP
    std::vector<EMA3_params> param_list{};
    param_list.reserve(range_EMA1.size() * range_EMA2.size() * range_EMA3.size() * range_UP.size() * range_DOWN.size() * MAX_OPEN_TRADES_TO_TEST.size());

    for (const uint max_op_tr : MAX_OPEN_TRADES_TO_TEST)
    {
        for (const int ema1 : range_EMA1)
        {
            for (const int ema2 : range_EMA2)
            {
                for (const int ema3 : range_EMA3)
                {
                    for (const float up : range_UP)
                    {
                        for (const float down : range_DOWN)
                        {
                            if (ema1>=ema2) continue;
                            if (ema2>=ema3) continue;
                            const EMA3_params to_add{ema1, ema2, ema3, up, down, max_op_tr};
                            param_list.push_back(to_add);
                        }
                    }
                }
            }
        }
    }

    std::cout << "Saved parameter list to test." << std::endl;
    std::cout << "Running all backtests..." << std::endl;

    random_shuffle_vector_params(param_list);

    // std::vector<std::vector<EMA3_params>> SplitedVector = SplitVector(param_list, NB_THREADS);

    for (const auto para : param_list)
    {
        const RUN_RESULTf res = PROCESS(PAIRS, para.ema1, para.ema2, para.ema3, para.up, para.down, para.max_open_trades);

        if (res.calmar_ratio_monthly > best.calmar_ratio_monthly && res.gain_pc > 800.0f && res.gain_pc < 1000000.0f && res.nb_posi_entered >= MIN_NUMBER_OF_TRADES && res.max_DD > MIN_ALLOWED_MAX_DRAWBACK)
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
