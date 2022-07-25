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

static const size_t NB_PAIRS = 4;
const int NB_POSITION_MAX = 4;
const std::vector<std::string> COINS = {"BTC", "ETH", "BNB", "XRP"};

const std::vector<std::string> DATAFILES = {"./data/Binance/1h/BTC-USDT.csv",
                                            "./data/Binance/1h/ETH-USDT.csv",
                                            "./data/Binance/1h/BNB-USDT.csv",
                                            "./data/Binance/1h/XRP-USDT.csv"};
const std::string STRAT_NAME = "TRIX";

const float start_year = 2017; // forced year to start (applies if data below is available)
const float FEE = 0.07;        // FEES in %
const float USDT_amount_initial = 1000.0;
int MIN_NUMBER_OF_TRADES = 150;                // minimum number of trades required (to avoid some noise / lucky circunstances)
const float MIN_ALLOWED_MAX_DRAWBACK = -50.0; // %
const float MAX_ALLOWED_DAYS_BETWEEN_PORTFOLIO_ATH = 365;
const float STOCH_RSI_UPPER = 0.800;
const float STOCH_RSI_LOWER = 0.200;
const float minimum_yearly_gain_pc = -100.0; // pc
int i_start_year = 0;
long int start_indexes[NB_PAIRS];

// RANGE OF EMA PERIDOS TO TESTs
const int period_max_EMA = 400;
// const int range_step = 2;
//std::vector<int> range_EMA = {180};
std::vector<int> range_EMA = integer_range(40, period_max_EMA, 2);
std::vector<int> range_trixLength = integer_range(5, 50, 1);
std::vector<int> range_trixSignal = integer_range(10, 100, 1);
//////////////////////////

uint i_print = 0;

