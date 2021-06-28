
CC = gcc
CXX = g++
CFLAGS = -g
CXXFLAGS = -g -std=c++11
LDFLAGS = -g
LOADLIBES =
LDLIBS = -lpcap -lnids -lcrypto -lssl

target := quinella
modules := main utils
modules_objs := $(foreach mod, $(modules), $(mod).o)

all: $(modules_objs)
	$(CXX) $(LDFLAGS) $^ -o $(target) $(LOADLIBES) $(LDLIBS)

%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

clean:
	rm -f $(modules_objs)
