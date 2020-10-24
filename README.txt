----------------------------------------------------------
Devin Suy

Resume: https://devinsuy.com/resume
LinkedIn: https://www.linkedin.com/in/devin-suy-8651b2139/
GitHub: https://github.com/devinsuy

----------------------------------------------------------

Included src files:
	"ParsedUrl.h"
	"ParsedUrl.cpp"
	"RequestHandler.h"
	"RequestHandler.cpp"
	"Profiler.h"
	"Profiler.cpp"
	"main.cpp"
	"makefile"
	
	
Build Instructions:
	Navigate to src directory
	Simply run make command
	NOTE: LDFLAGS = -lssl -lcrypto
	
	
Supplemental Files:
	Screenshots of --url and --profile usage for:
		https://devinsuy.com/
		https://devinsuy.com/links
		https://postman-echo.com/get
		(See Screenshots/ directory) 
		
	Sample Terminal Output
		(see "sample_output.txt")
		

HTTP(S) Request Handler CLI Tool
--------------------------------

Functionality:
	--url <link>
		Performs HTTPS GET request, prints response to console
		
	--profile <link> <num_iterations>
		Perform consecutive requests, generate timing and size metrics:
			Number of requests
			Fastest time
			Slowest time
			Mean & median times
			Percentage of successful requests
			HTTP status code
			Smallest repsonse size in bytes
			Largest response size in bytes 
	
	--help
		Displays example usage and syntax menu 
		
	exit
		Terminals CLI tool 
	