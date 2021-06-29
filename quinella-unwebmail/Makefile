
CC = gcc
CXX = g++
CFLAGS = -g
CXXFLAGS = -g -std=c++11
LDFLAGS = -g
LOADLIBES =
LDLIBS = -lpcap -lnids -lcrypto -lssl

target := build/quinella
modules := deps/unprettysoup/unprettysoup utils packets tcp http main         \
	       tls_cipher_suite tls tls_crypto
modules_objs := $(foreach mod, $(modules), build/$(mod).o)

all: makedirs link

link: $(modules_objs)
	$(CXX) $(LDFLAGS) $^ -o $(target) $(LOADLIBES) $(LDLIBS)

makedirs:
	mkdir -p build/
	mkdir -p build/deps/unprettysoup

build/%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

clean:
	rm -f $(modules_objs)
	rm -rf build
