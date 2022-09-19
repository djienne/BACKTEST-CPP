#include <stdio.h>
#include <vector>
#include <array>
#include <time.h>
#include <chrono>
#include <limits>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <algorithm> // std::shuffle
#include <random>    // std::default_random_engine

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct RUN_RESULTf
{
    float WALLET_VAL_USDT;
    float gain_pc;
    float win_rate;
    float max_DD;
    float gain_over_DDC;
    float score;
    int nb_posi_entered;
    int ema1;
    int ema2;
    int ema3;
    int ema4;
    int trixLength;
    int trixSignal;
    float UP;
    float DOWN;
    float min_yearly_gain;
    float max_yearly_gain;
    std::vector<float> yearly_gains;
    std::vector<float> years_yearly_gains;
    float RSI_limit;
    float RSI_limit2;
    float gain_limit;
    float up;
    float down;
    float total_fees_paid;
    int max_delta_t_new_ATH;
    float calmar_ratio;
    uint max_open_trades;
};

struct trix_params
{
    int ema1;
    int trixLength;
    int trixSignal;
    uint max_open_trades;
};

struct BigWill_params
{
    int AO_fast;
    int AO_slow;
    int ema_f;
    int ema_s;
    uint max_open_trades;
};

struct EMA3_params
{
    int ema1;
    int ema2;
    int ema3;
    float up;
    float down;
    uint max_open_trades;
};

struct SR_params
{
    int ema_fast;
    int ema_slow;
    uint max_open_trades;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float vector_product(const std::vector<float> &vec, const std::vector<float> &vec2);
float vector_product(const std::array<float, 4> &vec, const std::vector<float> &vec2);
float vector_product(const std::vector<float> &vec2, const std::array<float, 4> &vec);
float vector_product(const std::array<float, 4> &vec2, const std::array<float, 4> &vec);
float vector_product(const std::array<float, 1> &vec2, const std::array<float, 1> &vec);
float vector_product(const std::array<float, 5> &vec2, const std::array<float, 5> &vec);
float vector_product(const std::array<float, 6> &vec2, const std::array<float, 6> &vec);
float vector_product(const std::array<float, 7> &vec2, const std::array<float, 7> &vec);
float vector_product(const std::array<float, 9> &vec2, const std::array<float, 9> &vec);
float vector_product(const std::array<float, 10> &vec2, const std::array<float, 10> &vec);
float vector_product(const std::array<float, 12> &vec2, const std::array<float, 12> &vec);
float vector_product(const std::array<float, 19> &vec2, const std::array<float, 19> &vec);
float vector_product(const std::array<float, 20> &vec2, const std::array<float, 20> &vec);
float vector_product(const std::array<float, 21> &vec2, const std::array<float, 21> &vec);
float vector_product(const std::array<float, 30> &vec2, const std::array<float, 30> &vec);
float vector_product(const std::array<float, 33> &vec2, const std::array<float, 33> &vec);
float vector_product(const std::array<float, 34> &vec2, const std::array<float, 34> &vec);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float find_average(const std::vector<float> &vec);
float find_min(const std::vector<float> &vec);
float find_max(const std::vector<float> &vec);
int find_max(const std::vector<int> &vec);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int get_hour_from_timestamp(const int timestamp);

int get_year_from_timestamp(const int timestamp);

int get_month_from_timestamp(const int timestamp);

int get_day_from_timestamp(const int timestamp);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

double get_wall_time();

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

double process_mem_usage();

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<int> integer_range(const int min, const int max, const int step);
std::vector<int> integer_range(const int min, const int max);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<float> float_range(const float min, const float max, const float step);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string ReplaceAll(std::string str, const std::string &from, const std::string &to);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float calculate_calmar_ratio(const std::vector<int> &times, const std::vector<float> &wallet_vals, const float &max_DD);
float calculate_calmar_ratio_monthly(const std::vector<int> &times, const std::vector<float> &wallet_vals, const float &max_DD);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void random_shuffle_vector(std::vector<float> &vec_in);

void random_shuffle_vector(std::vector<int> &vec_in);

void random_shuffle_vector_params(std::vector<trix_params> &vec_in);

void random_shuffle_vector_params(std::vector<BigWill_params> &vec_in);

void random_shuffle_vector_params(std::vector<SR_params> &vec_in);

void random_shuffle_vector_params(std::vector<EMA3_params> &vec_in);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<std::vector<EMA3_params>> SplitVector(const std::vector<EMA3_params>& vec, const int n);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
