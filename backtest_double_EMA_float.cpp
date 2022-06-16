#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <math.h>
#include <unordered_map>
#include "tools.hh"
#include "custom_talib_wrapper.hh"
#include <ta-lib/ta_libc.h>

using namespace std;

const float FRACTION_PER_POSI = 1.0; // FRACTION OF CAPITAL PER POSITION
const float LEV = 1.0;               // LEVERAGE
const float FEE = 0.02;              // FEES
const float FUNDING_FEE = 0.01;      // FUNDING FEE APPLIED EVERY 8 hours
const bool CAN_LONG = true;          // LONG ON OR OFF
const bool CAN_SHORT = true;         // SHORT ON OR OFF
const std::string DATAFILE = "./data/BTCUSDT_1HOUR.txt";

// RANGE OF EMA PERIDOS TO TEST
std::vector<int> range1 = integer_range(2, 200);
std::vector<int> range2 = integer_range(2, 200);
//////////////////////////

uint i_print = 0;

std::unordered_map<std::string, std::vector<float>> EMA_LISTS{};
std::vector<float> year{};
std::vector<float> hour{};
std::vector<float> month{};
std::vector<float> day{};

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
    std::cout << "EMA: " << best.ema1 << " " << best.ema2 << std::endl;
    std::cout << "Best Gain: " << best.gain_pc << "%" << std::endl;
    std::cout << "Win rate: " << best.win_rate << "%" << std::endl;
    std::cout << "max DD: " << best.max_DD << "%" << std::endl;
    std::cout << "Gain over DDC: " << best.gain_over_DDC << std::endl;
    std::cout << "Score: " << best.score << std::endl;
    std::cout << "Number of trades: " << best.nb_posi_entered << std::endl;
    std::cout << "-------------------------------------\n" << std::endl;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void INITIALIZE_DATA(const KLINEf &kline)
{
    std::vector<int> list_ema = {};

    for (int i = 1; i <= 205; i++)
    {
        list_ema.push_back(i);
    }

    for (const int i : list_ema)
    {
        EMA_LISTS["EMA" + std::to_string(i)] = EMA(kline, i);
    }

    for (uint ii = 0; ii < kline.nb; ii++)
    {
        year.push_back(get_year_from_timestamp(kline.d_time[ii]));
        hour.push_back(get_hour_from_timestamp(kline.d_time[ii]));
        month.push_back(get_month_from_timestamp(kline.d_time[ii]));
        day.push_back(get_day_from_timestamp(kline.d_time[ii]));
    }

    cout << "Initialized calculations." << endl;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RUN_RESULTf PROCESS(const KLINEf &KLINEf, const int ema1_v, const int ema2_v)
{
    std::vector<float> EMA1 = EMA_LISTS["EMA" + std::to_string(ema1_v)];
    std::vector<float> EMA2 = EMA_LISTS["EMA" + std::to_string(ema2_v)];

    bool LAST_ITERATION = false, OPEN_LONG_CONDI = false, OPEN_SHORT_CONDI = false, CLOSE_LONG_CONDI = false, CLOSE_SHORT_CONDI = false;
    bool IN_POSITION = false, IN_LONG = false, IN_SHORT = false;
    int nb_profit = 0, nb_loss = 0, NB_POSI_ENTERED = 0;
    int nb_max = KLINEf.nb;
    float current_price = 0, starting_price = 0, pc_change = 0, pc_change_with_max = 0, max_drawdown = 0, price_position_open = 0;
    float USDT_amount = 1000.0;
    float MAX_USDT_AMOUNT = USDT_amount;
    float amount_b = 0;

    std::vector<float> close = KLINEf.d_close;

    int ii_begin = find_max(range1) + 2;

    for (size_t ii = ii_begin; ii < KLINEf.nb; ii++)
    {
        if (ii == nb_max - 1)
        {
            LAST_ITERATION = true;
        }

        current_price = close[ii];

        if (ii == ii_begin)
        {
            starting_price = close[ii];
        }

        pc_change = (USDT_amount - 1000.0) / 1000.0 * 100.0;

        if (USDT_amount > MAX_USDT_AMOUNT)
        {
            MAX_USDT_AMOUNT = USDT_amount;
        }

        pc_change_with_max = (USDT_amount - MAX_USDT_AMOUNT) / MAX_USDT_AMOUNT * 100.0;

        if (pc_change_with_max < max_drawdown)
        {
            max_drawdown = pc_change_with_max;
        }

        double USDT_amount_check = 100.0;

        if (IN_SHORT && IN_POSITION)
        {
            USDT_amount_check = USDT_amount - (current_price - price_position_open) / price_position_open * FRACTION_PER_POSI * USDT_amount * LEV;
        }
        if (IN_LONG && IN_POSITION)
        {
            USDT_amount_check = USDT_amount + (current_price - price_position_open) / price_position_open * FRACTION_PER_POSI * USDT_amount * LEV;
        }

        if (USDT_amount_check <= 0)
        {
            RUN_RESULTf result;

            result.AMOUNT_USDT = 0;
            result.gain_over_DDC = 0;
            result.gain_pc = 0;
            result.max_DD = -100.0;
            result.nb_posi_entered = NB_POSI_ENTERED;
            result.win_rate = 0;
            result.score = 0;
            result.ema1 = ema1_v;
            result.ema2 = ema2_v;

            return result;
        }

        // Funding fees

        if (IN_POSITION)
        {
            if (((hour[ii] >= 2) && (hour[ii - 1] < 2)) || ((hour[ii] >= 10) && (hour[ii - 1] < 10)) || ((hour[ii] >= 18) && (hour[ii - 1] < 18)))
            {
                float to_rm = current_price / price_position_open * FRACTION_PER_POSI * USDT_amount * LEV * FUNDING_FEE / 100.0;
                USDT_amount = USDT_amount - to_rm;
            }
        }

        // check if should go in position

        OPEN_LONG_CONDI = (EMA2[ii] >= EMA1[ii]) && (EMA2[ii - 1] <= EMA1[ii - 1]);
        OPEN_SHORT_CONDI = (EMA2[ii] <= EMA1[ii]) && (EMA2[ii - 1] >= EMA1[ii - 1]);

        CLOSE_LONG_CONDI = (EMA2[ii] <= EMA1[ii]) && (EMA2[ii - 1] >= EMA1[ii - 1]);
        CLOSE_SHORT_CONDI = (EMA2[ii] >= EMA1[ii]) && (EMA2[ii - 1] <= EMA1[ii - 1]);

        // IT IS IMPORTANT TO CHECK FIRST FOR CLOSING POSITION AND THEN FOR OPENING POSITION

        // CLOSE SHORT
        if ((IN_POSITION && IN_SHORT) && (CLOSE_SHORT_CONDI || LAST_ITERATION))
        {
            amount_b = USDT_amount;

            USDT_amount = USDT_amount - (current_price - price_position_open) / price_position_open * FRACTION_PER_POSI * USDT_amount * LEV;

            // apply FEEs
            if (FEE > 0)
            {
                USDT_amount = USDT_amount - current_price / price_position_open * FRACTION_PER_POSI * amount_b * LEV * FEE / 100.0;
            }
            else
            {
                USDT_amount = USDT_amount - current_price / price_position_open * FRACTION_PER_POSI * amount_b * FEE / 100.0;
            }
            //

            IN_POSITION = false;
            IN_SHORT = false;
            IN_LONG = false;

            pc_change = -(current_price - price_position_open) / price_position_open * 100.0;

            if (current_price < price_position_open)
            {
                nb_profit = nb_profit + 1;
            }
            else
            {
                nb_loss = nb_loss + 1;
            }
        }
        // CLOSE LONG
        if ((IN_POSITION && IN_LONG) && (CLOSE_LONG_CONDI || LAST_ITERATION))
        {
            amount_b = USDT_amount;

            USDT_amount = USDT_amount + (current_price - price_position_open) / price_position_open * FRACTION_PER_POSI * USDT_amount * LEV;

            // apply FEEs
            if (FEE > 0)
            {
                USDT_amount = USDT_amount - current_price / price_position_open * FRACTION_PER_POSI * amount_b * LEV * FEE / 100.0;
            }
            else
            {
                USDT_amount = USDT_amount - current_price / price_position_open * FRACTION_PER_POSI * amount_b * FEE / 100.0;
            }
            //

            IN_POSITION = false;
            IN_SHORT = false;
            IN_LONG = false;

            pc_change = (current_price - price_position_open) / price_position_open * 100.0;

            if (current_price > price_position_open)
            {
                nb_profit = nb_profit + 1;
            }
            else
            {
                nb_loss = nb_loss + 1;
            }
        }

        // Check to open position (should always be after check of closing)

        // OPEN SHORT
        if ((IN_POSITION == false) && (OPEN_SHORT_CONDI && CAN_SHORT))
        {

            price_position_open = current_price;

            // apply FEEs
            if (FEE > 0.0)
            {
                USDT_amount = USDT_amount - FRACTION_PER_POSI * USDT_amount * LEV * FEE / 100.0;
            }
            else
            {
                USDT_amount = USDT_amount - FRACTION_PER_POSI * USDT_amount * FEE / 100.0;
            }

            IN_POSITION = true;
            IN_LONG = false;
            IN_SHORT = true;

            NB_POSI_ENTERED = NB_POSI_ENTERED + 1;
        }
        // OPEN LONG
        if ((IN_POSITION == false) && (OPEN_LONG_CONDI && CAN_LONG))
        {

            price_position_open = current_price;

            // apply FEEs
            if (FEE > 0.0)
            {
                USDT_amount = USDT_amount - FRACTION_PER_POSI * USDT_amount * LEV * FEE / 100.0;
            }
            else
            {
                USDT_amount = USDT_amount - FRACTION_PER_POSI * USDT_amount * FEE / 100.0;
            }

            IN_POSITION = true;
            IN_LONG = true;
            IN_SHORT = false;

            NB_POSI_ENTERED = NB_POSI_ENTERED + 1;
        }
    }

    float gain = (USDT_amount - 1000.0) / 1000.0 * 100.0;
    float WR = float(nb_profit) / float(NB_POSI_ENTERED) * 100.0;

    float DDC = (1.0 / (1.0 + max_drawdown / 100.0) - 1.0) * 100.0;

    float score = gain / DDC * WR;

    i_print++;

    if (i_print == 1000)
    {
        i_print = 0;
        std::cout << "DONE: EMA: " << ema1_v << " and EMA: " << ema2_v << endl;
    }

    RUN_RESULTf result;

    result.AMOUNT_USDT = USDT_amount;
    result.gain_over_DDC = gain / DDC;
    result.gain_pc = gain;
    result.max_DD = max_drawdown;
    result.nb_posi_entered = NB_POSI_ENTERED;
    result.win_rate = WR;
    result.score = score;
    result.ema1 = ema1_v;
    result.ema2 = ema2_v;

    return result;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

KLINEf read_input_data(const std::string &input_file_path)
{
    KLINEf kline;

    ifstream myfile(input_file_path);
    std::string line;
    int ts;
    float op, hi, lo, cl;
    int ho;

    if (myfile.is_open())
    {
        while (getline(myfile, line))
        {
            myfile >> ts >> op >> hi >> lo >> cl;
            kline.d_time.push_back(ts);
            kline.d_open.push_back(op);
            kline.d_high.push_back(hi);
            kline.d_low.push_back(lo);
            kline.d_close.push_back(cl);
        }
        myfile.close();

        kline.nb = int(kline.d_close.size());
    }
    return kline;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    std::cout << "\n-------------------------------------" << std::endl;
    std::cout << "Strategy to test: EMA crossover simple" << std::endl;
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

    KLINEf kline = read_input_data(DATAFILE);

    INITIALIZE_DATA(kline);

    RUN_RESULTf best{};
    best.gain_over_DDC = -100.0;

    int i_print2 = 0;

    std::cout << "Begining day :: " << year[0] << "/" << month[0] << "/"  << day[0] << std::endl;
    std::cout << "End day      :: " << year.back() << "/" << month.back() << "/" << day.back() << std::endl;
    std::cout << "CAN LONG = " << CAN_LONG << "; CAN SHORT = " << CAN_SHORT << std::endl;

    // MAIN LOOP

    for (int ema1 : range1)
    {
        for (int ema2 : range2)
        {
            if (ema1 == ema2)
                continue;

            RUN_RESULTf res = PROCESS(kline, ema1, ema2);

            if (res.gain_over_DDC > best.gain_over_DDC && res.gain_pc < 10000.0 && res.nb_posi_entered >= 50)
            {
                best = res;
            }
        }
    }

    print_best_res(best);
}