std::unordered_map<std::string, std::vector<float>> EMA_LISTS{};
std::unordered_map<std::string, std::vector<float>> TRIX_LISTS{};
std::unordered_map<std::string, std::vector<float>> StochRSI_LISTS{};
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
    std::cout << "EMA TRIX_L TRIX_S     : " << best.ema1 << " " << best.trixLength << " " << best.trixSignal << std::endl;
    std::cout << "Best Gain: " << best.gain_pc << "%" << std::endl;
    std::cout << "Porfolio : " << best.WALLET_VAL_USDT << "$ (started with 1000$)" << std::endl;
    std::cout << "Win rate : " << best.win_rate << "%" << std::endl;
    std::cout << "max DD   : " << best.max_DD << "%" << std::endl;
    std::cout << "Gain/DDC : " << best.gain_over_DDC << std::endl;
    std::cout << "Score    : " << best.score << std::endl;
    std::cout << "Number of trades: " << best.nb_posi_entered << std::endl;
    std::cout << "Yearly gains: " << std::endl;
    for (uint i = 0; i < best.yearly_gains.size(); i++)
    {
        std::cout << best.years_yearly_gains[i] << " :: " << best.yearly_gains[i] << "%" << std::endl;
    }
    std::cout << "Total fees paid: " << round(best.total_fees_paid * 100.0) / 100.0 << "$ (started with 1000$)" << std::endl;

    std::cout << "-------------------------------------" << std::endl;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<KLINEf> INITIALIZE_DATA(std::vector<KLINEf> PAIRS)
{
    start_indexes[0] = period_max_EMA + 2;
    // find initial indexes (different starting times)
    for (uint i = 0; i < PAIRS[0].d_time.size(); i++)
    {
        for (uint ic = 1; ic < NB_PAIRS; ic++)
        {
            if (PAIRS[ic].d_time[0] == PAIRS[0].d_time[i])
            {
                start_indexes[ic] = i;
                std::cout << "Start for " + COINS[ic] << " : " << i << std::endl;
            }
        }
    }

    for (uint ic = 1; ic < NB_PAIRS; ic++)
        { 
            while(PAIRS[ic].d_close.size()<PAIRS[0].d_close.size())
                {
                    PAIRS[ic].d_time.insert(PAIRS[ic].d_time.begin(), 0);
                    PAIRS[ic].d_open.insert(PAIRS[ic].d_open.begin(), 0.0f);
                    PAIRS[ic].d_high.insert(PAIRS[ic].d_high.begin(), 0.0f);
                    PAIRS[ic].d_low.insert(PAIRS[ic].d_low.begin(), 0.0f);
                    PAIRS[ic].d_close.insert(PAIRS[ic].d_close.begin(), 0.0f);
                    PAIRS[ic].nb++;
                }

                for (uint ii = 0; ii < start_indexes[ic]+5; ii++)
                {
                    PAIRS[ic].d_time[ii]=PAIRS[0].d_time[ii];
                }
        }

    //
    for (uint ic = 0; ic < COINS.size(); ic++)
    {
        std::cout << COINS[ic] << std::endl;

        // StochRSI = TALIB_STOCHRSI_K(kline.d_close,14,3,3);
        StochRSI_LISTS[COINS[ic]] = TALIB_STOCHRSI_not_averaged(PAIRS[ic].d_close, 14, 14);
        cout << "Calculated STOCHRSI." << endl;

        for (const uint i : range_EMA)
        {
            EMA_LISTS[COINS[ic] + "_EMA" + std::to_string(i)] = TALIB_EMA(PAIRS[ic].d_close, i);
        }
        cout << "Calculated EMAs." << endl;
    }

    cout << "Initialized calculations." << endl;

    return PAIRS;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RUN_RESULTf PROCESS(const std::vector<KLINEf> &PAIRS, const int ema_v, const int trixLength_v, const int trixSignal_v)
{
    nb_tested++;

    RUN_RESULTf result{};

    std::array<std::vector<float>, NB_PAIRS> TRIX_HISTO{};

    for (uint ip = 0; ip < NB_PAIRS; ip++)
    {
        TRIX_HISTO[ip] = TALIB_TRIX(PAIRS[ip].d_close, trixLength_v, trixSignal_v);
    }

    const int nb_max = PAIRS[0].nb;

    bool LAST_ITERATION = false;
    bool OPEN_LONG_CONDI = false;
    bool CLOSE_LONG_CONDI = false;
    int nb_profit = 0, nb_loss = 0, NB_POSI_ENTERED = 0;
    float pc_change_with_max = 0, max_drawdown = 0;
    std::array<float, 4> price_position_open;
    float USDT_amount = USDT_amount_initial;
    float WALLET_VAL_begin_year = USDT_amount_initial;
    float MAX_WALLET_VAL_USDT = USDT_amount_initial;
    float total_fees_paid_USDT = 0.0;
    float WALLET_VAL_USDT = USDT_amount_initial;

    std::vector<float> COIN_AMOUNTS = {0.0, 0.0, 0.0, 0.0};
    int ACTIVE_POSITIONS = 0;

    int ii_begin = start_indexes[0];

    for (uint ii = ii_begin; ii < nb_max; ii++)
    {
        if (ii == nb_max - 1) LAST_ITERATION = true;

        for (uint ic = 0; ic < NB_PAIRS; ic++)
        {
            if (ii<start_indexes[ic]) continue;

            // condition for open / close position

            OPEN_LONG_CONDI = PAIRS[ic].d_close[ii] > EMA_LISTS[COINS[ic] + "_EMA" + std::to_string(ema_v)][ii] && TRIX_HISTO[ic][ii] > 0.0 && StochRSI_LISTS[COINS[ic]][ii] < STOCH_RSI_UPPER;
            CLOSE_LONG_CONDI = TRIX_HISTO[ic][ii] < 0.0 && StochRSI_LISTS[COINS[ic]][ii] > STOCH_RSI_LOWER;

            // IT IS IMPORTANT TO CHECK FIRST FOR CLOSING POSITION AND THEN FOR OPENING POSITION

            // CLOSE LONG
            if (COIN_AMOUNTS[ic] > 0.0 && (CLOSE_LONG_CONDI || LAST_ITERATION))
            {
                USDT_amount += COIN_AMOUNTS[ic] * PAIRS[ic].d_close[ii];
                COIN_AMOUNTS[ic] = 0.0;

                ACTIVE_POSITIONS -= 1;

                // apply FEEs
                const float fe = COIN_AMOUNTS[ic] * PAIRS[ic].d_close[ii] * FEE / 100.0;
                USDT_amount -= fe;
                total_fees_paid_USDT += fe;
                //
                if (PAIRS[ic].d_close[ii] >= price_position_open[ic])
                {
                    nb_profit++;
                }
                else
                {
                    nb_loss++;
                }
            }

            // OPEN LONG
            if (COIN_AMOUNTS[ic] == 0.0 && OPEN_LONG_CONDI && LAST_ITERATION == false && ACTIVE_POSITIONS<NB_POSITION_MAX)
            {
                price_position_open[ic] = PAIRS[ic].d_close[ii];

                const float usdMultiplier = 1.0/(NB_POSITION_MAX-ACTIVE_POSITIONS);

                COIN_AMOUNTS[ic] = USDT_amount * usdMultiplier / PAIRS[ic].d_close[ii];
                USDT_amount -= USDT_amount * usdMultiplier;

                // apply FEEs
                const float fe = COIN_AMOUNTS[ic] * FEE / 100.0;
                COIN_AMOUNTS[ic] -= fe;
                total_fees_paid_USDT += fe * PAIRS[ic].d_close[ii];
                //

                ACTIVE_POSITIONS += 1;

                NB_POSI_ENTERED++;
            }
        }

        // check wallet status
        if (CLOSE_LONG_CONDI || LAST_ITERATION)
        {
            std::vector<float> closes{};
            closes.reserve(NB_PAIRS);
            for (size_t ip = 0; ip < NB_PAIRS; ip++) closes.push_back(PAIRS[ip].d_close[ii]);
            WALLET_VAL_USDT = USDT_amount + vector_product(COIN_AMOUNTS, closes);
            if (WALLET_VAL_USDT > MAX_WALLET_VAL_USDT) MAX_WALLET_VAL_USDT = WALLET_VAL_USDT;
            pc_change_with_max = (WALLET_VAL_USDT - MAX_WALLET_VAL_USDT) / MAX_WALLET_VAL_USDT * 100.0;
            if (pc_change_with_max < max_drawdown) max_drawdown = pc_change_with_max;
        }
    }

    std::vector<float> last_closes{};
    last_closes.reserve(NB_PAIRS);
    for (uint ip = 0; ip < NB_PAIRS; ip++) last_closes.push_back(PAIRS[ip].d_close[nb_max-1]);
    WALLET_VAL_USDT = USDT_amount + vector_product(COIN_AMOUNTS, last_closes);

    const float gain = (WALLET_VAL_USDT - USDT_amount_initial) / USDT_amount_initial * 100.0;
    const float WR = float(nb_profit) / float(NB_POSI_ENTERED) * 100.0;
    const float DDC = (1.0 / (1.0 + max_drawdown / 100.0) - 1.0) * 100.0;
    const float score = gain / DDC * WR;

    i_print++;
    if (i_print == 100)
    {
        i_print = 0;
        std::cout << "DONE: EMA: " << ema_v << " and trixLength: " << trixLength_v << " and trixSignal: " << trixSignal_v << endl;
    }

    result.WALLET_VAL_USDT = USDT_amount;
    result.gain_over_DDC = gain / DDC;
    result.gain_pc = gain;
    result.max_DD = max_drawdown;
    result.nb_posi_entered = NB_POSI_ENTERED;
    result.win_rate = WR;
    result.score = score;
    result.ema1 = ema_v;
    result.trixLength = trixLength_v;
    result.trixSignal = trixSignal_v;
    result.total_fees_paid = total_fees_paid_USDT;

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
    }

    myfile.close();

    kline.nb = int(kline.d_close.size());

    std::cout << "Loaded data file." << std::endl;
    return kline;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    const double t_begin = get_wall_time();
    std::cout << "\n-------------------------------------" << std::endl;
    std::cout << "Strategy to test: " << STRAT_NAME << std::endl;
    std::cout << "DATA FILES TO PROCESS: " << std::endl;
    for (const std::string &dataf : DATAFILES)
    {
        std::cout << dataf << std::endl;
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

    std::vector<KLINEf> PAIRS;

    for (const std::string &dataf : DATAFILES)
    {
        PAIRS.push_back(read_input_data(dataf));
    }

    PAIRS = INITIALIZE_DATA(PAIRS);

    RUN_RESULTf best{};
    best.gain_over_DDC = -100.0;

    int i_print2 = 0;

    const int last_idx = PAIRS[0].nb - 1;

    int year = get_year_from_timestamp(PAIRS[0].d_time[0]);
    int hour = get_hour_from_timestamp(PAIRS[0].d_time[0]);
    int month = get_month_from_timestamp(PAIRS[0].d_time[0]);
    int day = get_day_from_timestamp(PAIRS[0].d_time[0]);

    int last_year = get_year_from_timestamp(PAIRS[0].d_time[last_idx]);
    int last_hour = get_hour_from_timestamp(PAIRS[0].d_time[last_idx]);
    int last_month = get_month_from_timestamp(PAIRS[0].d_time[last_idx]);
    int last_day = get_day_from_timestamp(PAIRS[0].d_time[last_idx]);

    // Display info
    std::cout << "Begin day      : " << year << "/" << month << "/" << day << std::endl;
    std::cout << "End day        : " << last_year << "/" << last_month << "/" << last_day << std::endl;
    std::cout << "OPEN/CLOSE FEE : " << FEE << " %" << std::endl;
    std::cout << "Minimum number of trades required    : " << MIN_NUMBER_OF_TRADES << std::endl;
    std::cout << "Maximum drawback (=drawdown) allowed : " << MIN_ALLOWED_MAX_DRAWBACK << " %" << std::endl;
    std::cout << "StochRSI Upper Band   : " << STOCH_RSI_UPPER << std::endl;
    std::cout << "StochRSI Lower Band   : " << STOCH_RSI_LOWER << std::endl;
    std::cout << "EMA period max tested : " << find_max(range_EMA) << std::endl;
    std::cout << "trixLength max tested : " << find_max(range_trixLength) << std::endl;
    std::cout << "trixSignal max tested : " << find_max(range_trixSignal) << std::endl;
    std::cout << "-------------------------------------" << std::endl;

    // MAIN LOOP

    for (int ema : range_EMA)
    {
        for (int trixL : range_trixLength)
        {
            for (int trixS : range_trixSignal)
            {

                RUN_RESULTf res = PROCESS(PAIRS, ema, trixL, trixS);

                if (res.gain_over_DDC > best.gain_over_DDC && res.gain_pc < 100000.0 && res.nb_posi_entered >= MIN_NUMBER_OF_TRADES && res.max_DD > MIN_ALLOWED_MAX_DRAWBACK && find_min(res.yearly_gains) > minimum_yearly_gain_pc)
                {
                    best = res;
                }
            }
        }
    }

    print_best_res(best);
    const double t_end = get_wall_time();

    std::cout << "Number of backtests performed : " << nb_tested << std::endl;
    std::cout << "Time taken                    : " << t_end - t_begin << " seconds " << std::endl;
    const double ram_usage = process_mem_usage();
    std::cout << "RAM usage                     : " << std::round(ram_usage * 10.0) / 10.0 << " MB" << std::endl;
    std::cout << "-------------------------------------" << std::endl;

    TA_Shutdown();
}
