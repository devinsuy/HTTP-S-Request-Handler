#ifndef PROFILER_
#define PROFILER_

#include "RequestHandler.h"
#include <vector>
#include <set>
#include <limits.h>
#include <algorithm>

class Profiler{
    public:
        const char *verb;
        const char *path;
        const char *host;
        const int num_reqs;
        int failed_reqs, min_bytes, max_bytes;
        float success_pct;
        std::vector<std::chrono::milliseconds> req_times;
        std::chrono::milliseconds min_time, max_time, mean_time, median_time;

        void send_reqs();
        void agg_time();
        void agg_success_pct();
        void show_profile();
        Profiler(const char* host, const int iter_count, const char* path="/", const char* verb="GET");
};

#endif