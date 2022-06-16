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