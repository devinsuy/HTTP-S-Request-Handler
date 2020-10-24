CC = g++
src = $(wildcard *.cpp)
obj = $(src:.cpp=.o)

LDFLAGS = -lssl -lcrypto

all: run

ReqCLI: $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)

run: ReqCLI
	./ReqCLI

.PHONY: clean
clean:
	rm -f $(obj) myprog