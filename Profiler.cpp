#include "Profiler.h"

Profiler::Profiler(const char* host, const int iter_count, const char* path, const char* verb)
: verb(verb), num_reqs(iter_count), host(host), path(path), failed_reqs(0), min_bytes(INT_MAX), max_bytes(INT_MIN){
    send_reqs();
    agg_time();
    agg_success_pct();
    show_profile();
}

// Initiate the requests, collect the timing and size data
void Profiler::send_reqs(){
    for (int i = 0; i < num_reqs; i++){
        RequestHandler r = RequestHandler(host, path, false);
        // Log failed requests count 
        if (!r.req_success){
            failed_reqs++;
        } else{
            // Track the minimum and maximum size of our responses
            if (r.resp_size > max_bytes) max_bytes = r.resp_size;
            if (r.resp_size < min_bytes) min_bytes = r.resp_size;

            // Build a collection of all request times for bookkeeping
            req_times.push_back(r.total_time);
        }
    }
}

// Aggregate the timing data to find min/max/mean/meadian times
void Profiler::agg_time(){
    if (num_reqs == 1){
        min_time = max_time = mean_time = median_time = req_times[0];
    } else{
        sort(req_times.begin(), req_times.end());
        min_time = req_times[0];
        max_time = req_times[req_times.size()-1];
        
        // Derive median time, average middle times if even number of elements
        int mid_index = req_times.size() / 2;
        std::chrono::milliseconds mid_time = req_times[mid_index];
        if (req_times.size() % 2 == 0){
            std::chrono::milliseconds left_mid_time = req_times[mid_index-1];
            median_time = mid_time + left_mid_time;
        } else{
            median_time = mid_time;
        }

        // Derive mean time
        std::chrono::milliseconds time_sum = req_times[0];
        for (int i = 1; i < req_times.size(); i++) time_sum += req_times[i];
        mean_time = time_sum / req_times.size();
    }
}

// Calculate the total percentage of successful requests made
void Profiler::agg_success_pct(){
    success_pct = (num_reqs - failed_reqs) / num_reqs;
    success_pct = 100 * ((int)(success_pct * 100) / 100.0);
}

// Display profiling results
void Profiler::show_profile(){
    std::cout << "Profiling Results\n-----------------"
            << "\nRequests made to: " << host << path
            << "\nSuccess percentage: " << success_pct
            << "\nTotal number of requests: " << num_reqs
            << "\n\nTiming\n------"
            << "\nFastest request: " << min_time.count()
            << "\nSlowest request: " << max_time.count()
            << "\nMedian request: " << median_time.count()
            << "\nMean request: " << mean_time.count()
            << "\n\nSize\n----"
            << "\nSmallest response: " << min_bytes << " bytes"
            << "\nLargest response: " << max_bytes << " bytes\n\n";
}