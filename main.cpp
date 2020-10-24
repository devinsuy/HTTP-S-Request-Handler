#include "ParsedUrl.h"
#include "RequestHandler.h"
#include "Profiler.h"

void show_help() {
    printf("\nExample Usage:\n--------------\n");
    printf("--help\t\tDisplay example usage menu\n\n");
    printf("--url\t\tPerforms HTTPS GET request, prints response to console\n");
    printf("\t\tSYNTAX:\t\t--url <link>\n");
    printf("\t\tEXAMPLE:\t--url https://postman-echo.com/get\n\n");
    printf("--profile\tPerform consecutive requests, generate timing and size metrics\n");
    printf("\t\tSYNTAX:\t\t--url <link> <num_iterations>\n");
    printf("\t\tEXAMPLE:\t--url https://devinsuy.com/links 10\n\n");
    printf("exit\t\tTerminates CLI tool\n\n\n");
}

int main(){
    bool exit = false;
    printf("\nRequest Handler CLI Tool\n------------------------\n");
    show_help();
    std::string raw_args;
    std::vector<std::string> args;
    
    while(!exit){
        // Parse space delimited arguments
        std::cout << "> ";
        std::getline(std::cin, raw_args);
        args = ParsedUrl::split(raw_args, ' ');

        // Terminate CLI tool
        if (args[0] == "exit") { 
            printf("\nExiting . . .\n");
            exit = true; 
        }
        // Display syntax and example usage
        else if(args[0] == "--help"){ 
            show_help(); 
        }
        // Send HTTPS request to provided URL, log response to console
        else if (args[0] == "--url"){
            if (args.size() != 2){
                printf("INVALID arguments for --url\n");
                show_help();
            }
            else{
                ParsedUrl url = ParsedUrl(args[1]);
                if (url.valid){
                    RequestHandler r = RequestHandler(url.full_host.c_str(), url.resource.c_str());
                }
                else{
                    printf("Invalid URL \"%s\"!\n\n", args[1].c_str());
                } 
            }
        }
        // Send HTTPS to provided URL a set amount of times, generate response timing/size metrics
        else if (args[0] == "--profile"){
            if (args.size() != 3) {
                printf("INVALID arguments for --profile\n");
                show_help();
            }
            else{
                // Validate iter_count argument
                int iter_count;
                try{
                    iter_count = std::stoi(args[2]);
                } catch(const std::invalid_argument& ia){
                    printf("INVALID number of iterations \"%s\"\n", args[2].c_str());
                    show_help();
                    continue;
                }
                // Validate URL
                ParsedUrl url = ParsedUrl(args[1]);
                if (url.valid){
                    Profiler p = Profiler(url.full_host.c_str(), iter_count, url.resource.c_str());
                }
                else{
                    printf("Invalid URL \"%s\"!\n\n", args[1].c_str());
                } 
            }
        }
        // Input validation 
        else {
            printf("\"%s\" is not a valid command\n", args[0].c_str());
            show_help();
        }
    }
}