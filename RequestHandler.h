#ifndef REQUEST_HANDLER_
#define REQUEST_HANDLER_

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <string.h>
#include <netdb.h>
#include <iostream>
#include <chrono> 

// TODO: Extend functionality for full HTTP protocol
class RequestHandler{
    public:
        const char *verb;
        const char *path;
        const char *host;
        char req_head[4096];
        SSL *ssl;
        int sock, s, req_success, BUFFER_SIZE, resp_size, status_code;
        std::chrono::milliseconds total_time;
        const bool print;
        
        // Forward function declarations
        int init();
        int resolve_host(struct sockaddr_in *);
        void print_errs();
        char* generate_req();
        int send_request(const char *);
        int rcv_response();
        int send_rcv();
        static int find_delim(const char *, const char);

        // Initialize and send request, log response to console
        // sets req_success = 0 if errors encountered
        RequestHandler(const char* host, const char* path="/", const bool print=true, const char* verb="GET");
};

#endif