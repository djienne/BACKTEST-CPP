#include "tools.hh"

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