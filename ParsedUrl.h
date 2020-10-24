#ifndef PARSED_URL_
#define PARSED_URL_
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

// Given a URL parses and stores the host and resource 
class ParsedUrl{
    public:
        std::string url, host, full_host, resource;
        bool valid;
        std::vector<std::string> pieces;
        static std::vector<std::string> split(const std::string, const char);
        bool is_valid();
        ParsedUrl(const std::string u);
};

#endif