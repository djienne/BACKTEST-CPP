#include "tools.hh"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float find_average(const std::vector<float> &vec)
{
    float summ = 0.0;
    for (const float &val : vec)
    {
        summ += val;
    }
    return summ / float(vec.size());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float find_min(const std::vector<float> &vec)
{
    float min_val = std::numeric_limits<float>::max();

    for (const float val : vec)
    {
        if (val < min_val)
        {
            min_val = val;
        }
    }
    return min_val;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float vector_product(const std::vector<float> &vec, const std::vector<float> &vec2)
{
    float out = 0.0;
    for (uint i = 0; i < vec.size(); i++)
    {
        out += vec[i] * vec2[i];
    }
    return out;
}

float vector_product(const std::array<float, 4> &vec, const std::vector<float> &vec2)
{
    float out = 0.0;
    for (uint i = 0; i < vec2.size(); i++)
    {
        out += vec[i] * vec2[i];
    }
    return out;
}

float vector_product(const std::vector<float> &vec2, const std::array<float, 4> &vec)
{
    float out = 0.0;
    for (uint i = 0; i < vec2.size(); i++)
    {
        out += vec[i] * vec2[i];
    }
    return out;
}

float vector_product(const std::array<float, 4> &vec2, const std::array<float, 4> &vec)
{
    float out = 0.0;
    for (uint i = 0; i < vec2.size(); i++)
    {
        out += vec[i] * vec2[i];
    }
    return out;
}

float vector_product(const std::array<float, 5> &vec2, const std::array<float, 5> &vec)
{
    float out = 0.0;
    for (uint i = 0; i < vec2.size(); i++)
    {
        out += vec[i] * vec2[i];
    }
    return out;
}

float vector_product(const std::array<float, 6> &vec2, const std::array<float, 6> &vec)
{
    float out = 0.0;
    for (uint i = 0; i < vec2.size(); i++)
    {
        out += vec[i] * vec2[i];
    }
    return out;
}
float vector_product(const std::array<float, 7> &vec2, const std::array<float, 7> &vec)
{
    float out = 0.0;
    for (uint i = 0; i < vec2.size(); i++)
    {
        out += vec[i] * vec2[i];
    }
    return out;
}
float vector_product(const std::array<float, 9> &vec2, const std::array<float, 9> &vec)
{
    float out = 0.0;
    for (uint i = 0; i < vec2.size(); i++)
    {
        out += vec[i] * vec2[i];
    }
    return out;
}

float vector_product(const std::array<float, 10> &vec2, const std::array<float, 10> &vec)
{
    float out = 0.0;
    for (uint i = 0; i < vec2.size(); i++)
    {
        out += vec[i] * vec2[i];
    }
    return out;
}

float vector_product(const std::array<float, 19> &vec2, const std::array<float, 19> &vec)
{
    float out = 0.0;
    for (uint i = 0; i < vec2.size(); i++)
    {
        out += vec[i] * vec2[i];
    }
    return out;
}

float vector_product(const std::array<float, 1> &vec2, const std::array<float, 1> &vec)
{
    float out = 0.0;
    for (uint i = 0; i < vec2.size(); i++)
    {
        out += vec[i] * vec2[i];
    }
    return out;
}
float vector_product(const std::array<float, 12> &vec2, const std::array<float, 12> &vec)
{
    float out = 0.0;
    for (uint i = 0; i < vec2.size(); i++)
    {
        out += vec[i] * vec2[i];
    }
    return out;
}

float vector_product(const std::array<float, 21> &vec2, const std::array<float, 21> &vec)
{
    float out = 0.0;
    for (uint i = 0; i < vec2.size(); i++)
    {
        out += vec[i] * vec2[i];
    }
    return out;
}
float vector_product(const std::array<float, 30> &vec2, const std::array<float, 30> &vec)
{
    float out = 0.0;
    for (uint i = 0; i < vec2.size(); i++)
    {
        out += vec[i] * vec2[i];
    }
    return out;
}

float vector_product(const std::array<float, 20> &vec2, const std::array<float, 20> &vec)
{
    float out = 0.0;
    for (uint i = 0; i < vec2.size(); i++)
    {
        out += vec[i] * vec2[i];
    }
    return out;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float find_max(const std::vector<float> &vec)
{
    float max_val = 0.0;

    for (const float val : vec)
    {
        if (val > max_val)
        {
            max_val = val;
        }
    }
    return max_val;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int find_max(const std::vector<int> &vec)
{
    int max_val = 0.0;

    for (const int val : vec)
    {
        if (val > max_val)
        {
            max_val = val;
        }
    }
    return max_val;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int get_hour_from_timestamp(const int timestamp)
{
    time_t rawtime = timestamp;
    struct tm ts;
    char buf[80];

    // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz" ->  "%a %Y-%m-%d %H:%M:%S %Z"
    ts = *localtime(&rawtime);
    strftime(buf, sizeof(buf), "%H", &ts);

    return std::stoi(buf);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int get_year_from_timestamp(const int timestamp)
{
    time_t rawtime = timestamp;
    struct tm ts;
    char buf[80];

    // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
    ts = *localtime(&rawtime);
    strftime(buf, sizeof(buf), "%Y", &ts);

    return std::stoi(buf);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int get_month_from_timestamp(const int timestamp)
{
    time_t rawtime = timestamp;
    struct tm ts;
    char buf[80];

    // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
    ts = *localtime(&rawtime);
    strftime(buf, sizeof(buf), "%m", &ts);

    return std::stoi(buf);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int get_day_from_timestamp(const int timestamp)
{
    time_t rawtime = timestamp;
    struct tm ts;
    char buf[80];

    // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
    ts = *localtime(&rawtime);
    strftime(buf, sizeof(buf), "%d", &ts);

    return std::stoi(buf);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

double get_wall_time()
{
    std::chrono::high_resolution_clock m_clock;
    double time = std::chrono::duration_cast<std::chrono::seconds>(m_clock.now().time_since_epoch()).count();
    return time;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

double process_mem_usage()
{
    double vm_usage = 0.0;
#if defined(__linux__)

    // the two fields we want
    unsigned long vsize;
    long rss;
    {
        std::string ignore;
        std::ifstream ifs("/proc/self/stat", std::ios_base::in);
        ifs >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >>
            ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> vsize >> rss;
    }
    long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
    vm_usage = vsize / 1024.0 / 1024.0;
#endif
    return vm_usage;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<int> integer_range(const int min, const int max, const int step)
{
    std::vector<int> the_range;

    if (min > max)
        std::abort();

    for (int i = min; i <= max - 1; i = i + step)
    {
        the_range.push_back(i);
    }

    return the_range;
}

std::vector<int> integer_range(const int min, const int max)
{
    std::vector<int> the_range;

    if (min > max)
        std::abort();

    for (int i = min; i <= max; i++)
    {
        the_range.push_back(i);
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

    for (int i = 0; i <= nb; i++)
    {
        the_range.push_back(min + float(i) * step);
    }

    return the_range;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string ReplaceAll(std::string str, const std::string &from, const std::string &to)
{
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float calculate_calmar_ratio(const std::vector<int> &times, const std::vector<float> &wallet_vals, const float &max_DD)
{

    if (times.size() <= 4)
        return -100.0;

    const uint last_idx = times.size() - 1;
    const int first_year = get_year_from_timestamp(times[0]);
    const int first_month = get_month_from_timestamp(times[0]);
    const int first_day = get_day_from_timestamp(times[0]);
    const int last_year = get_year_from_timestamp(times[last_idx]);
    const int last_month = get_month_from_timestamp(times[last_idx]);
    const int last_day = get_day_from_timestamp(times[last_idx]);

    const float factor_first_year = (365.0f - float(first_month) * 30.0f - float(first_day)) / 365.0f;
    const float factor_last_year = (float(last_month) * 30.0f + float(last_day)) / 365.0f;

    std::vector<float> vals_begin_years{};
    vals_begin_years.reserve(10);

    vals_begin_years.push_back(1000.0);

    for (uint ii = 1; ii < times.size(); ii++)
    {
        const int year_b = get_year_from_timestamp(times[ii - 1]);
        const int year = get_year_from_timestamp(times[ii]);

        if (year_b != year || ii == times.size() - 1)
        {
            vals_begin_years.push_back(wallet_vals[ii]);
        }
    }

    std::vector<float> yearly_pc_changes{};
    yearly_pc_changes.reserve(10);
    for (uint iy = 1; iy < vals_begin_years.size(); iy++)
    {
        yearly_pc_changes.push_back((vals_begin_years[iy] - vals_begin_years[iy - 1]) / vals_begin_years[iy - 1] * 100.0f);
    }

    // yearly_pc_changes.erase(yearly_pc_changes.begin()); // could remove first and/or lost because the year is not complete

    yearly_pc_changes[0] = yearly_pc_changes[0] * factor_first_year;
    yearly_pc_changes[yearly_pc_changes.size() - 1] = yearly_pc_changes[yearly_pc_changes.size() - 1] * factor_last_year;

    return find_average(yearly_pc_changes) / max_DD;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void random_shuffle_vector(std::vector<float> &vec_in)
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine e(seed);
    std::shuffle(vec_in.begin(), vec_in.end(), e);
}

void random_shuffle_vector(std::vector<int> &vec_in)
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine e(seed);
    std::shuffle(vec_in.begin(), vec_in.end(), e);
}

void random_shuffle_vector_params(std::vector<trix_params> &vec_in)
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine e(seed);
    std::shuffle(vec_in.begin(), vec_in.end(), e);
}

void random_shuffle_vector_params(std::vector<BigWill_params> &vec_in)
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine e(seed);
    std::shuffle(vec_in.begin(), vec_in.end(), e);
}

void random_shuffle_vector_params(std::vector<SR_params> &vec_in)
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine e(seed);
    std::shuffle(vec_in.begin(), vec_in.end(), e);
}

void random_shuffle_vector_params(std::vector<EMA3_params> &vec_in)
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine e(seed);
    std::shuffle(vec_in.begin(), vec_in.end(), e);
}