#include "ParsedUrl.h"

// Splits a string by a given delimiter
std::vector<std::string> ParsedUrl::split(const std::string str, const char delim){
    std::string buff;                 
    std::stringstream ss(str);      
    std::vector<std::string> tokens;
    while(getline(ss, buff, delim))
        tokens.push_back(buff);
    return tokens;
} 

bool ParsedUrl::is_valid(){
    if (url.find(".") == -1){
        return false;
    }
    else{
        pieces = split(url, '/');
        // URL contains leading protocol
        if (pieces[0].find("http") != -1){
            if (pieces.size() < 3) {
                return false;
            }
            else{
                host = pieces[2];
            }
        }
        // URL leads with just the hostname
        else{
            host = pieces[0];
        }
        for(int i = 0; i < url.length() - 1; i++){
            if (url[i] == '.' && url[i+1] == '.') return false;
        }
        full_host = (host.substr(0,3) != "www") ? ("www." + host) : host;
        
        return (host.find(".") != -1);
    }
}

ParsedUrl::ParsedUrl(const std::string u) : url(u) {
    if (valid = is_valid()) {
        // Resource immediately follows the host  
        int rsc_start_index = url.find(host) + host.length();
        resource = rsc_start_index == url.length() ? "/" : url.substr(rsc_start_index);
        // Format for trailing backslash
        if (resource[resource.length()-1] != '/'){
            resource += "/";
        } 
    }
} 